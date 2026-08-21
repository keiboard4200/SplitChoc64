# ZMK Studio — RC14

RC14 enables ZMK Studio on the LEFT/central half only.

## Build settings

LEFT uses:

- snippet: `studio-rpc-usb-uart`
- CMake: `-DCONFIG_ZMK_STUDIO=y`

RIGHT remains a normal split peripheral build.

## Unlock

Hold either physical Fn key and press Menu.

That sends:

`&studio_unlock`

The normal Menu key remains unchanged on the Base layer.

## Layers

- Base
- Fn
- Extra 1 (reserved for Studio)
- Extra 2 (reserved for Studio)

The reserved layers are exposed to Studio-enabled builds and can be enabled/renamed there.

## Physical layout

The existing SplitChoc64 Ver012 physical layout contains 64 `keys` entries and
uses the user's KLE widths/positions, so Studio can render the actual stagger and
wide keycaps rather than a generic 1U grid.

## Important

After ZMK Studio saves a runtime keymap, later edits to the stock `.keymap` file
will not automatically replace that stored Studio keymap. Use "Restore Stock
Settings" in ZMK Studio when you intentionally want to return to the firmware
keymap.
