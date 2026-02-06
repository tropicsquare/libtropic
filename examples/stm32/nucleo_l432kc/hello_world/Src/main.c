/**
 * @file main.c
 * @brief Simple "Hello, World!" example of using Libtropic on STM32 Nucleo L432KC board.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 *
 * This example project is based on the SPI/SPI_FullDuplex_ComPolling example from STM32 example
 * library which was created by the MCD Application Team.
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <inttypes.h>
#include <stdio.h>

#include "libtropic.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_stm32_nucleo_l432kc.h"
#include "psa/crypto.h"

/** @addtogroup STM32L4xx_HAL_Examples
 * @{
 */

/** @addtogroup SPI_FullDuplex_ComPolling
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Message to send with Ping L3 command. */
#define PING_MSG "This is Hello World message from TROPIC01!!"
/* Size of the Ping message, including '\0'. */
#define PING_MSG_SIZE 44

/* Choose pairing keypair for slot 0. */
#if LT_USE_SH0_ENG_SAMPLE
#define LT_EX_SH0_PRIV sh0priv_eng_sample
#define LT_EX_SH0_PUB sh0pub_eng_sample
#elif LT_USE_SH0_PROD0
#define LT_EX_SH0_PRIV sh0priv_prod0
#define LT_EX_SH0_PUB sh0pub_prod0
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/* DEBUG UART handle declaration */
static UART_HandleTypeDef UartHandle;

/* RNG handle declaration */
RNG_HandleTypeDef RNGHandle;

/**
 * @brief   Configures the DEBUG UART peripheral
 *          Put the USART peripheral in the Asynchronous mode (UART Mode)
 *          UART configured as follows:
 *          - Word Length = 8 Bits (7 data bit + 1 parity bit) :
 *                        BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
 *          - Stop Bit    = One Stop bit
 *          - Parity      = NONE parity
 *          - BaudRate    = 115200 baud
 *          - Hardware flow control disabled (RTS and CTS signals)
 *
 * @return HAL_StatusTypeDef
 */
static HAL_StatusTypeDef DBG_UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    /* Enable the HSI clock */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        /* Error */
        while (1)
            ;
    }

    /* Configure HSI as USART clock source */
    USARTx_RCC_CONFIG(RCC_USARTxCLKSOURCE_HSI);

    /* Enable peripherals and GPIO Clocks */
    /* Enable GPIO TX/RX clock */
    USART_DBG_TX_GPIO_CLK_ENABLE();
    USART_DBG_RX_GPIO_CLK_ENABLE();
    /* Enable USART_DBG clock */
    USART_DBG_CLK_ENABLE();

    /* Configure UART pins */
    GPIO_InitStruct.Pin = USART_DBG_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART_DBG_TX_AF;
    HAL_GPIO_Init(USART_DBG_TX_GPIO_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = USART_DBG_RX_PIN;
    GPIO_InitStruct.Alternate = USART_DBG_RX_AF;
    HAL_GPIO_Init(USART_DBG_RX_GPIO_PORT, &GPIO_InitStruct);

    UartHandle.Instance = USART2;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
    /*  Translates LF to CFLF, as this is what most serial monitors expect
        by default
    */
    if (ch == '\n') {
        HAL_UART_Transmit(&UartHandle, (uint8_t *)"\r\n", 2, 0xFFFF);
    }
    else {
        HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);
    }

    return ch;
}

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
#ifdef MASTER_BOARD
    GPIO_InitTypeDef GPIO_InitStruct;
#endif

    /* STM32L4xx HAL library initialization:
         - Configure the Flash prefetch
         - Systick timer is configured by default as source of time base, but user
           can eventually implement his proper time base source (a general purpose
           timer for example or other time source), keeping in mind that Time base
           duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
           handled in milliseconds basis.
         - Set NVIC Group Priority to 4
         - Low Level Initialization
       */
    HAL_Init();

    /* Configure the system clock to 80 MHz */
    SystemClock_Config();

    /* Configure LED3 */
    BSP_LED_Init(LED3);

    if (DBG_UART_Init() != HAL_OK) {
        Error_Handler();
    }

    /* IMPORTANT: Initialize RNG peripheral.
       Do not forget to do this in your application, as the
       Libtropic HAL uses RNG for entropy source! */
    RNGHandle.Instance = RNG;
    if (HAL_RNG_Init(&RNGHandle) != HAL_OK) {
        Error_Handler();
    }

    /* libtropic related code BEGIN */
    /* libtropic related code BEGIN */
    /* libtropic related code BEGIN */
    /* libtropic related code BEGIN */
    /* libtropic related code BEGIN */

    printf("======================================\n");
    printf("==== TROPIC01 Hello World Example ====\n");
    printf("======================================\n");

    /* Cryptographic function provider initialization.

        In production, this would typically be done only once,
        usually at the start of the application or before
        the first use of cryptographic functions but no later than
        the first occurrence of any Libtropic function */
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "PSA Crypto initialization failed, status=%ld (psa_status_t)\n", status);
        return -1;
    }

    /* Libtropic handle.

        It is declared here (on stack) for
        simplicity. In production, you put it on heap if needed. */
    lt_handle_t lt_handle = {0};

    /* Device structure.

        Modify this according to your environment. Default values
        are compatible with RPi and our RPi shield.

        The device structure has to be zero initialized!
        STM32 HAL depends on zero init values. */
    lt_dev_stm32_nucleo_l432kc_t device = {0};

    device.spi_instance = LT_SPI_INSTANCE;
    device.baudrate_prescaler = SPI_BAUDRATEPRESCALER_16;

    /* Enable clock of the GPIO bank where our custom chip select output is present. */
    LT_SPI_CS_CLK_ENABLE(); /* Defined in main.h. */
    device.spi_cs_gpio_bank = LT_SPI_CS_BANK;
    device.spi_cs_gpio_pin = LT_SPI_CS_PIN;

    /* IMPORTANT: Do not forget to initialize RNG peripheral
        at the beginning of your application using HAL_RNG_Init()! */
    device.rng_handle = &RNGHandle;

    lt_handle.l2.device = &device;

    /* Crypto abstraction layer (CAL) context. */
    lt_ctx_mbedtls_v4_t crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    printf("Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to initialize handle, ret=%s\n", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    /* We need to ensure we are not in the Startup Mode, as L3 commands are available only in the
     * Application Firmware.
     */
    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    /* Keys are chosen based on the CMake option LT_SH0_KEYS. */
    ret = lt_verify_chip_and_start_secure_session(&lt_handle, LT_EX_SH0_PRIV, LT_EX_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_0, lt_ret_verbose(ret));
        fprintf(stderr,
                "Check if you use correct SH0 keys! Hint: if you use an engineering sample chip, "
                "compile with "
                "-DLT_SH0_KEYS=eng_sample\n");
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    uint8_t recv_buf[PING_MSG_SIZE];
    printf("Sending Ping command...\n");
    printf("\t--> Message sent to TROPIC01: '%s'\n", PING_MSG);
    ret = lt_ping(&lt_handle, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        fprintf(stderr, "Ping command failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("\t<-- Message received from TROPIC01: '%s'\n", recv_buf);

    printf("Aborting Secure Session...");
    ret = lt_session_abort(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    /* Cryptographic function provider deinitialization.

        In production, this would be done only once, typically
        during termination of the application. */
    mbedtls_psa_crypto_free();

    /* libtropic related code END */
    /* libtropic related code END */
    /* libtropic related code END */
    /* libtropic related code END */
    /* libtropic related code END */

    /* Not strictly necessary, but we deinitialize RNG here to
       demonstrate proper usage. */
    if (HAL_RNG_DeInit(&RNGHandle) != HAL_OK) {
        Error_Handler();
    }

    /* Infinite loop */
    while (1) {
        BSP_LED_Toggle(LED3);
        HAL_Delay(200);
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
    while (1) {
        /* Toggle LED3 for error */
        BSP_LED_Toggle(LED3);
        HAL_Delay(1000);
    }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follows :
 *            System Clock source            = PLL (MSI)
 *            SYSCLK(Hz)                     = 80000000
 *            HCLK(Hz)                       = 80000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 4000000
 *            PLL_M                          = 1
 *            PLL_N                          = 40
 *            PLL_R                          = 2
 *            PLL_P                          = 7
 *            PLL_Q                          = 4
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* MSI is enabled after System reset, activate PLL with MSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        /* Initialization Error */
        while (1)
            ;
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                   RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        /* Initialization Error */
        while (1)
            ;
    }
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
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif

/**
 * @}
 */

/**
 * @}
 */
