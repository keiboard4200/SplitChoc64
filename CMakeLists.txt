# SplitChoc64 compatibility glue for current ZMK main.
#
# The current upstream analog-stick driver already handles the Zephyr 4.x
# INPUT_CALLBACK_DEFINE() signature internally.
#
# It still calls the older plural endpoint helper name, so map only that name.
zephyr_compile_definitions(
  zmk_endpoints_send_mouse_report=zmk_endpoint_send_mouse_report
)
