/*
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT zephyr_led_sensor

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include "led_sensor.h"

struct led_sensor_config {
    struct gpio_dt_spec led;
};

struct led_sensor_data {
    struct sensor_value value;
};

int led_sensor_set_value(const struct device *dev, int value)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    struct led_sensor_data *data = dev->data;

    data->value.val1 = value;
    data->value.val2 = 0;

    return 0;
}

static int led_sensor_sample_fetch(const struct device *dev,
                                   enum sensor_channel chan)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if ((chan != SENSOR_CHAN_ALL) &&
        (chan != (enum sensor_channel)SENSOR_CHAN_LED_STATE)) {
        return -ENOTSUP;
    }

    /*
     * Task requirement:
     * sensor_sample_fetch() turns the LED ON.
     */
    int ret = gpio_pin_set_dt(&config->led, 1);
    if (ret < 0) {
        return ret;
    }

    data->value.val1 = 1;
    data->value.val2 = 0;

    return 0;
}

static int led_sensor_channel_get(const struct device *dev,
                                  enum sensor_channel chan,
                                  struct sensor_value *val)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if (chan != (enum sensor_channel)SENSOR_CHAN_LED_STATE) {
        return -ENOTSUP;
    }

    /*
     * Task requirement:
     * sensor_channel_get() turns the LED OFF.
     */
    int ret = gpio_pin_set_dt(&config->led, 0);
    if (ret < 0) {
        return ret;
    }

    *val = data->value;

    return 0;
}

static int led_sensor_init(const struct device *dev)
{
    const struct led_sensor_config *config = dev->config;

    if (!gpio_is_ready_dt(&config->led)) {
        return -ENODEV;
    }

    return gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
}

static DEVICE_API(sensor, led_sensor_api) = {
    .sample_fetch = led_sensor_sample_fetch,
    .channel_get = led_sensor_channel_get,
};

#define LED_SENSOR_INIT(inst)                                      \
    static const struct led_sensor_config led_sensor_config_##inst = { \
        .led = GPIO_DT_SPEC_INST_GET(inst, gpios),                \
    };                                                            \
                                                                  \
    static struct led_sensor_data led_sensor_data_##inst;         \
                                                                  \
    SENSOR_DEVICE_DT_INST_DEFINE(                                 \
        inst,                                                     \
        led_sensor_init,                                          \
        NULL,                                                     \
        &led_sensor_data_##inst,                                  \
        &led_sensor_config_##inst,                                \
        POST_KERNEL,                                              \
        CONFIG_KERNEL_INIT_PRIORITY_DEVICE,                              \
        &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_INIT)
