# SplitChoc64 RC21 — advanced JOY tuning

RC21 keeps all RC20 controls and adds six more ZMK Studio assignable behaviors:

- JOY Deadzone +
- JOY Deadzone -
- JOY Min Speed +
- JOY Min Speed -
- JOY Curve +
- JOY Curve -

Existing behaviors remain:
- JOY On/Off
- JOY Angle +5 deg
- JOY Angle -5 deg
- JOY Max Speed +
- JOY Max Speed -
- JOY Reset Defaults

## Deadzone
Range: 0% to 40%, 5% steps.
Default: 10%.

## Minimum speed
Range: 0% to 50%, 5% steps.
Default: 10%.

This controls the output just outside the deadzone. Higher values make the
pointer start moving more decisively; lower values allow finer micro-movement.

## Curve
Four levels:
- 0: Linear
- 1: Gentle precision
- 2: Balanced (default)
- 3: Strong precision

Higher curve levels reduce movement more strongly near center while preserving
full-scale movement near maximum stick travel.

## Reset Defaults
Restores all runtime settings:
- JOY enabled
- angle = 0°
- max speed = 100%
- deadzone = 10%
- minimum speed = 10%
- curve = Balanced (2)

All values are persisted using Zephyr settings.
