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

	if (dev == NULL) {
		printf("Device not found.\n");
		return;
	}
	if (!device_is_ready(dev)) {
		printf("Device %s is not ready.\n", dev->name);
		return;
	}

	while (1) {
		k_sleep(K_SECONDS(2));
	}
}
