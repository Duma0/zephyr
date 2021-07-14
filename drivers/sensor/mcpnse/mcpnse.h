/*
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_MCPNSE_MCPNSE_H_
#define ZEPHYR_DRIVERS_SENSOR_MCPNSE_MCPNSE_H_

#include <errno.h>

#include <zephyr/types.h>
#include <device.h>
#include <drivers/sensor.h>
#include <sys/util.h>
#include <drivers/gpio.h>

#define MCPNSE_REG_CONFIG		0x01
#define MCPNSE_REG_UPPER_LIMIT		0x02
#define MCPNSE_REG_LOWER_LIMIT		0x03
#define MCPNSE_REG_CRITICAL		0x04
#define MCPNSE_REG_TEMP_AMB		0x05

/* 16 bits control configuration and state.
 *
 * * Bit 0 controls alert signal output mode
 * * Bit 1 controls interrupt polarity
 * * Bit 2 disables upper and lower threshold checking
 * * Bit 3 enables alert signal output
 * * Bit 4 records alert status
 * * Bit 5 records interrupt status
 * * Bit 6 locks the upper/lower window registers
 * * Bit 7 locks the critical register
 * * Bit 8 enters shutdown mode
 * * Bits 9-10 control threshold hysteresis
 */
#define MCPNSE_CFG_ALERT_MODE_INT	BIT(0)
#define MCPNSE_CFG_ALERT_ENA		BIT(3)
#define MCPNSE_CFG_ALERT_STATE		BIT(4)
#define MCPNSE_CFG_INT_CLEAR		BIT(5)

/* 16 bits are used for temperature and state encoding:
 * * Bits 0..11 encode the temperature in a 2s complement signed value
 *   in Celsius with 1/16 Cel resolution
 * * Bit 12 is set to indicate a negative temperature
 * * Bit 13 is set to indicate a temperature below the lower threshold
 * * Bit 14 is set to indicate a temperature above the upper threshold
 * * Bit 15 is set to indicate a temperature above the critical threshold
 */
#define MCPNSE_TEMP_SCALE_CEL		16 /* signed */
#define MCPNSE_TEMP_SIGN_BIT		BIT(12)
#define MCPNSE_TEMP_ABS_MASK		((uint16_t)(MCPNSE_TEMP_SIGN_BIT - 1U))
#define MCPNSE_TEMP_LWR_BIT		BIT(13)
#define MCPNSE_TEMP_UPR_BIT		BIT(14)
#define MCPNSE_TEMP_CRT_BIT		BIT(15)

#define MCPNSE_REG_RESOLUTION          0x08

struct mcpnse_data {
	const struct device *i2c_master;

	uint16_t reg_val;
};

struct mcpnse_config {
	const char *i2c_bus;
	uint16_t i2c_addr;
	uint8_t resolution;
};

int mcpnse_reg_read(const struct device *dev, uint8_t reg, uint16_t *val);
int mcpnse_reg_write_16bit(const struct device *dev, uint8_t reg,
			    uint16_t val);
int mcpnse_reg_write_8bit(const struct device *dev, uint8_t reg,
			   uint8_t val);

/* Encode a signed temperature in scaled Celsius to the format used in
 * register values.
 */
static inline uint16_t mcpnse_temp_reg_from_signed(int temp)
{
	/* Get the 12-bit 2s complement value */
	uint16_t rv = temp & MCPNSE_TEMP_ABS_MASK;

	if (temp < 0) {
		rv |= MCPNSE_TEMP_SIGN_BIT;
	}
	return rv;
}

/* Decode a register temperature value to a signed temperature in
 * scaled Celsius.
 */
static inline int mcpnse_temp_signed_from_reg(uint16_t reg)
{
	int rv = reg & MCPNSE_TEMP_ABS_MASK;

	if (reg & MCPNSE_TEMP_SIGN_BIT) {
		/* Convert 12-bit 2s complement to signed negative
		 * value.
		 */
		rv = -(1U + (rv ^ MCPNSE_TEMP_ABS_MASK));
	}
	return rv;
}

#endif /* ZEPHYR_DRIVERS_SENSOR_MCPNSE_MCPNSE_H_ */
