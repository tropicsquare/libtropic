#ifndef LIBTROPIC_PORT_POSIX_USB_DONGLE_H
#define LIBTROPIC_PORT_POSIX_USB_DONGLE_H

/**
 * @file libtropic_port_posix_usb_dongle.h
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Port for communication with USB UART Dongle (TS1302).
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LT_USB_DONGLE_READ_WRITE_DELAY 10
#define LT_USB_DONGLE_SPI_TRANSFER_BUFF_SIZE_MAX ((TR01_L1_LEN_MAX * 2) + 1)

/**
 * @brief Device structure for USB Dongle POSIX port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_posix_usb_dongle_t {
    /** @public @brief Path to USB UART device. */
    char dev_path[LT_DEVICE_PATH_MAX_LEN];
    /** @public @brief UART baudrate. */
    uint32_t baud_rate;

    /** @private @brief UART device file descriptor. */
    int fd;
} lt_dev_posix_usb_dongle_t;

#ifdef __cplusplus
}
#endif

#endif  // LIBTROPIC_PORT_POSIX_USB_DONGLE_H