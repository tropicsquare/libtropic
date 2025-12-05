#ifndef LIBTROPIC_PORT_ESP_IDF_H
#define LIBTROPIC_PORT_ESP_IDF_H

/**
 * @file libtropic_port_esp_idf.h
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Declarations for the ESP-IDF port.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @brief Device structure for ESP-IDF port.
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_esp_idf_t {
    /** @brief @public GPIO pin used for chip select. */
    gpio_num_t spi_cs_gpio_pin;
    /** @brief @public SPI host peripheral ID. */
    spi_host_device_t spi_host_id;
    /** @brief @public SPI MISO pin. */
    int spi_miso_pin;
    /** @brief @public SPI MOSI pin. */
    int spi_mosi_pin;
    /** @brief @public SPI CLK pin. */
    int spi_clk_pin;
    /** @brief @public SPI CLK frequency (Hz). */
    int spi_clk_hz;

    /** @brief @private SPI handle. */
    spi_device_handle_t spi_handle;
#if LT_USE_INT_PIN
    /** @brief @public GPIO pin connected to TROPIC01's interrupt pin. */
    gpio_num_t int_gpio_pin;
    /** @brief @private Semaphore for the TROPIC01's interrupt pin. */
    SemaphoreHandle_t int_gpio_sem;
#endif
} lt_dev_esp_idf_t;

#endif  // LIBTROPIC_PORT_ESP_IDF_H