/*
系统配置数据在此
*/


#ifndef _MAINCONFIG_H_
#define _MAINCONFIG_H_
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "system.h"
#define REED_Pin GPIO_PIN_0   //干簧管引脚
#define REED_GPIO_Port GPIOA
#define REED_EXTI_IRQn EXTI0_IRQn

//------------------------------------------ 语音芯片WTN6040 忙引脚
#define WTN_BUSY_Pin			GPIO_PIN_15//忙引脚
#define WTN_GPIO_Port			GPIOB
#define WTN_BUSY_EXTI_IRQn		EXTI15_10_IRQn
#define  CLK_2A                 PBout(13) //时钟输出（位带操作）
#define  P_DATA_2A               PBout(12) //数据输出（位带操作）
#define  CLK_2A_PIN             GPIO_PIN_13//时钟引脚
#define  P_DATA_2A_PIN          GPIO_PIN_12//数据引脚
#define  SYSCLOSE_PIN		GPIO_PIN_8 //关闭系统引脚，高电平关闭
#define  SYSCLOSE_PORT		GPIOB
#define  SYSIO				PBout(8)   //位带操作

//DTH11引脚
#define DHT11_DATA_PIN			GPIO_PIN_15 //数据
#define DTH11_PORT				GPIOA     
#define DHT11_OUT				PAout(15)	//发送
#define DTH11_IN				PAin(15)	//接收

#define TIMER2_T                  1000       //T = 1000ms   //次数采集周期
#define TIMER2_PLAY_WAIT          5*1000
#define M_SIZE                 sizeof(struct Customerinfo*)   //消息大小
#define TIMER_CLOSESYSTEM		  5								//5秒
//卡路里系数
#define HOT_CAR                7            //健身车
#define HOT_MANBUJI			   5           //漫步就
#define HOT_NOUYAOQI		   3			//扭腰器
#define HOT_HUACHUANQI		   4			//划船器
#define HOT_FUJIBAN			   8			//腹肌板
#define HOT_ZZBANMO			   6			//自重式背部按摩
#define HOT_TUOYUANJI		   4            //椭圆机
#define HOT_TJROUTUI		   1			//太极揉推
#define HOT_TUIJU			   8			//推举
#define HOT_QQBAN			   4			//跷跷板
#define HOT_QIMA			   5			//骑马器
#define HOT_ZHONGBAI		   3			//钟摆器
#define MAXSPORTFREQ		   5			//最大运动频率     当运动频率大于此数时播报提示

#define PLAYARRAYLENGTH        50			//播放数组长度
//语音地址
#define S_TEN               10               //十
#define S_HUNDRED			11				//百
#define S_THOUSAND          12				//千
#define S_THISSPORT         13				//本次您活动了，
#define S_SPORTTIM          14              //活动时间
#define S_THISKCAL          15				//消耗热量
#define S_NUMBER			16				//次
#define S_HOUR				17				//时
#define S_MINUTE            18				//分
#define S_SECOND			19				//秒
#define S_KCAL              20				//千卡
#define S_POINT             21           //点 地址
#define S_WELCOME           22				//欢迎再次使用
#define S_MAXSPORT_TIP		0x20            //运动频率过大提示

//TFT相关
#define REGISTERREAD		0x81			//读寄存器
#define REGISTERWRITE		0x80
#define VOL_STATUS			0x55			//播放状态

//HMI相关
#define HMIARRAYLENGTH      64				//发送数组大小
#define QRCODE				"https://weixin.hbqifan.com?DevName=WH-BLE 102&DevAddress=9C:A5:25:95:80:1F&EquipmentNumber=00010000300009000004&"   //二维码连接信息
//---------------------------HMI控件
#define HMI_SPEED			"n0.val="		//速度
#define HMI_HOUR			"n1.val="		//时
#define HMI_MINUTE			"n2.val="		//分
#define HMI_SECOND			"n3.val="		//秒
#define HMI_COUNT			"n4.val="		//次数
#define HMI_KCAL			"x0.val="		//热量
#define HMI_GIF_EN			"gm0.en="       //动画开关 0=关，1=开
#define HTM_GIF_DIS			"gm0.dis="		//动画速度 0----100
#define HTM_BATTERY_J       "j0.val="		//电池电量进度条
#define HTM_QRCODE			"qr0.txt="		//二维码控件
#define HTM_AUDIO1			"audio1="		//音频通道1控制
#define HTM_T1				"t1.txt="
#define HMI_TEMP            "x1.val="		//温度
#define HMI_RH				"x2.val="		//湿度


//调试开关
//#define MYDEBUG
//互斥信号量开关
#define UART_MUTEX           0                 //串口是否使用互斥信号量保护，使用=1，不使用=0
//蓝牙地址长度
#define MAC_LENGTH			19
//stm32地址段
#define STM32_FLASH_BASE			0x08000000   //起始地址
#define STM32_FLASH_SIZE			1024*128	//flash大小128K
#define STM_SECTOR_SIZE				1024*2           //扇区大小
#define ENDADRR             STM32_FLASH_BASE+STM32_FLASH_SIZE-1024   //结尾地址
#define EEPROMLARGE			1024*2           //eeprom大小2K
#define EEPROM_BEGIN_ADRR   ENDADRR-EEPROMLARGE   //eeprom起始地址
#define READLEN				17					//读取数据长度

//字符串拼接
#define QR_HEAD				"https://weixin.hbqifan.com?DevName=WH-BLE 102&DevAddress="
#define QR_EQNUM			"&EquipmentNumber="
#define QR_END				"&"
//预编译选择
//#define TFTPLAY
extern uint16_t SportCount;        //运动计次
extern uint16_t TempCount;
//上传数据信息
#define SERIALDATLEN			20				//上传数据长度
#define  BUG                    1             //1调试 0实际接口
//串口分配
#if (BUG==1) 
	#define BLE_UART				Uart_2         //蓝牙串口设备接口
	#define DEBUG_UART				Uart_1			//调试串口接口
	#define HMI_UART				Uart_3        //HMI串口
	//设备串口波特率设置
	#define BLE_BAND				115200         //蓝牙串口
	#define DEBUG_BAND				115200        //调试用串口
	#define HMI_BAND				9600		//HML串口屏(淘晶驰)
#else if (BUG==0)                     //实际板中的接口
	#define BLE_UART				Uart_1         //蓝牙串口设备接口
	#define DEBUG_UART				Uart_2			//调试串口接口
	#define HMI_UART				Uart_3        //HMI串口
	//设备串口波特率设置
	#define BLE_BAND				57600         //蓝牙串口
	#define DEBUG_BAND				115200        //调试用串口
	#define HMI_BAND				9600		//HML串口屏(淘晶驰)
#endif

		  
typedef struct SportInfo      //运动信息
{
	uint16_t count;//运动次数
	uint16_t freq;//运动频率
	long hot;//消耗热量
	uint16_t tim;//运动时间
	uint8_t    playstate;//播放状态
}Customerinfo;

//分段运行频率
#define RATE_1_HZ		1
#define RATE_5_HZ		5
#define RATE_10_HZ		10
#define RATE_25_HZ		25
#define RATE_50_HZ		50
#define RATE_100_HZ		100
#define RATE_200_HZ 	200
#define RATE_250_HZ 	250
#define RATE_500_HZ 	500
#define RATE_1000_HZ 	1000

#define MAIN_LOOP_RATE  RATE_10_HZ					//主循环频率(Hz)
#define MAIN_LOOP_DT      (1000/MAIN_LOOP_RATE)          //主循环周期(ms)

#define RATE_DO_EXECUTE(RATE_HZ, TICK) ((TICK % (MAIN_LOOP_RATE / RATE_HZ)) == 0)  //  RATE_HZ:运行频率    ---分段运行     就是几个周期运行一次    定义运行周期

#define HMI_STACK_PERIOD        50            //单位ms  线程运行周期
//按周期运行

#define PERIOD_DO_EXECUTE(TICK,PERIOD)          ((TICK)%(PERIOD/HMI_STACK_PERIOD) == 0)
#define HMI_PERIOD              500
#define ADC_PERIOD				1000
#define MES_PERIOD              200



#endif // !_MAINCONFIG_H_
