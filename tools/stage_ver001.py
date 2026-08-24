#!/usr/bin/env python3
from pathlib import Path
import re, shutil

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT / "hardware" / "main-pcb" / "Ver000"
DST = ROOT / "DEVELOPMENT" / "Ver001"

LEFT_OLD = [
    (136.050,20.25),(136.050,39.30),(145.575,39.30),(145.575,58.35),
    (150.337,58.35),(150.337,77.40),(159.863,77.40),(159.863,97.95),
    (150.338,97.95),(150.338,117.00),
]
LEFT_NEW = [
    (133.000125,20.25),(133.000125,41.35),(142.525125,41.35),(142.525125,60.40),
    (147.287125,60.40),(147.287125,79.45),(156.812625,79.45),(156.812625,95.90),
    (147.288000,95.90),(147.288000,117.00),
]
RIGHT_OLD = [
    (152.863,20.25),(152.863,39.30),(162.388,39.30),(162.388,58.35),
    (167.150,58.35),(167.150,77.40),(176.675,77.40),(176.675,97.95),
    (167.151,97.95),(167.151,117.00),
]
RIGHT_NEW = [
    (152.912875,20.25),(152.912875,38.75),(162.437875,38.75),(162.437875,57.80),
    (167.199875,57.80),(167.199875,76.85),(176.725375,76.85),(176.725375,98.50),
    (167.200750,98.50),(167.200750,117.00),
]

POINT_RE = re.compile(r'\((start|end)\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)\)')
WIDTH_RE = re.compile(r'\(width\s+([0-9.]+)\)')
UUID_RE = re.compile(r'\(uuid\s+"([^"]+)"\)')

def iter_blocks(text, head):
    needle = "(" + head
    i = 0
    while True:
        s = text.find(needle, i)
        if s < 0:
            return
        depth = 0
        ins = False
        esc = False
        for j in range(s, len(text)):
            c = text[j]
            if ins:
                if esc:
                    esc = False
                elif c == "\\":
                    esc = True
                elif c == '"':
                    ins = False
            else:
                if c == '"':
                    ins = True
                elif c == "(":
                    depth += 1
                elif c == ")":
                    depth -= 1
                    if depth == 0:
                        yield s, j + 1, text[s:j + 1]
                        i = j + 1
                        break
        else:
            raise RuntimeError(f"unterminated {head} block")

def fmt(v):
    return f"{v:.6f}".rstrip("0").rstrip(".")

def key(x, y):
    return (round(float(x), 6), round(float(y), 6))

def transform_edgecuts(path, old_pts, new_pts):
    text = path.read_text(encoding="utf-8")
    mp = {key(*a): b for a, b in zip(old_pts, new_pts)}
    chunks=[]; pos=0; touched=0
    for s,e,b in iter_blocks(text,"gr_line"):
        chunks.append(text[pos:s]); nb=b
        if '(layer "Edge.Cuts")' in b:
            pts=[key(x,y) for _,x,y in POINT_RE.findall(b)]
            if any(p in mp for p in pts):
                def repl(m):
                    nonlocal touched
                    p=key(m.group(2),m.group(3))
                    if p not in mp:
                        return m.group(0)
                    nx,ny=mp[p]; touched+=1
                    return f"({m.group(1)} {fmt(nx)} {fmt(ny)})"
                nb=POINT_RE.sub(repl,nb)
                if WIDTH_RE.search(nb):
                    nb=WIDTH_RE.sub("(width 0.15)",nb,1)
        chunks.append(nb); pos=e
    chunks.append(text[pos:])
    if touched != 20:
        raise RuntimeError(f"{path.name}: expected 20 seam endpoint replacements, got {touched}")
    path.write_text("".join(chunks),encoding="utf-8",newline="")

def move_left_h4(path):
    text=path.read_text(encoding="utf-8")
    chunks=[]; pos=0; changed=0
    for s,e,b in iter_blocks(text,"footprint"):
        chunks.append(text[pos:s]); nb=b
        if '(property "Reference" "H4"' in b:
            m=re.search(r'\(at\s+(-?\d+(?:\.\d+)?)\s+(-?\d+(?:\.\d+)?)(\s+-?\d+(?:\.\d+)?)?\)',b)
            if not m: raise RuntimeError("H4 at missing")
            x,y=float(m.group(1)),float(m.group(2))
            if abs(x-146)>1e-6 or abs(y-113.25)>1e-6:
                raise RuntimeError(f"unexpected H4 pos {x},{y}")
            rot=m.group(3) or ""
            nb=b[:m.start()]+f"(at 143 113.25{rot})"+b[m.end():]
            changed+=1
        chunks.append(nb); pos=e
    chunks.append(text[pos:])
    if changed!=1: raise RuntimeError(f"H4 count={changed}")
    path.write_text("".join(chunks),encoding="utf-8",newline="")

def segment_uuid(block):
    m=UUID_RE.search(block)
    return m.group(1) if m else None

def remove_segment_uuids(text, uuids):
    uuids=set(uuids); chunks=[]; pos=0; removed=set()
    for s,e,b in iter_blocks(text,"segment"):
        chunks.append(text[pos:s])
        u=segment_uuid(b)
        if u in uuids:
            removed.add(u)
        else:
            chunks.append(b)
        pos=e
    chunks.append(text[pos:])
    missing=uuids-removed
    if missing: raise RuntimeError(f"segments not found: {sorted(missing)}")
    return "".join(chunks)

def replace_segment_end(text, uuid_value, new_end):
    chunks=[]; pos=0; count=0
    for s,e,b in iter_blocks(text,"segment"):
        chunks.append(text[pos:s]); nb=b
        if segment_uuid(b)==uuid_value:
            nb=re.sub(r'\(end\s+[-\d.]+\s+[-\d.]+\)',f"(end {fmt(new_end[0])} {fmt(new_end[1])})",b,count=1)
            count+=1
        chunks.append(nb); pos=e
    chunks.append(text[pos:])
    if count!=1: raise RuntimeError(f"segment {uuid_value} count={count}")
    return "".join(chunks)

def seg_block(start,end,net,uuid_value,layer="B.Cu",width=0.15):
    return (
        "\t(segment\n"
        f"\t\t(start {fmt(start[0])} {fmt(start[1])})\n"
        f"\t\t(end {fmt(end[0])} {fmt(end[1])})\n"
        f"\t\t(width {fmt(width)})\n"
        f"\t\t(layer \"{layer}\")\n"
        f"\t\t(net {net})\n"
        f"\t\t(uuid \"{uuid_value}\")\n"
        "\t)\n"
    )

def append_segments(text, segments):
    idx=text.find("\n\t(zone")
    if idx<0:
        idx=text.rfind("\n)")
    payload="".join(seg_block(**s) for s in segments)
    return text[:idx]+"\n"+payload+text[idx:]

def apply_user_routing_left(path):
    text=path.read_text(encoding="utf-8")
    text=replace_segment_end(text,"1b469062-d246-4d57-aab9-db95bf063345",(146.2,95.7166))
    text=remove_segment_uuids(text,["7f483828-4066-4e04-b41e-15ac2443fc4a"])
    adds=[
        dict(start=(150.513,90.8),end=(153.288,93.575),net=4,uuid_value="0bdb7188-89b0-4d9a-a95c-1d26e7201870"),
        dict(start=(146.2,93.2),end=(148.6,90.8),net=4,uuid_value="557e5c43-76e3-4aa8-93da-a8fa86c15ae2"),
        dict(start=(146.2,95.7166),end=(146.2,93.2),net=4,uuid_value="9cd6f001-2063-4f8c-a23d-0a719b82b36e"),
        dict(start=(148.6,90.8),end=(150.513,90.8),net=4,uuid_value="c953d030-eb6f-4dcd-a28b-58af3a655595"),
    ]
    text=append_segments(text,adds)
    path.write_text(text,encoding="utf-8",newline="")

def apply_user_routing_right(path):
    text=path.read_text(encoding="utf-8")
    remove=[
        "002a2b4c-d32b-45eb-8cd8-35d0b9c3fbd8","0e252a8a-f984-4f73-8b5d-e55165378e89",
        "389924e1-d2ca-45eb-9395-cf449c543ee1","3dd03ecc-b23b-4f8d-a747-832cde654970",
        "5fc46490-d79a-45fd-9cce-2766a4bf31ff","93f2fe04-80c8-4107-a257-7a8d1de0d9a7",
        "95705398-11c8-4569-b014-2573d9565d23","a1d76b58-7705-448d-a862-25737887a806",
        "ac1868b7-7ddb-4541-a7b3-adc78461e9a9","b8612307-3790-41c2-af54-089a70a201d9",
        "e9a1f94e-4715-4245-bb27-41bb463cec14","f6ccd880-6d51-407d-a1c0-5a2362d32a3b",
    ]
    text=remove_segment_uuids(text,remove)
    adds=[
        dict(start=(166.913,42.905513),end=(166.913,46.375),net=6,uuid_value="003c6cb5-a0e6-4f4a-9612-35ed47ab227f"),
        dict(start=(176.1336,76.4664),end=(176.661387,76.4664),net=6,uuid_value="1896f13a-644a-452b-a8de-996b6a9131f0"),
        dict(start=(161.7401,38.2599),end=(162.267387,38.2599),net=6,uuid_value="1a24506c-b81c-40b8-a54c-7382fb6a2a09"),
        dict(start=(160.0632,32.8234),end=(161.1233,33.8835),net=6,uuid_value="2efbd360-c26e-4780-9cb3-cd2681454f79"),
        dict(start=(175.4103,75.7431),end=(176.1336,76.4664),net=6,uuid_value="3172d534-693c-4f78-8e4e-dcc246d70a71"),
        dict(start=(161.1233,33.8835),end=(161.1233,37.6431),net=6,uuid_value="387c3e14-0446-4aa6-bebb-f16ba2651f1a"),
        dict(start=(162.267387,38.2599),end=(166.913,42.905513),net=6,uuid_value="7994827d-5c71-4234-a31b-193ca1e01738"),
        dict(start=(160.0632,30.0002),end=(160.0632,32.8234),net=6,uuid_value="7d759aaa-285c-416e-971d-507a7fbf5c28"),
        dict(start=(174.3502,68.1002),end=(174.3502,70.9234),net=6,uuid_value="8bab4770-7b23-4e57-b600-06ef6d47bf97"),
        dict(start=(157.388,27.325),end=(160.0632,30.0002),net=6,uuid_value="ab99d62e-7e6e-4d58-a8d8-131dde4ee8db"),
        dict(start=(176.661387,76.4664),end=(181.2,81.005013),net=6,uuid_value="c295f4fa-e730-4857-9626-a4274932ffd9"),
        dict(start=(175.4103,71.9835),end=(175.4103,75.7431),net=6,uuid_value="cda42adb-5f8b-4fcb-a3ed-da53bb4677b4"),
        dict(start=(174.3502,70.9234),end=(175.4103,71.9835),net=6,uuid_value="ceca0d52-28ae-4e0c-a129-872cae441b69"),
        dict(start=(161.1233,37.6431),end=(161.7401,38.2599),net=6,uuid_value="e61fd499-fe84-4a5f-84b5-5fbd97e37fb9"),
        dict(start=(181.2,81.005013),end=(181.2,84.475),net=6,uuid_value="ed4c8915-d4ec-4854-97b1-68b4cd4f10c7"),
        dict(start=(171.675,65.425),end=(174.3502,68.1002),net=6,uuid_value="ee681cf7-9244-4b09-9743-3b712b193d22"),
    ]
    text=append_segments(text,adds)
    path.write_text(text,encoding="utf-8",newline="")

def clip_polygon_ymax(points, ymax):
    if not points: return []
    out=[]; prev=points[-1]; prev_in=prev[1] <= ymax + 1e-12
    for cur in points:
        cur_in=cur[1] <= ymax + 1e-12
        if cur_in != prev_in:
            dy=cur[1]-prev[1]
            if abs(dy)>1e-15:
                t=(ymax-prev[1])/dy
                out.append((prev[0]+t*(cur[0]-prev[0]),ymax))
        if cur_in: out.append(cur)
        prev,prev_in=cur,cur_in
    cleaned=[]
    for p in out:
        if not cleaned or abs(p[0]-cleaned[-1][0])>1e-9 or abs(p[1]-cleaned[-1][1])>1e-9:
            cleaned.append(p)
    if len(cleaned)>1 and abs(cleaned[0][0]-cleaned[-1][0])<1e-9 and abs(cleaned[0][1]-cleaned[-1][1])<1e-9:
        cleaned.pop()
    return cleaned

def rewrite_pts_block(poly_block,newpts):
    ps=poly_block.find("(pts")
    depth=0; pe=None
    for j in range(ps,len(poly_block)):
        if poly_block[j]=="(": depth+=1
        elif poly_block[j]==")":
            depth-=1
            if depth==0: pe=j+1; break
    rep="(pts\n\t\t\t\t"+" ".join(f"(xy {x:.6f} {y:.6f})" for x,y in newpts)+"\n\t\t\t)"
    return poly_block[:ps]+rep+poly_block[pe:]

def clip_switch_silk(path, refs, ymax=8.125):
    text=path.read_text(encoding="utf-8")
    chunks=[]; pos=0; stats={}
    for s,e,fb in iter_blocks(text,"footprint"):
        chunks.append(text[pos:s]); nfb=fb
        mref=re.search(r'\(property\s+"Reference"\s+"([^"]+)"',fb)
        ref=mref.group(1) if mref else None
        if ref in refs:
            pc=[]; pp=0; changed=0
            for ps,pe,pb in iter_blocks(fb,"fp_poly"):
                pc.append(fb[pp:ps]); npb=pb
                if '(layer "B.SilkS")' in pb:
                    pts=[(float(x),float(y)) for x,y in re.findall(r'\(xy\s+([-\d.]+)\s+([-\d.]+)\)',pb)]
                    if len(pts)>=3 and max(y for x,y in pts)>ymax+1e-12:
                        clipped=clip_polygon_ymax(pts,ymax)
                        if len(clipped)>=3:
                            npb=rewrite_pts_block(pb,clipped); changed+=1
                pc.append(npb); pp=pe
            pc.append(fb[pp:]); nfb="".join(pc); stats[ref]=changed
        chunks.append(nfb); pos=e
    chunks.append(text[pos:])
    missing=set(refs)-set(stats)
    if missing: raise RuntimeError(f"switch refs not found: {sorted(missing)}")
    path.write_text("".join(chunks),encoding="utf-8",newline="")

def move_k_texts(path, refs, dx=0.5):
    text=path.read_text(encoding="utf-8")
    chunks=[];pos=0;done={}
    for s,e,fb in iter_blocks(text,"footprint"):
        chunks.append(text[pos:s]);nfb=fb
        mref=re.search(r'\(property\s+"Reference"\s+"([^"]+)"',fb)
        ref=mref.group(1) if mref else None
        if ref in refs:
            pc=[];pp=0;count=0
            for ts,te,tb in iter_blocks(fb,"fp_text"):
                pc.append(fb[pp:ts]);ntb=tb
                if re.match(r'\(fp_text\s+user\s+"K"',tb) and '(layer "B.SilkS")' in tb:
                    m=re.search(r'\(at\s+([-\d.]+)\s+([-\d.]+)(?:\s+([-\d.]+))?\)',tb)
                    if m:
                        x=float(m.group(1));y=float(m.group(2));rot=m.group(3)
                        rep=f"(at {x+dx:.6f} {y:.6f}"+(f" {rot}" if rot else "")+")"
                        ntb=tb[:m.start()]+rep+tb[m.end():];count+=1
                pc.append(ntb);pp=te
            pc.append(fb[pp:]);nfb="".join(pc);done[ref]=count
        chunks.append(nfb);pos=e
    chunks.append(text[pos:])
    path.write_text("".join(chunks),encoding="utf-8",newline="")
    if set(done)!=set(refs): raise RuntimeError("missing diode refs")

def copy_optional(src,dst):
    if src.is_file():
        dst.parent.mkdir(parents=True,exist_ok=True); shutil.copy2(src,dst)
    elif src.is_dir():
        if dst.exists(): shutil.rmtree(dst)
        shutil.copytree(src,dst)

def main():
    if DST.exists(): shutil.rmtree(DST)
    (DST/"LEFT").mkdir(parents=True)
    (DST/"RIGHT").mkdir(parents=True)
    (DST/"0_COMMON").mkdir(parents=True)

    for side_dir,stem,dname in [
        ("2_LEFT","SplitChoc64_LEFT","LEFT"),
        ("1_RIGHT","SplitChoc64_RIGHT","RIGHT"),
    ]:
        src=BASE/side_dir; dst=DST/dname
        for ext in [".kicad_pcb",".kicad_pro",".kicad_sch",".kicad_prl"]:
            copy_optional(src/f"{stem}{ext}",dst/f"{stem}{ext}")
        for name in ["fp-lib-table","sym-lib-table","3dmodels"]:
            copy_optional(src/name,dst/name)
    copy_optional(BASE/"0_COMMON"/"3dmodels",DST/"0_COMMON"/"3dmodels")

    left=DST/"LEFT"/"SplitChoc64_LEFT.kicad_pcb"
    right=DST/"RIGHT"/"SplitChoc64_RIGHT.kicad_pcb"

    transform_edgecuts(left,LEFT_OLD,LEFT_NEW)
    transform_edgecuts(right,RIGHT_OLD,RIGHT_NEW)
    move_left_h4(left)
    apply_user_routing_left(left)
    apply_user_routing_right(right)
    clip_switch_silk(left,{"SWL24"},8.125)
    clip_switch_silk(right,{"SWR1","SWR17"},8.125)
    move_k_texts(right,{"DR1","DR9","DR17","DR24"},0.5)

    readme="""# SplitChoc64 DEVELOPMENT / Ver001

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
"""
    (DST/"README.md").write_text(readme,encoding="utf-8")

if __name__=="__main__":
    main()
