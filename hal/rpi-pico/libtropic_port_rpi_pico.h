#ifndef LIBTROPIC_PORT_RPI_PICO_H
#define LIBTROPIC_PORT_RPI_PICO_H

/**
 * @file libtropic_port_rpi_pico.h
 * @author Wuard
 * @author Tropic Square s.r.o.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 * @brief Port for Raspberry Pi Pico and Pico 2 (RP2040 and RP2350) using native SPI (and GPIO for chip
 * select).
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 **/

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "libtropic_port.h"
#include "pico/stdlib.h"

/**
 * @brief Device structure for Raspberry Pi Pico port.
 *
 * The user fills in the parameters before passing the handle to libtropic.
 */
typedef struct lt_dev_rpi_pico_t {
    /** @brief @public SPI instance (e.g., spi0 or spi1). */
    spi_inst_t *spi_instance;
    /** @brief @public SPI CLK frequency (Hz). */
    uint spi_baudrate;
    /** @brief @public Pin used for chip select (GPIO). */
    uint cs_pin;
    /** @brief @public SPI MISO pin. */
    uint pin_miso;
    /** @brief @public SPI MOSI pin. */
    uint pin_mosi;
    /** @brief @public SPI CLK pin. */
    uint pin_sck;

#ifdef LT_USE_INT_PIN
    /** @brief @public GPIO pin connected to TROPIC01's interrupt pin. */
    uint int_gpio_pin;
#endif

    /** @brief @private Device structure initialization flag. */
    bool initialized;
} lt_dev_rpi_pico_t;

#endif  // LIBTROPIC_PORT_RPI_PICO_H
