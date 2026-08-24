# SplitChoc64 Ver001 candidate — DEVELOPMENT

**Status: under development / not a formal release.**

This directory is intentionally separated from formal `Ver000` and future formal `Ver001`.
It contains the current DRC-clean PCB candidate while mechanical and connector changes are still pending.

Current PCB baseline:
- LEFT: final mating outline, H4 moved 3.0 mm left, user routing corrections, local silk fix.
- RIGHT: final mating outline, user routing corrections, local silk fix.
- DRC: user-confirmed 0 errors / 0 warnings on 2026-08-24 using the Ver000 project rule set.
- Component placement, nets and schematic intent otherwise inherit Ver000 unless explicitly changed above.
- KiCad project/schematic/library tables and referenced component 3D models are inherited from Ver000.

Mating geometry:
- Main seam key-center pitch accepted at approximately 20.05 mm.
- Case inner wall target: PCB edge +0.2 mm clearance.
- Case wall thickness target: 1.0 mm.

## Next development tasks
1. Change J3/J4 to FFC connectors.
2. Change the battery connector so the battery-side terminal does not need modification.
3. Design the top plate as 3D-printable data.
4. Define/design the bottom plate.

Do not generate manufacturing data from this folder until these tasks are complete and DRC/mechanical checks are repeated.
