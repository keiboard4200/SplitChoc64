# Joystick firmware

This directory is a snapshot of the validated SplitChoc64 ZMK firmware for the RIGHT-side 2765 analog joystick configuration.

## Status

- RIGHT = Central / USB-BLE host
- LEFT = Peripheral
- Current production PCB baseline = Ver012
- JOY_X = P0.02 / ADC0
- JOY_Y = P0.03 / ADC1
- Sampling target = 125 Hz
- ZMK Studio retained

During the repository-layout migration, the same firmware is intentionally kept at repository root so the existing GitHub Actions workflow is not broken before path-aware build validation is completed.

Do not modify Ver012 hardware geometry from firmware work.
