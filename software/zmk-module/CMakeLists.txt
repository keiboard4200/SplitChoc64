# SplitChoc64 uses the external PAW3222 ZMK driver.
# Local source is limited to board-specific support and behaviors.

target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/status_led.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/behavior_mouse_button_toggle.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/behavior_paw3222_cpi.c)
