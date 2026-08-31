#pragma once

/*
 * Compatibility for zmk-driver-analog-stick against the newer Zephyr input API.
 *
 * The driver's input_split.c uses:
 *   INPUT_CALLBACK_DEFINE(device, callback)
 *
 * Current Zephyr uses:
 *   INPUT_CALLBACK_DEFINE(name, device, callback)
 *
 * input/input.h is included first so its original macro is available, then
 * redefine the public macro to synthesize a unique callback name from __LINE__.
 */
#include <zephyr/input/input.h>

#define SPLITCHOC64_CAT_(a, b) a##b
#define SPLITCHOC64_CAT(a, b) SPLITCHOC64_CAT_(a, b)

#ifdef INPUT_CALLBACK_DEFINE
#define SPLITCHOC64_INPUT_CALLBACK_DEFINE_3 INPUT_CALLBACK_DEFINE
#undef INPUT_CALLBACK_DEFINE
#define INPUT_CALLBACK_DEFINE(dev, callback) \
    SPLITCHOC64_INPUT_CALLBACK_DEFINE_3( \
        SPLITCHOC64_CAT(splitchoc64_input_cb_, __LINE__), dev, callback)
#endif
