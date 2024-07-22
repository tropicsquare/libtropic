#include <stdint.h>

#include "ts_common.h"
#include "ts_l1.h"

#include "stm32f4xx_hal.h"

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Definition for UART used to communicate with model               //
//                                                                  //
//////////////////////////////////////////////////////////////////////

/*
    Nucleo F439ZI, CN8 Connector:
          _ _
         |_|_|
         |_|_|
         |_|_|
         |_|_|
         |_|_| TX (RX on serial adapter)
    GND  |_|_| RX (TX on serial adapter)
    GND  |_|_|
         |_|_|
*/

#define UART_MODEL                           UART5
#define UART_MODEL_CLK_ENABLE()              __HAL_RCC_UART5_CLK_ENABLE();
#define UART_MODEL_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define UART_MODEL_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART_MODEL_FORCE_RESET()             __HAL_RCC_UART5_FORCE_RESET()
#define UART_MODEL_RELEASE_RESET()           __HAL_RCC_UART5_RELEASE_RESET()

// Used pins
#define UART_MODEL_TX_PIN                    GPIO_PIN_12
#define UART_MODEL_TX_GPIO_PORT              GPIOC
#define UART_MODEL_TX_AF                     GPIO_AF8_UART5
#define UART_MODEL_RX_PIN                    GPIO_PIN_2
#define UART_MODEL_RX_GPIO_PORT              GPIOD
#define UART_MODEL_RX_AF                     GPIO_AF8_UART5\

// #define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////
//                                                                  //
// End of user setup                                                //
//                                                                  //
//////////////////////////////////////////////////////////////////////


// Uart transport layer defines
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
static UART_HandleTypeDef UartHandle;
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

//static int ts_tropic01_power_on(void)
//{
//    tx_buffer.TAG = TAG_E_POWER_ON;
//    return ts_communicate(NULL);
//}
//
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

//////////////////////////////////////////////////////////////////////
//                                                                  //
// ts_l1 HAL definitions:                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

ts_ret_t ts_l1_spi_csn_low(const ts_handle_t *h)
{
    UNUSED(h);
    ////LOG_OUT("-- Driving Chip Select to Low.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_LOW;
    return ts_communicate(NULL);
}

ts_ret_t ts_l1_spi_csn_high(const ts_handle_t *h)
{
    UNUSED(h);
    ////LOG_OUT("-- Driving Chip Select to High.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_HIGH;
    return ts_communicate(NULL);
}

ts_ret_t ts_l1_init(ts_handle_t *h)
{
    UNUSED(h);

    GPIO_InitTypeDef  GPIO_InitStruct;

    // enable clock for gpio
    UART_MODEL_TX_GPIO_CLK_ENABLE();
    UART_MODEL_RX_GPIO_CLK_ENABLE();
    // And enable clock for used uart
    UART_MODEL_CLK_ENABLE();

    // GPIO config:
    // UART TX GPIO pin configuration
    GPIO_InitStruct.Pin       = UART_MODEL_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART_MODEL_TX_AF;

    HAL_GPIO_Init(UART_MODEL_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = UART_MODEL_RX_PIN;
    GPIO_InitStruct.Alternate = UART_MODEL_RX_AF;

    HAL_GPIO_Init(UART_MODEL_RX_GPIO_PORT, &GPIO_InitStruct);

    bzero(tx_buffer.BUFFER, MAX_UART_BUFFER_LEN);
    bzero(rx_buffer.BUFFER, MAX_UART_BUFFER_LEN);

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

    ts_reset_target();

    return TS_OK;
}

ts_ret_t ts_l1_deinit(ts_handle_t *h)
{
    UNUSED(h);

    // Deinitialize serial port used as a transport layer for tropic chip
    UART_MODEL_FORCE_RESET();
    UART_MODEL_RELEASE_RESET();

    HAL_GPIO_DeInit(UART_MODEL_TX_GPIO_PORT, UART_MODEL_TX_PIN);
    HAL_GPIO_DeInit(UART_MODEL_RX_GPIO_PORT, UART_MODEL_RX_PIN);

    return TS_OK;
}

ts_ret_t ts_l1_spi_transfer(const ts_handle_t *h, const uint8_t * tx_data, uint8_t * rx_data,
                                               uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(h);
    // LOG_OUT("\r\n\tAhoj :)\r\n");
    int status;
    int tx_payload_length = tx_data_length;
    int rx_payload_length = 0;

    tx_buffer.TAG    = TAG_E_SPI_SEND;
    tx_buffer.LENGTH = (uint16_t)tx_payload_length;

    // copy tx_data to tx payload
    for (int i = 0; i < tx_payload_length; i++) {
        *(tx_buffer.PAYLOAD + i) = *(tx_data + i);
    }

    status = ts_communicate(&tx_payload_length);
    if (status != 0) {
        return status;
    }
    rx_payload_length = rx_buffer.LENGTH + MIN_UART_PAYLOAD_LEN;
    // LOG_OUT("\r\n\trx_payload_length: %d\r\n", rx_payload_length);


    // copy rx payload to rx_data
    for (int i = 0; i < rx_payload_length; i++) {
        *(rx_data + i) = *(rx_buffer.PAYLOAD + i);
    }

    return TS_OK;
}


ts_ret_t ts_l1_delay(const ts_handle_t *h, const uint32_t ms)
{
    UNUSED(h);
    if (ms > TS_L1_DELAY_MS_MAX) {
        return TS_PARAM_ERR;
    }

    HAL_Delay(ms);

    return TS_OK;
}
