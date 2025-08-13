/**
 * @file lt_port_stm32.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"
// Pin definitions are in main.h:
#include "libtropic_common.h"
#include "libtropic_port.h"
#include "main.h"
#include "libtropic_logging.h"
#include "lt_port_stm32_nucleo_f439zi.h"

lt_dev_stm32_nucleo_f439zi device;


lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    UNUSED(s2);
    size_t bytes_left = count;
    uint8_t *buff_ptr = buff;
    while (bytes_left) {
        uint32_t random_data = HAL_RNG_GetRandomNumber(&device.rng_handle);
        size_t cpy_cnt = bytes_left < sizeof(random_data) ? bytes_left : sizeof(random_data);
        memcpy(buff_ptr, &random_data, cpy_cnt);
        bytes_left -= cpy_cnt;
        buff_ptr += cpy_cnt;
    }

    return LT_OK;
}

/* Most of this SPI code is taken from:
   vendor/STM32CubeF4/Projects/STM32F429I-Discovery/Examples/SPI/SPI_FullDuplex_ComPolling/Src/main.c
*/


lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    UNUSED(s2);

    HAL_GPIO_WritePin(LT_SPI_CS_BANK, LT_SPI_CS_PIN, GPIO_PIN_RESET);
    while (HAL_GPIO_ReadPin(LT_SPI_CS_BANK, LT_SPI_CS_PIN)) {
        ;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    UNUSED(s2);

    HAL_GPIO_WritePin(LT_SPI_CS_BANK, LT_SPI_CS_PIN, GPIO_PIN_SET);
    while (!HAL_GPIO_ReadPin(LT_SPI_CS_BANK, LT_SPI_CS_PIN)) {
        ;
    }

    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    UNUSED(s2);

    if (HAL_RNG_Init(&device.rng_handle) != HAL_OK) {
        return LT_FAIL;
    }

    /* Set the SPI parameters */
    device.spi_handle.Instance = LT_SPI_INSTANCE;
    device.spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    device.spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
    device.spi_handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    device.spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    device.spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    // spi_handle.Init.CRCPolynomial     = 7;
    device.spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
    device.spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    device.spi_handle.Init.NSS = SPI_NSS_HARD_OUTPUT;
    device.spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
    device.spi_handle.Init.Mode = SPI_MODE_MASTER;

    if (HAL_SPI_Init(&device.spi_handle) != HAL_OK) {
        return LT_FAIL;
    }

    // GPIO for chip select:
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    LT_SPI_CS_CLK_ENABLE();
    HAL_GPIO_WritePin(LT_SPI_CS_BANK, LT_SPI_CS_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = LT_SPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LT_SPI_CS_BANK, &GPIO_InitStruct);

#if LT_USE_INT_PIN
    /* GPIO for INT pin */
    LT_INT_CLK_ENABLE();
    GPIO_InitStruct.Pin = LT_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LT_INT_BANK, &GPIO_InitStruct);
#endif

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    UNUSED(s2);

    if (HAL_RNG_DeInit(&device.rng_handle) != HAL_OK) {
        return LT_FAIL;
    }

    if (HAL_SPI_DeInit(&device.spi_handle) != HAL_OK) {
        LT_LOG_ERROR("Failed to deinit SPI!");
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        return LT_L1_DATA_LEN_ERROR;
    }
    int ret = HAL_SPI_TransmitReceive(&device.spi_handle, s2->buff + offset, s2->buff + offset, tx_data_length, timeout);
    if (ret != HAL_OK) {
        return LT_FAIL;
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
    UNUSED(s2);
    uint32_t time_initial = HAL_GetTick();
    uint32_t time_actual;
    while ((HAL_GPIO_ReadPin(LT_INT_BANK, LT_INT_PIN) == 0)) {
        time_actual = HAL_GetTick();
        if ((time_actual - time_initial) > ms) {
            return LT_L1_INT_TIMEOUT;
        }
        // HAL_Delay(ms);
    }

    return LT_OK;
}
#endif
