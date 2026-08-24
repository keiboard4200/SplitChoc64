SplitChoc64 Ver009 — 4_PLATE JLC cutout correction

Reason for revision
- Uploaded RIGHT TOP Gerber was inspected.
- The 35 switch rectangles WERE present in Edge_Cuts.gbr.
- However both NPTH.drl and PTH.drl were empty, because the M2 holes had been drawn only
  as Edge.Cuts circles instead of real mechanical drill holes.
- This revision makes the plate fabrication intent unambiguous.

Changes in 3_PLATE
1. M2 mounting holes are now real 2.2 mm NPTH pads.
   -> KiCad Gerber/Drill export must produce actual coordinates in the NPTH drill file.
2. Every Choc V2 top-plate opening is now four explicit closed Edge.Cuts line segments.
   -> no gr_rect dependency; each 13.95 x 13.95 mm routed opening is an explicit closed contour.
3. No silkscreen.
4. No JOY/J3/J4 cutout.
5. Plate thickness remains 1.6 mm.
6. Main PCB/schematic files are unchanged from Ver008.

JLCPCB export/check
- Plot F.Cu, B.Cu, F.Mask, B.Mask and Edge.Cuts.
- Generate Drill Files with NPTH included.
- ZIP Gerbers + drill files together.
- Before ordering, open the ZIP in a Gerber viewer and verify:
  * TOP: all 29/35 square openings visible
  * TOP/BOTTOM: four 2.2 mm NPTH holes visible
- If JLC's quote preview still visually omits internal routed contours even though a Gerber
  viewer shows them, send the Board Outline/Edge_Cuts file to engineering and state:
  "Closed inner contours are routed board cutouts."

Plate-mount dimensions
- Choc V2 opening: 13.95 x 13.95 mm
- Plate thickness: 1.6 mm
- M2 NPTH: 2.2 mm
