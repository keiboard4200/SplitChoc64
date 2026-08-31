#!/usr/bin/env python3
from pathlib import Path
import re, sys, yaml

ROOT = Path(__file__).resolve().parents[1]
SHIELD = ROOT / "boards/shields/splitchoc64"
errors = []

required = [
    ROOT / "zephyr/module.yml",
    ROOT / "config/west.yml",
    ROOT / "build.yaml",
    SHIELD / "Kconfig.shield",
    SHIELD / "Kconfig.defconfig",
    SHIELD / "splitchoc64.dtsi",
    SHIELD / "splitchoc64_left.overlay",
    SHIELD / "splitchoc64_right.overlay",
    SHIELD / "splitchoc64.keymap",
    SHIELD / "splitchoc64-layouts.dtsi",
    SHIELD / "splitchoc64.zmk.yml",
]
for p in required:
    if not p.exists():
        errors.append(f"missing: {p.relative_to(ROOT)}")

for p in [ROOT / "zephyr/module.yml", ROOT / "config/west.yml", ROOT / "build.yaml", SHIELD / "splitchoc64.zmk.yml"]:
    try:
        yaml.safe_load(p.read_text())
    except Exception as e:
        errors.append(f"YAML parse failed {p.relative_to(ROOT)}: {e}")

dtsi = (SHIELD / "splitchoc64.dtsi").read_text()
keymap = (SHIELD / "splitchoc64.keymap").read_text()
left = (SHIELD / "splitchoc64_left.overlay").read_text()
right = (SHIELD / "splitchoc64_right.overlay").read_text()
kconfig = (SHIELD / "Kconfig.defconfig").read_text()
build = (ROOT / "build.yaml").read_text()

rcs = re.findall(r"RC\((\d+),(\d+)\)", dtsi)
if len(rcs) != 64 or len(set(rcs)) != 64:
    errors.append(f"matrix transform must contain 64 unique positions (found {len(rcs)})")

if 'diode-direction = "col2row";' not in dtsi:
    errors.append("COL2ROW missing")
if "col-offset = <6>;" not in right:
    errors.append("right col-offset 6 missing")

# Final split architecture: RIGHT central, LEFT peripheral.
if "if SHIELD_SPLITCHOC64_RIGHT" not in kconfig or "config ZMK_SPLIT_ROLE_CENTRAL" not in kconfig:
    errors.append("RIGHT central role is not defined")
if 'default "SplitChoc64"' not in kconfig:
    errors.append("Bluetooth/USB keyboard name must be SplitChoc64")
if "SHIELD_SPLITCHOC64_LEFT" in kconfig.split("config ZMK_SPLIT_ROLE_CENTRAL")[0]:
    errors.append("LEFT must not be configured as central")

# Studio and pointing belong to RIGHT/central.
if "splitchoc64_right" not in build or "studio-rpc-usb-uart" not in build or "CONFIG_ZMK_STUDIO=y" not in build:
    errors.append("RIGHT Studio build configuration missing")
if "CONFIG_ZMK_POINTING=y" not in (SHIELD / "splitchoc64_right.conf").read_text():
    errors.append("RIGHT pointing support missing")

if build.count("xiao_ble//zmk") != 3:
    errors.append("build.yaml should contain left, right, and settings_reset targets")
if not all(x in build for x in ["splitchoc64_left", "splitchoc64_right", "settings_reset"]):
    errors.append("build targets incomplete")

module = yaml.safe_load((ROOT / "zephyr/module.yml").read_text())
if module.get("name") != "zmk-keyboard-splitchoc64":
    errors.append("module name mismatch")
settings = module.get("build", {}).get("settings", {})
if settings.get("board_root") != ".":
    errors.append("board_root must be '.'")
if settings.get("dts_root") != ".":
    errors.append("dts_root must be '.'")

if errors:
    print("SPLITCHOC64 STATIC VALIDATION: FAIL")
    for e in errors:
        print(" -", e)
    sys.exit(1)

print("SPLITCHOC64 STATIC VALIDATION: PASS")
print(" - RIGHT = central / host-facing")
print(" - LEFT = peripheral")
print(" - Bluetooth/USB name = SplitChoc64")
print(" - RIGHT Studio + pointing configuration present")
print(" - Matrix transform = 64 unique positions")
print(" - Build targets = left/right/settings_reset")
print(" - Zephyr module board_root/dts_root = OK")
