#!/usr/bin/env python3
from pathlib import Path
import re, sys, yaml

ROOT = Path(__file__).resolve().parents[1]
SHIELD = ROOT / "boards/shields/splitchoc64"

errors = []

required = [
    ROOT/"zephyr/module.yml",
    ROOT/"config/west.yml",
    ROOT/"build.yaml",
    SHIELD/"Kconfig.shield",
    SHIELD/"Kconfig.defconfig",
    SHIELD/"splitchoc64.dtsi",
    SHIELD/"splitchoc64_left.overlay",
    SHIELD/"splitchoc64_right.overlay",
    SHIELD/"splitchoc64.keymap",
    SHIELD/"splitchoc64-layouts.dtsi",
    SHIELD/"splitchoc64.zmk.yml",
]
for p in required:
    if not p.exists():
        errors.append(f"missing: {p.relative_to(ROOT)}")

# YAML parse
for p in [ROOT/"zephyr/module.yml", ROOT/"config/west.yml", ROOT/"build.yaml", SHIELD/"splitchoc64.zmk.yml"]:
    try:
        yaml.safe_load(p.read_text())
    except Exception as e:
        errors.append(f"YAML parse failed {p.relative_to(ROOT)}: {e}")

dtsi = (SHIELD/"splitchoc64.dtsi").read_text()
keymap = (SHIELD/"splitchoc64.keymap").read_text()
left = (SHIELD/"splitchoc64_left.overlay").read_text()
right = (SHIELD/"splitchoc64_right.overlay").read_text()
build = (ROOT/"build.yaml").read_text()

rcs = re.findall(r"RC\((\d+),(\d+)\)", dtsi)
if len(rcs) != 64:
    errors.append(f"transform positions != 64 ({len(rcs)})")
if len(set(rcs)) != 64:
    errors.append("duplicate RC() position in transform")

# Count behavior bindings only inside layer bindings blocks.
blocks = re.findall(r"bindings\s*=\s*<(.*?)>;", keymap, re.S)
if len(blocks) != 2:
    errors.append(f"expected 2 keymap layers, found {len(blocks)}")
else:
    for i,b in enumerate(blocks):
        cnt = len(re.findall(r"&(?:kp|mo|trans)\b", b))
        if cnt != 64:
            errors.append(f"layer {i} bindings != 64 ({cnt})")

if "K_APP" in keymap:
    errors.append("obsolete/undefined K_APP remains; use K_MENU")
if "&kp K_MENU" not in keymap:
    errors.append("Menu key missing")

if 'diode-direction = "col2row";' not in dtsi:
    errors.append("COL2ROW missing")
if "col-offset = <6>;" not in right:
    errors.append("right col-offset 6 missing")

# GPIO counts
def count_gpio_list(text, prop):
    m = re.search(rf"{prop}\s*(.*?)\s*;", text, re.S)
    if not m:
        return 0
    return len(re.findall(r"<&gpio[01]\s+\d+", m.group(1)))

if count_gpio_list(dtsi, "row-gpios") != 5:
    errors.append("shared row-gpios count != 5")
if count_gpio_list(left, "col-gpios") != 6:
    errors.append("left col-gpios count != 6")
if count_gpio_list(right, "col-gpios") != 8:
    errors.append("right col-gpios count != 8")

if build.count("xiao_ble//zmk") != 3:
    errors.append("build.yaml should contain 3 xiao_ble//zmk builds")
if "splitchoc64_left" not in build or "splitchoc64_right" not in build or "settings_reset" not in build:
    errors.append("build targets incomplete")

module = yaml.safe_load((ROOT/"zephyr/module.yml").read_text())
if module.get("name") != "zmk-keyboard-splitchoc64":
    errors.append("module name mismatch")

if errors:
    print("RC3 STATIC VALIDATION: FAIL")
    for e in errors:
        print(" -", e)
    sys.exit(1)

print("RC3 STATIC VALIDATION: PASS")
print(" - Zephyr module marker: OK")
print(" - Transform: 64 unique positions")
print(" - Keymap: 2 x 64 bindings")
print(" - Matrix GPIO counts: rows=5, left cols=6, right cols=8")
print(" - Right split col-offset: 6")
print(" - COL2ROW: OK")
print(" - Build targets: left/right/settings_reset")
print(" - Menu keycode: K_MENU")
