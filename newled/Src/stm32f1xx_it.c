/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "cmsis_os.h"
#include "MainConfig.h"
#include "APPTooL.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
uint8_t DataIndex = 0;
uint8_t DataLen;
uint8_t ReceData[256];//接收数据缓冲区
/* USER CODE BEGIN EV */
uint8_t Usart2_Index = 0;
uint8_t Usart2_Data[256];//接收数据缓冲区
uint8_t Usart2_DataLen;
/*串口3*/
uint8_t Usart3_Index = 0;
uint8_t Usart3_Data[256];//接收数据缓冲区
_Bool Usart3_Over;//接收完毕标志
uint8_t Usart3_DataLen;

uint16_t TempCount = 0;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt.
  */
//************************************
// 函数:    UART1_CallBack
// 函数全名:  UART1_CallBack
// 函数类型:    public 
// 返回值:   void
// Qualifier:串口1回调函数
//************************************

//结构体做函数的参数必须传指针
void UartCallback(Uart_Structure_t *uart_struct)  
{
	static uint8_t i = 0;
	uint8_t tempData = 0;
	uint8_t clear = clear;
	UART_HandleTypeDef* uartHandle = &uart_struct->huart;
	ReceMessage *meRec = &uart_struct->message;						//传地址
	if (__HAL_UART_GET_FLAG(uartHandle, UART_FLAG_RXNE) != RESET)
	{
		//meRec.redata[meRec.dataIndex] = 0;

		tempData = uartHandle->Instance->DR;	//读取接收到的数据

		meRec->redata[meRec->dataIndex] = tempData;
		meRec->dataIndex++;
		i++;

	}
	else if (RESET != __HAL_UART_GET_FLAG(uartHandle, UART_FLAG_IDLE))
	{

		clear = uartHandle->Instance->SR;
		clear = uartHandle->Instance->DR;
		i = 0;
		meRec->datalen = meRec->dataIndex;
		meRec->dataIndex = 0;
		meRec->reover = 1;
		//__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
	}
	__HAL_UART_CLEAR_FLAG(uartHandle, UART_FLAG_TC);
}
void USART1_IRQHandler(void)
{
  //HAL_UART_IRQHandler(&huart1);//-----------------------------原函数
	//UART1_CallBack();
	UartCallback(&Uart_1);
}
void USART2_IRQHandler(void)
{
	//HAL_UART_IRQHandler(&huart1);-----------------------------原函数
	//UART2_CallBack();
	UartCallback(&Uart_2);

}
void USART3_IRQHandler(void)
{
	//HAL_UART_IRQHandler(&huart1);-----------------------------原函数
	//UART3_CallBack();
	UartCallback(&Uart_2);
}
/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim3);
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
* @brief This function handles EXTI line[15:10] interrupts.
*/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_15)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0);
	}
	if (GPIO_Pin == GPIO_PIN_0)
	{
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1);
		
			//TempCount++; //中断计数
			//SportCount++;
			//SingleTrig(CallInterreput, HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0), 0, 1, 0);
		
		
	}

}
/************************************************************************/
/* ADC和DMA中断                                                         */
/************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc1);
}

/**
* @brief This function handles ADC1 and ADC2 global interrupts.
*/
void ADC1_2_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&hadc1);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
