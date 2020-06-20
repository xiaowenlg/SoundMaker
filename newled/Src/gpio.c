/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "MainConfig.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/

void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin = REED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(REED_GPIO_Port, &GPIO_InitStruct);

	//HAL_GPIO_WritePin(GPIOC, led0_Pin | led1_Pin | GPIO_PIN_3, GPIO_PIN_RESET);

	/*Configure GPIO pins : PCPin PCPin */
	GPIO_InitStruct.Pin = led0_Pin | led1_Pin | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*WTN6040引脚初始化*/
	/*PB13---OUT*/
	GPIO_InitStruct.Pin = CLK_2A_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(WTN_GPIO_Port, &GPIO_InitStruct);
	/*PB12---OUT*/
	GPIO_InitStruct.Pin = P_DATA_2A_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(WTN_GPIO_Port, &GPIO_InitStruct);
	/*PB15---INPUT*/
	GPIO_InitStruct.Pin = WTN_BUSY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(WTN_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SYSCLOSE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SYSCLOSE_PORT, &GPIO_InitStruct);
	/* EXTI interrupt init*/
/*	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn)*/;
	
}

//************************************
// 函数名称:    GPIO_WRCMD
// 函数全名:  GPIO_WRCMD
// 权限:    public 
// 返回值:   void
// 描述: 写一组IO口
// 参数: GPIO_TypeDef * GPIOx
// 参数: uint16_t cmd
//************************************
void GPIO_WRCMD(GPIO_TypeDef *GPIOx, uint16_t cmd)
{
	GPIOx->ODR = cmd;
}
//************************************
// 函数:    GPIO_RECMD
// 函数全名:  GPIO_RECMD
// 函数类型:    public 
// 返回值:   uint16_t
// Qualifier:读一组IO口
// 参数: GPIO_TypeDef * GPIOx
//************************************
uint16_t  GPIO_RECMD(GPIO_TypeDef *GPIOx)
{
	uint16_t res = (GPIOx->IDR);
	return res;
}

void JDQ_Control(_Bool b)
{
	if (b)
		JDQ = 1;//关闭电源
	else
		JDQ = 0;//开电源
}


/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
