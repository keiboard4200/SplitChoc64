# SplitChoc64 RC24 — Hardware Bring-up Procedure

Final architecture:
- RIGHT = Central / host-facing half
- LEFT = Peripheral
- 2765 JOY = RIGHT
- ZMK Studio = RIGHT
- BLE device name = SplitChoc64

## Stage 0 — Visual / continuity inspection before power
PASS:
- no visible solder bridges,
- MCU orientation correct,
- battery polarity correct,
- no short between 3V3 and GND,
- no short between battery positive and GND,
- power switch works mechanically.

Use USB first. Do not connect LiPo first.

## Stage 1 — Flash RIGHT only
Flash `splitchoc64_right.uf2`.

PASS:
- XIAO boots,
- USB enumerates,
- no reset loop,
- keyboard is detected by host.

If persistent configuration is suspect, flash `settings_reset.uf2`, then reflash RIGHT.

## Stage 2 — RIGHT key matrix
Test every RIGHT key by USB.

PASS:
- every RIGHT key produces exactly one intended logical key,
- no stuck key,
- no ghost activation.

## Stage 3 — RIGHT-local JOY basic input
Before LEFT is involved, verify the 2765 connected to RIGHT.

PASS:
- X and Y both affect cursor,
- diagonal movement works,
- JOY On/Off works,
- no major idle drift.

## Stage 4 — Flash LEFT and verify split keyboard
Flash `splitchoc64_left.uf2`.

PASS:
- LEFT connects to RIGHT automatically,
- all LEFT keys appear through RIGHT,
- physical key order is correct,
- reconnect succeeds after LEFT power cycle.

## Stage 5 — Bluetooth host pairing
The host-visible name should be **SplitChoc64**.

Stock controls:
- Fn+1..5: BLE profiles 1..5
- Fn+-: previous profile
- Fn+=: next profile
- Fn+Backspace: clear current profile
- Fn+Tab: USB/BLE preference toggle

PASS:
- SplitChoc64 appears,
- typing works over BLE,
- reconnect works after central power cycle,
- multiple host profiles can be selected.

## Stage 6 — ZMK Studio
Connect RIGHT by USB. Use Fn+Menu to unlock Studio.

PASS:
- SplitChoc64 physical layout appears,
- 64 keys are shown correctly,
- key binding changes can be saved and tested.

## Stage 7 — JOY direction correction
Start defaults:
- ON
- angle 0°
- max speed 100%
- deadzone 10%
- min speed 10%
- curve Balanced

Tune angle with ±5° behaviors.

Example:
physical 11 o'clock -> cursor 12 o'clock
target correction is roughly +30°.

## Stage 8 — JOY response tuning
Tune in this order:
1. Deadzone
2. Angle
3. Max speed
4. Curve
5. Minimum speed

PASS:
- no unwanted idle drift,
- precise small movement,
- comfortable full-speed movement,
- smooth diagonals,
- settings persist after power cycle,
- RESET restores defaults.

## Stage 9 — Battery / wireless-only operation
After stable USB operation:
- test both halves on battery,
- verify RIGHT central reconnects to host,
- verify LEFT reconnects to RIGHT,
- verify JOY still works.

## Stage 10 — Endurance
Use 30–60 minutes and watch for:
- split disconnects,
- stuck keys,
- cursor drift,
- resets,
- BLE reconnect failures,
- abnormal battery drain.
