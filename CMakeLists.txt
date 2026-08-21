# SplitChoc64 compatibility glue for zmk-driver-analog-stick vs current ZMK main.

# Endpoint API rename.
zephyr_compile_definitions(
  zmk_endpoints_send_mouse_report=zmk_endpoint_send_mouse_report
)

# The driver source uses the older two-argument INPUT_CALLBACK_DEFINE().
# Force-include a small compatibility header only for this build.
zephyr_compile_options(
  -include
  ${CMAKE_CURRENT_LIST_DIR}/include/splitchoc64_zmk_compat.h
)
