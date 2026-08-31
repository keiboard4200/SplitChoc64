# SplitChoc64 Ver001

Clean GitHub upload package, organized to follow Ver000.

- `0_COMMON`: shared footprints / 3D assets
- `1_RIGHT`: RIGHT KiCad project + latest ERC/DRC reports
- `2_LEFT`: LEFT KiCad project + latest ERC/DRC reports
- `3_PLATE`: accepted TopPlate STEP files
- `4_CASE`: accepted BottomCase / BatteryLid / trackball mechanical STEP files
- `5_MANIFUCTUR`: reserved for manufacturing exports

Validation reports included with KiCad projects:
- LEFT: ERC 0 errors / 0 warnings; DRC 0 violations / 0 unconnected / 0 footprint errors
- RIGHT: ERC 0 errors / 0 warnings; DRC 0 violations / 0 unconnected / 0 footprint errors

This package intentionally omits intermediate revision files, temporary validation notes, screenshots, old mechanical versions, and development history artifacts.
