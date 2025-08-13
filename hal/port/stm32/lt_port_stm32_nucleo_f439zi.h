#ifndef LT_PORT_STM32_NUCLEO_F439ZI_H
#define LT_PORT_STM32_NUCLEO_F439ZI_H

/**
 * @file lt_port_stm32_nucleo_f439zi.h
 * @author Tropic Square s.r.o.
 * @brief Port for STM32 F439ZI using native SPI HAL (and GPIO HAL for chip select).
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_port.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Device structure for STM32 F439ZI port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_stm32_nucleo_f439zi {
    
    /** @brief @private Random number generator handle. */
    RNG_HandleTypeDef rng_handle;

    /** @brief @private SPI handle. */
    SPI_HandleTypeDef spi_handle;
} lt_dev_stm32_nucleo_f439zi;

#endif  // LT_PORT_STM32_NUCLEO_F439ZI_H