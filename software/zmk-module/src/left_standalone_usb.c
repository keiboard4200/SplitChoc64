/*
 * SplitChoc64 LEFT standalone USB keyboard support.
 *
 * The LEFT half remains a normal ZMK split peripheral when it is not attached
 * to a USB HID host.  When USB reaches HID-ready state, the local key matrix
 * is translated directly to the LEFT half of the SplitChoc64 keymap and sent
 * over USB.  The split transport is disabled while that USB HID connection is
 * active, so a key press is not delivered to both LEFT USB and RIGHT central.
 */

#include <errno.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/keys.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/hid.h>
#include <zmk/keys.h>
#include <zmk/split/transport/peripheral.h>
#include <zmk/usb.h>
#include <zmk/usb_hid.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/* Defined by ZMK's split peripheral implementation. */
extern const struct zmk_split_transport_peripheral *active_transport;

#define LEFT_FN_POS 56
#define LEFT_SHIFT_POS 41
#define NUMFN_TAPPING_TERM_MS 250
#define POSITION_COUNT 64

static bool layer1_active;
static bool mouse_layer_active;
static uint32_t active_code[POSITION_COUNT];

/* Global SplitChoc64 positions belonging to the LEFT half. */
static const uint32_t base_code[POSITION_COUNT] = {
    [0] = ESC,    [1] = N1,     [2] = N2,     [3] = N3,     [4] = N4,    [5] = N5,
    [14] = TAB,   [15] = Q,     [16] = W,     [17] = E,     [18] = R,    [19] = T,
    [28] = CAPS,  [29] = A,     [30] = S,     [31] = D,     [32] = F,    [33] = G,
    [41] = LSHIFT,[42] = Z,     [43] = X,     [44] = C,     [45] = V,    [46] = B,
    [53] = LCTRL, [54] = LGUI,  [55] = LALT,  [57] = SPACE,
};

/* Layer 1 entries which replace Base directly.  Zero means transparent. */
static const uint32_t layer1_code[POSITION_COUNT] = {
    [14] = BSPC,
    [18] = FSLH,
    [19] = MINUS,
    [28] = ENTER,
    [32] = KP_MULTIPLY,
    [33] = KP_PLUS,
    [46] = KP_DOT,
};

struct numfn_state {
    uint8_t position;
    uint32_t tap_code;
    uint32_t hold_code;
    bool pressed;
    bool hold_sent;
    struct k_work_delayable hold_work;
};

static struct numfn_state numfn_keys[] = {
    {.position = 15, .tap_code = KP_N7, .hold_code = F7},
    {.position = 16, .tap_code = KP_N8, .hold_code = F8},
    {.position = 17, .tap_code = KP_N9, .hold_code = F9},
    {.position = 29, .tap_code = KP_N4, .hold_code = F4},
    {.position = 30, .tap_code = KP_N5, .hold_code = F5},
    {.position = 31, .tap_code = KP_N6, .hold_code = F6},
    {.position = 42, .tap_code = KP_N1, .hold_code = F1},
    {.position = 43, .tap_code = KP_N2, .hold_code = F2},
    {.position = 44, .tap_code = KP_N3, .hold_code = F3},
    {.position = 45, .tap_code = KP_N0, .hold_code = F10},
};

static int emit_encoded(uint32_t encoded, bool pressed) {
    if (!encoded || !zmk_usb_is_hid_ready()) {
        return 0;
    }

    return raise_zmk_keycode_state_changed_from_encoded(encoded, pressed, k_uptime_get());
}

static struct numfn_state *find_numfn(uint32_t position) {
    for (size_t i = 0; i < ARRAY_SIZE(numfn_keys); i++) {
        if (numfn_keys[i].position == position) {
            return &numfn_keys[i];
        }
    }
    return NULL;
}

static void numfn_hold_work_handler(struct k_work *work) {
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);
    struct numfn_state *state = CONTAINER_OF(dwork, struct numfn_state, hold_work);

    if (!state->pressed || !zmk_usb_is_hid_ready()) {
        return;
    }

    state->hold_sent = true;
    emit_encoded(state->hold_code, true);
}

static void reset_local_state(void) {
    layer1_active = false;
    mouse_layer_active = false;
    memset(active_code, 0, sizeof(active_code));

    for (size_t i = 0; i < ARRAY_SIZE(numfn_keys); i++) {
        k_work_cancel_delayable(&numfn_keys[i].hold_work);
        numfn_keys[i].pressed = false;
        numfn_keys[i].hold_sent = false;
    }

    /* Clear ZMK's local HID state so reconnect cannot resurrect a held key. */
    memset(zmk_hid_get_keyboard_report(), 0, sizeof(struct zmk_hid_keyboard_report));
    memset(zmk_hid_get_consumer_report(), 0, sizeof(struct zmk_hid_consumer_report));
    zmk_hid_unregister_mods(0xFF);
    zmk_hid_implicit_modifiers_release();
}

static int set_split_transport_enabled(bool enabled) {
    if (!active_transport || !active_transport->api || !active_transport->api->set_enabled) {
        return 0;
    }

    int err = active_transport->api->set_enabled(enabled);
    if (err < 0) {
        LOG_WRN("LEFT standalone USB: split transport %s failed (%d)",
                enabled ? "enable" : "disable", err);
    }
    return err;
}

static int standalone_usb_connection_listener(const zmk_event_t *eh) {
    const struct zmk_usb_conn_state_changed *ev = as_zmk_usb_conn_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->conn_state == ZMK_USB_CONN_HID && zmk_usb_is_hid_ready()) {
        LOG_INF("LEFT standalone USB keyboard active");
        reset_local_state();
        set_split_transport_enabled(false);
    } else {
        reset_local_state();
        set_split_transport_enabled(true);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

static int handle_numfn(struct numfn_state *state, bool pressed) {
    if (pressed) {
        state->pressed = true;
        state->hold_sent = false;
        k_work_reschedule(&state->hold_work, K_MSEC(NUMFN_TAPPING_TERM_MS));
        return 0;
    }

    if (!state->pressed) {
        return 0;
    }

    state->pressed = false;
    k_work_cancel_delayable(&state->hold_work);

    if (state->hold_sent) {
        emit_encoded(state->hold_code, false);
    } else {
        emit_encoded(state->tap_code, true);
        emit_encoded(state->tap_code, false);
    }

    state->hold_sent = false;
    return 0;
}

static int standalone_position_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
    if (!ev || !zmk_usb_is_hid_ready() || ev->position >= POSITION_COUNT) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    const uint32_t position = ev->position;

    /* LEFT Fn is momentary Layer 1 in both Base and the local Mouse state. */
    if (position == LEFT_FN_POS) {
        layer1_active = ev->state;
        if (ev->state && mouse_layer_active) {
            mouse_layer_active = false;
        }
        return ZMK_EV_EVENT_BUBBLE;
    }

    /* LEFT Fn + LEFT Shift enters the persistent Mouse layer, matching Layer 1. */
    if (!mouse_layer_active && layer1_active && position == LEFT_SHIFT_POS) {
        if (ev->state) {
            mouse_layer_active = true;
        }
        return ZMK_EV_EVENT_BUBBLE;
    }

    /* LEFT has no pointing hardware.  A key in Mouse layer exits it and emits Base. */
    if (mouse_layer_active && ev->state) {
        mouse_layer_active = false;
        active_code[position] = base_code[position];
        emit_encoded(active_code[position], true);
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (!ev->state && active_code[position]) {
        emit_encoded(active_code[position], false);
        active_code[position] = 0;
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (layer1_active) {
        struct numfn_state *numfn = find_numfn(position);
        if (numfn) {
            handle_numfn(numfn, ev->state);
            return ZMK_EV_EVENT_BUBBLE;
        }
    }

    if (ev->state) {
        uint32_t code = base_code[position];
        if (layer1_active && layer1_code[position]) {
            code = layer1_code[position];
        }
        active_code[position] = code;
        emit_encoded(code, true);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

static int standalone_hid_keycode_pressed(const struct zmk_keycode_state_changed *ev) {
    int err;

    if (!is_mod(ev->usage_page, ev->keycode) &&
        zmk_hid_is_pressed(ZMK_HID_USAGE(ev->usage_page, ev->keycode))) {
        zmk_hid_release(ZMK_HID_USAGE(ev->usage_page, ev->keycode));
        if (ev->usage_page == HID_USAGE_KEY) {
            zmk_usb_hid_send_keyboard_report();
        } else if (ev->usage_page == HID_USAGE_CONSUMER) {
            zmk_usb_hid_send_consumer_report();
        }
    }

    err = zmk_hid_press(ZMK_HID_USAGE(ev->usage_page, ev->keycode));
    if (err < 0) {
        return err;
    }

    int explicit_mods_changed = zmk_hid_register_mods(ev->explicit_modifiers);
    int implicit_mods_changed = zmk_hid_implicit_modifiers_press(ev->implicit_modifiers);

    if (ev->usage_page != HID_USAGE_KEY &&
        (explicit_mods_changed > 0 || implicit_mods_changed > 0)) {
        zmk_usb_hid_send_keyboard_report();
    }

    if (ev->usage_page == HID_USAGE_KEY) {
        return zmk_usb_hid_send_keyboard_report();
    }
    if (ev->usage_page == HID_USAGE_CONSUMER) {
        return zmk_usb_hid_send_consumer_report();
    }
    return -ENOTSUP;
}

static int standalone_hid_keycode_released(const struct zmk_keycode_state_changed *ev) {
    int err = zmk_hid_release(ZMK_HID_USAGE(ev->usage_page, ev->keycode));
    if (err < 0) {
        return err;
    }

    int explicit_mods_changed = zmk_hid_unregister_mods(ev->explicit_modifiers);
    int implicit_mods_changed = zmk_hid_implicit_modifiers_release();

    if (ev->usage_page != HID_USAGE_KEY &&
        (explicit_mods_changed > 0 || implicit_mods_changed > 0)) {
        zmk_usb_hid_send_keyboard_report();
    }

    if (ev->usage_page == HID_USAGE_KEY) {
        return zmk_usb_hid_send_keyboard_report();
    }
    if (ev->usage_page == HID_USAGE_CONSUMER) {
        return zmk_usb_hid_send_consumer_report();
    }
    return -ENOTSUP;
}

static int standalone_keycode_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (!ev || !zmk_usb_is_hid_ready()) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->state) {
        standalone_hid_keycode_pressed(ev);
    } else {
        standalone_hid_keycode_released(ev);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

static int standalone_usb_init(void) {
    for (size_t i = 0; i < ARRAY_SIZE(numfn_keys); i++) {
        k_work_init_delayable(&numfn_keys[i].hold_work, numfn_hold_work_handler);
    }
    reset_local_state();
    return 0;
}

SYS_INIT(standalone_usb_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

ZMK_LISTENER(splitchoc64_left_usb_position, standalone_position_listener);
ZMK_SUBSCRIPTION(splitchoc64_left_usb_position, zmk_position_state_changed);

ZMK_LISTENER(splitchoc64_left_usb_keycode, standalone_keycode_listener);
ZMK_SUBSCRIPTION(splitchoc64_left_usb_keycode, zmk_keycode_state_changed);

ZMK_LISTENER(splitchoc64_left_usb_connection, standalone_usb_connection_listener);
ZMK_SUBSCRIPTION(splitchoc64_left_usb_connection, zmk_usb_conn_state_changed);
