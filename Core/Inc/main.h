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
#define WP_Pin       GPIO_PIN_9
#define WP_GPIO_Port GPIOA
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
int chargePercentage(double adcValue);
int chargeVoltage(double adcValue);
int updateBatteryLife(void);
void buzzer(uint16_t frequency, uint32_t duration);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_DC_Pin GPIO_PIN_13
#define OLED_DC_GPIO_Port GPIOC
#define OLED_RES_Pin GPIO_PIN_14
#define OLED_RES_GPIO_Port GPIOC
#define START_Pin GPIO_PIN_0
#define START_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_3
#define OLED_CS_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_4
#define BUZZER_GPIO_Port GPIOA
#define AUX_POWER_Pin GPIO_PIN_12
#define AUX_POWER_GPIO_Port GPIOB
#define RIGHT_Pin GPIO_PIN_13
#define RIGHT_GPIO_Port GPIOB
#define UP_Pin GPIO_PIN_14
#define UP_GPIO_Port GPIOB
#define LEFT_Pin GPIO_PIN_15
#define LEFT_GPIO_Port GPIOB
#define DOWN_Pin GPIO_PIN_8
#define DOWN_GPIO_Port GPIOA
#define WP_Pin GPIO_PIN_9
#define WP_GPIO_Port GPIOA
#define SELECT_Pin GPIO_PIN_10
#define SELECT_GPIO_Port GPIOA
#define BAT_FET_Pin GPIO_PIN_3
#define BAT_FET_GPIO_Port GPIOB
#define B_Pin GPIO_PIN_4
#define B_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_5
#define A_GPIO_Port GPIOB
#define HAPTIC_Pin GPIO_PIN_8
#define HAPTIC_GPIO_Port GPIOB
#define BOOST_POW_Pin GPIO_PIN_9
#define BOOST_POW_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
