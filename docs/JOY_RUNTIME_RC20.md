# SplitChoc64 RC20 — runtime JOY configuration

ZMK Studio assignable behaviors:

- JOY On/Off
- JOY Angle +5 deg
- JOY Angle -5 deg
- JOY Max Speed +
- JOY Max Speed -
- JOY Reset Defaults

Angle correction:
- range: -45° to +45°
- step: 5°
- positive = clockwise on screen
- example: physical 11 o'clock +30° => cursor 12 o'clock

Max speed:
- 25%, 50%, 75%, 100%, 125%, 150%, 175%, 200%
- default = 100%

Reset Defaults:
- JOY ON
- angle = 0°
- max speed = 100%

Settings are stored about 2 seconds after the last change and restored after reboot.

Stock shortcut:
- Fn + Grave = JOY On/Off

RIGHT ADC and zmk,input-split remain unchanged from the build-proven RC19 path.
