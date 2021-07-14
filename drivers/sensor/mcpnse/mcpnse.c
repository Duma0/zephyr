/*
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT microchip,mcpnse

#include <errno.h>

#include <kernel.h>
#include <drivers/i2c.h>
#include <init.h>
#include <sys/byteorder.h>
#include <sys/__assert.h>
#include <logging/log.h>

#include "mcpnse.h"

LOG_MODULE_REGISTER(MCPNSE, CONFIG_SENSOR_LOG_LEVEL);

int mcpnse_reg_read(const struct device *dev, uint8_t reg, uint16_t *val)
{
	struct mcpnse_data *data = dev->data;
	const struct mcpnse_config *cfg = dev->config;
	int rc = i2c_write_read(data->i2c_master, cfg->i2c_addr,
				&reg, sizeof(reg),
				val, sizeof(*val));

	if (rc == 0) {
		*val = sys_be16_to_cpu(*val);
	}

	return rc;
}

int mcpnse_reg_write_16bit(const struct device *dev, uint8_t reg,
			    uint16_t val)
{
	struct mcpnse_data *data = dev->data;
	const struct mcpnse_config *cfg = dev->config;

	uint8_t buf[3];

	buf[0] = reg;
	sys_put_be16(val, &buf[1]);

	return i2c_write(data->i2c_master, buf, sizeof(buf), cfg->i2c_addr);
}

int mcpnse_reg_write_8bit(const struct device *dev, uint8_t reg,
			   uint8_t val)
{
	struct mcpnse_data *data = dev->data;
	const struct mcpnse_config *cfg = dev->config;
	uint8_t buf[2] = {
		reg,
		val,
	};

	return i2c_write(data->i2c_master, buf, sizeof(buf), cfg->i2c_addr);
}

static int mcpnse_set_temperature_resolution(const struct device *dev,
					      uint8_t resolution)
{
	return mcpnse_reg_write_8bit(dev, MCPNSE_REG_RESOLUTION, resolution);
}

static int mcpnse_sample_fetch(const struct device *dev,
				enum sensor_channel chan)
{
	struct mcpnse_data *data = dev->data;

	__ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL || chan == SENSOR_CHAN_AMBIENT_TEMP);

	return mcpnse_reg_read(dev, MCPNSE_REG_TEMP_AMB, &data->reg_val);
}

static int mcpnse_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	const struct mcpnse_data *data = dev->data;
	int temp = mcpnse_temp_signed_from_reg(data->reg_val);

	__ASSERT_NO_MSG(chan == SENSOR_CHAN_AMBIENT_TEMP);

	val->val1 = temp / MCPNSE_TEMP_SCALE_CEL;
	temp -= val->val1 * MCPNSE_TEMP_SCALE_CEL;
	val->val2 = (temp * 1000000) / MCPNSE_TEMP_SCALE_CEL;

	return 0;
}

static const struct sensor_driver_api mcpnse_api_funcs = {
	.sample_fetch = mcpnse_sample_fetch,
	.channel_get = mcpnse_channel_get,
};

int mcpnse_init(const struct device *dev)
{
	struct mcpnse_data *data = dev->data;
	const struct mcpnse_config *cfg = dev->config;
	int rc = 0;

	data->i2c_master = device_get_binding(cfg->i2c_bus);
	if (!data->i2c_master) {
		LOG_ERR("mcpnse: i2c master not found: %s", cfg->i2c_bus);
		return -EINVAL;
	}

	rc = mcpnse_set_temperature_resolution(dev, cfg->resolution);
	if (rc) {
		LOG_ERR("Could not set the resolution of mcpnse module");
		return rc;
	}

	return rc;
}

static struct mcpnse_data mcpnse_data;
static const struct mcpnse_config mcpnse_cfg = {
	.i2c_bus = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
	.resolution = DT_INST_PROP(0, resolution),
};

DEVICE_DT_INST_DEFINE(0, mcpnse_init, NULL,
		      &mcpnse_data, &mcpnse_cfg, POST_KERNEL,
		      CONFIG_SENSOR_INIT_PRIORITY, &mcpnse_api_funcs);
