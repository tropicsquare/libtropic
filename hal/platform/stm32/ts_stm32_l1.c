#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"
// Pin definitions are in main.h:
#include "main.h"

#include "libtropic_common.h"
#include "libtropic_platform.h"

#ifdef USE_UART

#define MIN_UART_PAYLOAD_LEN  3
#define MAX_UART_BUFFER_LEN  (MIN_UART_PAYLOAD_LEN + TS_L1_LEN_MAX)

// This structure holds data transported over uart
typedef struct
{
    union
    {
        uint8_t BUFFER[MAX_UART_BUFFER_LEN];
        struct __attribute__((__packed__, __aligned__))
        {
            uint8_t  TAG;
            uint16_t LENGTH;
            uint8_t  PAYLOAD[TS_L1_LEN_MAX];
        };
    };
} buffer_s;

// Possible values for TAG field
typedef enum
{
    TAG_E_SPI_DRIVE_CSN_LOW  = 0x01,
    TAG_E_SPI_DRIVE_CSN_HIGH = 0x02,
    TAG_E_SPI_SEND           = 0x03,
    TAG_E_POWER_ON           = 0x04,
    TAG_E_POWER_OFF          = 0x05,
    TAG_E_WAIT               = 0x06,
    TAG_E_RESET_TARGET       = 0x10,
    TAG_E_INVALID            = 0xfd,
    TAG_E_UNSUPPORTED        = 0xfe,
} tag_e;

// Uart handle
static UART_HandleTypeDef UartHandle = {0};
// reception buffer
static buffer_s rx_buffer;
// emission buffer
static buffer_s tx_buffer;


static int ts_communicate (int * tx_payload_length_ptr)
{
    int ret = HAL_ERROR;

    // number of bytes to send
    int nb_bytes_to_send = MIN_UART_PAYLOAD_LEN;
    if (tx_payload_length_ptr != NULL)
    {
        nb_bytes_to_send += *tx_payload_length_ptr;
    }

    // update payload length field
    tx_buffer.LENGTH = nb_bytes_to_send - MIN_UART_PAYLOAD_LEN;

    ret = HAL_UART_Transmit(&UartHandle, tx_buffer.BUFFER, nb_bytes_to_send, 0xFFF);
    if(ret != HAL_OK) {
        return TS_L1_SPI_ERROR;
    }

    ret = HAL_UART_Receive(&UartHandle, rx_buffer.BUFFER, MIN_UART_PAYLOAD_LEN, 0xFFFF);
    if(ret != HAL_OK) {
        return TS_L1_SPI_ERROR;
    }

    if (rx_buffer.LENGTH > 0) {
        ret = HAL_UART_Receive(&UartHandle, rx_buffer.PAYLOAD, rx_buffer.LENGTH, 0xFFFF);
        if(ret != HAL_OK) {
            return TS_L1_SPI_ERROR;
        }
    }

    if (rx_buffer.TAG == TAG_E_INVALID)
    {
        return TS_L1_SPI_ERROR;
    }
    // server does not know what to do with the sent tag
    else if (rx_buffer.TAG == TAG_E_UNSUPPORTED)
    {
        return TS_L1_SPI_ERROR;
    }
    // RX tag and TX tag should be identical
    else if (rx_buffer.TAG != tx_buffer.TAG)
    {
        return TS_L1_SPI_ERROR;
    }

    return TS_OK;
}

// This function simulates powering TROPIC01 ON
//static int ts_tropic01_power_on(void)
//{
//    tx_buffer.TAG = TAG_E_POWER_ON;
//    return ts_communicate(NULL);
//}
//
// This function simulates powering TROPIC01 OFF
//static int ts_tropic01_power_off(void)
//{
//    tx_buffer.TAG = TAG_E_POWER_OFF;
//    return ts_communicate(NULL);
//}

static int ts_reset_target(void)
{
    tx_buffer.TAG = TAG_E_RESET_TARGET;
    return ts_communicate(NULL);
}

ts_ret_t ts_l1_spi_csn_low(ts_handle_t *h)
{
    UNUSED(h);
    ////LOG_OUT("-- Driving Chip Select to Low.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_LOW;
    return ts_communicate(NULL);
}

ts_ret_t ts_l1_spi_csn_high(ts_handle_t *h)
{
    UNUSED(h);
    ////LOG_OUT("-- Driving Chip Select to High.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_HIGH;
    return ts_communicate(NULL);
}

ts_ret_t ts_l1_platform_init(ts_handle_t *h)
{
    UNUSED(h);

    // UART init:
    // Enable clock for used uart
    UART_MODEL_CLK_ENABLE();
    // UART configuration
    UartHandle.Instance        = UART_MODEL;
    UartHandle.Init.BaudRate   = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
      return TS_L1_SPI_ERROR;
    }

    // Empty buffers used to talk to model
    memset(tx_buffer.BUFFER, 0, MAX_UART_BUFFER_LEN);
    memset(rx_buffer.BUFFER, 0, MAX_UART_BUFFER_LEN);
    ts_reset_target();

    return TS_OK;
}

ts_ret_t ts_l1_platform_deinit(ts_handle_t *h)
{
    UNUSED(h);

    if (HAL_UART_DeInit(&UartHandle) != HAL_OK) {
      return TS_L1_SPI_ERROR;
    }

    return TS_OK;
}

ts_ret_t ts_l1_spi_transfer (ts_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(h);

    int status;
    int tx_payload_length = tx_data_length;
    int rx_payload_length = 0;

    tx_buffer.TAG    = TAG_E_SPI_SEND;
    tx_buffer.LENGTH = (uint16_t)tx_payload_length;

    // copy tx_data to tx payload
    memcpy(&tx_buffer.PAYLOAD, h->l2_buff, tx_payload_length);

    status = ts_communicate(&tx_payload_length);
    if (status != TS_OK) {
        return status;
    }
    rx_payload_length = rx_buffer.LENGTH + MIN_UART_PAYLOAD_LEN;

    memcpy(h->l2_buff + offset, &rx_buffer.PAYLOAD, rx_payload_length);

    return TS_OK;
}

ts_ret_t ts_l1_delay(ts_handle_t *h, uint32_t ms)
{
    UNUSED(h);

    HAL_Delay(ms);

    return TS_OK;
}

#else


#define TS_SPI_CS_BANK   GPIOE
#define TS_SPI_CS_PIN    GPIO_PIN_4
#define TS_SPI_INSTANCE  SPI4

// Use 1 here to enable SPI on FPGA board
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

ts_ret_t ts_l1_platform_init(ts_handle_t *h)
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

ts_ret_t ts_l1_platform_deinit(ts_handle_t *h)
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

    HAL_Delay(ms);

    return TS_OK;
}

#endif
