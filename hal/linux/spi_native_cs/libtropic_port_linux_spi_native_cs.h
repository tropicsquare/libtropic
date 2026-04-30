#ifndef LIBTROPIC_PORT_LINUX_SPI_NATIVE_CS_H
#define LIBTROPIC_PORT_LINUX_SPI_NATIVE_CS_H

/**
 * @file libtropic_port_linux_spi_native_cs.h
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Port for communication using Generic SPI UAPI with native CS handling and GPIO UAPI for
 * interrupt handling.
 *
 * @warning This HAL is experimental. It can be modified or removed in the next release without notice.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#if LT_USE_INT_PIN
#include <linux/gpio.h>
#endif

#define LT_SPI_PATH_MAX_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device structure for Linux SPI port with native CS handling.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       Libtropic.
 */
typedef struct lt_dev_linux_spi_native_cs_t {
    /** @public @brief SPI speed in Hz. */
    int spi_speed;
    /** @public @brief Path to the SPI device. */
    char spi_dev[LT_SPI_PATH_MAX_LEN];
#if LT_USE_INT_PIN
    /** @public @brief Path to the GPIO device. */
    char gpio_dev[LT_SPI_PATH_MAX_LEN];
    /** @public @brief Number of the GPIO pin to map interrupt pin to. */
    int gpio_int_num;
#endif

    /** @private @brief SPI file descriptor. */
    int spi_fd;
#if LT_USE_INT_PIN
    /** @private @brief GPIO file descriptor. */
    int gpio_fd;
    /** @private @brief GPIO request structure for interrupt pin. */
    struct gpio_v2_line_request gpioreq_int;
#endif

    /** @private @brief True if frame transfer is in progress.
     * This tracks the state of "virtual CS", which is toggled by lt_port_spi_csn_* functions.
     * Those functions do not do anything except setting transfer variables, which allows
     * us to track if the current frame was finished or Libtropic still reads data
     * from preloaded buffer.
     *
     * This variable is useful only to keep track of actual lt_port_spi_transfer usage in Libtropic.
     * If the L1 implementation in Libtropic would ever change to make this HAL incompatible,
     * it would be immediately discovered.
     */
    int frame_in_progress;

    /** @private @brief True if lt_port_spi_transfer was already called during current frame. If true,
     * lt_port_spi_transfer does not do any communication and immediately returns.
     *
     * Normally, Libtropic transfers frame by parts. E.g., it first transfers 1 byte to receive
     * CHIP_STATUS. This is possible thanks to separate CS handling. In this HAL, separate CS is not
     * available and as such we have to transfer whole buffer (even though actual frame may be smaller)
     * at once. Hence only single SPI transfer is done.
     */
    int frame_completed;

} lt_dev_linux_spi_native_cs_t;

#ifdef __cplusplus
}
#endif

#endif  // LIBTROPIC_PORT_LINUX_SPI_NATIVE_CS_H