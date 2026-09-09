#define DT_DRV_COMPAT zmk_behavior_paw3222_cpi

#include <errno.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>

#include <paw3222.h>

#define PAW3222_CPI_X_REG 0x0d
#define PAW3222_CPI_STEP 38U
#define PAW3222_CPI_REG_MIN 16U
#define PAW3222_CPI_REG_MAX 127U

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#if DT_HAS_COMPAT_STATUS_OKAY(pixart_paw3222)
#define PAW3222_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pixart_paw3222)
#define PAW3222_SPI_MODE                                                                            \
    (SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_TRANSFER_MSB)

static const struct spi_dt_spec paw3222_spi = SPI_DT_SPEC_GET(PAW3222_NODE, PAW3222_SPI_MODE, 0);

static int read_cpi_register(uint8_t *value)
{
    uint8_t addr = PAW3222_CPI_X_REG;

    const struct spi_buf tx_buf = {
        .buf = &addr,
        .len = sizeof(addr),
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1,
    };

    struct spi_buf rx_bufs[] = {
        {
            .buf = NULL,
            .len = sizeof(addr),
        },
        {
            .buf = value,
            .len = 1,
        },
    };
    const struct spi_buf_set rx = {
        .buffers = rx_bufs,
        .count = ARRAY_SIZE(rx_bufs),
    };

    if (!spi_is_ready_dt(&paw3222_spi)) {
        return -ENODEV;
    }

    return spi_transceive_dt(&paw3222_spi, &tx, &rx);
}
#endif

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event)
{
    ARG_UNUSED(event);

#if DT_HAS_COMPAT_STATUS_OKAY(pixart_paw3222)
    uint8_t reg_value;
    int ret = read_cpi_register(&reg_value);
    if (ret < 0) {
        return ret;
    }

    if (binding->param1 == 0) {
        if (reg_value <= PAW3222_CPI_REG_MIN) {
            return ZMK_BEHAVIOR_OPAQUE;
        }
        reg_value--;
    } else {
        if (reg_value >= PAW3222_CPI_REG_MAX) {
            return ZMK_BEHAVIOR_OPAQUE;
        }
        reg_value++;
    }

    return paw32xx_set_resolution(DEVICE_DT_GET(PAW3222_NODE),
                                  (uint16_t)reg_value * PAW3222_CPI_STEP);
#else
    ARG_UNUSED(binding);
    return ZMK_BEHAVIOR_OPAQUE;
#endif
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event)
{
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api paw3222_cpi_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

#define PAW3222_CPI_BEHAVIOR_INST(n)                                                               \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &paw3222_cpi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PAW3222_CPI_BEHAVIOR_INST)

#endif
