/**
 * @file libtropic_port_arduino.c
 * @author Tropic Square s.r.o.
 * @brief Port for the Arduino framework.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <Arduino.h>
#include <SPI.h>

#include "libtropic_port_arduino.h"
#include "libtropic_port.h"

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

    Serial.println("Initializing SPI...");
    pinMode(device->spi_cs_pin, OUTPUT);
    digitalWrite(device->spi_cs_pin, HIGH);
    device->spi->begin();

    Serial.println("Initializing RNG...\n");
    randomSeed(device->rng_seed);

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
    LT_UNUSED(ms);
    lt_dev_arduino_t *device = (lt_dev_arduino_t *)(s2->device);

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