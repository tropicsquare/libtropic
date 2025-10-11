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
 * @brief Device structure for Arduino port.
 *
 * @note Initializing this structure is handled by the Tropic01.begin() method.
 */
typedef struct lt_dev_arduino_t {
    /** @public @brief SPI chip select pin. */
    uint16_t spi_cs_pin;
#ifdef LT_USE_INT_PIN
    /** @public @brief Pin to which TROPIC01's GPO or interrupt pin is connected to. */
    uint16_t int_gpo_pin;
#endif
    /** @public @brief SPI settings. */
    SPISettings spi_settings;
    /** @public @brief Seed for random number generator. */
    unsigned int rng_seed;

    /** @private @brief Pointer to the SPI class. */
    SPIClass *spi;
} lt_dev_arduino_t;

#endif  // LIBTROPIC_PORT_ARDUINO_H