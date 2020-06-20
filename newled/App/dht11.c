#include "dht11.h"
#include "usart.h"
#include "MainConfig.h"
GPIO_InitTypeDef DHT11_GPIO_Init;

DHT11_Data_TypeDef DHT11_Data;
extern uint32_t tt;
/****************************************************************
*@function: dht11_gpio_init
*@input   : none
*@output  : none
*@describe: DHT11的GPIO口初始化
*@author  : sola
****************************************************************/
void dht11_gpio_init(void)
{ 
    __DHT11_PIN_CLK();               //开启时钟
    
    DHT11_GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    DHT11_GPIO_Init.Pin = DHT11_PIN;
    DHT11_GPIO_Init.Pull = GPIO_NOPULL;
    DHT11_GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_PORT, &DHT11_GPIO_Init);
}

/****************************************************************
*@function: DHT11_MODE
*@input   : mode    （0/1）
*@output  : none
*@describe: DHT11 GPIO输入输出模式选择
*@author  : sola
****************************************************************/
void DHT11_MODE(uint8_t mode)
{
    if(mode == 0)
    {
        DHT11_GPIO_Init.Mode = GPIO_MODE_INPUT;
        DHT11_GPIO_Init.Pin = DHT11_PIN;
        DHT11_GPIO_Init.Pull = GPIO_NOPULL;
		DHT11_GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DHT11_PORT, &DHT11_GPIO_Init);
    }
    else if(mode == 1)
    {
        DHT11_GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
        DHT11_GPIO_Init.Pin = DHT11_PIN;
        DHT11_GPIO_Init.Pull = GPIO_NOPULL;
		DHT11_GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DHT11_PORT, &DHT11_GPIO_Init);
    }
}

/*---------------------------应用层----------------------------*/

/****************************************************************
*@function: dht11_delay
*@input   : n (MHz)  
*@output  : none
*@describe: 延时
*@author  : sola
****************************************************************/
void  wait(unsigned long n)
{   
        do{
        n--;
        }while(n);
}

/****************************************************************
*@function: dht11_init
*@input   : none
*@output  : none
*@describe: DHT11 起始信号
*@author  : sola
****************************************************************/
void dht11_init(void)
{
    __DHT11_GPIO_INIT();
}

/****************************************************************
*@function: ReadByte
*@input   : none
*@output  : temp
*@describe: 读一个字节
*@author  : sola
****************************************************************/
uint8_t ReadByte(void) {

	uint8_t i, temp = 0;
	uint32_t time = 0, time2 = 0;
	for(i=0;i<8;i++) {

		while(__DHT11_READ() == 0);
		
		__DHT11_DELAY_US(40);               //40us
	
		if(__DHT11_READ() == 1) {
			time = HAL_GetTick();
			while (__DHT11_READ() == 1)
			{
				if (HAL_GetTick()-time>1000)//超时检测
				{
					time = HAL_GetTick();
					//Uart_printf(&DEBUG_UART.huart, "TimOuting======");
					return ERROR;
				}
			}
			

			temp |= (uint8_t)(0x01<<(7-i)); 
		}
		else {
			
			temp &= (uint8_t)~(0x01<<(7-i));
		}
	}	
	return temp;
}

/****************************************************************
*@function: Read_DHT11
*@input   : *DHT11_Data
*@output  : temp
*@describe: DHT11 读数据
*@author  : sola
****************************************************************/
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data) 
{
    
	uint32_t time = 0,time2 = 0;

	__DHT11_MODE(1);            //配置为输出
	
	__DHT11_OUT_L();            //拉低
	
	__DHT11_DELAY_MS(18);       //18ms

	__DHT11_OUT_H();            //拉高
    
    __DHT11_DELAY_US(13);       //13us

	__DHT11_MODE(0);            //配置为输入

	if(__DHT11_READ() == 0) {

		
		while (__DHT11_READ() == 0);
		while (__DHT11_READ() == 1) ;
		DHT11_Data->humi_int= ReadByte();   //读取湿度整数

		DHT11_Data->humi_deci= ReadByte();  //读取湿度小数

		DHT11_Data->temp_int= ReadByte();   //读取温度整数

		DHT11_Data->temp_deci= ReadByte();  //读取温度整数

		DHT11_Data->check_sum= ReadByte();  //应答读取

		__DHT11_MODE(1);                    //配置为输出

		__DHT11_OUT_H();

		if(DHT11_Data->check_sum == 
                                    DHT11_Data->humi_int + 
                                        DHT11_Data->humi_deci + 
                                            DHT11_Data->temp_int+ 
                                                DHT11_Data->temp_deci) {
			
			return SUCCESS;
		}
		else {		
			return ERROR;	
		}
		
	}
	
    return 0;
}


/****************************************************************
*@function: DHT11_PRINTF
*@input   : none
*@output  : none
*@describe: DHT11 打印数据
*@author  : sola
****************************************************************/
#if 1
void DHT11_PRINTF(void) {

    if(Read_DHT11(&DHT11_Data) == SUCCESS) {
              
            printf("\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",
                                                                DHT11_Data.humi_int,
                                                                DHT11_Data.humi_deci,
                                                                DHT11_Data.temp_int,
                                                                DHT11_Data.temp_deci);
  } 
}
#endif
