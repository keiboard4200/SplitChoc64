#define DT_DRV_COMPAT zmk_input_processor_discard

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/input/input.h>

#include <drivers/input_processor.h>

static int discard_handle_event(const struct device *dev, struct input_event *event,
                                uint32_t param1, uint32_t param2,
                                struct zmk_input_processor_state *state) {
    ARG_UNUSED(dev);
    ARG_UNUSED(event);
    ARG_UNUSED(param1);
    ARG_UNUSED(param2);
    ARG_UNUSED(state);

    return ZMK_INPUT_PROC_STOP;
}

static const struct zmk_input_processor_driver_api discard_api = {
    .handle_event = discard_handle_event,
};

#define DISCARD_INST(n)                                                        \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,              \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &discard_api);

DT_INST_FOREACH_STATUS_OKAY(DISCARD_INST)
