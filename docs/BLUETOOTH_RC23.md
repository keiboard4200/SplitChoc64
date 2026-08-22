# SplitChoc64 RC24 — Bluetooth readiness

## Device name

The RIGHT/central half defines:

`ZMK_KEYBOARD_NAME = "SplitChoc64"`

Therefore the host-facing keyboard should appear as **SplitChoc64** during BLE pairing.

The LEFT half is the split peripheral and does not act as the host-facing keyboard.

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

## Split architecture

RIGHT:
- ZMK split central
- USB/BLE host HID
- ZMK Studio
- local 2765 ADC/input generation
- JOY runtime processing

LEFT:
- ZMK split peripheral
- key matrix
- forwards key events to RIGHT

Actual RF pairing, reconnection, split-link stability and power consumption
still require physical hardware.
