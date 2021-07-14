/*
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>

#define UCEL_PER_CEL 1000000
#define UCEL_PER_MCEL 1000
#define TEMP_INITIAL_CEL 25
#define TEMP_WINDOW_HALF_UCEL 500000

void main(void)
{
	const struct device *dev = DEVICE_DT_GET_ANY(microchip_mcpnse);
	int rc;
	static unsigned int count;

	if (dev == NULL) {
		printf("Device not found.\n");
		return;
	}
	if (!device_is_ready(dev)) {
		printf("Device %s is not ready.\n", dev->name);
		return;
	}

	while (1) {
		struct sensor_value temp;
		

		rc = sensor_sample_fetch(dev);
		if (rc != 0) {
			printf("sensor_sample_fetch error: %d\n", rc);
			break;
		}

		++count;

		rc = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		if (rc != 0) {
			printf("sensor_channel_get error: %d\n", rc);
			break;
		}

		printf("\033[37;1m#\033[32;1m%5u \
\033[37;1m@\033[32;1m%6u\033[37;1mMS: \
\033[36;1m%f\033[31;1m°C\033[0m\n",
		    	count, 
				k_uptime_get_32(),
		    	sensor_value_to_double(&temp));

		k_sleep(K_MSEC(150));
	}
}
