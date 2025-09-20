/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MULT_S1_Pin GPIO_PIN_0
#define MULT_S1_GPIO_Port GPIOC
#define MULT_S2_Pin GPIO_PIN_1
#define MULT_S2_GPIO_Port GPIOC
#define MULT_S3_Pin GPIO_PIN_2
#define MULT_S3_GPIO_Port GPIOC
#define PT_OUT_Pin GPIO_PIN_3
#define PT_OUT_GPIO_Port GPIOC
#define PYRO_Pin GPIO_PIN_2
#define PYRO_GPIO_Port GPIOA
#define LC_CS_Pin GPIO_PIN_4
#define LC_CS_GPIO_Port GPIOA
#define LC_SCK_Pin GPIO_PIN_5
#define LC_SCK_GPIO_Port GPIOA
#define LC_MISO_Pin GPIO_PIN_6
#define LC_MISO_GPIO_Port GPIOA
#define LC_MOSI_Pin GPIO_PIN_7
#define LC_MOSI_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOB
#define TEMP_MISO_Pin GPIO_PIN_14
#define TEMP_MISO_GPIO_Port GPIOB
#define TEMP_MOSI_Pin GPIO_PIN_15
#define TEMP_MOSI_GPIO_Port GPIOB
#define T1_CS_Pin GPIO_PIN_6
#define T1_CS_GPIO_Port GPIOC
#define TEMP_SCK_Pin GPIO_PIN_7
#define TEMP_SCK_GPIO_Port GPIOC
#define T2_CS_Pin GPIO_PIN_8
#define T2_CS_GPIO_Port GPIOC
#define USB_D_N_Pin GPIO_PIN_11
#define USB_D_N_GPIO_Port GPIOA
#define USB_D_P_Pin GPIO_PIN_12
#define USB_D_P_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_15
#define FLASH_CS_GPIO_Port GPIOA
#define FLASH_SCK_SD_CLK_Pin GPIO_PIN_10
#define FLASH_SCK_SD_CLK_GPIO_Port GPIOC
#define FLASH_MISO_SD_MISO_Pin GPIO_PIN_11
#define FLASH_MISO_SD_MISO_GPIO_Port GPIOC
#define FLASH_MOSI_SD_MOSI_Pin GPIO_PIN_12
#define FLASH_MOSI_SD_MOSI_GPIO_Port GPIOC
#define SD_CS_Pin GPIO_PIN_2
#define SD_CS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
