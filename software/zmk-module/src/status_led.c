#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/sys/util.h>

#define STATUS_LED_NODE DT_ALIAS(status_led)

#if DT_NODE_EXISTS(STATUS_LED_NODE)
static const struct gpio_dt_spec status_led = GPIO_DT_SPEC_GET(STATUS_LED_NODE, gpios);

static int splitchoc64_status_led_init(void)
{
    if (!gpio_is_ready_dt(&status_led)) {
        return -ENODEV;
    }

    /*
     * Ver001 DLED1/DLED2 are independent firmware status LEDs.
     * High = LED on.  Light once the firmware has reached application init.
     */
    return gpio_pin_configure_dt(&status_led, GPIO_OUTPUT_ACTIVE);
}

SYS_INIT(splitchoc64_status_led_init, APPLICATION, 90);
#endif
