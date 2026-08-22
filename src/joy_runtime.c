#define DT_DRV_COMPAT splitchoc64_behavior_joy_config

#include <stdint.h>
#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>

#define JOY_CMD_TOGGLE 0
#define JOY_CMD_ANGLE_INC 1
#define JOY_CMD_ANGLE_DEC 2
#define JOY_CMD_SPEED_INC 3
#define JOY_CMD_SPEED_DEC 4
#define JOY_CMD_RESET 5
#define JOY_CMD_DEADZONE_INC 6
#define JOY_CMD_DEADZONE_DEC 7
#define JOY_CMD_MIN_SPEED_INC 8
#define JOY_CMD_MIN_SPEED_DEC 9
#define JOY_CMD_CURVE_INC 10
#define JOY_CMD_CURVE_DEC 11

#define JOY_ANGLE_MIN (-45)
#define JOY_ANGLE_MAX 45
#define JOY_ANGLE_STEP 5
#define JOY_SPEED_MIN 25
#define JOY_SPEED_MAX 200
#define JOY_SPEED_STEP 25
#define JOY_SPEED_DEFAULT 100

#define JOY_DEADZONE_MIN 0
#define JOY_DEADZONE_MAX 40
#define JOY_DEADZONE_STEP 5
#define JOY_DEADZONE_DEFAULT 10

#define JOY_MIN_SPEED_MIN 0
#define JOY_MIN_SPEED_MAX 50
#define JOY_MIN_SPEED_STEP 5
#define JOY_MIN_SPEED_DEFAULT 10

/* Curve levels:
 * 0 = linear
 * 1 = gentle precision
 * 2 = balanced
 * 3 = strong precision
 */
#define JOY_CURVE_MIN 0
#define JOY_CURVE_MAX 3
#define JOY_CURVE_DEFAULT 2

#define JOY_SAVE_DELAY_MS 2000

struct joy_runtime_state {
    bool enabled;
    int8_t angle_deg;
    uint8_t speed_pct;
    uint8_t deadzone_pct;
    uint8_t min_speed_pct;
    uint8_t curve_level;
} __packed;

static struct joy_runtime_state joy_state = {
    .enabled = true,
    .angle_deg = 0,
    .speed_pct = JOY_SPEED_DEFAULT,
    .deadzone_pct = JOY_DEADZONE_DEFAULT,
    .min_speed_pct = JOY_MIN_SPEED_DEFAULT,
    .curve_level = JOY_CURVE_DEFAULT,
};

struct trig_q10 {
    int8_t deg;
    int16_t cos_q10;
    int16_t sin_q10;
};

static const struct trig_q10 trig_table[] = {
    {-45,  724,  -724},
    {-40,  784,  -658},
    {-35,  839,  -587},
    {-30,  887,  -512},
    {-25,  928,  -433},
    {-20,  962,  -350},
    {-15,  989,  -265},
    {-10, 1008,  -178},
    { -5, 1020,   -89},
    {  0, 1024,     0},
    {  5, 1020,    89},
    { 10, 1008,   178},
    { 15,  989,   265},
    { 20,  962,   350},
    { 25,  928,   433},
    { 30,  887,   512},
    { 35,  839,   587},
    { 40,  784,   658},
    { 45,  724,   724},
};

static struct k_work_delayable joy_save_work;

static void joy_save_work_handler(struct k_work *work) {
    ARG_UNUSED(work);
    settings_save_one("splitchoc64/joy/state", &joy_state, sizeof(joy_state));
}

static void joy_schedule_save(void) {
    k_work_reschedule(&joy_save_work, K_MSEC(JOY_SAVE_DELAY_MS));
}

static int joy_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;
    if (!settings_name_steq(name, "state", &next) || next) return -ENOENT;
    if (len != sizeof(joy_state)) return -EINVAL;

    struct joy_runtime_state loaded;
    int rc = read_cb(cb_arg, &loaded, sizeof(loaded));
    if (rc < 0) return rc;

    if (loaded.angle_deg < JOY_ANGLE_MIN || loaded.angle_deg > JOY_ANGLE_MAX ||
        (loaded.angle_deg % JOY_ANGLE_STEP) != 0 ||
        loaded.speed_pct < JOY_SPEED_MIN || loaded.speed_pct > JOY_SPEED_MAX ||
        (loaded.speed_pct % JOY_SPEED_STEP) != 0 ||
        loaded.deadzone_pct < JOY_DEADZONE_MIN || loaded.deadzone_pct > JOY_DEADZONE_MAX ||
        (loaded.deadzone_pct % JOY_DEADZONE_STEP) != 0 ||
        loaded.min_speed_pct < JOY_MIN_SPEED_MIN || loaded.min_speed_pct > JOY_MIN_SPEED_MAX ||
        (loaded.min_speed_pct % JOY_MIN_SPEED_STEP) != 0 ||
        loaded.curve_level < JOY_CURVE_MIN || loaded.curve_level > JOY_CURVE_MAX) {
        return -EINVAL;
    }

    joy_state = loaded;
    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(splitchoc64_joy, "splitchoc64/joy", NULL,
                               joy_settings_set, NULL, NULL);

static const struct trig_q10 *joy_trig(void) {
    int idx = (joy_state.angle_deg - JOY_ANGLE_MIN) / JOY_ANGLE_STEP;
    idx = CLAMP(idx, 0, (int)ARRAY_SIZE(trig_table) - 1);
    return &trig_table[idx];
}

static int16_t joy_clamp_i16(int32_t v) {
    if (v > INT16_MAX) return INT16_MAX;
    if (v < INT16_MIN) return INT16_MIN;
    return (int16_t)v;
}

static void joy_reset_defaults(void) {
    joy_state.enabled = true;
    joy_state.angle_deg = 0;
    joy_state.speed_pct = JOY_SPEED_DEFAULT;
    joy_state.deadzone_pct = JOY_DEADZONE_DEFAULT;
    joy_state.min_speed_pct = JOY_MIN_SPEED_DEFAULT;
    joy_state.curve_level = JOY_CURVE_DEFAULT;
}

struct joy_behavior_config {
    uint8_t command;
};

static int joy_behavior_pressed(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    ARG_UNUSED(event);
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    if (!dev) return -ENODEV;
    const struct joy_behavior_config *cfg = dev->config;

    switch (cfg->command) {
    case JOY_CMD_TOGGLE:
        joy_state.enabled = !joy_state.enabled;
        break;
    case JOY_CMD_ANGLE_INC:
        joy_state.angle_deg = MIN((int)joy_state.angle_deg + JOY_ANGLE_STEP, JOY_ANGLE_MAX);
        break;
    case JOY_CMD_ANGLE_DEC:
        joy_state.angle_deg = MAX((int)joy_state.angle_deg - JOY_ANGLE_STEP, JOY_ANGLE_MIN);
        break;
    case JOY_CMD_SPEED_INC:
        joy_state.speed_pct = MIN((int)joy_state.speed_pct + JOY_SPEED_STEP, JOY_SPEED_MAX);
        break;
    case JOY_CMD_SPEED_DEC:
        joy_state.speed_pct = MAX((int)joy_state.speed_pct - JOY_SPEED_STEP, JOY_SPEED_MIN);
        break;
    case JOY_CMD_RESET:
        joy_reset_defaults();
        break;
    case JOY_CMD_DEADZONE_INC:
        joy_state.deadzone_pct = MIN((int)joy_state.deadzone_pct + JOY_DEADZONE_STEP, JOY_DEADZONE_MAX);
        break;
    case JOY_CMD_DEADZONE_DEC:
        joy_state.deadzone_pct = MAX((int)joy_state.deadzone_pct - JOY_DEADZONE_STEP, JOY_DEADZONE_MIN);
        break;
    case JOY_CMD_MIN_SPEED_INC:
        joy_state.min_speed_pct = MIN((int)joy_state.min_speed_pct + JOY_MIN_SPEED_STEP, JOY_MIN_SPEED_MAX);
        break;
    case JOY_CMD_MIN_SPEED_DEC:
        joy_state.min_speed_pct = MAX((int)joy_state.min_speed_pct - JOY_MIN_SPEED_STEP, JOY_MIN_SPEED_MIN);
        break;
    case JOY_CMD_CURVE_INC:
        joy_state.curve_level = MIN((int)joy_state.curve_level + 1, JOY_CURVE_MAX);
        break;
    case JOY_CMD_CURVE_DEC:
        joy_state.curve_level = MAX((int)joy_state.curve_level - 1, JOY_CURVE_MIN);
        break;
    default:
        return -ENOTSUP;
    }

    joy_schedule_save();
    return ZMK_BEHAVIOR_OPAQUE;
}

static int joy_behavior_released(struct zmk_behavior_binding *binding,
                                 struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api joy_behavior_api = {
    .binding_pressed = joy_behavior_pressed,
    .binding_released = joy_behavior_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define JOY_BEHAVIOR_INST(n) \
    static const struct joy_behavior_config joy_behavior_config_##n = { \
        .command = DT_INST_PROP(n, command), \
    }; \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, &joy_behavior_config_##n, \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, \
                            &joy_behavior_api);

DT_INST_FOREACH_STATUS_OKAY(JOY_BEHAVIOR_INST)

static int joy_runtime_init(void) {
    k_work_init_delayable(&joy_save_work, joy_save_work_handler);
    return 0;
}

SYS_INIT(joy_runtime_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);



static uint32_t joy_abs_i32(int32_t v) {
    if (v >= 0) {
        return (uint32_t)v;
    }

    /* Avoid signed overflow for INT32_MIN by widening before negation. */
    return (uint32_t)(-(int64_t)v);
}

static uint32_t joy_isqrt_u32(uint32_t x) {
    uint32_t op = x;
    uint32_t res = 0;
    uint32_t one = 1uL << 30;

    while (one > op) {
        one >>= 2;
    }

    while (one != 0) {
        if (op >= res + one) {
            op -= res + one;
            res = res + 2 * one;
        }
        res >>= 1;
        one >>= 2;
    }

    return res;
}

static uint16_t joy_curve_q10(uint16_t mag_q10) {
    uint32_t m = MIN((uint32_t)mag_q10, 1024u);

    switch (joy_state.curve_level) {
    case 0:
        return (uint16_t)m;
    case 1:
        /* Blend linear and square: 75% linear + 25% x^2 */
        return (uint16_t)((3u * m + ((m * m) / 1024u)) / 4u);
    case 2:
        /* Balanced precision: 50% linear + 50% x^2 */
        return (uint16_t)((m + ((m * m) / 1024u)) / 2u);
    case 3:
    default:
        /* Strong precision near center: x^2 */
        return (uint16_t)((m * m) / 1024u);
    }
}

static void joy_apply_response(int32_t *x, int32_t *y) {
    uint32_t ax = joy_abs_i32(*x);
    uint32_t ay = joy_abs_i32(*y);
    uint32_t mag = joy_isqrt_u32(ax * ax + ay * ay);

    if (mag == 0) {
        *x = 0;
        *y = 0;
        return;
    }

    /* zmk-analog-input output is already relative motion. We normalize against
     * a conservative reference magnitude of 32 counts per synchronized sample.
     * Hardware tuning can later adjust this if actual 2765 output differs.
     */
    const uint32_t ref_mag = 32;
    uint32_t mag_q10 = MIN((mag * 1024u) / ref_mag, 1024u);

    uint32_t dead_q10 = ((uint32_t)joy_state.deadzone_pct * 1024u) / 100u;
    if (mag_q10 <= dead_q10) {
        *x = 0;
        *y = 0;
        return;
    }

    /* Remap the usable range after deadzone back to 0..1024. */
    mag_q10 = ((mag_q10 - dead_q10) * 1024u) / MAX(1u, 1024u - dead_q10);

    uint32_t curved_q10 = joy_curve_q10((uint16_t)mag_q10);

    uint32_t min_q10 = ((uint32_t)joy_state.min_speed_pct * 1024u) / 100u;
    uint32_t response_q10 = min_q10 + ((1024u - min_q10) * curved_q10) / 1024u;

    /* Preserve vector direction while changing only its magnitude response. */
    *x = (int32_t)((*x * (int32_t)response_q10) / 1024);
    *y = (int32_t)((*y * (int32_t)response_q10) / 1024);
}

#if DT_HAS_CHOSEN(splitchoc64_joy_input) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

static int32_t joy_acc_x;
static int32_t joy_acc_y;

static void joy_input_handler(struct input_event *evt, void *user_data) {
    ARG_UNUSED(user_data);

    if (evt->type == INPUT_EV_REL) {
        if (evt->code == INPUT_REL_X) joy_acc_x += evt->value;
        else if (evt->code == INPUT_REL_Y) joy_acc_y += evt->value;
    }

    if (!evt->sync) return;

    int32_t x = joy_acc_x;
    int32_t y = joy_acc_y;
    joy_acc_x = 0;
    joy_acc_y = 0;

    if (!joy_state.enabled || (x == 0 && y == 0)) return;

    joy_apply_response(&x, &y);
    if (x == 0 && y == 0) return;

    const struct trig_q10 *t = joy_trig();

    /* Positive correction rotates cursor direction clockwise on screen.
     * Example: 11 o'clock physical input +30 deg => 12 o'clock cursor.
     */
    int32_t rx = ((x * t->cos_q10) - (y * t->sin_q10)) / 1024;
    int32_t ry = ((x * t->sin_q10) + (y * t->cos_q10)) / 1024;

    rx = (rx * joy_state.speed_pct) / 100;
    ry = (ry * joy_state.speed_pct) / 100;

    zmk_hid_mouse_movement_set(joy_clamp_i16(rx), joy_clamp_i16(ry));
    zmk_endpoint_send_mouse_report();
    zmk_hid_mouse_movement_set(0, 0);
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_CHOSEN(splitchoc64_joy_input)),
                      joy_input_handler, NULL);

#endif
