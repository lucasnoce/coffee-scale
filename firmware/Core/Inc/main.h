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
#define BTN_BACK_Pin GPIO_PIN_3
#define BTN_BACK_GPIO_Port GPIOA
#define BTN_BACK_EXTI_IRQn EXTI3_IRQn
#define BTN_L_Pin GPIO_PIN_4
#define BTN_L_GPIO_Port GPIOA
#define BTN_L_EXTI_IRQn EXTI4_IRQn
#define BTN_R_Pin GPIO_PIN_5
#define BTN_R_GPIO_Port GPIOA
#define BTN_R_EXTI_IRQn EXTI9_5_IRQn
#define BTN_OK_Pin GPIO_PIN_6
#define BTN_OK_GPIO_Port GPIOA
#define BTN_OK_EXTI_IRQn EXTI9_5_IRQn
#define HX711_DOUT_LoopBack_Pin GPIO_PIN_8
#define HX711_DOUT_LoopBack_GPIO_Port GPIOA
#define HX711_SCK_LoopBack_Pin GPIO_PIN_9
#define HX711_SCK_LoopBack_GPIO_Port GPIOA
#define HX711_SCK_LoopBack_EXTI_IRQn EXTI9_5_IRQn
#define HX711_SCK_Pin GPIO_PIN_10
#define HX711_SCK_GPIO_Port GPIOA
#define HX711_DOUT_Pin GPIO_PIN_11
#define HX711_DOUT_GPIO_Port GPIOA
#define HX711_DOUT_EXTI_IRQn EXTI15_10_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
