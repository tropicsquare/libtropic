/**
  ******************************************************************************
  * @file    UART/UART_Printf/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USART_DBG/UARTx instance used and associated
   resources */

/* Definition for USART_DBG clock resources */
#define USART_DBG                           USART3
#define USART_DBG_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
#define USART_DBG_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USART_DBG_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define USART_DBG_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USART_DBG_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()


#define USART_DBG_TX_PIN                    GPIO_PIN_8
#define USART_DBG_TX_GPIO_PORT              GPIOD
#define USART_DBG_TX_AF                     GPIO_AF7_USART3
#define USART_DBG_RX_PIN                    GPIO_PIN_9
#define USART_DBG_RX_GPIO_PORT              GPIOD
#define USART_DBG_RX_AF                     GPIO_AF7_USART3

/* Definition for USART_DBG Pins */
#ifdef USE_UART
/* UART used to communicate with model  */
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
// GPIOs
#define UART_MODEL_TX_PIN                    GPIO_PIN_12
#define UART_MODEL_TX_GPIO_PORT              GPIOC
#define UART_MODEL_TX_AF                     GPIO_AF8_UART5
#define UART_MODEL_RX_PIN                    GPIO_PIN_2
#define UART_MODEL_RX_GPIO_PORT              GPIOD
#define UART_MODEL_RX_AF                     GPIO_AF8_UART5
#endif
#ifdef USE_SPI
/* User can use this section to tailor SPIx instance used and associated
   resources */
/* Definition for SPIx clock resources */
#define SPIx                             SPI4
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI4_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI4_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI4_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_2
#define SPIx_SCK_GPIO_PORT               GPIOE
#define SPIx_SCK_AF                      GPIO_AF5_SPI4
#define SPIx_MISO_PIN                    GPIO_PIN_5
#define SPIx_MISO_GPIO_PORT              GPIOE
#define SPIx_MISO_AF                     GPIO_AF5_SPI4
#define SPIx_MOSI_PIN                    GPIO_PIN_6
#define SPIx_MOSI_GPIO_PORT              GPIOE
#define SPIx_MOSI_AF                     GPIO_AF5_SPI4
#endif
/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */
