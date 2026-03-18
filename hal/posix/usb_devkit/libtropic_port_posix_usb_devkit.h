#ifndef LIBTROPIC_PORT_POSIX_USB_DEVKIT_H
#define LIBTROPIC_PORT_POSIX_USB_DEVKIT_H

/**
 * @file libtropic_port_posix_usb_devkit.h
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 * @brief Port for communication with TROPIC01 USB DevKit.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdbool.h>

#include "libtropic_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LT_USB_DEVKIT_READ_WRITE_DELAY 10
#define LT_USB_DEVKIT_SPI_TRANSFER_BUFF_SIZE_MAX ((TR01_L1_LEN_MAX * 2) + 1)

/**
 * @brief Device structure for TROPIC01 USB DevKit POSIX port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_posix_usb_devkit_t {
    /** @public @brief Path to USB UART device. */
    char dev_path[LT_DEVICE_PATH_MAX_LEN];
    /** @public @brief UART baudrate. */
    uint32_t baud_rate;

    /** @private @brief UART device file descriptor. */
    int fd;
    /** @private @brief Whether the USB DevKit is running legacy FW. */
    bool legacy_fw;
} lt_dev_posix_usb_devkit_t;

#ifdef __cplusplus
}
#endif

#endif  // LIBTROPIC_PORT_POSIX_USB_DEVKIT_H