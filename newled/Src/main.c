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
uint8_t coefficient = 0;//��·��ϵ��
char EquipmentNumber[20] = EMID;//�豸ID
SemaphoreHandle_t xSemaphore_BLE = NULL;
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
void HMI_Data_Init();

void AppData_Init()
{
	coefficient = HOT_CAR;//��·��ϵ��
	LimitSportFreq = MAXSPORTFREQ;//����˶�Ƶ�ʳ�ʼ��
	SYSIO = 0;
	HMI_Data_Init();//HMI����ʾ���ݳ�ʼ��
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
	//����
	USART_Init(&DEBUG_UART.huart,USART1, DEBUG_BAND);
	USART_Init(&BLE_UART.huart, USART2, BLE_BAND);
	USART_Init(&HMI_UART.huart, USART3, HMI_BAND);
#else if(BUG==0)   //ʵ�ʰ���
	USART_Init(&DEBUG_UART.huart, USART2, DEBUG_BAND);
	USART_Init(&BLE_UART.huart, USART1, BLE_BAND);
	USART_Init(&HMI_UART.huart, USART3, HMI_BAND);
#endif

	
	MX_TIM2_Init(1000 - 1);//1MS
	DWT_Delay_Init();
	/* ���������ź��� */
	xSemaphore = xSemaphoreCreateMutex();
	xSemaphore_BLE = xSemaphoreCreateMutex();
	//HAL_TIM_Base_Start_IT(&htim2);
		
	
}

	
void creatQueue()
{
	
	xQueuel_tim = xQueueCreate(10, sizeof(struct SportInfo*));//������Ϣ����
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
  osKernelStart();//�˺����ǿ�ʼ���Ⱥ���������������г�ʼ����������
  
  while (1)
  {
  
  }
 
}


//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//	UNUSED(huart);
//	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);   // ��ת��
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
// ����:    HMI_Data_Init
// ����ȫ��:  HMI_Data_Init
// ��������:    public 
// ����ֵ:   void
// Qualifier:��ʼ����ʾ��Ϣ
// ����: void
//************************************
void HMI_Data_Init(void)
{
	
	
	HMI_SetVal(&HMI_UART.huart, HMI_KCAL, 0);//��������
	HMI_SetVal(&HMI_UART.huart, HMI_COUNT, 0);//���ʹ���
	//����ʱ��
	HMI_SetVal(&HMI_UART.huart, HMI_HOUR, 0);//ʱ
	HMI_SetVal(&HMI_UART.huart, HMI_MINUTE, 0);//��
	HMI_SetVal(&HMI_UART.huart, HMI_SECOND, 0);//��
	HMI_SetVal(&HMI_UART.huart, HMI_SPEED, 0);//�����ٶ�ֵ
	HMI_SetVal(&HMI_UART.huart, HMI_KCAL, 0);//��������
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
