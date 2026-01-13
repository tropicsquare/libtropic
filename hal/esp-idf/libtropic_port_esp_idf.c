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

#if LT_USE_INT_PIN
// traceISR_EXIT_TO_SCHEDULER is a FreeRTOS tracing macro used by portYIELD_FROM_ISR.
// It should be defined in FreeRTOS headers (typically as an empty macro when tracing
// is disabled), but some ESP-IDF configurations omit this definition.
// Defining it here as an empty macro is the correct behavior when tracing is not enabled.
// See: FreeRTOS trace macros in FreeRTOS.h and FreeRTOSConfig.h
#ifndef traceISR_EXIT_TO_SCHEDULER
#define traceISR_EXIT_TO_SCHEDULER() \
    do {                             \
    } while (0)
#endif

/**
 * @brief ISR handler for the TROPIC01's interrupt pin.
 *
 * @param esp_idf_dev lt_dev_esp_idf_t device structure
 */
static void IRAM_ATTR int_gpio_pin_isr_handler(void *esp_idf_dev)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(esp_idf_dev);
    BaseType_t higher_priority_task_woken = pdFALSE;

    // Give the semaphore, signaling the waiting task.
    xSemaphoreGiveFromISR(dev->int_gpio_sem, &higher_priority_task_woken);
    // If giving the semaphore unblocked a higher priority task, it is recommended
    // to request a context switch before the interrupt is exited.
    portYIELD_FROM_ISR(higher_priority_task_woken);
}
#endif

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    esp_err_t esp_ret;
    lt_ret_t lt_ret;

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
                                    .max_transfer_sz = TR01_L1_LEN_MAX,
                                    .flags = (SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS)};

    // Initialize the SPI bus.
    esp_ret = spi_bus_initialize(dev->spi_host_id, &spi_bus_cfg, SPI_DMA_CH_AUTO);
    if (esp_ret != ESP_OK) {
        LT_LOG_ERROR("spi_bus_initialize() failed: %s", esp_err_to_name(esp_ret));
        return LT_FAIL;
    }

    // Create configuration for the SPI device.
    spi_device_interface_config_t spi_dev_cfg = {.mode = 0,  // TROPIC01 supports only CPOL=0 and CPHA=0.
                                                 .clock_speed_hz = dev->spi_clk_hz,
                                                 .spics_io_num = -1,  // We handle CS ourselves.
                                                 .queue_size = 7,     // 7 is often used as default.
                                                 .pre_cb = NULL,
                                                 .post_cb = NULL};

    // Add the SPI device to the bus.
    esp_ret = spi_bus_add_device(dev->spi_host_id, &spi_dev_cfg, &dev->spi_handle);
    if (esp_ret != ESP_OK) {
        LT_LOG_ERROR("spi_bus_add_device() failed: %s", esp_err_to_name(esp_ret));
        lt_ret = LT_FAIL;
        goto spi_bus_add_device_error;
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
    esp_ret = gpio_config(&spi_cs_gpio_cfg);
    if (esp_ret != ESP_OK) {
        LT_LOG_ERROR("gpio_config() failed: %s", esp_err_to_name(esp_ret));
        lt_ret = LT_FAIL;
        goto cs_gpio_config_error;
    }

    // Set SPI CS GPIO pin level to high.
    esp_ret = gpio_set_level(dev->spi_cs_gpio_pin, 1);
    if (esp_ret != ESP_OK) {
        LT_LOG_ERROR("gpio_set_level() failed: %s", esp_err_to_name(esp_ret));
        lt_ret = LT_FAIL;
        goto cs_gpio_set_level_error;
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
    esp_ret = gpio_config(&int_gpio_cfg);
    if (esp_ret != ESP_OK) {
        LT_LOG_ERROR("gpio_config() failed: %s", esp_err_to_name(esp_ret));
        lt_ret = LT_FAIL;
        goto int_gpio_config_error;
    }

    // Create a semaphore used for the GPIO interrupt pin.
    dev->int_gpio_sem = xSemaphoreCreateBinary();
    if (!dev->int_gpio_sem) {
        LT_LOG_ERROR("Failed to create semaphore with xSemaphoreCreateBinary!");
        lt_ret = LT_FAIL;
        goto xSemaphoreCreateBinary_error;
    }

    // Register the ISR handler for the GPIO interrupt pin.
    esp_ret = gpio_isr_handler_add(dev->int_gpio_pin, int_gpio_pin_isr_handler, dev);
    if (esp_ret != ESP_OK) {
        LT_LOG_ERROR("gpio_isr_handler_add() failed: %s", esp_err_to_name(esp_ret));
        lt_ret = LT_FAIL;
        goto gpio_isr_handler_add_error;
    }
#endif

    return LT_OK;

#if LT_USE_INT_PIN
gpio_isr_handler_add_error:
    vSemaphoreDelete(dev->int_gpio_sem);
    dev->int_gpio_sem = NULL;
xSemaphoreCreateBinary_error:
int_gpio_config_error:
    gpio_reset_pin(dev->int_gpio_pin);
#endif
cs_gpio_set_level_error:
cs_gpio_config_error:
    gpio_reset_pin(dev->spi_cs_gpio_pin);
    spi_bus_remove_device(dev->spi_handle);
    dev->spi_handle = NULL;
spi_bus_add_device_error:
    spi_bus_free(dev->spi_host_id);

    return lt_ret;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);

    // Return values are ignored, because the entire cleanup should always be done.
#if LT_USE_INT_PIN
    gpio_isr_handler_remove(dev->int_gpio_pin);
    if (dev->int_gpio_sem != NULL) {
        vSemaphoreDelete(dev->int_gpio_sem);
        dev->int_gpio_sem = NULL;
    }
    gpio_reset_pin(dev->int_gpio_pin);
#endif
    gpio_reset_pin(dev->spi_cs_gpio_pin);
    if (dev->spi_handle != NULL) {
        spi_bus_remove_device(dev->spi_handle);
        dev->spi_handle = NULL;
    }
    spi_bus_free(dev->spi_host_id);

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

    return LT_OK;
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

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout_ms)
{
    lt_dev_esp_idf_t *dev = (lt_dev_esp_idf_t *)(s2->device);
    esp_err_t ret;
    spi_transaction_t spi_transaction;
    spi_transaction_t *trans_result;
    TickType_t ticks_to_wait = pdMS_TO_TICKS(timeout_ms), start_ticks, elapsed_ticks, remaining_ticks;

    // Prepare the SPI transaction.
    memset(&spi_transaction, 0, sizeof(spi_transaction));
    spi_transaction.length = tx_len * 8;
    spi_transaction.tx_buffer = s2->buff + offset;
    spi_transaction.rx_buffer = s2->buff + offset;

    // Record start time to track total elapsed time.
    start_ticks = xTaskGetTickCount();

    // Queue the SPI transaction with the specified timeout.
    // This ensures we don't block indefinitely if the queue is full.
    ret = spi_device_queue_trans(dev->spi_handle, &spi_transaction, ticks_to_wait);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_device_queue_trans() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }

    // Calculate remaining time for the get_trans_result operation.
    elapsed_ticks = xTaskGetTickCount() - start_ticks;
    if (elapsed_ticks >= ticks_to_wait) {
        // Already exceeded timeout during queue operation.
        LT_LOG_ERROR("Timeout exceeded during spi_device_queue_trans()");
        return LT_FAIL;
    }
    remaining_ticks = ticks_to_wait - elapsed_ticks;

    // Get the transaction result with the remaining timeout.
    // This is where the actual SPI transfer executes.
    ret = spi_device_get_trans_result(dev->spi_handle, &trans_result, remaining_ticks);
    if (ret != ESP_OK) {
        LT_LOG_ERROR("spi_device_get_trans_result() failed: %s", esp_err_to_name(ret));
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    LT_UNUSED(s2);
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
    LT_UNUSED(s2);

    esp_fill_random(buff, count);
    return LT_OK;
}

int lt_port_log(const char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = vprintf(format, args);
    va_end(args);

    return ret;
}