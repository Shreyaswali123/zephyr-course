/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_LED_SENSOR_H_
#define ZEPHYR_DRIVERS_SENSOR_LED_SENSOR_H_

#include <zephyr/drivers/sensor.h>

/*
 * LED sensor-specific channels.
 */
enum led_sensor_channel {
    SENSOR_CHAN_LED_STATE = SENSOR_CHAN_PRIV_START,
};

#endif /* ZEPHYR_DRIVERS_SENSOR_LED_SENSOR_H_ */