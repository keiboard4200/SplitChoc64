# SplitChoc64 uses the external PAW3222 ZMK driver.
# Local source is limited to board-specific support and behaviors.

target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/status_led.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/behavior_mouse_button_toggle.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/behavior_paw3222_cpi.c)

# ZMK intentionally excludes its host-side HID/keymap path from split peripherals.
# LEFT keeps the normal peripheral role, but additionally exposes only its local
# half as a USB keyboard when attached to a USB HID host.  Reuse ZMK's HID state
# and USB HID report implementation, plus the keycode event type, for that path.
if(CONFIG_SHIELD_SPLITCHOC64_LEFT)
  target_sources(app PRIVATE ${APPLICATION_SOURCE_DIR}/src/hid.c)
  target_sources(app PRIVATE ${APPLICATION_SOURCE_DIR}/src/usb_hid.c)
  target_sources(app PRIVATE ${APPLICATION_SOURCE_DIR}/src/events/keycode_state_changed.c)
  target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/left_standalone_usb.c)
endif()
