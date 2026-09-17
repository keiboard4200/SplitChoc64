/*
 * SplitChoc64 local emergency bootloader chord.
 *
 * This path deliberately does not depend on the split transport, host output,
 * or the ZMK keymap.  Holding the local Ctrl + local Space for 20 seconds
 * reboots only that physical half into its UF2 bootloader.
 *
 * LEFT : LCTRL (position 53) + LEFT SPACE  (position 57)
 * RIGHT: RCTRL (position 63) + RIGHT SPACE (position 58)
 */

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>

#include <dt-bindings/zmk/reset.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

#if IS_ENABLED(CONFIG_RETENTION_BOOT_MODE)
#include <zephyr/retention/bootmode.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define EMERGENCY_BOOT_HOLD_MS 20000

#if IS_ENABLED(CONFIG_SHIELD_SPLITCHOC64_LEFT)
#define EMERGENCY_CTRL_POS 53
#define EMERGENCY_SPACE_POS 57
#define EMERGENCY_SIDE_NAME "LEFT"
#elif IS_ENABLED(CONFIG_SHIELD_SPLITCHOC64_RIGHT)
#define EMERGENCY_CTRL_POS 63
#define EMERGENCY_SPACE_POS 58
#define EMERGENCY_SIDE_NAME "RIGHT"
#else
#error "emergency_bootloader.c requires a SplitChoc64 LEFT or RIGHT shield"
#endif

static bool ctrl_pressed;
static bool space_pressed;
static struct k_work_delayable emergency_boot_work;

static void cancel_emergency_boot(void) {
    k_work_cancel_delayable(&emergency_boot_work);
}

static void emergency_boot_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    /* Re-check the chord at expiry so a release always cancels the request. */
    if (!ctrl_pressed || !space_pressed) {
        return;
    }

    LOG_WRN("%s emergency bootloader chord held for 20 seconds", EMERGENCY_SIDE_NAME);

#if IS_ENABLED(CONFIG_RETENTION_BOOT_MODE)
    int ret = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
    if (ret < 0) {
        LOG_ERR("%s emergency bootloader: bootmode_set failed (%d)", EMERGENCY_SIDE_NAME,
                ret);
        return;
    }
    sys_reboot(SYS_REBOOT_WARM);
#else
    /* Adafruit nRF52 UF2 bootloader request used by ZMK's &bootloader behavior. */
    sys_reboot(RST_UF2);
#endif
}

static int emergency_bootloader_position_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    bool relevant = false;

    if (ev->position == EMERGENCY_CTRL_POS) {
        ctrl_pressed = ev->state;
        relevant = true;
    } else if (ev->position == EMERGENCY_SPACE_POS) {
        space_pressed = ev->state;
        relevant = true;
    }

    if (!relevant) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ctrl_pressed && space_pressed) {
        /* Rescheduling is harmless if key bounce/repeated state events occur. */
        k_work_reschedule(&emergency_boot_work, K_MSEC(EMERGENCY_BOOT_HOLD_MS));
    } else {
        cancel_emergency_boot();
    }

    /* Never consume the normal Ctrl/Space events. */
    return ZMK_EV_EVENT_BUBBLE;
}

static int emergency_bootloader_init(void) {
    ctrl_pressed = false;
    space_pressed = false;
    k_work_init_delayable(&emergency_boot_work, emergency_boot_work_handler);
    return 0;
}

SYS_INIT(emergency_bootloader_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

ZMK_LISTENER(splitchoc64_emergency_bootloader, emergency_bootloader_position_listener);
ZMK_SUBSCRIPTION(splitchoc64_emergency_bootloader, zmk_position_state_changed);
