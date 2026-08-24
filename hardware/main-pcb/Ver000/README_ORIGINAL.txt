SplitChoc64 Ver000 corrected split

0_COMMON: shared libraries and 3D assets
1_RIGHT : independent right KiCad project
2_LEFT  : independent left KiCad project

Corrections:
- RIGHT/LEFT schematics are rebuilt directly from the integrated schematic.
  Wires, labels, junctions and no-connect markers on each side are retained from the source.
- Each PCB has exactly one closed Edge.Cuts keyboard outline.
- Panel/mouse-bite outer-frame lines are excluded.

Run ERC and DRC on each side before fabrication.

Latest cleanup:
- Removed MB1/MB2 from LEFT and MB3/MB4 from RIGHT.
- Restored DLED2 to RIGHT.
- Fixed LEFT ultra-short Edge.Cuts segment.

3D model portability fix:
- Master STEP files remain in 0_COMMON.
- MCU / Choc socket / Choc switch STEP files are mirrored into each side project's 3dmodels/.
- PCB model references now use ${KIPRJMOD}/3dmodels/... so each side opens independently in KiCad 3D Viewer.
