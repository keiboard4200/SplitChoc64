RIGHT JLCPCB assembly files

Upload:
- JLCPCB_BOM_RIGHT.csv
- JLCPCB_CPL_RIGHT.csv

Do not upload KiCad's raw *-pos.csv directly to JLCPCB without converting headers:
Ref -> Designator
PosX -> Mid X
PosY -> Mid Y
Rot -> Rotation
Side -> Layer

The BOM uses explicit designator lists and excludes hand-solder/DNP parts.
