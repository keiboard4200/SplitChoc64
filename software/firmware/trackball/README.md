# Trackball firmware

This directory is reserved for the SplitChoc64 PAW3222 / Torabo trackball ZMK variant.

## Hardware interface

RIGHT-side J4 assignment for the trackball adapter:

| J4 pin | XIAO signal | Trackball use |
|---|---|---|
| 1 | 3V3 | 3V3 |
| 2 | GND | GND |
| 3 | P0.02 | SCLK |
| 4 | P0.03 | SDIO |
| 5 | P0.28 | MOTION |
| 6 | P0.04 | CS |
| 7 | P0.05 | SPARE |

The PAW3222 implementation must remain separate from the 2765 analog joystick code. Do not overwrite the validated joystick firmware while developing this variant.

Planned work:

- PAW3222 sensor driver/integration
- SPI/SDIO configuration
- MOTION handling
- pointer processing and tuning
- LEFT/RIGHT split build validation
- ZMK Studio regression check
