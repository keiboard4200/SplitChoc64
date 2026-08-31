# SplitChoc64 Ver001 uses the external PAW3222 ZMK driver.
# Local source is limited to board-specific support such as the discrete status LED.

target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/status_led.c)
