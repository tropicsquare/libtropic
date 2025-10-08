#ifndef LIBTROPIC_PORT_ARDUINO_H
#define LIBTROPIC_PORT_ARDUINO_H

/**
 * @file libtropic_port_arduino.h
 * @author Tropic Square s.r.o.
 * @brief Port for the Arduino framework.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <Arduino.h>
#include <SPI.h>

#include "libtropic_port.h"

/**
 * @brief TODO
 *
 */
typedef struct lt_dev_arduino_t {
    /** @public @brief TODO */
    uint16_t spi_cs_pin;
#ifdef LT_USE_INT_PIN
    /** @public @brief TODO */
    uint16_t int_gpio_pin;
#endif
    /** @public @brief TODO */
    SPISettings spi_settings;
    /** @public @brief TODO */
    unsigned int rng_seed;

    /** @private @brief TODO */
    SPIClass *spi;
} lt_dev_arduino_t;

#endif  // LIBTROPIC_PORT_ARDUINO_H