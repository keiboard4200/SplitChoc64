# SplitChoc64 RC23 — Bluetooth readiness

## Device name

The LEFT/central half defines:

`ZMK_KEYBOARD_NAME = "SplitChoc64"`

Therefore the host-facing keyboard should appear as **SplitChoc64** during BLE pairing.

The RIGHT half is the split peripheral and does not act as the host-facing keyboard.

## Default BLE profiles

ZMK provides five host profiles by default.

Stock Fn shortcuts:

| Shortcut | Action |
|---|---|
| Fn + 1 | Select BLE profile 1 |
| Fn + 2 | Select BLE profile 2 |
| Fn + 3 | Select BLE profile 3 |
| Fn + 4 | Select BLE profile 4 |
| Fn + 5 | Select BLE profile 5 |
| Fn + - | Previous BLE profile |
| Fn + = | Next BLE profile |
| Fn + Backspace | Clear bond for selected profile |
| Fn + Tab | Toggle preferred output USB / BLE |
| Fn + Menu | ZMK Studio unlock |
| Fn + ` | JOY On / Off |

### Pairing procedure

1. Select a profile with Fn+1..5.
2. If that profile has never been paired, SplitChoc64 will advertise for pairing.
3. If the profile already contains a bond, use Fn+Backspace to clear it.
4. On the PC/phone, also forget the old SplitChoc64 entry before pairing again.

## HID descriptor note

JOY/pointing enables mouse HID reports in addition to keyboard reports.
BLE hosts can cache HID descriptors. If firmware changes the HID descriptor,
forget SplitChoc64 on the host, clear the corresponding keyboard profile, and
pair again.

## Split architecture

LEFT:
- ZMK split central
- USB/BLE host HID
- ZMK Studio
- JOY runtime processing

RIGHT:
- ZMK split peripheral
- key matrix
- JOY ADC/input generation
- forwards input to LEFT

Actual RF pairing, reconnection, split-link stability and power consumption
still require physical hardware.
