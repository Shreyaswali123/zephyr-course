#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <zephyr/shell/shell.h>
#include "led_sensor.h"

#define LED_SENSOR_NODE DT_NODELABEL(led_sensor)

static const struct device *sensor = DEVICE_DT_GET(LED_SENSOR_NODE);


/* sensor fetch command */
static int cmd_sensor_fetch(const struct shell *shell,
                            size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = sensor_sample_fetch(sensor);

    if (ret < 0) {
        shell_error(shell, "sensor_sample_fetch() failed: %d", ret);
        return ret;
    }

    shell_print(shell, "sensor_sample_fetch() successful");
    return 0;
}


/* sensor read command */
static int cmd_sensor_read(const struct shell *shell,
                           size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    struct sensor_value value;

    int ret = sensor_channel_get(sensor,
                                 SENSOR_CHAN_LED_STATE,
                                 &value);

    if (ret < 0) {
        shell_error(shell, "sensor_channel_get() failed: %d", ret);
        return ret;
    }

    shell_print(shell, "LED state: %d", value.val1);

    return 0;
}


/* sensor info command */
static int cmd_sensor_info(const struct shell *shell,
                           size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(shell, "Device: %s", sensor->name);
    shell_print(shell, "Ready: %s",
                device_is_ready(sensor) ? "YES" : "NO");

    return 0;
}


/* sensor root command */
SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmds,
    SHELL_CMD(fetch, NULL, "Fetch sensor sample", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Read LED sensor value", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Show sensor information", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_subcmds,
                   "LED sensor commands", NULL);


int main(void)
{
    struct sensor_value value;

    if (!device_is_ready(sensor)) {
        printk("LED sensor not ready\n");
        return 0;
    }

    printk("LED sensor shell test started\n");
    printk("Use: sensor fetch | sensor read | sensor info\n");

    while (1) {
        int ret = sensor_sample_fetch(sensor);

        if (ret < 0) {
            printk("sample_fetch failed: %d\n", ret);
            break;
        }

        printk("sample_fetch: LED ON\n");

        ret = led_sensor_set_value(sensor, 42);

        if (ret < 0) {
            printk("led_sensor_set_value failed: %d\n", ret);
            break;
        }

        printk("custom API: sensor value changed to 42\n");

        k_sleep(K_SECONDS(1));

        ret = sensor_channel_get(sensor,
                                 SENSOR_CHAN_LED_STATE,
                                 &value);

        if (ret < 0) {
            printk("channel_get failed: %d\n", ret);
            break;
        }

        printk("channel_get: LED OFF, value = %d\n",
               value.val1);

        k_sleep(K_SECONDS(1));
    }

    return 0;
}