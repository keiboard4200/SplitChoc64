# J3/J4 expansion header (Ver012)

The five signal pins are deliberately **not consumed by RC1**.

| Header pin | Net suffix | XIAO pad | XIAO name | nRF52840 |
|---|---|---:|---|---|
| 1 | 3V3 | 12 | 3V3 | power |
| 2 | GND | 13/33 | GND | ground |
| 3 | JOY_X | 1 | D0 | P0.02 |
| 4 | JOY_Y | 2 | D1 | P0.03 |
| 5 | JOY_SW | 3 | D2 | P0.28 |
| 6 | JOY_SDA | 5 | D4 | P0.04 |
| 7 | JOY_SCL | 6 | D5 | P0.05 |

This leaves all five GPIO signals available for a future analog joystick, I2C device,
or remapped SPI pointing-device interface without changing the manufactured keyboard PCB.

Do not add the pointing driver to RC1 until the base keyboard firmware has been validated.
