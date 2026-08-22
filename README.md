# SplitChoc64 — ZMK Firmware

Firmware repository for the SplitChoc64 Ver012 split keyboard.

## Current status

The current `main` branch is the pre-hardware firmware baseline. GitHub Actions build #34 passed after the repository cleanup.

Physical validation still requires the manufactured keyboard hardware.

## Hardware

- Controller: Seeed XIAO nRF52840 / XIAO BLE
- ZMK board target: `xiao_ble//zmk`
- 64 keys total
- LEFT: 29 keys
- RIGHT: 35 keys
- Matrix diode direction: `col2row`
- RIGHT = split Central
- LEFT = split Peripheral
- RIGHT = USB/BLE host-facing half
- RIGHT = ZMK Studio host-facing half
- RIGHT = local 2765 analog joystick ADC/processing half

## Bluetooth

The advertised keyboard name is:

`SplitChoc64`

The Central half is RIGHT. Bluetooth profile selection/management and USB/BLE output controls are available from the Fn layer.

If the stored Bluetooth name or bonds need to be cleared after firmware/configuration changes, flash `settings_reset.uf2`, then flash the normal RIGHT firmware again.

## ZMK Studio

ZMK Studio support is enabled on the RIGHT/Central firmware. The Studio unlock action is available from the Fn layer.

Studio is used for supported runtime keymap changes. Joystick tuning is implemented by dedicated ZMK behaviors/settings rather than treating those parameters as ordinary key assignments.

## 2765 joystick

The analog joystick is connected to and processed by the RIGHT/Central half.

Runtime controls include:

- joystick ON/OFF
- angle adjustment in 5-degree steps
- maximum cursor speed adjustment
- minimum cursor speed adjustment
- deadzone adjustment
- response-curve adjustment
- reset joystick settings to defaults

Joystick settings are persistent.

The angle adjustment exists so the physical thumb direction can be corrected independently of the desired on-screen cursor direction. For example, a natural physical push near 11 o'clock can be rotated so the cursor travels toward 12 o'clock.

## Firmware artifacts

GitHub Actions builds three UF2 files:

1. `splitchoc64_left.uf2` — LEFT / Peripheral
2. `splitchoc64_right.uf2` — RIGHT / Central, host-facing firmware and joystick processing
3. `settings_reset.uf2` — clears persistent ZMK settings when required

## Repository structure

- `.github/workflows/` — GitHub Actions firmware build
- `boards/shields/splitchoc64/` — SplitChoc64 shield, matrix, split and hardware definitions
- `config/` — ZMK build configuration / west manifest
- `dts/` — custom Devicetree bindings
- `include/` — SplitChoc64 firmware headers
- `src/` — custom joystick/input processing and runtime settings
- `docs/` — hardware bring-up, connector and matrix/pin mapping documentation
- `tools/` — validation/support tooling
- `zephyr/module.yml` — Zephyr/ZMK module declaration
- `build.yaml` — firmware build targets

## Hardware documentation retained in `docs/`

- `HARDWARE_BRINGUP.md` — staged physical bring-up procedure
- `EXTENSION_J3_J4.md` — J3/J4 extension connector information
- `MATRIX_LOGICAL_MAP.csv` — logical matrix mapping
- `VER012_PINMAP.csv` — Ver012 hardware pin map

These files describe the current hardware and are intentionally retained; old RC-by-RC validation reports and temporary migration/delete notes have been removed from `main`.

## Build

Pushing changes to `main` runs the GitHub Actions workflow. A successful run produces the firmware artifacts above.

This repository is also a ZMK/Zephyr module. The module exposes the custom shield, Devicetree bindings and custom source required by SplitChoc64.

## Physical validation still required

Before treating the firmware as hardware-validated, check on the assembled keyboard:

- LEFT and RIGHT matrix scanning / every key
- RIGHT Central ↔ LEFT Peripheral split connection
- USB keyboard operation from RIGHT
- Bluetooth advertising and pairing as `SplitChoc64`
- Bluetooth profile switching and bond clearing
- ZMK Studio connection/unlock
- 2765 X/Y direction and center stability
- joystick deadzone, angle, min/max speed and response curve
- joystick ON/OFF and reset-default behaviors
- battery reporting, charging and normal power behavior

Firmware changes after this point should preferably be driven by physical bring-up results rather than additional RC-numbered snapshots.
