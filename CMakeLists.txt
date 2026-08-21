# SplitChoc64 compatibility glue for external analog-stick driver.
# ZMK main renamed zmk_endpoints_send_mouse_report() to
# zmk_endpoint_send_mouse_report(). Keep the third-party driver source
# untouched and map the legacy symbol at compile time.
zephyr_compile_definitions(
  zmk_endpoints_send_mouse_report=zmk_endpoint_send_mouse_report
)
