/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
 ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H
#include "usart.h"
#ifdef __cplusplus
 extern "C" {
#endif 

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
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void UART1_CallBack();
void UART2_CallBack();//串口2接收回调函数

void UartCallback(Uart_Structure_t *uart_struct);//串口回调函数——————通用
void USART1_IRQHandler(void);
void CallInterreput(uint8_t v);
void TIM6_IRQHandler(void);
/* USER CODE BEGIN EFP */
//串口1变量
extern uint8_t ReceData[256];//接收数据缓冲区
extern _Bool ReceOver;//接收完毕标志
extern  uint8_t DataLen;
/* USER CODE END EFP */
//串口2变量
extern uint8_t Usart2_Data[256];//接收数据缓冲区
extern _Bool Usart2_Over;//接收完毕标志
extern  uint8_t Usart2_DataLen;

extern uint8_t Usart3_Data[256];//接收数据缓冲区
extern _Bool Usart3_Over;//接收完毕标志
extern  uint8_t Usart3_DataLen;
#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
