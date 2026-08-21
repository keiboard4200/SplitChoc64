# SplitChoc64 Ver012 — ZMK RC3

RC3 is the pre-hardware ZMK baseline generated from the Ver012 KiCad connectivity.

## Hardware mapping

- Controller: XIAO nRF52840 / XIAO BLE
- Current ZMK board target: `xiao_ble//zmk`
- LEFT: 29 keys, 5 rows × 6 populated columns
- RIGHT: 35 keys, 5 rows × 8 populated columns
- Diode direction: `col2row`
- LEFT: split central
- RIGHT: split peripheral
- RIGHT matrix transform column offset: 6
- J3/J4 five signal GPIOs are intentionally unused and reserved for future pointing-device work

## Keymap

The 64 positions follow the supplied KLE order exactly.

Both `Fn` keys use `&mo 1`. Layer 1 remains transparent in RC3 so no unrequested Fn layout is invented before hardware bring-up.

## Current ZMK repository structure

RC3 follows the current ZMK custom-shield/module structure:

- `zephyr/module.yml`
- `boards/shields/splitchoc64/`
- `config/west.yml`
- `build.yaml`
- `.github/workflows/build.yml`

The GitHub build workflow detects `zephyr/module.yml` and loads this repository as an extra ZMK module, making the top-level shield directory visible to Zephyr.

## Build targets

`build.yaml` requests:

1. `xiao_ble//zmk` + `splitchoc64_left`
2. `xiao_ble//zmk` + `splitchoc64_right`
3. `xiao_ble//zmk` + `settings_reset`

## Static check

Run:

```bash
python tools/validate_rc3.py
```

The included static validator checks the 64-position transform, two 64-binding layers, matrix GPIO counts, COL2ROW, right-side offset, module marker, build targets, and current Menu keycode.

## Deep sleep

RC3 deliberately does **not** force `CONFIG_ZMK_SLEEP=y` while developing against ZMK `main`.
Deep sleep can be enabled after the base split firmware is proven on hardware and the chosen ZMK revision is pinned.

## Not yet physically testable

- manufactured PCB matrix scanning
- BLE split radio operation
- battery and charging behavior
- future J3/J4 pointing-device interface
