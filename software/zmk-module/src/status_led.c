#include <errno.h>
#include <stdbool.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zmk/event_manager.h>

#define STATUS_LED_NODE DT_ALIAS(status_led)

#if DT_NODE_EXISTS(STATUS_LED_NODE)

static const struct gpio_dt_spec status_led = GPIO_DT_SPEC_GET(STATUS_LED_NODE, gpios);
static struct k_work_delayable status_led_work;
static bool status_led_on;
static uint32_t blink_period_ms;

static void status_led_set(bool on)
{
    status_led_on = on;
    gpio_pin_set_dt(&status_led, on ? 1 : 0);
}

static void status_led_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (blink_period_ms == 0U) {
        return;
    }

    status_led_set(!status_led_on);
    k_work_reschedule(&status_led_work, K_MSEC(blink_period_ms));
}

static void status_led_apply(bool solid_on, uint32_t period_ms)
{
    blink_period_ms = period_ms;
    k_work_cancel_delayable(&status_led_work);

    if (period_ms == 0U) {
        status_led_set(solid_on);
        return;
    }

    status_led_set(true);
    k_work_reschedule(&status_led_work, K_MSEC(period_ms));
}

#if defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>

/*
 * Central / host-facing RIGHT LED policy:
 *   solid ON       = USB or BLE host connected
 *   slow blink 1 s = bonded BLE profile selected but currently disconnected
 *   fast blink 250 ms = open BLE profile / pairing-ready state
 *   OFF            = no usable endpoint state
 */
static void status_led_update_central(void)
{
    struct zmk_endpoint_instance selected = zmk_endpoint_get_selected();

    if (selected.transport == ZMK_TRANSPORT_USB) {
        status_led_apply(true, 0U);
        return;
    }

    if (selected.transport == ZMK_TRANSPORT_BLE && zmk_ble_active_profile_is_connected()) {
        status_led_apply(true, 0U);
        return;
    }

    if (zmk_endpoint_get_preferred_transport() == ZMK_TRANSPORT_BLE) {
        if (zmk_ble_active_profile_is_open()) {
            status_led_apply(false, 250U);
        } else {
            status_led_apply(false, 1000U);
        }
        return;
    }

    status_led_apply(false, 0U);
}

static int status_led_endpoint_listener(const zmk_event_t *eh)
{
    ARG_UNUSED(eh);
    status_led_update_central();
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(splitchoc64_status_led_endpoint, status_led_endpoint_listener);
ZMK_SUBSCRIPTION(splitchoc64_status_led_endpoint, zmk_endpoint_changed);
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(splitchoc64_status_led_endpoint, zmk_ble_active_profile_changed);
#endif

#else

#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/split/bluetooth/peripheral.h>

/*
 * Peripheral LEFT LED policy:
 *   solid ON       = split link to RIGHT central established
 *   slow blink 1 s = waiting for the central half
 */
static void status_led_update_peripheral(void)
{
    if (zmk_split_bt_peripheral_is_connected()) {
        status_led_apply(true, 0U);
    } else {
        status_led_apply(false, 1000U);
    }
}

static int status_led_peripheral_listener(const zmk_event_t *eh)
{
    ARG_UNUSED(eh);
    status_led_update_peripheral();
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(splitchoc64_status_led_peripheral, status_led_peripheral_listener);
ZMK_SUBSCRIPTION(splitchoc64_status_led_peripheral, zmk_split_peripheral_status_changed);

#endif

static int splitchoc64_status_led_init(void)
{
    int ret;

    if (!gpio_is_ready_dt(&status_led)) {
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&status_led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    k_work_init_delayable(&status_led_work, status_led_work_handler);

#if defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    status_led_update_central();
#else
    status_led_update_peripheral();
#endif

    return 0;
}

SYS_INIT(splitchoc64_status_led_init, APPLICATION, 90);

#endif
