/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include "queue.h"
#include "semphr.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
#define SETUARTMAXLEN(n)            256*n                //串口接收最大数据长度(n>0且n∈Z)
extern SemaphoreHandle_t xSemaphore;
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */
void USART_Init(UART_HandleTypeDef *huart, USART_TypeDef *uart, uint32_t baud);
void MX_USART1_UART_Init(uint32_t baud);
void MX_USART2_UART_Init(uint32_t baud);
void MX_USART3_UART_Init(uint32_t baud);

HAL_StatusTypeDef MY_USART_SendData(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef MY_USART_SendByte(UART_HandleTypeDef *huart, uint8_t byt);
HAL_StatusTypeDef MY_USART_chars(UART_HandleTypeDef *huart, uint8_t *pData);
void sendstring(unsigned char *string, UART_HandleTypeDef *huart);
void Uart_printf(UART_HandleTypeDef *huart, const char *fmt, ...);
char * convert_func(char *str, unsigned int ch, int num);
void convert_t(unsigned int ch);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

typedef struct UartInfo
{
	_Bool   reover;             //接收完成标志
	uint8_t datalen;            //接收到的数据长度
	uint8_t dataIndex;          //接收数组的索引
	UART_HandleTypeDef *uartHandl;  //串口句柄
	void(*p)(void *v);          //数据函数
	uint8_t redata[SETUARTMAXLEN(1)];        //接收缓冲区(数组)
}ReceMessage;

typedef struct Uart_Structure
{
	UART_HandleTypeDef huart;            //串口句柄
	ReceMessage message;                //串口接收信息结构体
}Uart_Structure_t;


extern Uart_Structure_t Uart_1;
extern Uart_Structure_t Uart_2;
extern Uart_Structure_t Uart_3;
#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
