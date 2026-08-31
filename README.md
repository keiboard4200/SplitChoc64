# SplitChoc64

SplitChoc64 is a 64-key split low-profile keyboard project built around Seeed XIAO nRF52840 and ZMK.

This repository contains firmware/software, PCB design, mechanical/3D-print data, manufacturing outputs, and project documentation.

## Repository layout

```text
SplitChoc64/
├─ hardware/                 # PCB, case, plate, trackball mechanical data and manufacturing files
│  ├─ main-pcb/
│  │  ├─ Ver000/
│  │  └─ Ver001/
│  └─ trackball-addon/
├─ software/                 # ZMK build/config/module source and archived firmware variants
│  ├─ build.yaml
│  ├─ west.yml
│  ├─ zmk-module/            # active Ver001 ZMK module
│  ├─ firmware/
│  │  ├─ joystick/           # legacy Ver000/2765 implementation
│  │  └─ trackball/          # Ver001 PAW3222 documentation
│  └─ tools/
├─ docs/                     # Project documentation
├─ archive/                  # Historical/original import data
├─ DEVELOPMENT/              # Development snapshots retained for traceability
├─ zephyr/module.yml         # Zephyr entry point redirecting to software/zmk-module
├─ .github/                  # GitHub Actions
└─ README.md
```

## Hardware baseline

- **Ver000**: first manufactured SplitChoc64 production hardware baseline (legacy internal name: Ver012).
- **Ver001**: current manufactured hardware/firmware baseline, stored under `hardware/main-pcb/Ver001/`.
- Kailh Choc V2 hot-swap switches.
- Seeed XIAO nRF52840 Plus on both halves.

## Active Ver001 pointing device

Ver001 uses the RIGHT-side PAW3222 trackball through the PCB-mounted 6-pin FFC interface. The active ZMK module under `software/zmk-module/` is configured for this hardware.

The older 2765 analog-joystick implementation is retained only as a legacy reference under `software/firmware/joystick/`; it is not compiled into the active Ver001 firmware.

Detailed Ver001 trackball wiring and firmware notes are in `software/firmware/trackball/README.md`.

## ZMK build

GitHub Actions reads:

- Build matrix: `software/build.yaml`
- West/config path: `software/`
- SplitChoc64 Zephyr/ZMK module: `software/zmk-module/`

`zephyr/module.yml` remains at repository root because ZMK/Zephyr module discovery expects the module entry point there. The implementation is organized under `software/`.

## Firmware identity

- Shield: `splitchoc64_left` / `splitchoc64_right`
- RIGHT: Central / USB-BLE host / PAW3222 trackball
- LEFT: Peripheral
- Bluetooth name: `SplitChoc64`
- ZMK Studio: retained
- `settings_reset` build: retained

## Validation status

GitHub Actions validates LEFT, RIGHT, and `settings_reset` builds. Physical PAW3222 direction, CPI and feel must still be tuned on the assembled Ver001 hardware.

## License

See the license files and upstream component licenses included with each subproject before redistribution or commercial use.
