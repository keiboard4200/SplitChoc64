# SplitChoc64

SplitChoc64 is a 64-key split low-profile keyboard project built around Seeed XIAO nRF52840 and ZMK.

This repository is the single source for firmware, PCB design, mechanical/3D-print data, manufacturing outputs, and project documentation.

## Hardware baseline

- Main PCB production baseline: **Ver012**
- Ver012 PCB outline, key positions, and interlocking geometry are treated as frozen production geometry.
- RIGHT half: Central / USB-BLE host
- LEFT half: Peripheral
- Kailh Choc V2 hot-swap switches

## Pointing-device variants

### Joystick

The existing production firmware supports the 2765 analog joystick on the RIGHT half.

### Trackball

The trackball variant uses the Torabo-tsuki LP 19 mm mechanical concept with a PAW3222 14 mm sensor module and the dedicated J4-to-FFC adapter.

Trackball firmware is maintained separately from the joystick variant so the two input implementations can evolve without mixing device-specific code.

## Repository layout

```text
SplitChoc64/
├─ firmware/
│  ├─ joystick/        # 2765 ZMK implementation
│  └─ trackball/       # PAW3222/Torabo ZMK implementation
├─ hardware/
│  ├─ main-pcb/
│  │  └─ Ver012/
│  └─ trackball-addon/
├─ mechanical/
├─ docs/
└─ .github/
```

## Migration status

The repository is being migrated from the former firmware-only layout. During the migration, the proven joystick firmware remains at the repository root so the existing ZMK build workflow remains intact. An identical snapshot is also stored under `firmware/joystick/` as the future canonical firmware location.

The root firmware copy should only be removed after the workflow has been updated and both LEFT/RIGHT builds are revalidated from the new path.

## Firmware identity

- Shield: `splitchoc64_left` / `splitchoc64_right`
- RIGHT: Central
- LEFT: Peripheral
- Bluetooth name: `SplitChoc64`
- ZMK Studio: retained
- `settings_reset` build: retained

## License

See the license files and upstream component licenses included with each subproject before redistribution or commercial use.
