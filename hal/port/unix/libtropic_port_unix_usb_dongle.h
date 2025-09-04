#ifndef LIBTROPIC_PORT_UNIX_USB_DONGLE_H
#define LIBTROPIC_PORT_UNIX_USB_DONGLE_H

/**
 * @file libtropic_port_unix_usb_dongle.h
 * @author Tropic Square s.r.o.
 * @brief Port for communication with USB UART Dongle (TS1302).
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <linux/gpio.h>

#include "libtropic_port.h"

/**
 * @brief Device structure for Unix USB Dongle port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_unix_usb_dongle_t {
    /** @public @brief Path to USB UART device. */
    char dev_path[DEVICE_PATH_MAX_LEN];
    /** @public @brief UART baudrate. */
    uint32_t baud_rate;
    /** @public @brief Seed for the platform's random number generator. */
    unsigned int rng_seed;

    /** @private @brief UART device file descriptor. */
    int fd;
} lt_dev_unix_usb_dongle_t;

#endif  // LIBTROPIC_PORT_UNIX_USB_DONGLE_H