/**
 * @file libtropic_port_esp_idf.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 * @brief Port for ESP-IDF.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic_port_esp_idf.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_random.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"

/**
 * @brief ISR handler for the TROPIC01's interrupt pin.
 *
 * @param arg
 */
static void IRAM_ATTR int_gpio_pin_isr_handler(void *arg)
{
    LT_UNUSED(arg);
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Give the semaphore, signaling the waiting task.
    xSemaphoreGiveFromISR(gpio_isr_sem, &higher_priority_task_woken);
    // If giving the semaphore unblocked a higher priority task, it is recommended
    // to request a context switch before the interrupt is exited.
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    esp_err_t ret;

    // Create configuration for the SPI bus.
    spi_bus_config_t spi_bus_cfg = {.mosi_io_num = dev->spi_mosi_pin,
                                    .miso_io_num = dev->spi_miso_pin,
                                    .sclk_io_num = dev->spi_clk_pin,
                                    .quadwp_io_num = -1,
                                    .quadhd_io_num = -1,
                                    .data4_io_num = -1,
                                    .data5_io_num = -1,
                                    .data6_io_num = -1,
                                    .data7_io_num = -1,
                                    .max_transfer_sz = TR01_L1_LEN_MAX};

    // Initialize the SPI bus.
    ret = spi_bus_initialize(dev->spi_host_id, &spi_bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_bus_initialize() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }

    // Create configuration for the SPI device.
    spi_device_interface_config_t spi_dev_cfg = {.mode = 0,  // TROPIC01 supports only CPOL=0 and CPHA=0.
                                                 .clock_speed_hz = dev->spi_clk_hz,
                                                 .spics_io_num = -1,  // We handle CS ourselves.
                                                 .pre_cb = NULL,
                                                 .post_cb = NULL};

    // Add the SPI device to the bus.
    ret = spi_bus_add_device(dev->spi_host_id, &spi_dev_cfg, &dev->spi_handle);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_bus_add_device() failed: %s", esp_err_to_name(ret));
        lt_port_deinit(s2);
        return LT_FAIL;
    }

    // Create configuration for the SPI CS GPIO pin.
    gpio_config_t spi_cs_gpio_cfg = {.pin_bit_mask = (1ULL << dev->spi_cs_gpio_pin),
                                     .mode = GPIO_MODE_OUTPUT,
                                     .pull_up_en = GPIO_PULLUP_DISABLE,
                                     .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                     .intr_type = GPIO_INTR_DISABLE
#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
                                     ,
                                     .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE
#endif
    };

    // Configure the SPI CS GPIO pin.
    ret = gpio_config(&spi_cs_gpio_cfg);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("gpio_config() failed: %s", esp_err_to_name(ret));
        lt_port_deinit(s2);
        return LT_FAIL;
    }

    // Set SPI CS GPIO pin level to high.
    ret = gpio_set_level(dev->spi_cs_gpio_pin, 1);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("gpio_set_level() failed: %s", esp_err_to_name(ret));
        lt_port_deinit(s2);
        return LT_FAIL;
    }

#if LT_USE_INT_PIN
    // Create configuration for the GPIO connected to TROPIC01's interrupt pin.
    gpio_config_t int_gpio_cfg = {.pin_bit_mask = (1ULL << dev->int_gpio_pin),
                                  .mode = GPIO_MODE_INPUT,
                                  .pull_up_en = GPIO_PULLUP_DISABLE,
                                  .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                  .intr_type = GPIO_INTR_POSEDGE
#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
                                  ,
                                  .hys_ctrl_mode = GPIO_HYS_SOFT_DISABLE
#endif
    };

    // Configure the GPIO interrupt pin.
    ret = gpio_config(&int_gpio_cfg);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("gpio_config() failed: %s", esp_err_to_name(ret));
        lt_port_deinit(s2);
        return LT_FAIL;
    }

    // Create a semaphore used for the GPIO interrupt pin.
    dev->int_gpio_sem = xSemaphoreCreateBinary();
    if (!dev->int_gpio_sem) {
        LT_LOG_ERROR("Failed to create semaphore with xSemaphoreCreateBinary!");
        lt_port_deinit(s2);
        return LT_FAIL;
    }

    // Register the ISR handler for the GPIO interrupt pin.
    ret = gpio_isr_handler_add(dev->int_gpio_pin, int_gpio_pin_isr_handler, NULL);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("gpio_isr_handler_add() failed: %s", esp_err_to_name(ret));
        lt_port_deinit(s2);
        return LT_FAIL;
    }
#endif

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);

    // Return values are ignored, because the entire cleanup should always be done.
    spi_bus_remove_device(dev->spi_handle);
    spi_bus_free(dev->spi_host_id);
    gpio_reset_pin(dev->spi_cs_gpio_pin);
#if LT_USE_INT_PIN
    gpio_isr_handler_remove(dev->int_gpio_pin);
    gpio_reset_pin(dev->int_gpio_pin);
    vSemaphoreDelete(dev->int_gpio_sem);
#endif

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    esp_err_t ret;

    ret = gpio_set_level(dev->spi_cs_gpio_pin, 0);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("gpio_set_level() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    esp_err_t ret;

    ret = gpio_set_level(dev->spi_cs_gpio_pin, 1);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("gpio_set_level() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout_ms)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    esp_err_t ret;
    spi_transaction_t spi_transaction;

    // Prepare the SPI transaction.
    memset(&spi_transaction, 0, sizeof(spi_transaction));
    spi_transaction.length = tx_len;
    spi_transaction.tx_buffer = s2->buff + offset;
    spi_transaction.rx_buffer = s2->buff + offset;

    // Acquire the SPI bus.
    // portMAX_DELAY is required by the implementation.
    ret = spi_device_acquire_bus(dev->spi_handle, portMAX_DELAY);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_device_acquire_bus() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }

    // Execute the SPI transaction.
    ret = spi_device_polling_transmit(dev->spi_handle, &spi_transaction);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_device_polling_transmit() failed: %s", esp_err_to_name(ret));
        spi_device_release_bus(dev->spi_handle);
        return LT_FAIL;
    }

    // Release the SPI bus.
    ret = spi_device_release_bus(dev->spi_handle);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_device_release_bus() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    TickType_t ticks_to_wait = pdMS_TO_TICKS(ms);

    vTaskDelay(ticks_to_wait);
    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    TickType_t ticks_to_wait = pdMS_TO_TICKS(ms);

    if (xSemaphoreTake(dev->int_gpio_sem, ticks_to_wait) == pdTRUE) {
        return LT_OK;
    }

    return LT_L1_INT_TIMEOUT;
}
#endif

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);

    esp_fill_random(buff, count);
    return LT_OK;
}

void lt_port_log(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}