/**
  ******************************************************************************
  * @file    UART/UART_Printf/Src/main.c
  * @author  MCD Application Team
  * @brief   This example shows how to retarget the C library printf function
  *          to the UART.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "libtropic.h"
#include "cryptography_tests.h"

#include "ed25519.h"
#include "ts_sha256.h"

// Default initial Tropic handshake keys
#define PKEY_INDEX_BYTE   0
#define SHiPRIV_BYTES    {0xf0,0xc4,0xaa,0x04,0x8f,0x00,0x13,0xa0,0x96,0x84,0xdf,0x05,0xe8,0xa2,0x2e,0xf7,0x21,0x38,0x98,0x28,0x2b,0xa9,0x43,0x12,0xf3,0x13,0xdf,0x2d,0xce,0x8d,0x41,0x64};
#define SHiPUB_BYTES     {0x84,0x2f,0xe3,0x21,0xa8,0x24,0x74,0x08,0x37,0x37,0xff,0x2b,0x9b,0x88,0xa2,0xaf,0x42,0x44,0x2d,0xb0,0xd8,0xaa,0xcc,0x6d,0xc6,0x9e,0x99,0x53,0x33,0x44,0xb2,0x46};


#define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)
#define LOG_OUT_INFO(f_, ...) printf("\t[INFO] "f_, ##__VA_ARGS__)
#define LOG_OUT_VALUE(f_, ...) printf("\t\t\t\t\t"f_, ##__VA_ARGS__)
#define LOG_OUT_LINE(f_, ...) printf("\t-------------------------------------------------------------------------------------------------------------\r\n"f_, ##__VA_ARGS__)

void bytes_to_chars(uint8_t const *key, char *buffer, uint16_t len)
{
    uint16_t offset = 0;
    memset(buffer, 0, len);

    for (size_t i = 0; i < len; i++)
    {
        offset += sprintf(buffer + offset, "%02X", key[i]);
    }
    sprintf(buffer + offset, "%c", '\0');
}



/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup UART_Printf
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)
#define NUM_OF_PING_CMDS 1
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

/* UART handle declaration */
static UART_HandleTypeDef UartHandle;

/**
 * @brief   Configures the UART peripheral
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
static HAL_StatusTypeDef UART_Init(void)
{
  UartHandle.Instance        = USARTx;
  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
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
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
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
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  /* Initialize BSP Led for LED2 */
  BSP_LED_Init(LED2);

  if(UART_Init() != HAL_OK) {
    Error_Handler();
  }

    /************************************************************************************************************/
    /* Welcome banner */
    LOG_OUT("\r\n");
    LOG_OUT("\t\t======================================================================\r\n");
#if HAL_UART
    LOG_OUT("\t\t=====    libtropic stm32 demo, running over UART against model     ===\r\n");
#elif HAL_SPI
    LOG_OUT("\t\t=====    libtropic stm32 demo, running over SPI against FPGA       ===\r\n");
#endif
    LOG_OUT("\t\t======================================================================\r\n\n");

    LOG_OUT_INFO("List all possible return values:\r\n\n");
    for(int i=0; i<TS_L2_UNKNOWN_REQ+1; i++) {
        LOG_OUT_VALUE("(ts_ret_t) %d: \t%s \r\n", i,   ts_ret_verbose(i));
    }
    LOG_OUT("\r\n");
    /************************************************************************************************************/

    /************************************************************************************************************/
    LOG_OUT_INFO("Initialize transport layer (l1):\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ts_handle_t handle = {0};
    ts_ret_t ret = TS_FAIL;

    ret = ts_init(&handle);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_l1_init();\r\n");
    LOG_OUT_VALUE("%s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Get TROPIC01 chip's certificate\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t X509_cert[512] = {0};
    ret = ts_get_info_cert(&handle, X509_cert, 512);

    /* Following lines are only for printing results out: */
    char X509_cert_str[1024+1] = {0};
    bytes_to_chars(X509_cert, X509_cert_str, 512);
    LOG_OUT_INFO("ts_get_info_cert();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("X509 DER cert:       %s\r\n", X509_cert_str);
    //printf("\r\nCert debug:\r\n");
    //for (int x = 0; x<16;x++) {
    //  for(int i=0;i<32; i++) {
    //    printf("0x%02X,", X509_cert[i+32*x]);
    //  }
    //  printf("\r\n");
    //}
    //printf("\r\nEnd of cert debug\r\n");
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Establish a session by doing a handshake with provided keys\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t PKEY_INDEX  = PKEY_INDEX_BYTE;
    uint8_t SHiPRIV[]   = SHiPRIV_BYTES;
    uint8_t SHiPUB[]    = SHiPUB_BYTES;

    ret = ts_handshake(&handle, PKEY_INDEX, SHiPRIV, SHiPUB);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_handshake();\r\n");
    LOG_OUT_VALUE("%s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Test session by sending a ping command:\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t msg_out[L3_PING_MSG_MAX_LEN] = {0};
    uint8_t msg_in[L3_PING_MSG_MAX_LEN]  = {0};
    uint16_t len_ping = L3_PING_MSG_MAX_LEN;// rand() % L3_PING_MSG_MAX_LEN;

    for(int i=0; i<127;i++) {
        memcpy(msg_out + (32*i), "XSome content for Ping message Y", 32);
    }
    memcpy(msg_out + (32*127), "X The end of for Ping message Y", 31);

    ret = ts_ping(&handle, (uint8_t *)msg_out, (uint8_t *)msg_in, len_ping);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_ping();\r\n");
    LOG_OUT_VALUE("ts_ret_t:    %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("ping length: %d\r\n", len_ping);
    LOG_OUT_VALUE("msg compare: %s\r\n", !memcmp(msg_out, msg_in, L3_PING_MSG_MAX_LEN) ? "OK" : "ERROR");
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Get a few random bytes from the chip\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t buff[L3_RANDOM_VALUE_GET_LEN_MAX] = {0};
    uint16_t len_rand = 70;//L3_RANDOM_VALUE_GET_LEN_MAX;//rand() % L3_RANDOM_VALUE_GET_LEN_MAX;
    ret = ts_random_get(&handle, buff, len_rand);

    /* Following lines are only for printing results out: */
    char string[505] = {0};
    bytes_to_chars(buff, string, len_rand);
    LOG_OUT_INFO("ts_random_get();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("random length: %d\r\n", len_rand);
    LOG_OUT_VALUE("bytes:         %s\r\n", string);
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Generate L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_generate(&handle, ECC_SLOT_1, L3_ECC_KEY_GENERATE_CURVE_ED25519);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_ecc_key_generate();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Read public key corresponding to L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t key[64] = {0};
    uint8_t curve, origin;

    ret = ts_ecc_key_read(&handle, ECC_SLOT_1, key, 64, &curve, &origin);

    /* Following lines are only for printing results out: */
    uint8_t key_type = L3_ECC_KEY_GENERATE_CURVE_ED25519;
    int n_of_bytes_in_key = (key_type == L3_ECC_KEY_GENERATE_CURVE_ED25519 ? 32:64);
    char key_str[64] = {0};
    bytes_to_chars(key, key_str, n_of_bytes_in_key);
    LOG_OUT_INFO("ts_ecc_key_read();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("curve:         %s\r\n", (curve == L3_ECC_KEY_GENERATE_CURVE_ED25519 ? "ED25519" : "P256"));
    LOG_OUT_VALUE("origin:        %s\r\n", (origin == 0x01 ? "Generated" : "Saved"));
    LOG_OUT_VALUE("pubkey:        %s\r\n", key_str);
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("EDDSA Sign with L3_ECC_KEY_GENERATE_CURVE_ED25519 key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t msg[] = {'T','r','o','p','i','c',' ','S','q','u','a','r','e',' ','F','T','W','\0'};
    uint8_t rs[64];
    ret = ts_eddsa_sign(&handle, ECC_SLOT_1, msg, 17, rs, 64);

    /* Following lines are only for printing results out: */
    char R_str[64+1] = {0};
    char S_str[64+1] = {0};
    bytes_to_chars(rs, R_str, 32);
    bytes_to_chars(rs+32, S_str, 32);
    LOG_OUT_INFO("ts_eddsa_sign();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("msg:           %s\r\n", msg);
    LOG_OUT_VALUE("R:             %s\r\n", R_str);
    LOG_OUT_VALUE("S:             %s\r\n", S_str);
    LOG_OUT_INFO("Verify ED25519 signature on host side:\r\n");

    int verified = ed25519_sign_open(msg, 17, key, rs);
    LOG_OUT_VALUE("Signature is %s\r\n", verified == 0 ? "CORRECT" : "WRONG");
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase key in ECC_SLOT_1\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_erase(&handle, ECC_SLOT_1);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_ecc_key_erase();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Generate L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_generate(&handle, ECC_SLOT_2, L3_ECC_KEY_GENERATE_CURVE_P256);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_ecc_key_generate();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Read public key corresponding to L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t key2[64] = {0};
    uint8_t curve2, origin2;

    ret = ts_ecc_key_read(&handle, ECC_SLOT_2, key2, 64, &curve2, &origin2);

    /* Following lines are only for printing results out: */
    char key2_str[64+1] = {0};
    bytes_to_chars((uint8_t*)&key2, key2_str, 64);
    LOG_OUT_INFO("ts_ecc_key_read();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("curve:         %s\r\n", (curve2 == L3_ECC_KEY_GENERATE_CURVE_ED25519 ? "ED25519" : "P256"));
    LOG_OUT_VALUE("origin:        %s\r\n", (origin2 == 0x01 ? "Generated" : "Saved"));
    LOG_OUT_VALUE("pubkey:        %s\r\n", key2_str);
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("ECDSA Sign with L3_ECC_KEY_GENERATE_CURVE_P256 key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    uint8_t rs2[64];
    uint8_t ecdsa_msg[] = "Tropic Square FTW -- 32B message";
    uint8_t msg_hash[32];

    ts_sha256_ctx_t hctx = {0};
    ts_sha256_init(&hctx);
    ts_sha256_start(&hctx);
    ts_sha256_update(&hctx, ecdsa_msg, 32);
    ts_sha256_finish(&hctx, msg_hash);

    ret = ts_ecdsa_sign(&handle, ECC_SLOT_2, msg_hash, 32, rs2, 64);


    /* Following lines are only for printing results out: */
    char R_str2[64+1] = {0};
    char S_str2[64+1] = {0};
    char msg_hash_str[64+1] = {0};

    bytes_to_chars(rs2, R_str2, 32);
    bytes_to_chars(rs2+32, S_str2, 32);
    bytes_to_chars(msg_hash, msg_hash_str, 32);
    LOG_OUT_INFO("ts_eddsa_sign();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_VALUE("msg:           %s\r\n", ecdsa_msg);
    LOG_OUT_VALUE("MSG_HASH:      %s\r\n", msg_hash_str);
    LOG_OUT_VALUE("R:             %s\r\n", R_str2);
    LOG_OUT_VALUE("S:             %s\r\n", S_str2);



    /************************************************************************************************************/
    LOG_OUT_INFO("Erase key in ECC_SLOT_2\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_ecc_key_erase(&handle, ECC_SLOT_2);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_ecc_key_erase();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();



    /************************************************************************************************************/
    LOG_OUT_INFO("Denitialize transport layer (l1): :\r\n");
    /************************************************************************************************************/
    /* Example of a call: */

    ret = ts_deinit(&handle);

    /* Following lines are only for printing results out: */
    LOG_OUT_INFO("ts_l1_deinit();\r\n");
    LOG_OUT_VALUE("ts_ret_t:      %s\r\n", ts_ret_verbose(ret));
    LOG_OUT_LINE();

    LOG_OUT("\r\n\n");

  return 0;

  /* libtropic example code ends here */
  /* libtropic example code ends here */
  /* libtropic example code ends here */
  /* libtropic example code ends here */
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
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
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
