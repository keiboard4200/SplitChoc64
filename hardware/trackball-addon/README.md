# Trackball add-on hardware

This directory is the canonical location for the SplitChoc64 trackball add-on.

## Version baseline

- Formal repository production version: **Ver000**
- Legacy/internal package label: **Ver003**
- Ver000 preserves the approved Ver003 payload unchanged; only repository-level version designation is changed.
- Next trackball add-on revision: **Ver001**

## Mechanical baseline

- Torabo-tsuki LP 19 mm holder geometry
- 19.05 mm POM ball
- PAW3222 14 mm mouse sensor module
- dedicated R27 trackball adapter
- trackball-enabled RIGHT bottom case

## FFC adapter

The adapter PCB connects RIGHT J4 to a 6-pin 0.5 mm FFC connector.

### J4 mapping

| J4 | Function |
|---|---|
| 1 | 3V3 |
| 2 | GND |
| 3 | SCLK / P0.02 |
| 4 | SDIO / P0.03 |
| 5 | MOTION / P0.28 |
| 6 | CS / P0.04 |
| 7 | SPARE / P0.05 |

### FFC-side mapping

| J2 | Function |
|---|---|
| 1 | GND |
| 2 | SCLK |
| 3 | SDIO |
| 4 | MOTION |
| 5 | CS |
| 6 | 3V3 |

The physical connector orientation and same-side FFC contact direction must be checked during assembly; do not infer conductor order from the U-fold alone.

## Manufacturing baseline

The approved adapter mechanical revision is R27. Reference assemblies are for fit/visualization only and are not themselves manufacturing parts.

Legacy filenames and internal documents may still contain `Ver003`, `R27`, or `R26`; those names are intentionally retained for traceability to the approved manufacturing data.
