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

static int mcpnse_sample_fetch(const struct device *dev,
				enum sensor_channel chan)
{
	struct mcpnse_data *data = dev->data;
}

static int mcpnse_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	const struct mcpnse_data *data = dev->data;
}

static const struct sensor_driver_api mcpnse_api_funcs = {
	.sample_fetch = mcpnse_sample_fetch,
	.channel_get = mcpnse_channel_get,
};

int mcpnse_init(const struct device *dev)
{
	struct mcpnse_data *data = dev->data;
	const struct mcpnse_config *cfg = dev->config;
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
