/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

#include "led_sensor.h"

#define LED_SENSOR_NODE DT_NODELABEL(led_sensor)

int main(void)
{
    const struct device *sensor = DEVICE_DT_GET(LED_SENSOR_NODE);
    struct sensor_value value;

    if (!device_is_ready(sensor)) {
        printk("LED sensor not ready\n");
        return 0;
    }

    printk("LED sensor test started\n");

    while (1) {
    int ret = sensor_sample_fetch(sensor);

    if (ret < 0) {
        printk("sample_fetch failed: %d\n", ret);
        break;
    }

    printk("sample_fetch: LED ON\n");

    /*
     * Call the custom driver extension API.
     * This changes the driver's dynamic data value.
     */
    ret = led_sensor_set_value(sensor, 42);

    if (ret < 0) {
        printk("led_sensor_set_value failed: %d\n", ret);
        break;
    }

    printk("custom API: sensor value changed to 42\n");

    k_sleep(K_MSEC(1000));

    ret = sensor_channel_get(
        sensor,
        SENSOR_CHAN_LED_STATE,
        &value
    );

    if (ret < 0) {
        printk("channel_get failed: %d\n", ret);
        break;
    }

    printk("channel_get: LED OFF, value = %d\n", value.val1);

    k_sleep(K_MSEC(1000));
}

    return 0;
}