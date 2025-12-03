#ifndef LIBTROPIC_PORT_LINUX_SPI_H
#define LIBTROPIC_PORT_LINUX_SPI_H

/**
 * @file libtropic_port_linux_spi.h
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Port for communication using Generic SPI and GPIO Linux UAPI.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include <linux/gpio.h>

#include "libtropic_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device structure for Linux SPI port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_linux_spi_t {
    /** @public @brief SPI speed in Hz. */
    int spi_speed;
    /** @public @brief Path to the SPI device. */
    char spi_dev[LT_DEVICE_PATH_MAX_LEN];
    /** @public @brief Path to the GPIO device. */
    char gpio_dev[LT_DEVICE_PATH_MAX_LEN];
    /** @public @brief Number of the GPIO pin to map chip select to. */
    int gpio_cs_num;
#if LT_USE_INT_PIN
    /** @public @brief Number of the GPIO pin to map interrupt pin to. */
    int gpio_int_num;
#endif

    /** @private @brief SPI file descriptor. */
    int spi_fd;
    /** @private @brief GPIO file descriptor. */
    int gpio_fd;
    /** @private @brief GPIO request structure for chip select. */
    struct gpio_v2_line_request gpioreq_cs;
#if LT_USE_INT_PIN
    /** @private @brief GPIO request structure for interrupt pin. */
    struct gpio_v2_line_request gpioreq_int;
#endif
    /** @private @brief SPI mode. */
    uint32_t mode;
} lt_dev_linux_spi_t;

#ifdef __cplusplus
}
#endif

#endif  // LIBTROPIC_PORT_LINUX_SPI_H