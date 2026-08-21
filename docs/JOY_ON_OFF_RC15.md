# JOY ON/OFF — RC15

The 2765 joystick remains ON by default.

## Toggle

Press:

**Fn + ` (grave)**

to toggle layer 2, `JOY OFF`.

- JOY OFF layer inactive: joystick events reach the host normally.
- JOY OFF layer active: a central-side input processor returns
  `ZMK_INPUT_PROC_STOP`, so X/Y events are discarded before HID output.

The JOY OFF layer is transparent for all 64 keys, so keyboard typing continues
normally while the joystick is disabled.

Press **Fn + `** again to turn the joystick back on.

## Why this is central-side

The RIGHT peripheral continues to sample the 2765 and forward input events using
the already build-proven RC13 architecture. ON/OFF is applied on LEFT central
before HID output. This keeps ADC/split transport unchanged.

## ZMK Studio

Layer 2 is visible as `JOY OFF`. Studio can edit the layer's key bindings, but
the joystick-disable behavior is tied to layer 2 itself.

Studio unlock remains **Fn + Menu**.
