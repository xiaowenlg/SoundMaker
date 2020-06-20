/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "application.h"
#include "adc.h"
#include <stdio.h>
#include "system.h"
#include "semphr.h"
#include "MainConfig.h"
#include "dwt_stm32_delay.h"

#define EMID         "00010000400023000002"
QueueHandle_t xQueuel_tim = NULL;
uint8_t coefficient = 0;//卡路里系数
char EquipmentNumber[20] = EMID;//设备ID
SemaphoreHandle_t xSemaphore_BLE = NULL;
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
void HMI_Data_Init();

void AppData_Init()
{
	coefficient = HOT_CAR;//卡路里系数
	LimitSportFreq = MAXSPORTFREQ;//最大运动频率初始化
	SYSIO = 0;
	HMI_Data_Init();//HMI屏显示数据初始化
}

void Peripherals_init()
{
	MX_GPIO_Init();
	
	/*MX_USART1_UART_Init(115200);
	MX_USART2_UART_Init(115200);
	MX_USART3_UART_Init(9600);*/
	/*MX_USART1_UART_Init(UART_BAND_1);
	MX_USART2_UART_Init(UART_BAND_2);
	MX_USART3_UART_Init(UART_BAND_3);*/
#if (BUG==1)
	//调试
	USART_Init(&DEBUG_UART.huart,USART1, DEBUG_BAND);
	USART_Init(&BLE_UART.huart, USART2, BLE_BAND);
	USART_Init(&HMI_UART.huart, USART3, HMI_BAND);
#else if(BUG==0)   //实际板中
	USART_Init(&DEBUG_UART.huart, USART2, DEBUG_BAND);
	USART_Init(&BLE_UART.huart, USART1, BLE_BAND);
	USART_Init(&HMI_UART.huart, USART3, HMI_BAND);
#endif

	
	MX_TIM2_Init(1000 - 1);//1MS
	DWT_Delay_Init();
	/* 创建互斥信号量 */
	xSemaphore = xSemaphoreCreateMutex();
	xSemaphore_BLE = xSemaphoreCreateMutex();
	//HAL_TIM_Base_Start_IT(&htim2);
		
	
}

	
void creatQueue()
{
	
	xQueuel_tim = xQueueCreate(10, sizeof(struct SportInfo*));//创建消息队列
	if (xQueuel_tim != 0)
	{
#ifdef MYDEBUG		//HMI_SetTxt(HMI_UART, HTM_QRCODE, QRCODE);
		//MY_USART_SendData(&huart1, "xQueue1:successful!\r\n", 23);
#endif
		
	}
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  Peripherals_init();
  InitFirstTask();
 // MX_FREERTOS_Init();
  creatQueue();
  AppData_Init();
  osKernelStart();//此函数是开始调度函数，必须放在所有初始化函数后面
  
  while (1)
  {
  
  }
 
}


//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//	UNUSED(huart);
//	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);   // 翻转灯
//	//HAL_UART_Transmit_IT(&huart1, (uint8_t *)&aTxBuffer, 1);
//}




/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  
}
//************************************
// 函数:    HMI_Data_Init
// 函数全名:  HMI_Data_Init
// 函数类型:    public 
// 返回值:   void
// Qualifier:初始化显示信息
// 参数: void
//************************************
void HMI_Data_Init(void)
{
	
	
	HMI_SetVal(&HMI_UART.huart, HMI_KCAL, 0);//发送热量
	HMI_SetVal(&HMI_UART.huart, HMI_COUNT, 0);//发送次数
	//发送时间
	HMI_SetVal(&HMI_UART.huart, HMI_HOUR, 0);//时
	HMI_SetVal(&HMI_UART.huart, HMI_MINUTE, 0);//分
	HMI_SetVal(&HMI_UART.huart, HMI_SECOND, 0);//秒
	HMI_SetVal(&HMI_UART.huart, HMI_SPEED, 0);//发送速度值
	HMI_SetVal(&HMI_UART.huart, HMI_KCAL, 0);//发送热量
}
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
