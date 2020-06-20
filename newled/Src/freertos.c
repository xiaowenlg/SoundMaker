/* USER CODE BEGIN Header */
/**
******************************************************************************
* File Name          : freertos.c
* Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "application.h"
#include "tim.h"
#include "gpio.h"
#include "adc.h"
#include "MainConfig.h"
#include "APPTooL.h"
#include "dht11.h"
#include "string.h"
#include "StmFlash.h"
#include "stdint.h"
#define START_TASK_PRIO    3
#define START_STK_SIZE 		128  

uint32_t tt = 0;
TaskHandle_t StartTask_Handler;
TaskHandle_t ResultsTask_Handler;
TaskHandle_t HMITask_Handler;      
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId initStaskHandle;//开始线程句柄
void InitFirstTask();//开始线程初始化
void InitStak(void const *argument);//开始线程
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void start_task(void *pvParameters);
void results_task(void const * argument);
void HMI_task(void const *argument);//发送信息到串口屏
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
void HMI_SendQR(void);
void SendToBle(huart);					//上传运动信息
_Bool ReceOver = 0;
_Bool Usart2_Over = 0;//接收完毕标志
uint8_t DataLen = 0;
uint8_t ReceData[256] = { 0 };
uint8_t Usart2_Data[256];//接收数据缓冲区
uint8_t Usart2_DataLen;
//adc相关
uint32_t ADC_Value[100];
uint32_t ad1, ad2;
DHT11_Data_TypeDef DHT11_Data;
uint16_t playcount;
uint16_t playtim;
long  playhot;
uint8_t playstate = 0;
uint8_t playFreq = 0;
_Bool tim2State = 0;
_Bool playisover = 0;
_Bool iscolsesystem = 0;//关闭系统标志=============================
uint8_t timer2_CloseSystem = 0;//关闭系统倒计时
uint8_t recount = 0;
static uint32_t ulIdleCycleCount = 0UL;
uint8_t str[4] = { 0 };
uint8_t str1[4] = "MAC";
uint8_t adr_MAC[MAC_LENGTH] = { 0 };

//EEPROM相关变量
uint8_t datatemp[READLEN] = {0};
uint8_t endflg[8] = { "-N" };	//结尾标志
uint8_t readflg[8] = {0};		//读到的标志位

char qrdata[128] = {0};//二维码数组
uint8_t serialdat[SERIALDATLEN] = { 0 };
uint8_t uart_flag;
extern SemaphoreHandle_t xSemaphore_BLE;

uint8_t LimitSportFreq = 0;//最大运动频率--------------在main.c中初始化

uint8_t loopcount = 0;        //开机循环计数

/**
* @brief  FreeRTOS initialization
* @param  None
* @retval None
*/
void TRCallBack(uint8_t val);
//开始启动任务
void InitFirstTask()
{
	osThreadDef(firstTask, InitStak, 1, 0, 128);
	initStaskHandle = osThreadCreate(osThread(firstTask), NULL);
}
//app任务
void MX_FREERTOS_Init(void) {
	
	osThreadDef(defaultTask, StartDefaultTask, 7, 0, 128);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* definition and creation of myTask02 */
	osThreadDef(myTask02, StartTask02,5, 0, 128);
	myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

	xTaskCreate((TaskFunction_t)start_task,           
		(const char*)"start_task",          
		(uint16_t)START_STK_SIZE,       
		(void*)NULL,                  
		(UBaseType_t)START_TASK_PRIO,      
		(TaskHandle_t*)&StartTask_Handler);  

	xTaskCreate((TaskFunction_t)results_task,
		(const char*)"results_task",
		(uint16_t)START_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)4,
		(TaskHandle_t*)&ResultsTask_Handler);

	xTaskCreate((TaskFunction_t)HMI_task,
		(const char*)"hmi_task",
		(uint16_t)START_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)6,
		(TaskHandle_t*)&HMITask_Handler);

}
void CallInterreput(uint8_t v)//计次回调函数
{
	TempCount++; 
	SportCount++;
	iscolsesystem = 0;
#ifdef MYDEBUG
	Uart_printf(&huart1,"playcount==%d\r\n", playcount);
#endif
	if (tim2State ==0)
	{
		HAL_TIM_Base_Start_IT(&htim2);//打开定时器
		tim2State = 1;
	}
	vTaskResume(myTask02Handle);//恢复任务
}
void meTRCallBack(uint8_t val)//语音播放回调函数
{
	//播放函数
	uint8_t playarray[PLAYARRAYLENGTH] = { 0 };//播放数据数组
	uint8_t playdatalen = 0;
	playdatalen = GetPlayData(playcount, playtim, playhot / 1000.000, playarray);

	
#ifdef MYDEBUG

	for (int i = 0; i < playdatalen;i++)
	{
		Uart_printf(&huart1, "Data%d:%x\r\n", i, playarray[i]);
	}

#endif // MYDEBUG

#ifdef MYDEBUG
	Uart_printf(&huart1,"playarray==%d\r\n", playdatalen);//-----------------------测试
#endif	
	xSemaphoreTake(xSemaphore_BLE, portMAX_DELAY);//加入互斥锁，不然光被线程1打断，造成播报错误
	{
		playisover = WTN6040_PlayArray(playdatalen, playarray);
	
		if (tim2State==1)
		{
			HAL_TIM_Base_Stop_IT(&htim2);//关闭定时器
			tim2State = 0;
		}
		//HMI_SetVal(HMI_UART, HMI_COUNT, 0);//关闭HMI屏音乐播放
		//playisover = 1;
		iscolsesystem = playisover;
	}
	xSemaphoreGive(xSemaphore_BLE);
}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief  Function implementing the defaultTask thread.
* @param  argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void InitStak(void const *argument)           //启动线程的线程
{
	uint8_t *dataPoint = NULL;
	STMFLASH_Read(EEPROM_BEGIN_ADRR, (uint16_t *)datatemp, MAC_LENGTH/2+1);//先读一次

	memset(readflg, 0, 6);
	memcpy(readflg, &datatemp[17], 2);
	
	if (strcmp(endflg, readflg) == 0)//判断是否是第一次开机
	{
		
		memset(adr_MAC, 0, MAC_LENGTH);
		memcpy(adr_MAC, &datatemp[0], 17);//取内存中的蓝牙地址
		
	}
	else                                            //读取蓝牙信息
	{
		Uart_printf(&DEBUG_UART.huart, "No Same!\r\n");
		osDelay(500);//等待蓝牙稳定
		MY_USART_chars(&BLE_UART.huart, "+++a");
		
		while (1)
		{
			if (BLE_UART.message.reover == 1)
			{
				recount++;
				BLE_UART.message.reover = 0;
				
				if (recount>=2)
				{
					memset(str, 0, 4);
					memcpy(str, &BLE_UART.message.redata[3], 3);
					if (strcmp(str1, str) == 0)
					{
					
						MY_USART_chars(&BLE_UART.huart, "AT+ENTM\r\n");
						memset(adr_MAC, 0, MAC_LENGTH);
						memcpy(adr_MAC, &BLE_UART.message.redata[7], BLE_UART.message.datalen - 12);//取蓝牙地址
					
						for (int i = 1; i < 6; i++)							//蓝牙地址中插入":"
						{
							if (i == 1)
								insertOneByte(adr_MAC, MAC_LENGTH, ':', i * 2);
							else
								insertOneByte(adr_MAC, MAC_LENGTH, ':', i * 2 + (i - 1));

						}
						insertArray(adr_MAC, MAC_LENGTH - 2, "-N", 2, MAC_LENGTH-2);
						Eeprom_write(EEPROM_BEGIN_ADRR, (uint16_t *)adr_MAC, MAC_LENGTH);            //把蓝牙地址写入内存
						//Uart_printf(&huart1, "addr:%s", adr_MAC);//-------------------------------------------------------
						break; //推出while循环
					}
					memset(BLE_UART.message.redata, 0, BLE_UART.message.datalen);
				
				}
				else	
					MY_USART_chars(&BLE_UART.huart, "AT+MAC?\r\n");
			}
		
			osDelay(5);
		}
	
	 }
	
	//Uart_printf(DEBUG_UART, "EEPROM_addrMac:%s\r\n", adr_MAC);//-----------------------------测试点
	StrJoin(adr_MAC, EquipmentNumber, qrdata);//字符串连接
	osDelay(1000);//延时等待HMI屏启动成功
	taskENTER_CRITICAL();//进入临界区
	MX_FREERTOS_Init();
	vTaskDelete(initStaskHandle); //删除任务
	taskEXIT_CRITICAL();//推出临界区
}

void StartDefaultTask(void const * argument)         //串口信息处理函数       线程1
{
	uint8_t i;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
	 //   if (ReceOver == 1) //串口1接收处理
		//{
		//	ReceOver = 0;
		//	if (DataLen==ReceData[2]+3)
		//	{
		//		switch (ReceData[3])
		//		{
		//		case  REGISTERREAD://语音播放寄存器
		//			if (ReceData[4] = VOL_STATUS)
		//			{
		//				//读语音播放寄存器--------------------
		//			}
		//			break;
		//		default:
		//			break;
		//		}
		//	}
		//	//MY_USART_SendData(&huart1, ReceData, DataLen);
		//}

		//if (Usart2_Over==1) //串口2接收处理
		//{
		//	Usart2_Over = 0;
		//	//MY_USART_SendData(&huart1, Usart2_Data, Usart2_DataLen);
		//}
		if (uart_flag==1)
		{
			uart_flag = 0;
			//i = 0;
			//////////////////////////发送蓝牙数据
			//Uart_printf(&huart1, "playcon:%d", playcount);
			xSemaphoreTake(xSemaphore_BLE, portMAX_DELAY); //和线程2构成互斥锁，防止打断播报,即播放的时候不能上传数据
			{
				SendToBle(&BLE_UART.huart);
			}
			xSemaphoreGive(xSemaphore_BLE);
			
			
		}
		/*if (i++>10)
		{
			i = 0;
			SendToBle(BLE_UART);
		}*/
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		//osDelay(5);
	}
}


//************************************
// 函数:    StartTask02
// 函数全名:  StartTask02
// 函数类型:    public 
// 返回值:   void
// Qualifier:LED控制线程
// 参数: void const * argument
//************************************
void StartTask02(void const * argument)//接收定时器发送过来的信息，并发送到HMI屏          线程2
{
	Customerinfo *ptMsg;
	BaseType_t xResult;
	
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000); /* 设置最大等待时间为200ms */
	
	for (;;)
	{
		
		xResult = xQueueReceive(
			xQueuel_tim,                   /* 消息队列句柄 */
			(void *)&ptMsg,             /* 这里获取的是结构体的地址 */
			(TickType_t)xMaxBlockTime);/* 设置阻塞时间 */

		if (xResult == pdPASS)
		{
			playcount = ptMsg->count;
			playtim = ptMsg->tim;
			playhot = ptMsg->hot;
			playstate = ptMsg->playstate;
			playFreq = ptMsg->freq;

			
		}
		else
		{
#ifdef MYDEBUG
			MY_USART_chars(&huart1, "接收到消息失败!");//测试
#endif
			xSemaphoreTake(xSemaphore, portMAX_DELAY);
			{
				HMI_SetVal(&HMI_UART.huart, HMI_COUNT, 0);//关闭HMI屏音乐播放
				HMI_SetVal(&HMI_UART.huart, HMI_COUNT, 0);//关闭HMI屏音乐播放
				HMI_SetVal(&HMI_UART.huart, HTM_AUDIO1, 0);
			}
			xSemaphoreGive(xSemaphore);
			
			vTaskSuspend(myTask02Handle);//挂起任务(接收不到消息说明播放完毕定时器已经关闭)
			
		}
			
			SingleTrig(meTRCallBack, playstate, 0, 0, 1);
		osDelay(50);
	}
	/* USER CODE END StartTask02 */
}
void start_task(void *pvParameters)//检测运动信号
{
	
	for (;;)
	{
		SingleTrig(CallInterreput, HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0), 0, 1, 0);
		
	}
	osDelay(5);
}

void results_task(void const * argument)//adc检测，等待关机,发送二维码
{
	
	char *pcTaskName;
	DHT11_Data_TypeDef *dht;
	pcTaskName = (char *)argument;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	MX_DMA_Init();//DAM初始化
	MX_ADC1_Init();//ADC初始化
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 100);
	HMI_SendQR();
	dht11_init();
	for (;;)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
		
		if (iscolsesystem)
		{
			if (timer2_CloseSystem++ > TIMER_CLOSESYSTEM)
			{
				timer2_CloseSystem = 0;
				iscolsesystem = 0;
				SYSIO = 1;
#ifdef MYDEBUG
				Uart_printf(&huart1, "Close");
#endif
			}
		}
		else
		{
			timer2_CloseSystem = 0;
		}
		
		if (playFreq >= LimitSportFreq)           //运动频率过大提示
		{
			WTN6040_PlayOneByte(S_MAXSPORT_TIP);
		}
		//
		if (tim2State == 0 && playFreq == 0)//用于检测开机后没运动的情况，如果没用的 10s后关机
		{
			if (loopcount++>2*TIMER_CLOSESYSTEM)
			{
				loopcount = 0;
				SYSIO = 1;
			}
		}
		else
		{
			loopcount = 0;
		}
		
		vTaskDelay(1000 / portTICK_RATE_MS);
		
		
	}
}
void HMI_task(void const *argument)
{
	
	char *pcTaskName;
	pcTaskName = (char *)argument;
	uint32_t tick = 0;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = MAIN_LOOP_DT;
	xLastWakeTime = xTaskGetTickCount();
	uint8_t sus = 0;
	for (;;)
	{
		
		if (PERIOD_DO_EXECUTE(tick, HMI_PERIOD)) //发送温度和电压数据到HMI
		{
			
			xSemaphoreTake(xSemaphore_BLE, portMAX_DELAY); //和线程2构成互斥锁，防止打断播报,即播放的时候不能上传数据
			{
				if (sus == SUCCESS)
				{
					HMI_SetVal_t(&HMI_UART.huart, HMI_TEMP, DHT11_Data.temp_int * 100 + DHT11_Data.temp_deci); 
					HMI_SetVal_t(&HMI_UART.huart, HMI_RH, DHT11_Data.humi_int * 100 + DHT11_Data.humi_deci); 
					//Uart_printf(&DEBUG_UART.huart, "x1=%d", 1000);
					
					//printf("%s%d","x1.val=", 1000); fflush(stdout);
				}
				
				HMI_SetVal(&HMI_UART.huart, HTM_BATTERY_J, ad1 * 100 / 4096);
			}
			xSemaphoreGive(xSemaphore_BLE);
			//Uart_printf(&Uart_1.huart, "vAppHook=%d",ulIdleCycleCount);
		}
		if (PERIOD_DO_EXECUTE(tick,ADC_PERIOD)) //DHT11检测
		{
			xSemaphoreTake(xSemaphore, portMAX_DELAY);
			
				sus = Read_DHT11(&DHT11_Data);
			
			xSemaphoreGive(xSemaphore);
			for (uint8_t i = 0; i < 100; ++i)
			{
				ad1 += ADC_Value[i];
			}
			ad1 = ad1 / 100;
			//Uart_printf(&Uart_1.huart, "xiaowengl\r\n");
		}
		if (PERIOD_DO_EXECUTE(tick,MES_PERIOD)) //发送运动数值
		{
			//发送信息到HMI
			
			xSemaphoreTake(xSemaphore, portMAX_DELAY);
			{

				HMI_SetVal(&HMI_UART.huart, HMI_SPEED, playFreq);//发送速度值
				HMI_SetVal(&HMI_UART.huart, HMI_KCAL, playhot / 10);//发送热量
				HMI_SetVal(&HMI_UART.huart, HMI_COUNT, playcount);//发送次数

				//设置动画速度
				int v = playFreq * 100;
				if (v == 0)
					v = 1;
				if (v > 1000)
					v = 1000;

				HMI_SetVal(&HMI_UART.huart, HTM_GIF_DIS, v);
			}
			xSemaphoreGive(xSemaphore);//释放互斥信号量
		}
		if (PERIOD_DO_EXECUTE(tick,150))//发送时间
		{
			//发送时间
			xSemaphoreTake(xSemaphore, portMAX_DELAY);
			{
				uint8_t th = playtim / 3600, tme = playtim % 3600 / 60, ts = playtim % 3600 % 60;//时，分，秒
				HMI_SetVal(&HMI_UART.huart, HMI_HOUR, th);
				HMI_SetVal(&HMI_UART.huart, HMI_MINUTE, tme);
				HMI_SetVal(&HMI_UART.huart, HMI_SECOND, ts);
			}
			xSemaphoreGive(xSemaphore);//释放互斥信号量
		}
		vTaskDelayUntil(&xLastWakeTime, HMI_STACK_PERIOD);
		tick++;
	}
	

}
void vApplicationIdleHook(void)
{
	
	ulIdleCycleCount++;
	
}
//************************************
// 函数:    HMI_SendQR
// 函数全名:  HMI_SendQR
// 函数类型:    public 
// 返回值:   void
// Qualifier: 发送二维码，必须发送两次
// 参数: void
//************************************
void HMI_SendQR(void)//必须发送两次
{
	
	xSemaphoreTake(xSemaphore, portMAX_DELAY);
	{
		HMI_SetTxt(&HMI_UART.huart, HTM_QRCODE, qrdata);
		vTaskDelay(250 / portTICK_RATE_MS);
		HMI_SetTxt(&HMI_UART.huart, HTM_QRCODE, qrdata);
		vTaskDelay(250 / portTICK_RATE_MS);
	}
	xSemaphoreGive(xSemaphore);
	
}
void  QF_CRC(uint8_t *dat, uint8_t len)
{
	uint8_t crctemp_lo = 0, crctemp_hi, i = 0;
	//SendByte(0xaa);
	for (i = 0; i < len - 2; i++)
	{
		if (i == 0)
		{
			crctemp_hi = dat[i] ^ 0xe1;

			crctemp_lo = dat[i] ^ 0xe2;
		}

		else
		{
			crctemp_hi = crctemp_hi^dat[i];
			crctemp_lo = crctemp_lo^dat[i];
		}

	}
	//SendByte(crctemp_hi);
	dat[18] = crctemp_hi;
	dat[19] = crctemp_lo;
}
//上传数据整合
/*void SendToBle(UART_HandleTypeDef *huart)
{
	//1,发送设备信息
	memset(serialdat, 0, SERIALDATLEN);//清空数组
	serialdat[0] = 0xaa;
	serialdat[1] = 0x55;
	serialdat[2] = 0x01;
	serialdat[3] = EquipmentNumber[0];
	serialdat[4] = EquipmentNumber[1];
	serialdat[5] = EquipmentNumber[2];
	serialdat[6] = EquipmentNumber[3];
	serialdat[7] = EquipmentNumber[4];
	serialdat[8] = EquipmentNumber[5];
	serialdat[9] = EquipmentNumber[6];
	serialdat[10] = EquipmentNumber[7];
	serialdat[11] = EquipmentNumber[8];
	serialdat[12] = EquipmentNumber[9];
	serialdat[19] = 0xf6;
	MY_USART_SendData(huart, serialdat, SERIALDATLEN);
	//2,发送设备信息
	memset(serialdat, 0, SERIALDATLEN);//清空数组
	serialdat[0] = 0xaa;
	serialdat[1] = 0x55;
	serialdat[2] = 0x02;
	serialdat[3] = EquipmentNumber[10];
	serialdat[4] = EquipmentNumber[11];
	serialdat[5] = EquipmentNumber[12];
	serialdat[6] = EquipmentNumber[13];
	serialdat[7] = EquipmentNumber[14];
	serialdat[8] = EquipmentNumber[15];
	serialdat[9] = EquipmentNumber[16];
	serialdat[10] = EquipmentNumber[17];
	serialdat[11] = EquipmentNumber[18];
	serialdat[12] = EquipmentNumber[19];
	serialdat[19] = 0xf6;
	MY_USART_SendData(huart, serialdat, SERIALDATLEN);
	
	//第3帧数据
	memset(serialdat, 0, SERIALDATLEN);//清空数组
	serialdat[0] = 0xaa;
	serialdat[1] = 0x55;
	serialdat[2] = 0x03;
	serialdat[3] = playtim >> 8;
	serialdat[4] = playtim & 0x00ff;
	serialdat[5] = playcount >> 8;
	serialdat[6] = playcount & 0x00ff;
	serialdat[7] = playhot >> 8;
	serialdat[8] = playhot & 0x00fff;//转速无
	serialdat[9] = 0;//心率
	serialdat[10] = 10;
	serialdat[11] = playFreq>> 8;//坡度
	serialdat[12] = playFreq & 0x00ff;//档位
	//serialdat[13] = 13;//功率高位
	//serialdat[14] = 14;//功率低位
	//serialdat[15] = 15;//力矩高
	//serialdat[16] = 16;//力矩低位
	//serialdat[17] = 17;//校验位
	//serialdat[18] = 18;//校验位
	QF_CRC(serialdat, 20);
	MY_USART_SendData(huart, serialdat, SERIALDATLEN);
	
	//第4帧数据
	memset(serialdat, 0, SERIALDATLEN);//清空数组
	serialdat[0] = 0xaa;
	serialdat[1] = 0x55;
	serialdat[2] = 0x04;
	serialdat[3] = 1;//模式
	serialdat[4] = 2;//负荷高
	serialdat[5] = 3;//负荷低
	serialdat[6] = 4;//频率
	serialdat[7] = playcount >> 8;
	serialdat[8] = playcount & 0x00ff;//次数
	serialdat[9] = 7;
	serialdat[10] = 8;//最大肌力高
	serialdat[11] = 9;//最大肌力低
	serialdat[12] = 10;
	//serialdat[13] = ((190 - Freq_Heart) *(65 / 100) + 73) >> 8;//最大摄氧量高
	//serialdat[14] = ((190 - Freq_Heart) *(65 / 100) + 73) & 0x00ff;//最大摄氧量低位
	serialdat[15] = 0;//空
	serialdat[16] = 0;//空
	serialdat[17] = 0;//校验位
	QF_CRC(serialdat, 20);
	MY_USART_SendData(huart, serialdat, SERIALDATLEN);
	//第5帧
	memset(serialdat, 0, SERIALDATLEN);//清空数组
	serialdat[0] = 0xaa;
	serialdat[1] = 0x55;
	serialdat[2] = 0x05;
	QF_CRC(serialdat, 20);
	MY_USART_SendData(huart, serialdat, SERIALDATLEN);
	//第6帧
	memset(serialdat, 0, SERIALDATLEN);//清空数组
	serialdat[0] = 0xaa;
	serialdat[1] = 0x55;
	serialdat[2] = 0x00;
	serialdat[5] = 1;
	serialdat[19] = 0xff;
	MY_USART_SendData(huart, serialdat, SERIALDATLEN);

}*/

void SendToBle(UART_HandleTypeDef *huart)// 数据整合
{

	uint8_t serialdat_1[20] = { 0 };//设备号
	uint8_t serialdat_2[20] = { 0 };//设备号
	uint8_t serialdat_3[20] = { 0 };//数据1
	uint8_t serialdat_4[20] = { 0 };//数据2
	uint8_t serialdat_5[20] = { 0 };//数据3
	uint8_t serialdat_6[20] = { 0 };//数据4
	//第3帧数据
	serialdat_3[0] = 0xaa;
	serialdat_3[1] = 0x55;
	serialdat_3[2] = 0x03;
	serialdat_3[3] = playtim >> 8;
	serialdat_3[4] = playtim & 0x00ff;
	serialdat_3[5] = playcount>> 8;
	serialdat_3[6] = playcount & 0x00ff;
	serialdat_3[7] = playhot >> 8;
	serialdat_3[8] = playhot & 0x00ff;//转速无
	serialdat_3[9] = 0; //心率 -----------------无心率传感器
	serialdat_3[10] = 10;
	serialdat_3[11] = playFreq>> 8;//坡度
	serialdat_3[12] = playFreq & 0x00ff;//档位
	serialdat_3[13] = 13;//功率高位
	serialdat_3[14] = 14;//功率低位
	serialdat_3[15] = 15;//力矩高
	serialdat_3[16] = 16;//力矩低位
	serialdat_3[17] = 17;//校验位
	serialdat_3[18] = 18;//校验位
	serialdat_3[19] = 0xff;//校验位
	QF_CRC(serialdat_3, 20);

	//第4帧数据
	serialdat_4[0] = 0xaa;
	serialdat_4[1] = 0x55;
	serialdat_4[2] = 0x04;
	serialdat_4[3] = 1;//模式
	serialdat_4[4] = 2;//负荷高
	serialdat_4[5] = 3;//负荷低
	serialdat_4[6] = 4;//频率
	serialdat_4[7] = playcount >> 8;
	serialdat_4[8] = playcount & 0x00ff;//次数
	serialdat_4[9] = 7;
	serialdat_4[10] = 8;//最大肌力高
	serialdat_4[11] = 9;//最大肌力低
	serialdat_4[12] = 10;
	serialdat_4[13] = ((190 - 82) *(65 / 100) + 73) >> 8;//最大摄氧量高
	serialdat_4[14] = ((190 - 82) *(65 / 100) + 73) & 0x00ff;//最大摄氧量低位
	serialdat_4[15] = 0;//空
	serialdat_4[16] = 0;//空
	serialdat_4[17] = 0;//校验位
	serialdat_4[18] = 0;//校验位
	QF_CRC(serialdat_4, 20);
	//第5帧数据
	serialdat_5[0] = 0xaa;
	serialdat_5[1] = 0x55;
	serialdat_5[2] = 0x05;
	serialdat_5[3] = 0;
	serialdat_5[4] = 0;
	serialdat_5[5] = 0;
	serialdat_5[6] = 0;
	serialdat_5[7] = 0;
	serialdat_5[8] = 0;
	serialdat_5[9] = 0;
	serialdat_5[10] = 0;
	serialdat_5[11] = 0;
	serialdat_5[12] = 0;
	serialdat_5[13] = 0;
	serialdat_5[14] = 0;
	serialdat_5[15] = 0;
	serialdat_5[16] = 0;
	serialdat_5[17] = 0;
	serialdat_5[18] = 0;
	QF_CRC(serialdat_5, 20);


	serialdat_6[0] = 0xaa;
	serialdat_6[1] = 0x55;
	serialdat_6[2] = 0x0;
	serialdat_6[3] = 0;
	serialdat_6[4] = 0;
	serialdat_6[5] = 1;
	serialdat_6[6] = 0;
	serialdat_6[7] = 0;
	serialdat_6[8] = 0;
	serialdat_6[9] = 0;
	serialdat_6[10] = 0;
	serialdat_6[11] = 0;
	serialdat_6[12] = 0;
	serialdat_6[13] = 0;
	serialdat_6[14] = 0;
	serialdat_6[15] = 0;
	serialdat_6[16] = 0;
	serialdat_6[17] = 0;
	serialdat_6[18] = 0;
	serialdat_6[19] = 0xff;

	//设备信息aa 55 01 30 30 30 4a 53 2d 35 30 32 38 00 00 00 00 00 00 f6

	serialdat_1[0] = 0xaa;
	serialdat_1[1] = 0x55;
	serialdat_1[2] = 0x01;
	serialdat_1[3] = EquipmentNumber[0];
	serialdat_1[4] = EquipmentNumber[1];
	serialdat_1[5] = EquipmentNumber[2];
	serialdat_1[6] = EquipmentNumber[3];
	serialdat_1[7] = EquipmentNumber[4];
	serialdat_1[8] = EquipmentNumber[5];
	serialdat_1[9] = EquipmentNumber[6];
	serialdat_1[10] = EquipmentNumber[7];
	serialdat_1[11] = EquipmentNumber[8];
	serialdat_1[12] = EquipmentNumber[9];
	serialdat_1[13] = 0;
	serialdat_1[14] = 0;
	serialdat_1[15] = 0;
	serialdat_1[16] = 0;
	serialdat_1[17] = 0;
	serialdat_1[18] = 0;
	serialdat_1[19] = 0xf6;
	QF_CRC(serialdat_1, 20);
	//aa 55 02 30 30 30 4a 53 2d 35 30 32 38 00 00 00 00 00 00 f6
	serialdat_2[0] = 0xaa;
	serialdat_2[1] = 0x55;
	serialdat_2[2] = 0x02;
	serialdat_2[3] = EquipmentNumber[10];
	serialdat_2[4] = EquipmentNumber[11];
	serialdat_2[5] = EquipmentNumber[12];
	serialdat_2[6] = EquipmentNumber[13];
	serialdat_2[7] = EquipmentNumber[14];
	serialdat_2[8] = EquipmentNumber[15];
	serialdat_2[9] = EquipmentNumber[16];
	serialdat_2[10] = EquipmentNumber[17];
	serialdat_2[11] = EquipmentNumber[18];
	serialdat_2[12] = EquipmentNumber[19];
	serialdat_2[13] = 0x00;
	serialdat_2[14] = 0;
	serialdat_2[15] = 0;
	serialdat_2[16] = 0;
	serialdat_2[17] = 0;
	serialdat_2[18] = 0;
	serialdat_2[19] = 0xf6;
	QF_CRC(serialdat_2, 20);

	/*MY_USART_SendData(huart, serialdat_1, SERIALDATLEN);
	MY_USART_SendData(huart, serialdat_2, SERIALDATLEN);
	MY_USART_SendData(huart, serialdat_3, SERIALDATLEN);
	MY_USART_SendData(huart, serialdat_4, SERIALDATLEN);
	MY_USART_SendData(huart, serialdat_5, SERIALDATLEN);
	MY_USART_SendData(huart, serialdat_6, SERIALDATLEN);*/
	HAL_UART_Transmit(huart, serialdat_1, SERIALDATLEN, 20);
	HAL_UART_Transmit(huart, serialdat_2, SERIALDATLEN, 20);
	HAL_UART_Transmit(huart, serialdat_3, SERIALDATLEN, 20);
	HAL_UART_Transmit(huart, serialdat_4, SERIALDATLEN, 20);
	HAL_UART_Transmit(huart, serialdat_5, SERIALDATLEN, 20);
	HAL_UART_Transmit(huart, serialdat_6, SERIALDATLEN, 20);
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


//第3帧数据

