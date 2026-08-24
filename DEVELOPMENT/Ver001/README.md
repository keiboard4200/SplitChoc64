# SplitChoc64 DEVELOPMENT / Ver001

Status: **DEVELOPMENT / WIP checkpoint**. This is not a formal released version.

## Current checkpoint
- LEFT/RIGHT PCB mating outline redesigned.
- Main-row seam pitch when assembled: approximately **20.05 mm**.
- Mating-case validation condition: PCB-to-inner-wall clearance **0.2 mm**; wall thickness **1.0 mm**.
- LEFT lower-right M2 mounting hole H4 moved **3.0 mm left**.
- User manually corrected local routing after DRC review; those route geometries are reproduced here.
- Local silkscreen corrections are limited to the mating-edge interference areas.
- **DRC 0 errors / 0 warnings was confirmed by the user on 2026-08-24.**
- A final 0/0 `.rpt` file was not supplied, so no final DRC report is archived here.

## Provenance
The project/schematic/library-table/3D-component support files are inherited from:
`hardware/main-pcb/Ver000/`

`hardware/main-pcb/Ver000/` itself is not modified.

## 3D data
Existing PCB component 3D model directories from Ver000 are copied into this checkpoint.
The case/top plate/bottom plate are not marked final in this checkpoint.

## Planned next work
1. Change J3/J4 to FFC connectors.
2. Change the battery connector so the battery-side terminal does not need modification.
3. Design a 3D-printable top plate.
4. Define/design the bottom plate/case.

## Versioning
Work-in-progress: `DEVELOPMENT/Ver001/`
Formal/released versions: `hardware/main-pcb/VerXXX/`
