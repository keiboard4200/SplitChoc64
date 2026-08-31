# SplitChoc64 Ver001

Clean GitHub upload package, organized to follow Ver000.

- `0_COMMON`: shared footprints / 3D assets
- `1_RIGHT`: RIGHT KiCad project + latest ERC/DRC reports
- `2_LEFT`: LEFT KiCad project + latest ERC/DRC reports
- `3_PLATE`: accepted TopPlate STEP files
- `4_CASE`: accepted BottomCase / BatteryLid / trackball mechanical STEP files
- `5_MANIFUCTUR`: manufacturing exports (LEFT/RIGHT fabrication ZIPs and expanded Gerber/drill outputs)

Validation reports included with KiCad projects:
- LEFT: ERC 0 errors / 0 warnings; DRC 0 violations / 0 unconnected / 0 footprint errors
- RIGHT: ERC 0 errors / 0 warnings; DRC 0 violations / 0 unconnected / 0 footprint errors

`SHA256SUMS.txt` records SHA-256 values that were explicitly verified for the release-critical PCB and mechanical source artifacts. It is not a claim that the files under `5_MANIFUCTUR` were regenerated from the current KiCad PCB in this repository. A byte-for-byte regenerated Gerber/drill comparison requires a matching KiCad 9 `kicad-cli` environment.

The current accepted mechanical revisions in `4_CASE` include Ver001.006 LEFT BottomCase, Ver001.005 RIGHT BottomCase, Ver001.006 LEFT/RIGHT BatteryLids, Ver001.004 LEFT/RIGHT TopPlates, and the LEFT/RIGHT trackball adapters. The trackball-holder STEP is retained as a manufacturing/mechanical dependency but is not listed in `SHA256SUMS.txt` unless its SHA-256 has been explicitly revalidated.

This package intentionally omits intermediate revision files, temporary validation notes, screenshots, old mechanical versions, and development history artifacts.
