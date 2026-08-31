# SplitChoc64 Ver001 trackball firmware

Ver001 uses the PAW3222-based 14 mm mouse-sensor module on the RIGHT / Central half through the PCB-mounted 6-pin FFC connector.

## Ver001 hardware interface

The finalized RIGHT PCB exposes the trackball through J6 (`FH19C-6S-0.5SH`) on the B-side.

PCB J6 net order:

| J6 pin | PCB net | XIAO pin | nRF52840 GPIO |
|---|---|---|---|
| 1 | GND | GND | - |
| 2 | R_EXT_A | D0/A0 | P0.02 |
| 3 | R_EXT_B | D1/A1 | P0.03 |
| 4 | R_EXT_SDA | D4/SDA | P0.04 |
| 5 | R_EXT_SCL | D5/SCL | P0.05 |
| 6 | 3V3 | 3V3 | - |

The sensor-module FFC order is `1=3V3, 2=CS, 3=MOTION, 4=SDIO, 5=SCLK, 6=GND`.
With the same-electrode FFC between the opposing connectors, the order is reversed at the PCB connector. Therefore the firmware assignment is:

| PAW3222 signal | XIAO / GPIO |
|---|---|
| SCLK | D0 / P0.02 |
| SDIO | D1 / P0.03 |
| MOTION | D4 / P0.04 |
| CS | D5 / P0.05 |

The PAW3222 uses 3-wire SPI, so the same SDIO GPIO is assigned to both SPIM MOSI and MISO.

## ZMK integration

The active Ver001 build uses `sekigon-gonnoc/zmk-driver-paw3222` as a West module. RIGHT remains the split Central / USB-BLE host-facing half, and the trackball is local to RIGHT, so split input relay is not required for the normal Ver001 configuration.

The Ver000/legacy 2765 joystick implementation is retained separately under `software/firmware/joystick/` and is not compiled into the active Ver001 build.

## Initial Ver001 behavior

- Trackball: normal pointer movement through PAW3222
- Lay1: explicit mouse buttons and scroll key controls retained
- Lay3: Bluetooth/output management and ZMK Studio unlock retained
- Legacy joystick tuning bindings removed from the active Ver001 keymap

Pointer CPI/rotation and ball-based scrolling should be tuned on the assembled Ver001 hardware after physical bring-up.
