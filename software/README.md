# Software

This directory contains the software and firmware side of SplitChoc64.

## Layout

- `build.yaml` — ZMK build matrix used by GitHub Actions.
- `west.yml` — West manifest for fetching ZMK and dependencies.
- `zmk-module/` — SplitChoc64 Zephyr/ZMK module source, board/shield definitions, devicetree bindings and C sources.
- `firmware/joystick/` — preserved joystick firmware variant.
- `firmware/trackball/` — trackball firmware variant.
- `tools/` — software-side helper and validation tools.

## Build integration

The repository keeps only `zephyr/module.yml` at the repository root as the Zephyr module entry point. It points into `software/zmk-module/`.

GitHub Actions uses `software/build.yaml` as the build matrix and `software/` as the ZMK config path. This keeps the repository root readable without changing the hardware tree.
