SplitChoc64 - JLCPCB PCBA export

UPLOAD FILES
  BOM : FABRICATION/JLCPCB_BOM.csv
  CPL : FABRICATION/JLCPCB_CPL.csv

BOM columns
  Comment, Designator, Footprint, LCSC Part #

CPL columns
  Designator, Mid X, Mid Y, Layer, Rotation

KiCad built-in placement export settings
  File -> Fabrication Outputs -> Component Placement (.pos)
  Format : CSV
  Units  : Millimetres
  Sides  : Single file for both sides
  Output : FABRICATION/

KiCad native CPL headers map as:
  Ref -> Designator
  PosX -> Mid X
  PosY -> Mid Y
  Rot -> Rotation
  Side -> Layer

JLCPCB assembly population:
  64 x 1N4148W          C81598
   2 x 100R 0603        C22775
   2 x PCM12SMTR        C221841
   2 x BM02B-SRSS-TB    C160388
   2 x KT-0603B LED     C2288

DNP / hand solder:
  U1/U2 XIAO nRF52840 Plus
  J3/J4 1.27 mm headers
  SWL1-SWL29 / SWR1-SWR35 Choc socket/switch

IMPORTANT:
  Always inspect JLCPCB's component matching and placement preview before ordering,
  especially polarity and rotation of diodes, LEDs, connectors and switches.
