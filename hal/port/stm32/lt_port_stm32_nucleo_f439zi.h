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
    /** @brief @public Instance of STM SPI interface. Use STM32 macro (SPIX, e.g. SPI1). */
    SPI_TypeDef    *spi_instance;

    /** @brief @public GPIO pin used for chip select. Use STM32 macro (GPIO_PIN_XX). */
    uint16_t       spi_cs_gpio_pin;
    /** @brief @public GPIO bank of the pin used for chip select. Use STM32 macro (GPIOX). */
    GPIO_TypeDef   *spi_cs_gpio_bank;

#ifdef LT_USE_INT_PIN
    /** @brief @public GPIO pin used for interrupts. Use STM32 macro (GPIO_PIN_XX). */
    uint16_t       int_gpio_pin;
    /** @brief @public GPIO bank of the pin used for interrupts. Use STM32 macro (GPIOX). */
    GPIO_TypeDef   *int_gpio_bank;
#endif

    /** @brief @private Random number generator handle. */
    RNG_HandleTypeDef rng_handle;

    /** @brief @private SPI handle. */
    SPI_HandleTypeDef spi_handle;
} lt_dev_stm32_nucleo_f439zi;

#endif  // LT_PORT_STM32_NUCLEO_F439ZI_H