/**
 * @file libtropic_port_stm32_nucleo_f439zi.c
 * @author Tropic Square s.r.o.
 * @brief Port for STM32 F439ZI using native SPI HAL (and GPIO HAL for chip select).
 *
 * Most of this SPI code is inspired by https://github.com/STMicroelectronics/STM32CubeF4:
 * Projects/STM32F429I-Discovery/Examples/SPI/SPI_FullDuplex_ComPolling/Src/main.c
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_port_stm32_nucleo_f439zi.h"

#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "main.h"
#include "stm32f4xx_hal.h"

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);
    size_t bytes_left = count;
    uint8_t *buff_ptr = buff;
    int ret;
    uint32_t random_data;

    while (bytes_left) {
        ret = HAL_RNG_GenerateRandomNumber(&device->rng_handle, &random_data);
        if (ret != HAL_OK) {
            LT_LOG_ERROR("HAL_RNG_GenerateRandomNumber failed, ret=%d", ret);
            return LT_FAIL;
        }

        size_t cpy_cnt = bytes_left < sizeof(random_data) ? bytes_left : sizeof(random_data);
        memcpy(buff_ptr, &random_data, cpy_cnt);
        bytes_left -= cpy_cnt;
        buff_ptr += cpy_cnt;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);

    HAL_GPIO_WritePin(device->spi_cs_gpio_bank, device->spi_cs_gpio_pin, GPIO_PIN_RESET);
    while (HAL_GPIO_ReadPin(device->spi_cs_gpio_bank, device->spi_cs_gpio_pin));

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);

    HAL_GPIO_WritePin(device->spi_cs_gpio_bank, device->spi_cs_gpio_pin, GPIO_PIN_SET);
    while (!HAL_GPIO_ReadPin(device->spi_cs_gpio_bank, device->spi_cs_gpio_pin));

    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);
    int ret;

    ret = HAL_RNG_Init(&device->rng_handle);
    if (ret != HAL_OK) {
        LT_LOG_ERROR("Failed to init RNG, ret=%d", ret);
        return LT_FAIL;
    }

    // Set the SPI parameters.
    device->spi_handle.Instance = device->spi_instance;

    if (device->baudrate_prescaler == 0) {
        device->spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    }
    else {
        device->spi_handle.Init.BaudRatePrescaler = device->baudrate_prescaler;
    }

    device->spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
    device->spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    device->spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    device->spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    device->spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
    device->spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    device->spi_handle.Init.NSS = SPI_NSS_HARD_OUTPUT;
    device->spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
    device->spi_handle.Init.Mode = SPI_MODE_MASTER;

    ret = HAL_SPI_Init(&device->spi_handle);
    if (ret != HAL_OK) {
        LT_LOG_ERROR("Failed to init SPI, ret=%d", ret);
        return LT_L1_SPI_ERROR;
    }

    // GPIO for chip select.
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    LT_SPI_CS_CLK_ENABLE();
    HAL_GPIO_WritePin(device->spi_cs_gpio_bank, device->spi_cs_gpio_pin, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = device->spi_cs_gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(device->spi_cs_gpio_bank, &GPIO_InitStruct);

#if LT_USE_INT_PIN
    // GPIO for INT pin.
    LT_INT_CLK_ENABLE();
    GPIO_InitStruct.Pin = device->int_gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(device->int_gpio_bank, &GPIO_InitStruct);
#endif

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);
    int ret;

    ret = HAL_RNG_DeInit(&device->rng_handle);
    if (ret != HAL_OK) {
        LT_LOG_ERROR("Failed to deinit RNG, ret=%d", ret);
        return LT_FAIL;
    }

    ret = HAL_SPI_DeInit(&device->spi_handle);
    if (ret != HAL_OK) {
        LT_LOG_ERROR("Failed to deinit SPI, ret=%d", ret);
        return LT_L1_SPI_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);

    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        LT_LOG_ERROR("Invalid data length!");
        return LT_L1_DATA_LEN_ERROR;
    }
    int ret = HAL_SPI_TransmitReceive(&device->spi_handle, s2->buff + offset, s2->buff + offset, tx_data_length,
                                      timeout_ms);
    if (ret != HAL_OK) {
        LT_LOG_ERROR("HAL_SPI_TransmitReceive failed, ret=%d", ret);
        return LT_L1_SPI_ERROR;
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    UNUSED(s2);

    HAL_Delay(ms);

    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_stm32_nucleo_f439zi *device = (lt_dev_stm32_nucleo_f439zi *)(s2->device);
    uint32_t time_initial = HAL_GetTick();
    uint32_t time_actual;

    while ((HAL_GPIO_ReadPin(device->int_gpio_bank, device->int_gpio_pin) == 0)) {
        time_actual = HAL_GetTick();
        if ((time_actual - time_initial) > ms) {
            return LT_L1_INT_TIMEOUT;
        }
        // HAL_Delay(ms);
    }

    return LT_OK;
}
#endif
