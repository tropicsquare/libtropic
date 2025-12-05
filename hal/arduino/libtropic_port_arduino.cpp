/**
 * @file libtropic_port_arduino.cpp
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Port for the Arduino framework.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic_port_arduino.h"

#include <Arduino.h>
#include <SPI.h>
#include <stdarg.h>
#include <stdio.h>

#include "libtropic_port.h"

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

    // Setup SPI
    pinMode(device->spi_cs_pin, OUTPUT);
    digitalWrite(device->spi_cs_pin, HIGH);
    device->spi->begin();

    // Setup interrupt pin
#if LT_USE_INT_PIN
    pinMode(device->int_gpio_pin, INPUT);
#endif

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

    digitalWrite(device->spi_cs_pin, HIGH);
    device->spi->end();

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

    digitalWrite(device->spi_cs_pin, LOW);

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

    digitalWrite(device->spi_cs_pin, HIGH);

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout_ms)
{
    LT_UNUSED(timeout_ms);
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

    device->spi->beginTransaction(device->spi_settings);
    device->spi->transfer(s2->buff + offset, tx_len);
    device->spi->endTransaction();

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    LT_UNUSED(s2);

    delay(ms);

    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);
    unsigned long start_time = millis();
    unsigned long curr_time;

    while (!digitalRead(device->int_gpio_pin)) {
        curr_time = millis();
        if (curr_time - start_time > (unsigned long)ms) {
            return LT_L1_INT_TIMEOUT;
        }
    }

    return LT_OK;
}
#endif

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    LT_UNUSED(s2);

    for (size_t i = 0; i < count; i++) {
        ((uint8_t *)buff)[i] = random(0, 256);  // Generate a random byte (0-255)
    }

    return LT_OK;
}

void lt_port_log(const char *format, ...)
{
    // 1 KiB should be enough.
    // Using static to avoid stack overflow.
    static char log_buff[1024];
    va_list args;

    va_start(args, format);
    vsnprintf(log_buff, sizeof(log_buff), format, args);
    va_end(args);

    Serial.print(log_buff);
}