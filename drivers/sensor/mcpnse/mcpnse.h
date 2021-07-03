#ifndef ZEPHYR_DRIVERS_SENSOR_MCPNSE_MCPNSE_H_
#define ZEPHYR_DRIVERS_SENSOR_MCPNSE_MCPNSE_H_

#include <errno.h>

#include <zephyr/types.h>
#include <device.h>
#include <drivers/sensor.h>
#include <sys/util.h>
#include <drivers/gpio.h>

struct mcpnse_data {
	const struct device *i2c_master;

	uint16_t reg_val;
};

struct mcpnse_config {
	const char *i2c_bus;
	uint16_t i2c_addr;
	uint8_t resolution;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_MCPNSE_MCPNSE_H_ */
