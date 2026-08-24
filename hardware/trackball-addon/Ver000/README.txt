SplitChoc64 Trackball Add-on Ver003

Purpose:
  Manufacturing package for the SplitChoc64 Ver012 trackball add-on.
  The original Ver012 keyboard PCB geometry is not modified by this package.

Folder layout:
  1_FFC_ADAPTER/   KiCad 9 data for the J4 1.27 mm -> 6P 0.5 mm FFC adapter
  2_3D_PRINT/      3D manufacturing data separated by printable part
    1_TORABO_TRACKBALL_HOLDER/  Torabo-tsuki 19 mm trackball holder
    2_ADAPTER/                  SplitChoc64 external trackball adapter
    3_BOTTOM_CASE/              RIGHT bottom case with trackball interface / FFC pass-through
    4_REFERENCE_ASSEMBLY/       Reference-only assembled placement model; do not manufacture as one part
  3_DOCUMENTS/     Pin map, FFC routing and assembly notes
  VALIDATION_STAGE/ hashes and packaging notes

Important:
  The three manufacturing STEP files under folders 1-3 are each a single valid solid.
  The reference assembly is for placement/fit visualization only.
