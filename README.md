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
├─ software/                 # ZMK build/config/module source and firmware variants
│  ├─ build.yaml
│  ├─ west.yml
│  ├─ zmk-module/
│  ├─ firmware/
│  │  ├─ joystick/
│  │  └─ trackball/
│  └─ tools/
├─ docs/                     # Project documentation
├─ archive/                  # Historical/original import data
├─ DEVELOPMENT/              # Development snapshots retained for traceability
├─ zephyr/module.yml         # Small Zephyr entry point that redirects to software/zmk-module
├─ .github/                  # GitHub Actions
└─ README.md
```

## Hardware baseline

- **Ver000**: first manufactured SplitChoc64 production hardware baseline (legacy internal name: Ver012).
- **Ver001**: subsequent manufactured revision, stored under `hardware/main-pcb/Ver001/`.
- Kailh Choc V2 hot-swap switches.
- Seeed XIAO nRF52840 Plus on both halves.

## Pointing-device variants

### Joystick

The joystick firmware variant is stored under `software/firmware/joystick/`.

### Trackball

The trackball variant uses the Torabo-tsuki LP 19 mm mechanical concept and PAW3222-class sensor integration. Trackball firmware is stored separately under `software/firmware/trackball/`.

## ZMK build

GitHub Actions reads:

- Build matrix: `software/build.yaml`
- West/config path: `software/`
- SplitChoc64 Zephyr/ZMK module: `software/zmk-module/`

`zephyr/module.yml` remains at repository root only because ZMK/Zephyr module discovery expects the module entry point there. The actual source is organized under `software/`.

## Firmware identity

- Shield: `splitchoc64_left` / `splitchoc64_right`
- RIGHT: Central
- LEFT: Peripheral
- Bluetooth name: `SplitChoc64`
- ZMK Studio: retained
- `settings_reset` build: retained

## License

See the license files and upstream component licenses included with each subproject before redistribution or commercial use.
