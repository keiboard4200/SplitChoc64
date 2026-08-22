# SplitChoc64 Ver012 — ZMK RC19

RC3 is the pre-hardware ZMK baseline generated from the Ver012 KiCad connectivity.

## Hardware mapping

- Controller: XIAO nRF52840 / XIAO BLE
- Current ZMK board target: `xiao_ble//zmk`
- LEFT: 29 keys, 5 rows × 6 populated columns
- RIGHT: 35 keys, 5 rows × 8 populated columns
- Diode direction: `col2row`
- LEFT: split central
- RIGHT: split peripheral
- RIGHT matrix transform column offset: 6
- J3/J4 five signal GPIOs are intentionally unused and reserved for future pointing-device work

## Keymap

The 64 positions follow the supplied KLE order exactly.

Both `Fn` keys use `&mo 1`. Layer 1 remains transparent in RC3 so no unrequested Fn layout is invented before hardware bring-up.

## Current ZMK repository structure

RC3 follows the current ZMK custom-shield/module structure:

- `zephyr/module.yml`
- `boards/shields/splitchoc64/`
- `config/west.yml`
- `build.yaml`
- `.github/workflows/build.yml`

The GitHub build workflow detects `zephyr/module.yml` and loads this repository as an extra ZMK module, making the top-level shield directory visible to Zephyr.

## Build targets

`build.yaml` requests:

1. `xiao_ble//zmk` + `splitchoc64_left`
2. `xiao_ble//zmk` + `splitchoc64_right`
3. `xiao_ble//zmk` + `settings_reset`

## Static check

Run:

```bash
python tools/validate_rc3.py
```

The included static validator checks the 64-position transform, two 64-binding layers, matrix GPIO counts, COL2ROW, right-side offset, module marker, build targets, and current Menu keycode.

## Deep sleep

RC3 deliberately does **not** force `CONFIG_ZMK_SLEEP=y` while developing against ZMK `main`.
Deep sleep can be enabled after the base split firmware is proven on hardware and the chosen ZMK revision is pinned.

## Not yet physically testable

- manufactured PCB matrix scanning
- BLE split radio operation
- battery and charging behavior
- future J3/J4 pointing-device interface


## RC4 fix

RC4 adds the missing Zephyr module board root declaration:

```yaml
name: zmk-keyboard-splitchoc64
build:
  settings:
    board_root: .
```

Without `board_root: .`, Zephyr loads the module itself but does not search this
repository's `boards/shields/` directory, causing `No shield named
'splitchoc64_left' found`.


## RC5 — 2765
Adds RIGHT-side 2765 X/Y via nRF52840 SAADC and split analog-stick support. Trackball work is deferred.

## RC6 fix

GitHub Actions RC5 reached the custom shield and failed while parsing the
`zmk,analog-stick-split` proxy's `reg = <0>` property. RC6 explicitly sets the
root addressing for these virtual split-device nodes to one address cell and
zero size cells, matching the analog-stick driver's split example.


## RC7 fix
The split proxy node is now `analog_stick_split@0`, matching `reg = <0>` as required by Zephyr 4.1 dtc.

## RC8 — ZMK pointing API compatibility

RC7 reached C compilation of `zmk-driver-analog-stick`. Current ZMK main uses
`zmk_endpoint_send_mouse_report()` while the driver still calls the older
`zmk_endpoints_send_mouse_report()` name. RC8 registers this repository as a
CMake module and supplies a compile-time compatibility alias, leaving the
third-party driver source untouched.

## RC10 — remove obsolete global input macro shim

The upstream analog-stick driver now includes its own Zephyr 4.x
`INPUT_CALLBACK_DEFINE()` compatibility logic. RC9's forced-include macro shim
therefore became both unnecessary and harmful: because it was global, even the
`settings_reset` target failed.

RC10 removes that global shim and keeps only the endpoint helper-name
compatibility alias.

## RC11 — local Zephyr 4.1 driver patch

RC10 proved LEFT and settings-reset build successfully. RIGHT fails only because
the third-party driver selects the legacy two-argument `INPUT_CALLBACK_DEFINE`
branch on Zephyr 4.1. RC11 patches only that compatibility block during CMake
configure and does not redefine the macro globally.

The analog-stick split proxy is also placed in a dedicated address container so
root `/soc` addressing remains untouched.

## RC12 — ADC address-cell correction

RC11 accidentally removed the ADC controller's own child-address declaration
while leaving an address declaration at the root. That made `channel@0` and
`channel@1` inherit the SoC's 2-address + 1-size-cell format.

RC12 restores the Zephyr ADC layout:

```dts
&adc {
    #address-cells = <1>;
    #size-cells = <0>;
    ...
};
```

and removes the root override. The RC11 local analog-stick driver patch remains.

## RC13 — native ZMK input-split architecture

The previous analog-stick module linked mouse-HID functions into the RIGHT split
peripheral, where those host-facing symbols do not exist. RC13 changes architecture:
RIGHT only produces standard relative input events and forwards them through ZMK's
native `zmk,input-split`; LEFT alone owns the native `zmk,input-listener` and HID path.

## RC14 — ZMK Studio

RC14 keeps the RC13 JOY2765 implementation unchanged and adds ZMK Studio support
to the LEFT/central firmware. Use Fn+Menu to unlock Studio. Two additional
reserved layers are included for future Studio use.

The `studio` metadata feature flag is intentionally not added yet; it should be
added after the Studio-enabled GitHub Actions build is confirmed successful.

## RC15 — JOY ON/OFF

The 2765 is ON by default. `Fn + Grave` toggles the transparent `JOY OFF`
layer (layer 2). While layer 2 is active, the LEFT central input listener runs
a local discard processor that stops joystick X/Y events before HID output.
`Fn + Menu` remains the ZMK Studio unlock shortcut.

## RC16 — JOY toggle binding discovery fix

RC15 failed during devicetree processing because the custom
`zmk,input-processor-discard` binding was not discovered. RC16 places that
binding under `dts/bindings/input/`, where Zephyr's module binding discovery
searches it. No RC14 Studio or RC13 joystick signal-path logic was otherwise
changed.

## RC17 — actual Devicetree binding discovery fix

RC15 and RC16 repeatedly failed with:

`joy_discard ... lacks binding`

The binding YAML itself existed, but this repository's `zephyr/module.yml`
declared only `board_root`. It never declared `dts_root`, so Zephyr did not
search this module's `dts/bindings/` tree at all.

RC17 adds:

```yaml
build:
  cmake: .
  settings:
    board_root: .
    dts_root: .
```

The custom binding is also placed under `dts/bindings/input_processors/` and
includes `base.yaml`, matching normal ZMK module binding conventions.

No JOY event-path logic, Studio configuration, RIGHT ADC configuration, or
Fn+Grave toggle behavior was changed.

## RC19 — input processor remainder property fix

RC18 progressed through Devicetree generation and compiled the custom
`input_processor_discard.c`. The next failure was in ZMK's own
`pointing/input_listener.c`, which reads a `track-remainders` property from
every input processor.

RC19 declares that optional boolean property in the custom discard processor
binding. The JOY discard processor leaves it unset, so it evaluates false.
JOY routing, Studio, ADC configuration, and Fn+Grave toggle behavior are unchanged.
