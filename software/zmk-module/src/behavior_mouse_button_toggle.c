#define DT_DRV_COMPAT zmk_behavior_mouse_button_toggle

#include <stdbool.h>

#include <zephyr/device.h>
#include <zephyr/kernel.h>

#if IS_ENABLED(CONFIG_INPUT)
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/input/input.h>
#endif

#include <drivers/behavior.h>
#include <zmk/activity.h>
#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/layer_state_changed.h>

#define SPLIT_CHOC64_MOUSE_LAYER 4U

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct mouse_button_toggle_data {
    bool pressed;
};

#if IS_ENABLED(CONFIG_INPUT)
static int release_if_pressed(const struct device *dev)
{
    struct mouse_button_toggle_data *data = dev->data;

    if (!data->pressed) {
        return 0;
    }

    data->pressed = false;
    return input_report_key(dev, INPUT_BTN_0, 0, true, K_FOREVER);
}
#endif

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event)
{
    ARG_UNUSED(event);

#if IS_ENABLED(CONFIG_INPUT)
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct mouse_button_toggle_data *data = dev->data;

    data->pressed = !data->pressed;
    return input_report_key(dev, INPUT_BTN_0, data->pressed ? 1 : 0, true, K_FOREVER);
#else
    ARG_UNUSED(binding);
    return ZMK_BEHAVIOR_OPAQUE;
#endif
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event)
{
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api mouse_button_toggle_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

#define MOUSE_BUTTON_TOGGLE_INST(n)                                                               \
    static struct mouse_button_toggle_data mouse_button_toggle_data_##n;                           \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, &mouse_button_toggle_data_##n, NULL, POST_KERNEL,       \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &mouse_button_toggle_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MOUSE_BUTTON_TOGGLE_INST)

#if IS_ENABLED(CONFIG_INPUT)
#define RELEASE_TOGGLE_INSTANCE(n)                                                                \
    do {                                                                                           \
        int ret = release_if_pressed(DEVICE_DT_INST_GET(n));                                       \
        if (ret < 0) {                                                                             \
            return ret;                                                                            \
        }                                                                                          \
    } while (0);

static int mouse_button_toggle_event_listener(const zmk_event_t *eh)
{
    const struct zmk_layer_state_changed *layer_ev = as_zmk_layer_state_changed(eh);
    const struct zmk_activity_state_changed *activity_ev = as_zmk_activity_state_changed(eh);
    bool release = false;

    if (layer_ev != NULL && layer_ev->layer == SPLIT_CHOC64_MOUSE_LAYER && !layer_ev->state) {
        release = true;
    }

    if (activity_ev != NULL && activity_ev->state == ZMK_ACTIVITY_SLEEP) {
        release = true;
    }

    if (!release) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    DT_INST_FOREACH_STATUS_OKAY(RELEASE_TOGGLE_INSTANCE)

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(splitchoc64_mouse_button_toggle, mouse_button_toggle_event_listener);
ZMK_SUBSCRIPTION(splitchoc64_mouse_button_toggle, zmk_layer_state_changed);
ZMK_SUBSCRIPTION(splitchoc64_mouse_button_toggle, zmk_activity_state_changed);
#endif

#endif
