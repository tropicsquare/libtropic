#ifndef LIBTROPIC_PORT_UNIX_SPI_H
#define LIBTROPIC_PORT_UNIX_SPI_H

/**
 * @file libtropic_port_unix_spi.h
 * @author Tropic Square s.r.o.
 * @brief Port for communication using Generic SPI and GPIO Linux UAPI.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <linux/gpio.h>

#include "libtropic_port.h"

/**
 * @brief Device structure for Unix SPI port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_unix_spi_t {
    /** @public @brief SPI speed in Hz. */
    int spi_speed;
    /** @public @brief Path to the SPI device. */
    char spi_dev[DEVICE_PATH_MAX_LEN];
    /** @public @brief Path to the GPIO device. */
    char gpio_dev[DEVICE_PATH_MAX_LEN];
    /** @public @brief Number of the GPIO pin to map chip select to. */
    int gpio_cs_num;
    /** @public @brief Seed for the platform's random number generator. */
    unsigned int rng_seed;

    /** @private @brief SPI file descriptor. */
    int fd;
    /** @private @brief GPIO file descriptor. */
    int gpio_fd;
    /** @private @brief GPIO request (for GPIO configuration). */
    struct gpio_v2_line_request gpioreq;
    /** @private @brief SPI mode. */
    uint32_t mode;
} lt_dev_unix_spi_t;

#endif  // LIBTROPIC_PORT_UNIX_SPI_H