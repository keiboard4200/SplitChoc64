# Hardware bring-up sequence

1. Flash `settings_reset` once to both XIAO modules if they have existing ZMK bonds/settings.
2. Flash `splitchoc64_left` to LEFT and connect LEFT by USB.
3. Verify LEFT's 29 switches.
4. Flash `splitchoc64_right` to RIGHT.
5. Verify BLE split and all 64 switches.
6. Verify USB and BLE host operation.
7. Only after the base keyboard is proven, add J3/J4 pointing-device firmware.

Do not troubleshoot the future pointing-device driver at the same time as the first matrix/split bring-up.
