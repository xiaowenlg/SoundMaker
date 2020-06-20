/*!
 *
 *按键检测
 */
/*
（1）GPIO_Mode_AIN 模拟输入
（2）GPIO_Mode_IN_FLOATING 浮空输入
（3）GPIO_Mode_IPD 下拉输入
（4）GPIO_Mode_IPU 上拉输入
（5）GPIO_Mode_Out_OD 开漏输出
（6）GPIO_Mode_Out_PP 推挽输出
（7）GPIO_Mode_AF_OD 复用开漏输出
（8）GPIO_Mode_AF_PP 复用推挽输出
*/
#include "stm32f1xx_hal.h"
#include "button.h"
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"
/****    Debug     ****/
#define DBG_ENABLE
#define DBG_SECTION_NAME "button"
#define DBG_LEVEL        DBG_INFO
#define DBG_COLOR


#define MY_BUTTON_CALL(func, argv) \
    do { if ((func) != NULL) func argv; } while (0)

struct my_button_manage
{
	uint8_t num;
	TimerHandle_t timer;
    struct my_button *button_list[MY_BUTTON_LIST_MAX];
};

static struct my_button_manage button_manage;

TimerHandle_t IOtimer = NULL;//用于扫面GPIOB
uint16_t io_cnt = 0;//记录按下周期个数

//************************************
// 函数:    my_button_register
// 函数全名:  my_button_register
// 函数类型:    public 
// 返回值:   int
// Qualifier:按钮注册函数
// 参数: struct my_button * button
//************************************
uint8_t my_button_register(struct my_button *button)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	//开外设端口时钟
	if (button->GPIOx == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	if(button->GPIOx == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	if (button->GPIOx == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	if (button->GPIOx == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	if (button->GPIOx == GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Pin = button->pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
   if (button->press_logic_level == 0)
    {
	   GPIO_InitStruct.Pull = GPIO_PULLUP;//上拉输入
    }
    else
    {
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;//下拉输入
    }
   //初始化端口
   if (button->GPIOx == GPIOA)
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
   if (button->GPIOx == GPIOB)
	   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
   if (button->GPIOx == GPIOC)
	   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
   if (button->GPIOx == GPIOD)
	   HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
   if (button->GPIOx == GPIOE)
	   HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    button->cnt = 0;
    button->event = BUTTON_EVENT_NONE;
    button_manage.button_list[button_manage.num++] = button;
    
    return 0;
}

//************************************
// 函数:    my_button_scan
// 函数全名:  my_button_scan
// 函数类型:    public static 
// 返回值:   void
// Qualifier:按键扫描函数
// 参数: xTimerHandle pxTimer
//************************************
static void my_button_scan(xTimerHandle pxTimer)
{
    uint8_t i;
    uint16_t cnt_old;
	uint32_t ulTimerID;
	configASSERT(pxTimer);
	ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
	
	if (ulTimerID==0)
		//MY_USART_SendData(&huart1, "CLICK_CYC\r\n", 13);
    for (i = 0; i < button_manage.num; i++)
    {
        cnt_old = button_manage.button_list[i]->cnt;
		
		if (HAL_GPIO_ReadPin(button_manage.button_list[i]->GPIOx, button_manage.button_list[i]->pin) == button_manage.button_list[i]->press_logic_level)
        {
            button_manage.button_list[i]->cnt ++;

            if (button_manage.button_list[i]->cnt == MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_DOWN */
            {
                //LOG_D("BUTTON_DOWN");
				//MY_USART_SendData(&huart1, button_manage.button_list, 1);
                button_manage.button_list[i]->event = BUTTON_EVENT_CLICK_DOWN;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            else if (button_manage.button_list[i]->cnt == MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD */
            {
               // LOG_D("BUTTON_HOLD");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            else if (button_manage.button_list[i]->cnt > MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD_CYC */
            {
               // LOG_D("BUTTON_HOLD_CYC");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD_CYC;
                if (button_manage.button_list[i]->hold_cyc_period && button_manage.button_list[i]->cnt % (button_manage.button_list[i]->hold_cyc_period / MY_BUTTON_SCAN_SPACE_MS) == 0)
                    MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
        }
        else
        {
            button_manage.button_list[i]->cnt = 0;
            if (cnt_old >= MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS && cnt_old < MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_CLICK_UP */
            {
                //LOG_D("BUTTON_CLICK_UP");
                button_manage.button_list[i]->event = BUTTON_EVENT_CLICK_UP;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            else if (cnt_old >= MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD_UP */
            {
               // LOG_D("BUTTON_HOLD_UP");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD_UP; 
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
        }
    }
}
static void GpioTimerCallback(xTimerHandle pxTimer)
{
	uint32_t ulTimerID;
	uint16_t cnt_old;
	ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
	if (ulTimerID==1)
	{
		cnt_old = io_cnt;
		if (GPIO_RECMD(GPIOB) != 0xffff)
		{
			io_cnt++;
			if (io_cnt == MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS)
			{
				//io_cnt = 0;
				//要执行的函数

			}

		}
		else
		{
			io_cnt = 0;
		}
	}
}
uint8_t my_button_start()
{
    if (button_manage.timer != NULL)
        return -1;

    /* 创建定时器1 */
	 button_manage.timer = xTimerCreate("Timer_button",
		MY_BUTTON_SCAN_SPACE_MS,/* 定时长度，以OS Tick为单位，即10个OS Tick */
		pdTRUE,
		(void *)0,
		my_button_scan);
    /* 启动定时器 */
	 if (button_manage.timer != NULL)
	 {
		 if (xTimerStart(button_manage.timer, 100) != pdPASS)
		 {
			 return -1;
		 }
	  }

    return 0;
}

//************************************
// 函数:    scanGPIOB_start
// 函数全名:  scanGPIOB_start
// 函数类型:    public 
// 返回值:   uint8_t
// Qualifier:扫描GPIOB状态
//************************************
uint8_t scanGPIOB_start()
{
	io_cnt = 0;
	if (IOtimer != NULL)
		return -1;

	/* 创建定时器1 */
	IOtimer = xTimerCreate("Timer_GPIOB",
							MY_BUTTON_SCAN_SPACE_MS,/* 定时长度，以OS Tick为单位，即10个OS Tick */
							pdTRUE,
							(void *)1,
							GpioTimerCallback);
	/* 启动定时器 */
	if (IOtimer != NULL)
	{
		if (xTimerStart(IOtimer, 50) != pdPASS)
		{
			return -1;
		}
	}

	return 0;
}