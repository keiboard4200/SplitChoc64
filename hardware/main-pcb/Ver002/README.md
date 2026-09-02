# SplitChoc64 Ver002

Clean GitHub source promoted from the user-reviewed XIAO nRF52840 Plus correction.

- `0_COMMON`: shared footprints / 3D assets
- `1_RIGHT`: RIGHT KiCad project + ERC/DRC reports
- `2_LEFT`: LEFT KiCad project + ERC/DRC reports
- `3_PLATE`: accepted TopPlate STEP files
- `4_CASE`: accepted BottomCase / BatteryLid / trackball mechanical STEP files
- Manufacturing exports are intentionally not carried forward from Ver001; regenerate them from Ver002 when needed.

Validation confirmed by the user in KiCad 9.0 before promotion: LEFT/RIGHT ERC 0/0 and DRC 0/0.

Ver002 corrects XIAO nRF52840 Plus pads 28/29, removes unverified pads 30/31, preserves the project-specific front-side solder access pads 32/33, and leaves Ver001 unchanged as historical source.
