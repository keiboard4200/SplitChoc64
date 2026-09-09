#define DT_DRV_COMPAT zmk_behavior_mouse_button_toggle

#include <stdbool.h>

#include <zephyr/device.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct mouse_button_toggle_data {
    bool pressed;
};

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event)
{
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct mouse_button_toggle_data *data = dev->data;

    ARG_UNUSED(event);

    data->pressed = !data->pressed;
    return input_report_key(dev, INPUT_BTN_0, data->pressed ? 1 : 0, true, K_FOREVER);
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

#endif
