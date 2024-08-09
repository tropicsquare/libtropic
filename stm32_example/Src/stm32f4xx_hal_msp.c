/**
  ******************************************************************************
  * @file    UART/UART_Printf/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct = {0};

  /* Init pins for Debug uart port */
  if(huart->Instance == USART_DBG) {
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USART_DBG_TX_GPIO_CLK_ENABLE();
    USART_DBG_RX_GPIO_CLK_ENABLE();

    /* Enable USART_DBG clock */
    USART_DBG_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USART_DBG_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = USART_DBG_TX_AF;

    HAL_GPIO_Init(USART_DBG_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USART_DBG_RX_PIN;
    GPIO_InitStruct.Alternate = USART_DBG_RX_AF;

    HAL_GPIO_Init(USART_DBG_RX_GPIO_PORT, &GPIO_InitStruct);
  /* Init pins for Model uart port */
  }
#ifdef USE_UART
  else if(huart->Instance == UART_MODEL) {

   // GPIOs init:
    // Initialize GPIO pins used by UART
    GPIO_InitTypeDef  GPIO_InitStruct;
    // Enable clock for used gpios
    UART_MODEL_TX_GPIO_CLK_ENABLE();
    UART_MODEL_RX_GPIO_CLK_ENABLE();
    // UART TX GPIO pin configuration
    GPIO_InitStruct.Pin       = UART_MODEL_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = UART_MODEL_TX_AF;
    HAL_GPIO_Init(UART_MODEL_TX_GPIO_PORT, &GPIO_InitStruct);
    // UART RX GPIO pin configuration
    GPIO_InitStruct.Pin = UART_MODEL_RX_PIN;
    GPIO_InitStruct.Alternate = UART_MODEL_RX_AF;
    HAL_GPIO_Init(UART_MODEL_RX_GPIO_PORT, &GPIO_InitStruct);
  }
#endif
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART_DBG) {
    USART_DBG_FORCE_RESET();
    USART_DBG_RELEASE_RESET();
    HAL_GPIO_DeInit(USART_DBG_TX_GPIO_PORT, USART_DBG_TX_PIN);
    HAL_GPIO_DeInit(USART_DBG_RX_GPIO_PORT, USART_DBG_RX_PIN);
  }
#ifdef USE_UART
  else if(huart->Instance == UART_MODEL) {
    UART_MODEL_FORCE_RESET();
    UART_MODEL_RELEASE_RESET();
    HAL_GPIO_DeInit(UART_MODEL_TX_GPIO_PORT, UART_MODEL_TX_PIN);
    HAL_GPIO_DeInit(UART_MODEL_RX_GPIO_PORT, UART_MODEL_RX_PIN);
  }
#endif
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
#ifdef USE_SPI
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;

  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;

  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_MOSI_AF;

  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
#endif
}

/**
  * @brief SPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to its default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
#ifdef USE_SPI
  /*##-1- Reset peripherals ##################################################*/
  SPIx_FORCE_RESET();
  SPIx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Configure SPI SCK as alternate function  */
  HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
  /* Configure SPI MISO as alternate function  */
  HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
  /* Configure SPI MOSI as alternate function  */
  HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
#endif
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
