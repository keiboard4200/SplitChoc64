# SplitChoc64 RC11 compatibility for current ZMK main / Zephyr 4.1.

zephyr_compile_definitions(
  zmk_endpoints_send_mouse_report=zmk_endpoint_send_mouse_report
)

get_filename_component(SPLITCHOC64_WEST_ROOT "${ZEPHYR_BASE}" DIRECTORY)
set(SPLITCHOC64_ANALOG_SPLIT_SRC
    "${SPLITCHOC64_WEST_ROOT}/zmk-driver-analog-stick/src/analog_stick/input_split.c")

if(EXISTS "${SPLITCHOC64_ANALOG_SPLIT_SRC}")
  file(READ "${SPLITCHOC64_ANALOG_SPLIT_SRC}" _split_src)

  set(_old_compat [=[
/* Zephyr version compat: 3.5 = 2-arg INPUT_CALLBACK_DEFINE, 4.x = 3-arg */
#if __has_include(<zephyr/input/input_callback.h>)
#define SPLIT_INPUT_CB_DEFINE(dev, cb) INPUT_CALLBACK_DEFINE(dev, cb, NULL)
#define SPLIT_INPUT_CB_SIG(name) \
    static void name(struct input_event *evt, void *user_data)
#else
#define SPLIT_INPUT_CB_DEFINE(dev, cb) INPUT_CALLBACK_DEFINE(dev, cb)
#define SPLIT_INPUT_CB_SIG(name) \
    static void name(struct input_event *evt)
#endif
]=])

  set(_new_compat [=[
/* SplitChoc64 RC11: ZMK main currently builds on Zephyr 4.1.
 * Zephyr 4.1 INPUT_CALLBACK_DEFINE() takes (dev, callback, user_data).
 */
#define SPLIT_INPUT_CB_DEFINE(dev, cb) INPUT_CALLBACK_DEFINE(dev, cb, NULL)
#define SPLIT_INPUT_CB_SIG(name) \
    static void name(struct input_event *evt, void *user_data)
]=])

  string(FIND "${_split_src}" "${_old_compat}" _compat_pos)
  if(NOT _compat_pos EQUAL -1)
    string(REPLACE "${_old_compat}" "${_new_compat}" _split_src "${_split_src}")
    file(WRITE "${SPLITCHOC64_ANALOG_SPLIT_SRC}" "${_split_src}")
    message(STATUS "SplitChoc64: patched analog-stick input_split.c for Zephyr 4.1")
  else()
    message(STATUS "SplitChoc64: analog-stick input_split.c compatibility block already changed upstream; no patch applied")
  endif()
else()
  message(FATAL_ERROR "SplitChoc64: zmk-driver-analog-stick source not found")
endif()
