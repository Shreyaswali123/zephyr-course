#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/*
 * Use the application-specific devicetree alias.
 *
 * app-led is defined in app.overlay and points to
 * the board's LED.
 */
#define LED_NODE DT_ALIAS(app_led)

/*
 * Use the Kconfig-configured heartbeat period.
 *
 * Default: 500 ms
 * Range:   100-2000 ms
 */
#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;

    LOG_INF("Heartbeat LED starting");
    LOG_INF("Heartbeat period: %d ms", SLEEP_TIME_MS);

    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("LED GPIO device is not ready");
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
        LOG_ERR("Failed to configure LED GPIO");
        return 0;
    }

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) {
            LOG_ERR("Failed to toggle LED");
            return 0;
        }

        led_state = !led_state;

        LOG_INF("LED state: %s",
                led_state ? "ON" : "OFF");

        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}