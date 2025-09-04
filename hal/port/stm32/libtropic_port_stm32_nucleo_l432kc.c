/**
 * @file libtropic_port_stm32_nucleo_l432kc.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "stm32l4xx_hal.h"

// CS pin
#define LT_SPI_CS_BANK GPIOA
#define LT_SPI_CS_PIN GPIO_PIN_4
// Spi instance
#define LT_SPI_INSTANCE SPI1

// Random number generator's handle
RNG_HandleTypeDef rng;
// SPI handle declaration
SPI_HandleTypeDef SpiHandle;

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    UNUSED(s2);
    size_t bytes_left = count;
    uint8_t *buff_ptr = buff;
    while (bytes_left) {
        uint32_t random_data = HAL_RNG_GetRandomNumber(&rng);
        size_t cpy_cnt = bytes_left < sizeof(random_data) ? bytes_left : sizeof(random_data);
        memcpy(buff_ptr, &random_data, cpy_cnt);
        bytes_left -= cpy_cnt;
        buff_ptr += cpy_cnt;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *h)
{
    UNUSED(h);

    HAL_GPIO_WritePin(LT_SPI_CS_BANK, LT_SPI_CS_PIN, GPIO_PIN_RESET);
    while (HAL_GPIO_ReadPin(LT_SPI_CS_BANK, LT_SPI_CS_PIN)) {
        ;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *h)
{
    UNUSED(h);

    HAL_GPIO_WritePin(LT_SPI_CS_BANK, LT_SPI_CS_PIN, GPIO_PIN_SET);
    while (!HAL_GPIO_ReadPin(LT_SPI_CS_BANK, LT_SPI_CS_PIN)) {
        ;
    }

    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *h)
{
    UNUSED(h);

    // RNG
    rng.Instance = RNG;

    if (HAL_RNG_DeInit(&rng) != HAL_OK) {
        return LT_FAIL;
    }

    if (HAL_RNG_Init(&rng) != HAL_OK) {
        return LT_FAIL;
    }

    // SPI CS
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_WritePin(LT_SPI_CS_BANK, LT_SPI_CS_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = LT_SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LT_SPI_CS_BANK, &GPIO_InitStruct);
    // SPI hardware
    SpiHandle.Instance = LT_SPI_INSTANCE;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS = SPI_NSS_HARD_OUTPUT;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;

    if (HAL_SPI_Init(&SpiHandle) != HAL_OK) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *h)
{
    UNUSED(h);

    if (HAL_RNG_DeInit(&rng) != HAL_OK) {
        return LT_FAIL;
    }

    HAL_SPI_MspDeInit(&SpiHandle);

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        return LT_L1_DATA_LEN_ERROR;
    }
    int ret = HAL_SPI_TransmitReceive(&SpiHandle, h->buff + offset, h->buff + offset, tx_data_length, timeout_ms);
    if (ret != HAL_OK) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *h, uint32_t ms)
{
    UNUSED(h);

    HAL_Delay(ms);

    return LT_OK;
}
