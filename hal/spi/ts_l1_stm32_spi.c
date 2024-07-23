#include <stdint.h>
#include "ts_common.h"
#include "ts_l1.h"

#include "stm32f4xx_hal.h"

#define TS_SPI_CS_BANK   GPIOE
#define TS_SPI_CS_PIN    GPIO_PIN_4
#define TS_SPI_INSTANCE  SPI4

// Use 1 here to enable SPI switching on FPGA board
#define FPGA_REMOTE 1

/* Most of this SPI code is taken from:
   vendor/STM32CubeF4/Projects/STM32F429I-Discovery/Examples/SPI/SPI_FullDuplex_ComPolling/Src/main.c
*/

/* SPI handle declaration */
SPI_HandleTypeDef SpiHandle;

ts_ret_t ts_l1_spi_csn_low(ts_handle_t *h)
{
    UNUSED(h);

    HAL_GPIO_WritePin(TS_SPI_CS_BANK, TS_SPI_CS_PIN, GPIO_PIN_RESET);
    while(HAL_GPIO_ReadPin(TS_SPI_CS_BANK, TS_SPI_CS_PIN)) {
        ;
    }

    return TS_OK;
}

ts_ret_t ts_l1_spi_csn_high(ts_handle_t *h)
{
    UNUSED(h);

    HAL_GPIO_WritePin(TS_SPI_CS_BANK, TS_SPI_CS_PIN, GPIO_PIN_SET);
    while(!HAL_GPIO_ReadPin(TS_SPI_CS_BANK, TS_SPI_CS_PIN)) {
        ;
    }

    return TS_OK;
}


ts_ret_t ts_l1_init(ts_handle_t *h)
{
    UNUSED(h);

    /* Set the SPI parameters */
    SpiHandle.Instance               = TS_SPI_INSTANCE;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    //SpiHandle.Init.CRCPolynomial     = 7;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = SPI_NSS_HARD_OUTPUT;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;

    if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        return TS_FAIL;
    }

    // GPIO for chip select:
    GPIO_InitTypeDef  GPIO_InitStruct = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();
    HAL_GPIO_WritePin(TS_SPI_CS_BANK, TS_SPI_CS_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = TS_SPI_CS_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(TS_SPI_CS_BANK, &GPIO_InitStruct);

    #ifdef FPGA_REMOTE
    // Pin for controlling SPI access
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_Delay(100);
    #endif

    return TS_OK;
}

ts_ret_t ts_l1_deinit(ts_handle_t *h)
{
    UNUSED(h);

    #ifdef FPGA_REMOTE
    // Clear pin for controlling SPI access
    GPIO_InitTypeDef  GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_Delay(100);
    #endif

    HAL_SPI_MspDeInit(&SpiHandle);

    return TS_OK;
}

ts_ret_t ts_l1_spi_transfer(ts_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    if (offset + tx_data_length > TS_L1_LEN_MAX) {
        return TS_L1_DATA_LEN_ERROR;
    }
    int ret = HAL_SPI_TransmitReceive(&SpiHandle, h->l2_buff + offset, h->l2_buff + offset, tx_data_length, timeout);
    if(ret != HAL_OK) {
        return TS_L1_SPI_ERROR;
    }

    return TS_OK;
}


ts_ret_t ts_l1_delay(ts_handle_t *h, uint32_t ms)
{
    UNUSED(h);

    if (ms > TS_L1_DELAY_MS_MAX) {
        return TS_PARAM_ERR;
    }

    HAL_Delay(ms);

    return TS_OK;
}
