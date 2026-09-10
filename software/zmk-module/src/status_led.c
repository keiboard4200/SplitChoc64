#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zmk/activity.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>

#define STATUS_LED_NODE DT_ALIAS(status_led)
#define STATUS_DIM_PERCENT 50U
#define STATUS_FULL_PERCENT 100U
#define STATUS_SLOW_BLINK_MS 1000U
#define STATUS_FAST_BLINK_MS 250U

#if DT_NODE_EXISTS(STATUS_LED_NODE)

static const struct pwm_dt_spec status_led = PWM_DT_SPEC_GET(STATUS_LED_NODE);
static struct k_work_delayable status_led_work;
static bool status_led_on;
static uint8_t status_led_percent;
static uint32_t blink_period_ms;

static int status_led_set_percent(uint8_t percent)
{
    uint32_t pulse;

    if (percent > 100U) {
        percent = 100U;
    }

    pulse = (uint32_t)(((uint64_t)status_led.period * percent) / 100U);
    return pwm_set_pulse_dt(&status_led, pulse);
}

static void status_led_set(bool on)
{
    status_led_on = on;
    status_led_set_percent(on ? status_led_percent : 0U);
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

static void status_led_apply(uint8_t percent, uint32_t period_ms)
{
    status_led_percent = percent;
    blink_period_ms = period_ms;
    k_work_cancel_delayable(&status_led_work);

    if (percent == 0U) {
        status_led_set(false);
        return;
    }

    status_led_set(true);
    if (period_ms != 0U) {
        k_work_reschedule(&status_led_work, K_MSEC(period_ms));
    }
}

#if defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>

/*
 * Central / host-facing RIGHT PLED policy:
 *   solid 50%         = USB or BLE host connected
 *   slow blink 50%    = bonded BLE profile selected but disconnected
 *   fast blink 100%   = open BLE profile / pairing-ready state
 *   OFF               = no usable endpoint state or deep sleep
 */
static void status_led_update_central(void)
{
    struct zmk_endpoint_instance selected = zmk_endpoint_get_selected();

    if (zmk_activity_get_state() == ZMK_ACTIVITY_SLEEP) {
        status_led_apply(0U, 0U);
        return;
    }

    if (selected.transport == ZMK_TRANSPORT_USB) {
        status_led_apply(STATUS_DIM_PERCENT, 0U);
        return;
    }

    if (selected.transport == ZMK_TRANSPORT_BLE && zmk_ble_active_profile_is_connected()) {
        status_led_apply(STATUS_DIM_PERCENT, 0U);
        return;
    }

    if (zmk_endpoint_get_preferred_transport() == ZMK_TRANSPORT_BLE) {
        if (zmk_ble_active_profile_is_open()) {
            status_led_apply(STATUS_FULL_PERCENT, STATUS_FAST_BLINK_MS);
        } else {
            status_led_apply(STATUS_DIM_PERCENT, STATUS_SLOW_BLINK_MS);
        }
        return;
    }

    status_led_apply(0U, 0U);
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
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/usb.h>

/*
 * Peripheral LEFT PLED policy:
 *   solid 50%      = standalone USB HID ready OR split link to RIGHT established
 *   slow blink 50% = waiting for RIGHT central
 *   OFF            = deep sleep
 */
static void status_led_update_peripheral(void)
{
    if (zmk_activity_get_state() == ZMK_ACTIVITY_SLEEP) {
        status_led_apply(0U, 0U);
        return;
    }

    if (zmk_usb_is_hid_ready() || zmk_split_bt_peripheral_is_connected()) {
        status_led_apply(STATUS_DIM_PERCENT, 0U);
    } else {
        status_led_apply(STATUS_DIM_PERCENT, STATUS_SLOW_BLINK_MS);
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
ZMK_SUBSCRIPTION(splitchoc64_status_led_peripheral, zmk_usb_conn_state_changed);

#endif

static int status_led_activity_listener(const zmk_event_t *eh)
{
    ARG_UNUSED(eh);

#if defined(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    status_led_update_central();
#else
    status_led_update_peripheral();
#endif

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(splitchoc64_status_led_activity, status_led_activity_listener);
ZMK_SUBSCRIPTION(splitchoc64_status_led_activity, zmk_activity_state_changed);

static int splitchoc64_status_led_init(void)
{
    if (!pwm_is_ready_dt(&status_led)) {
        return -ENODEV;
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
