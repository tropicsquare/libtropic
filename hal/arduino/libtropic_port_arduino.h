#ifndef LIBTROPIC_PORT_ARDUINO_H
#define LIBTROPIC_PORT_ARDUINO_H

/**
 * @file libtropic_port_arduino.h
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Port for the Arduino framework.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
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
#if LT_USE_INT_PIN
    /** @public @brief Pin to which TROPIC01's GPO or interrupt pin is connected to. */
    uint16_t int_gpio_pin;
#endif
    /** @public @brief SPI settings. */
    SPISettings spi_settings;

    /** @private @brief Pointer to the SPI class. */
    SPIClass *spi;
} lt_dev_arduino_t;

#endif  // LIBTROPIC_PORT_ARDUINO_H