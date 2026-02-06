#ifndef LT_PORT_RPI_PICO_H
#define LT_PORT_RPI_PICO_H

/**
 * @file lt_port_rpi_pico_.c
 * @author Wuard
 * @brief Port for Raspberry Pi Pico (RP2040) using native SPI (and GPIO for chip select).
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
typedef struct lt_dev_pico {
    /** @brief @public SPI instance (e.g., spi0 or spi1). */
    spi_inst_t *spi_instance;

    /** @brief @public SPI frequency in Hz (e.g., 1 MHz, 4 MHz, etc.). */
    uint32_t spi_baudrate;

    /** @brief @public Pin used for chip select (GPIO). */
    uint cs_pin;

    /** @brief @public SPI pins */
    uint pin_miso;
    uint pin_mosi;
    uint pin_sck;

#ifdef LT_USE_INT_PIN
    /** @brief @public Pin used for interrupts (optional). */
    uint16_t int_gpio_pin;
#endif

    /** @brief @private Initialization flag */
    bool initialized;
} lt_dev_pico;

#endif  // LT_PORT_PICO_H
