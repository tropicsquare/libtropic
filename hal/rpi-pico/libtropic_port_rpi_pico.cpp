/**
 * @file lt_port_rpi_pico_.c
 * @author Wuard
 * @brief Port for Raspberry Pi Pico (RP2040) using native SPI (and GPIO for chip select).
**/

#include "libtropic_port_rpi_pico.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"

#include <string.h>
#include <stdint.h>

#include "pico/rand.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>



lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    // // lt_dev_pico *device = (lt_dev_pico *)(s2->device);
    // uint8_t *buff_ptr = (uint8_t *)buff;
    // size_t bytes_left = count;
    // uint32_t random_data;

    // while (bytes_left > 0) {
    //     uint32_t rnd = get_rand_32(); // pseudo-random del SDK
    //     size_t cpy_cnt = bytes_left < sizeof(random_data) ? bytes_left : sizeof(random_data);
    //     memcpy(buff_ptr, &random_data, cpy_cnt);
    //     buff_ptr += cpy_cnt;
    //     bytes_left -= cpy_cnt;
    // }

    // return LT_OK;

    LT_UNUSED(s2);

    uint8_t *out = (uint8_t *)buff;

    while (count > 0) {
        uint32_t r = get_rand_32();  // 32 bits de entropía real

        size_t chunk = (count >= 4) ? 4 : count;
        memcpy(out, &r, chunk);

        out   += chunk;
        count -= chunk;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);
    gpio_put(device->cs_pin, 0);
    while(gpio_get(device->cs_pin));
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);
    gpio_put(device->cs_pin, 1);
    while(!gpio_get(device->cs_pin));
    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    // Initialize SPI
    spi_init(device->spi_instance, device->spi_baudrate);

    gpio_set_function(device->pin_miso, GPIO_FUNC_SPI);
    gpio_set_function(device->pin_mosi, GPIO_FUNC_SPI);
    gpio_set_function(device->pin_sck,  GPIO_FUNC_SPI);

    gpio_set_function(device->cs_pin,   GPIO_FUNC_SIO);

    // CS as output
    gpio_init(device->cs_pin);
    gpio_set_dir(device->cs_pin, GPIO_OUT);
    gpio_put(device->cs_pin, 1);

#ifdef LT_USE_INT_PIN
    gpio_init(device->int_pin);
    gpio_set_dir(device->int_pin, GPIO_IN);
#endif

    device->initialized = true;
    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    spi_deinit(device->spi_instance);
    device->initialized = false;
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    if (offset + tx_data_length > TR01_L1_LEN_MAX) {
        LT_LOG_ERROR("Invalid data length!");
        return LT_L1_DATA_LEN_ERROR;
    }

    spi_write_read_blocking(device->spi_instance, s2->buff + offset, s2->buff + offset, tx_data_length);

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
    lt_dev_pico *device = (lt_dev_pico *)(s2->device);

    absolute_time_t start = get_absolute_time();
    while (gpio_get(device->int_pin) == 0) {
        if (absolute_time_diff_us(start, get_absolute_time()) / 1000 > ms) {
            return LT_L1_INT_TIMEOUT;
        }
        sleep_ms(1);
    }
    return LT_OK;
}
#endif

extern "C" int lt_port_log(const char *format, ...)
{
    static char log_buff[1024];
    va_list args;
    int ret;

    va_start(args, format);
    ret = vsnprintf(log_buff, sizeof(log_buff), format, args);
    va_end(args);

    if (ret > 0) {
        size_t len = strnlen(log_buff, sizeof(log_buff));
        for (size_t i = 0; i < len; ++i) {
            char c = log_buff[i];
            if (c == '\n') {
                Serial.println();
            } else {
                Serial.write(c);
            }
        }
        Serial.flush();
    }

    return ret;
}
