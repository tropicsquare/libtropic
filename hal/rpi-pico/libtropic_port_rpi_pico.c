/**
 * @file libtropic_port_rpi_pico.c
 * @author Wuard
 * @author Tropic Square s.r.o.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 * @brief Port for Raspberry Pi Pico and Pico 2 (RP2040 and RP2350) using native SPI (and GPIO for chip
 * select).
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 **/

#include "libtropic_port_rpi_pico.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "pico/rand.h"
#include "pico/stdlib.h"

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    LT_UNUSED(s2);

    uint8_t *out = (uint8_t *)buff;

    while (count > 0) {
        uint32_t r = get_rand_32();  // 32 bits of real entropy

        size_t chunk = (count >= 4) ? 4 : count;
        memcpy(out, &r, chunk);

        out += chunk;
        count -= chunk;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_rpi_pico_t *device = (lt_dev_rpi_pico_t *)(s2->device);
    gpio_put(device->cs_pin, 0);
    while (gpio_get(device->cs_pin));
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_rpi_pico_t *device = (lt_dev_rpi_pico_t *)(s2->device);
    gpio_put(device->cs_pin, 1);
    while (!gpio_get(device->cs_pin));
    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_rpi_pico_t *device = (lt_dev_rpi_pico_t *)(s2->device);

    device->initialized = false;

    // Initialize SPI
    // return the actual baudrate set by the hardware, which may be different from the requested one
    uint32_t baudrate = spi_init(device->spi_instance, device->spi_baudrate);

    if (baudrate != device->spi_baudrate) {
        LT_LOG_ERROR("Requested SPI baudrate %u Hz, but got %u Hz", device->spi_baudrate, baudrate);
        return LT_HAL_ERROR;
    }

    gpio_set_function(device->pin_miso, GPIO_FUNC_SPI);
    gpio_set_function(device->pin_mosi, GPIO_FUNC_SPI);
    gpio_set_function(device->pin_sck, GPIO_FUNC_SPI);

    // CS as output
    gpio_init(device->cs_pin);
    gpio_set_dir(device->cs_pin, GPIO_OUT);
    gpio_put(device->cs_pin, 1);

#ifdef LT_USE_INT_PIN
    gpio_init(device->int_gpio_pin);
    gpio_set_dir(device->int_gpio_pin, GPIO_IN);
#endif

    device->initialized = true;
    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_rpi_pico_t *device = (lt_dev_rpi_pico_t *)(s2->device);

    spi_deinit(device->spi_instance);
    device->initialized = false;
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length,
                              uint32_t timeout_ms)
{
    lt_dev_rpi_pico_t *device = (lt_dev_rpi_pico_t *)(s2->device);

    if (offset + tx_data_length > TR01_L1_LEN_MAX) {
        LT_LOG_ERROR("Invalid data length!");
        return LT_L1_DATA_LEN_ERROR;
    }

    // returns the number of bytes transferred, which should be equal to tx_data_length
    uint16_t dataLen = spi_write_read_blocking(device->spi_instance, s2->buff + offset,
                                               s2->buff + offset, tx_data_length);

    if (dataLen != tx_data_length) {
        LT_LOG_ERROR("SPI transfer failed! Expected to transfer %u bytes, but transferred %u bytes",
                     tx_data_length, dataLen);
        return LT_HAL_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    (void)s2;
    sleep_ms(ms);
    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_rpi_pico_t *device = (lt_dev_rpi_pico_t *)(s2->device);

    absolute_time_t start = get_absolute_time();
    while (gpio_get(device->int_gpio_pin) == 0) {
        if (absolute_time_diff_us(start, get_absolute_time()) / 1000 > ms) {
            return LT_L1_INT_TIMEOUT;
        }
        sleep_ms(1);
    }
    return LT_OK;
}
#endif

int lt_port_log(const char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = vfprintf(stderr, format, args);
    fflush(stderr);
    va_end(args);

    // Force the Pico HW to send USB packets now.
    stdio_flush();

    return ret;
}
