/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported variables ------------------------------------------------------- */
/* USER CODE BEGIN EV */
extern RNG_HandleTypeDef hrng;
/* USER CODE END EV */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TR01_PWR_Pin GPIO_PIN_0
#define TR01_PWR_GPIO_Port GPIOA
#define TR01_CS_Pin GPIO_PIN_4
#define TR01_CS_GPIO_Port GPIOA
#define TR01_SCK_Pin GPIO_PIN_5
#define TR01_SCK_GPIO_Port GPIOA
#define TR01_MISO_Pin GPIO_PIN_6
#define TR01_MISO_GPIO_Port GPIOA
#define TR01_MOSI_Pin GPIO_PIN_7
#define TR01_MOSI_GPIO_Port GPIOA
#define TR01_GPO_Pin GPIO_PIN_0
#define TR01_GPO_GPIO_Port GPIOB
#define APP_LED_Pin GPIO_PIN_9
#define APP_LED_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
