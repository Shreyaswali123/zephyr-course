/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_LED_SENSOR_H_
#define ZEPHYR_DRIVERS_SENSOR_LED_SENSOR_H_

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

/*
 * LED sensor-specific channels.
 */
enum led_sensor_channel {
    SENSOR_CHAN_LED_STATE = SENSOR_CHAN_PRIV_START,
};

/*
 * Custom extension API.
 *
 * Changes the driver's dynamic sensor value.
 */
int led_sensor_set_value(const struct device *dev, int value);

#endif /* ZEPHYR_DRIVERS_SENSOR_LED_SENSOR_H_ */