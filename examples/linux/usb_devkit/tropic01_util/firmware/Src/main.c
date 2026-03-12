/* USER CODE BEGIN Header */
/**
 * @file main.c
 * @brief Main source file for the tropic01_util application on TROPIC01 USB DevKit.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

// This file was generated using Stm32CubeMX and modified by Tropic Square.

/**
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_logging.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_stm32u5xx.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "psa/crypto.h"
#include "tusb.h"
#include "usb_devkit_messages.pb.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/** @brief Used for tracking the state of the USB Devkit's main loop. */
typedef enum {
    READ_MAGIC_BYTE_1,
    READ_MAGIC_BYTE_2,
    READ_DATA_LEN,
    READ_DATA,
    READ_VERIFY_CRC,
    PROCESS_DATA,
    WRITE_DATA
} usb_devkit_state_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define FRAME_MAGIC_BYTE_1 0xAA
#define FRAME_MAGIC_BYTE_2 0x55
#define FRAME_MAGIC_BYTES 2
#define FRAME_DATA_LEN_SIZE 2
#define FRAME_DATA_MAX_SIZE 4093
#define FRAME_CRC_SIZE 2
#define FRAME_MAX_SIZE (FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + FRAME_DATA_MAX_SIZE + FRAME_CRC_SIZE)

#define USB_READ_TIMEOUT_MS 50

#define CHECK_LT_RET           \
    {                          \
        if (LT_OK != lt_ret) { \
            Error_Handler();   \
        }                      \
    }

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

DCACHE_HandleTypeDef hdcache1;

RNG_HandleTypeDef hrng;

SPI_HandleTypeDef hspi1;

PCD_HandleTypeDef hpcd_USB_DRD_FS;

/* USER CODE BEGIN PV */

usb_devkit_state_t usb_devkit_state = READ_MAGIC_BYTE_1;
bool auto_cs_mode = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_DCACHE1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_RNG_Init(void);
static void MX_USB_DRD_FS_PCD_Init(void);

static size_t usb_read_chunk(uint8_t *buff, size_t to_read);
static size_t usb_write_chunk(const uint8_t *buff, size_t to_write);
static void process_raw_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp);
static void process_app_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp);
static bool process_pb_data(const uint8_t *pb_data_in, size_t pb_data_in_len, uint8_t *pb_data_out,
                            size_t pb_data_out_size, size_t *pb_data_out_len);
static void construct_frame(const uint8_t *data, size_t data_len, uint8_t *frame, size_t *frame_len);

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

/* USER CODE BEGIN PFP */

/**
 * @brief Read up to `to_read` bytes from the USB CDC RX FIFO into `buff`.
 *
 * This helper is non-blocking: when no bytes are currently available, it
 * returns 0. When data is available, it reads at most `to_read` bytes.
 *
 * @param[out] buff Destination buffer for received bytes.
 * @param[in] to_read Maximum number of bytes to read in this call.
 * @return Number of bytes actually read.
 */
static size_t usb_read_chunk(uint8_t *buff, size_t to_read)
{
    uint32_t avail_bytes = tud_cdc_available();
    if (avail_bytes == 0 || to_read == 0) {
        return 0;
    }

    size_t chunk_size = MIN(to_read, (size_t)avail_bytes);
    return (size_t)tud_cdc_read(buff, chunk_size);
}

/**
 * @brief Write up to `to_write` bytes to the USB CDC TX FIFO from `buff`.
 *
 * This helper is non-blocking: when there is no room in the TX FIFO, it
 * calls tud_cdc_write_flush() and returns 0. When room is available, it
 * writes at most `to_write` bytes.
 *
 * @param[in] buff Source buffer with bytes to write.
 * @param[in] to_write Number of bytes to write in this call.
 * @return Number of bytes actually written.
 */
static size_t usb_write_chunk(const uint8_t *buff, size_t to_write)
{
    uint32_t free_bytes = tud_cdc_write_available();
    if (free_bytes == 0 || to_write == 0) {
        tud_cdc_write_flush();
        return 0;
    }

    size_t chunk_size = MIN(to_write, (size_t)free_bytes);
    return (size_t)tud_cdc_write(buff, chunk_size);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// tinyUSB calls this function when the USB bus is reset or unplugged.
void tud_umount_cb(void) { usb_devkit_state = READ_MAGIC_BYTE_1; }

// tinyUSB calls this function when the USB is plugged and recognized.
void tud_mount_cb(void) { usb_devkit_state = READ_MAGIC_BYTE_1; }

/**
 * @brief Process USB DevKit raw command.
 *
 * @param[in]  cmd   USB DevKit command.
 * @param[out] resp  USB DevKit response.
 */
static void process_raw_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
{
    resp->which_type = UsbDevkitResp_raw_tag;
    resp->type.raw.result_code = RAW_RESP_RESULT_CODE_OK;

    switch (cmd->type.raw.which_type) {
        case RawCmd_send_spi_data_tag:
            // TODO: Implement direct SPI transfer and fill SendSpiDataResp.rx_data.
            resp->type.raw.result_code = RAW_RESP_RESULT_CODE_SPI_ERROR;
            break;

        case RawCmd_set_auto_cs_mode_tag:
            auto_cs_mode = cmd->type.raw.type.set_auto_cs_mode.on;
            break;

        case RawCmd_set_cs_tag:
            if (auto_cs_mode) {
                resp->type.raw.result_code = RAW_RESP_RESULT_CODE_AUTO_CS_MODE_ON;
            }
            else {
                HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin,
                                  cmd->type.raw.type.set_cs.high ? GPIO_PIN_SET : GPIO_PIN_RESET);
            }
            break;

        case RawCmd_set_tr01_pwr_tag:
            HAL_GPIO_WritePin(TR01_PWR_GPIO_Port, TR01_PWR_Pin,
                              cmd->type.raw.type.set_tr01_pwr.on ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case RawCmd_get_gpo_tag:
            resp->type.raw.which_type = RawResp_get_gpo_tag;
            resp->type.raw.type.get_gpo.high = (HAL_GPIO_ReadPin(TR01_GPO_GPIO_Port, TR01_GPO_Pin) ==
                                                GPIO_PIN_SET);
            break;

        default:
            resp->type.raw.result_code = RAW_RESP_RESULT_CODE_UNKNOWN_CMD;
            break;
    }
}

static void process_app_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
{
    (void)cmd;
    // TODO: Implement AppCmd handling via libtropic API calls.
    resp->which_type = UsbDevkitResp_error_tag;
    resp->type.error.code = ERROR_RESP_CODE_PB_UNKNOWN_CMD;
}

/**
 * @brief Decode UsbDevkitCmd from payload, execute command, and encode UsbDevkitResp.
 *
 * @param[in]  pb_data_in        Input protobuf bytes.
 * @param[in]  pb_data_in_len    Length of input protobuf bytes.
 * @param[out] pb_data_out       Output buffer for encoded UsbDevkitResp protobuf bytes.
 * @param[in]  pb_data_out_size  Size of pb_data_out buffer.
 * @param[out] pb_data_out_len   Number of bytes written to pb_data_out.
 * @retval     true              Response encoded successfully.
 * @retval     false             Response could not be encoded.
 */
static bool process_pb_data(const uint8_t *pb_data_in, size_t pb_data_in_len, uint8_t *pb_data_out,
                            size_t pb_data_out_size, size_t *pb_data_out_len)
{
    UsbDevkitCmd cmd = UsbDevkitCmd_init_zero;
    UsbDevkitResp resp = UsbDevkitResp_init_zero;

    pb_istream_t pb_in = pb_istream_from_buffer(pb_data_in, pb_data_in_len);
    if (!pb_decode(&pb_in, UsbDevkitCmd_fields, &cmd)) {
        resp.which_type = UsbDevkitResp_error_tag;
        resp.type.error.code = ERROR_RESP_CODE_PB_DECODE;
    }
    else {
        switch (cmd.which_type) {
            case UsbDevkitCmd_raw_tag:
                process_raw_cmd(&cmd, &resp);
                break;

            case UsbDevkitCmd_app_tag:
                process_app_cmd(&cmd, &resp);
                break;

            default:
                resp.which_type = UsbDevkitResp_error_tag;
                resp.type.error.code = ERROR_RESP_CODE_PB_UNKNOWN_CMD;
                break;
        }
    }

    pb_ostream_t pb_out = pb_ostream_from_buffer(pb_data_out, pb_data_out_size);
    if (!pb_encode(&pb_out, UsbDevkitResp_fields, &resp)) {
        return false;
    }

    *pb_data_out_len = pb_out.bytes_written;
    return true;
}

/**
 * @brief Construct the frame to send.
 *
 * @param[in]  data       DATA field in the frame.
 * @param[in]  data_len   DATA_LEN field in the frame.
 * @param[out] frame      Constructed frame.
 * @param[out] frame_len  Constructed frame length.
 */
static void construct_frame(const uint8_t *data, size_t data_len, uint8_t *frame, size_t *frame_len)
{
    // 1. Place magic bytes into the frame.
    frame[0] = FRAME_MAGIC_BYTE_2;
    frame[1] = FRAME_MAGIC_BYTE_1;
    // 2. Place DATA_LEN into the frame.
    frame[2] = (data_len >> 8) & 0xFF;
    frame[3] = data_len & 0xFF;
    // 3. Place DATA into the frame.
    memcpy(frame + 4, data, data_len);

    // 4. Calculate CRC16 over DATA_LEN || DATA and append it as big-endian.
    size_t data_pos = FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE;
    uint32_t data_len_word = 0;  // Needed because HAL_CRC_Calculate accepts uint32_t.
    memcpy(&data_len_word, frame + FRAME_MAGIC_BYTES, FRAME_DATA_LEN_SIZE);
    uint32_t out_crc_hw = HAL_CRC_Calculate(&hcrc, &data_len_word, (uint32_t)FRAME_DATA_LEN_SIZE);
    out_crc_hw = HAL_CRC_Accumulate(&hcrc, (uint32_t *)(void *)(frame + data_pos), (uint32_t)data_len);
    // 5. Get the calculated CRC.
    uint16_t out_crc = (uint16_t)(out_crc_hw & 0xFFFF);
    size_t out_crc_pos = FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + data_len;
    // 6. Place CRC into the frame.
    frame[out_crc_pos] = (uint8_t)((out_crc >> 8) & 0xFF);
    frame[out_crc_pos + 1] = (uint8_t)(out_crc & 0xFF);

    // 7. Calculate frame length.
    *frame_len = FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + data_len + FRAME_CRC_SIZE;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    // Note: SPI is initialized by Libtropic.
    MX_GPIO_Init();
    MX_CRC_Init();
    MX_DCACHE1_Init();
    MX_ICACHE_Init();
    MX_RNG_Init();
    MX_USB_DRD_FS_PCD_Init();
    /* USER CODE BEGIN 2 */

    // Initialize USB.
    if (!tusb_init()) {
        Error_Handler();
    }

    // Power up TROPIC01.
    HAL_GPIO_WritePin(TR01_PWR_GPIO_Port, TR01_PWR_Pin, GPIO_PIN_SET);

    // Setup Libtropic.
    lt_handle_t lt_handle = {0};
    lt_dev_stm32u5xx_t lt_device = {0};
    lt_ctx_mbedtls_v4_t lt_crypto_ctx = {0};
    lt_ret_t lt_ret;

    lt_device.spi_instance = SPI1;
    lt_device.baudrate_prescaler = SPI_BAUDRATEPRESCALER_32;
    lt_device.spi_cs_gpio_bank = TR01_CS_GPIO_Port;
    lt_device.spi_cs_gpio_pin = TR01_CS_Pin;
    lt_device.rng_handle = &hrng;
#ifdef LT_USE_INT_PIN
    lt_device.int_gpio_bank = TR01_GPO_GPIO_Port;
    lt_device.int_gpio_pin = TR01_GPO_Pin;
#endif

    lt_handle.l2.device = &lt_device;
    lt_handle.l3.crypto_ctx = &lt_crypto_ctx;

    // Initialize MbedTLS.
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        Error_Handler();
    }

    // Initialize Libtropic.
    lt_ret = lt_init(&lt_handle);
    CHECK_LT_RET;

    // Turn on the LED to indicate that the initialization succeeded.
    HAL_GPIO_WritePin(APP_LED_GPIO_Port, APP_LED_Pin, GPIO_PIN_SET);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    // Variables for the DATA_LEN field in the frame.
    uint8_t in_data_len_bytes[FRAME_DATA_LEN_SIZE];  // Incoming DATA_LEN bytes.
    size_t in_data_len_bytes_read = 0;               // Number of incoming DATA_LEN bytes read so far.
    size_t in_data_len = 0;                          // Parsed incoming DATA_LEN.
    // Variables for DATA field in the frame.
    uint8_t in_data[FRAME_DATA_MAX_SIZE];  // Incoming DATA bytes.
    size_t in_data_read = 0;               // Number of incoming DATA bytes read so far.
    uint8_t out_frame[FRAME_MAX_SIZE];     // Outgoing frame bytes.
    size_t out_frame_len = 0;              // Actual number of outgoing frame bytes.
    size_t out_frame_written = 0;          // Number of outgoing frame bytes written so far.
    // Variables for CRC field in the frame.
    uint8_t in_crc[FRAME_CRC_SIZE];  // Incoming CRC bytes.
    size_t in_crc_read = 0;          // Number of incoming CRC bytes read so far.
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        // TinyUSB task.
        tud_task();

        switch (usb_devkit_state) {
            case READ_MAGIC_BYTE_1: {
                uint8_t magic_byte_1;
                if (tud_cdc_read(&magic_byte_1, 1) == 1 && magic_byte_1 == FRAME_MAGIC_BYTE_1) {
                    usb_devkit_state = READ_MAGIC_BYTE_2;
                }
                break;
            }

            case READ_MAGIC_BYTE_2: {
                uint8_t magic_byte_2;
                if (tud_cdc_read(&magic_byte_2, 1) == 1) {
                    if (magic_byte_2 == FRAME_MAGIC_BYTE_2) {
                        in_data_len_bytes_read = 0;
                        usb_devkit_state = READ_DATA_LEN;
                    }
                    else {
                        usb_devkit_state = READ_MAGIC_BYTE_1;
                    }
                }
                break;
            }

            case READ_DATA_LEN: {
                uint8_t *read_ptr = in_data_len_bytes + in_data_len_bytes_read;
                size_t to_read = FRAME_DATA_LEN_SIZE - in_data_len_bytes_read;

                in_data_len_bytes_read += usb_read_chunk(read_ptr, to_read);
                if (in_data_len_bytes_read == FRAME_DATA_LEN_SIZE) {
                    // DATA_LEN comes as big-endian.
                    in_data_len = ((size_t)in_data_len_bytes[0] << 8) | (size_t)in_data_len_bytes[1];

                    if (in_data_len == 0 || in_data_len > FRAME_DATA_MAX_SIZE) {
                        usb_devkit_state = READ_MAGIC_BYTE_1;
                    }
                    else {
                        in_data_read = 0;
                        usb_devkit_state = READ_DATA;
                    }
                }
                break;
            }

            case READ_DATA: {
                uint8_t *read_ptr = in_data + in_data_read;
                size_t to_read = in_data_len - in_data_read;

                in_data_read += usb_read_chunk(read_ptr, to_read);
                if (in_data_read == in_data_len) {
                    in_crc_read = 0;
                    usb_devkit_state = READ_VERIFY_CRC;
                }
                break;
            }

            case READ_VERIFY_CRC: {
                uint8_t *read_ptr = in_crc + in_crc_read;
                size_t to_read = FRAME_CRC_SIZE - in_crc_read;

                in_crc_read += usb_read_chunk(read_ptr, to_read);
                if (in_crc_read == FRAME_CRC_SIZE) {
                    // CRC comes as big-endian.
                    uint16_t in_crc_parsed = ((uint16_t)in_crc[0] << 8) | (uint16_t)in_crc[1];
                    // 1. Calculate CRC for DATA_LEN.
                    uint32_t frame_data_len_word = 0;
                    memcpy(&frame_data_len_word, in_data_len_bytes, FRAME_DATA_LEN_SIZE);
                    HAL_CRC_Calculate(&hcrc, &frame_data_len_word, (uint32_t)FRAME_DATA_LEN_SIZE);
                    // 2. Calculate CRC for DATA.
                    uint32_t in_crc_hw = HAL_CRC_Accumulate(&hcrc, (uint32_t *)(void *)in_data,
                                                            (uint32_t)in_data_len);
                    // 3. Compare CRCs.
                    if (in_crc_parsed == (uint16_t)(in_crc_hw & 0xFFFF)) {
                        usb_devkit_state = PROCESS_DATA;
                    }
                    else {
                        usb_devkit_state = READ_MAGIC_BYTE_1;
                    }
                }
                break;
            }

            case PROCESS_DATA: {
                // 1. Process protobuf payload.
                uint8_t pb_out[UsbDevkitResp_size] = {0};
                size_t pb_out_len = 0;
                if (!process_pb_data(in_data, in_data_len, pb_out, sizeof(pb_out), &pb_out_len)) {
                    Error_Handler();
                }
                // 2. Construct frame.
                construct_frame(pb_out, pb_out_len, out_frame, &out_frame_len);
                out_frame_written = 0;
                usb_devkit_state = WRITE_DATA;
                break;
            }

            case WRITE_DATA: {
                uint8_t *write_ptr = out_frame + out_frame_written;
                size_t to_write = out_frame_len - out_frame_written;

                out_frame_written += usb_write_chunk(write_ptr, to_write);
                if (out_frame_written == out_frame_len) {
                    tud_cdc_write_flush();
                    usb_devkit_state = READ_MAGIC_BYTE_1;
                }
                break;
            }
        }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 1;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{
    /* USER CODE BEGIN CRC_Init 0 */

    /* USER CODE END CRC_Init 0 */

    /* USER CODE BEGIN CRC_Init 1 */

    /* USER CODE END CRC_Init 1 */
    hcrc.Instance = CRC;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
    hcrc.Init.GeneratingPolynomial = 32773;
    hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
    hcrc.Init.InitValue = 0;
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    if (HAL_CRC_Init(&hcrc) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN CRC_Init 2 */

    /* USER CODE END CRC_Init 2 */
}

/**
 * @brief DCACHE1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_DCACHE1_Init(void)
{
    /* USER CODE BEGIN DCACHE1_Init 0 */

    /* USER CODE END DCACHE1_Init 0 */

    /* USER CODE BEGIN DCACHE1_Init 1 */

    /* USER CODE END DCACHE1_Init 1 */
    hdcache1.Instance = DCACHE1;
    hdcache1.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
    if (HAL_DCACHE_Init(&hdcache1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN DCACHE1_Init 2 */

    /* USER CODE END DCACHE1_Init 2 */
}

/**
 * @brief ICACHE Initialization Function
 * @param None
 * @retval None
 */
static void MX_ICACHE_Init(void)
{
    /* USER CODE BEGIN ICACHE_Init 0 */

    /* USER CODE END ICACHE_Init 0 */

    /* USER CODE BEGIN ICACHE_Init 1 */

    /* USER CODE END ICACHE_Init 1 */

    /** Enable instruction cache (default 2-ways set associative cache)
     */
    if (HAL_ICACHE_Enable() != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ICACHE_Init 2 */

    /* USER CODE END ICACHE_Init 2 */
}

/**
 * @brief RNG Initialization Function
 * @param None
 * @retval None
 */
static void MX_RNG_Init(void)
{
    /* USER CODE BEGIN RNG_Init 0 */

    /* USER CODE END RNG_Init 0 */

    /* USER CODE BEGIN RNG_Init 1 */

    /* USER CODE END RNG_Init 1 */
    hrng.Instance = RNG;
    hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN RNG_Init 2 */

    /* USER CODE END RNG_Init 2 */
}

/**
 * @brief USB_DRD_FS Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_DRD_FS_PCD_Init(void)
{
    /* USER CODE BEGIN USB_DRD_FS_Init 0 */

    /* USER CODE END USB_DRD_FS_Init 0 */

    /* USER CODE BEGIN USB_DRD_FS_Init 1 */

    /* USER CODE END USB_DRD_FS_Init 1 */
    hpcd_USB_DRD_FS.Instance = USB_DRD_FS;
    hpcd_USB_DRD_FS.Init.dev_endpoints = 8;
    hpcd_USB_DRD_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_DRD_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_DRD_FS.Init.Sof_enable = DISABLE;
    hpcd_USB_DRD_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_DRD_FS.Init.lpm_enable = DISABLE;
    hpcd_USB_DRD_FS.Init.battery_charging_enable = DISABLE;
    hpcd_USB_DRD_FS.Init.vbus_sensing_enable = DISABLE;
    hpcd_USB_DRD_FS.Init.bulk_doublebuffer_enable = DISABLE;
    hpcd_USB_DRD_FS.Init.iso_singlebuffer_enable = DISABLE;
    if (HAL_PCD_Init(&hpcd_USB_DRD_FS) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USB_DRD_FS_Init 2 */

    // Map the Packet Memory Area (PMA) for the CDC ACM Endpoints
    HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00, PCD_SNG_BUF, 0x14);   // 0x00 = EP0 OUT
    HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80, PCD_SNG_BUF, 0x54);   // 0x80 = EP0 IN
    HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x81, PCD_SNG_BUF, 0x94);   // 0x81 = EP1 IN (Data)
    HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x01, PCD_SNG_BUF, 0xD4);   // 0x01 = EP1 OUT (Data)
    HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x82, PCD_SNG_BUF, 0x114);  // 0x82 = EP2 IN (Cmd)

    /* USER CODE END USB_DRD_FS_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    // We set the USB_DP_Pin here to 0 to force the USB host to re-enumerate when the device resets.
    // This is needed after entering the DFU Mode, so the user doesn't have to replug the device after
    // a firmware update.
    HAL_GPIO_WritePin(GPIOA, TR01_PWR_Pin | APP_LED_Pin | USB_DP_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin, GPIO_PIN_SET);

    /*Configure GPIO pins : TR01_PWR_Pin TR01_CS_Pin APP_LED_Pin */
    GPIO_InitStruct.Pin = TR01_PWR_Pin | TR01_CS_Pin | APP_LED_Pin | USB_DP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : TR01_GPO_Pin */
    GPIO_InitStruct.Pin = TR01_GPO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TR01_GPO_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */

    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief  Retargets the C library printf function to the USB.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
    /*  Translates LF to CFLF, as this is what most serial monitors expect
        by default
    */
    if (ch == '\n') {
        tud_cdc_write("\r\n", 2);
    }
    else {
        tud_cdc_write(&ch, 1);
    }

    tud_cdc_write_flush();
    return ch;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
        HAL_GPIO_WritePin(APP_LED_GPIO_Port, APP_LED_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(APP_LED_GPIO_Port, APP_LED_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
