/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
/* GPIO工作模式
（1）GPIO_Mode_AIN 模拟输入
（2）GPIO_Mode_IN_FLOATING 浮空输入
（3）GPIO_Mode_IPD 下拉输入
（4）GPIO_Mode_IPU 上拉输入
（5）GPIO_Mode_Out_OD 开漏输出
（6）GPIO_Mode_Out_PP 推挽输出
（7）GPIO_Mode_AF_OD 复用开漏输出
（8）GPIO_Mode_AF_PP 复用推挽输出
*/
#ifndef MY_BUTTON_H
#define MY_BUTTON_H



#define MY_BUTTON_DOWN_MS        50
#define MY_BUTTON_HOLD_MS        700

#define MY_BUTTON_SCAN_SPACE_MS  20
#define MY_BUTTON_LIST_MAX       10

typedef void (*my_button_callback)(void*);

enum my_button_event
{
    BUTTON_EVENT_CLICK_DOWN,
    BUTTON_EVENT_CLICK_UP,
    BUTTON_EVENT_HOLD,
    BUTTON_EVENT_HOLD_CYC,
    BUTTON_EVENT_HOLD_UP,
    BUTTON_EVENT_NONE
};

struct my_button
{
    uint8_t  press_logic_level;
    uint16_t cnt;
    uint16_t hold_cyc_period;
    uint16_t pin;                 //引脚号
	GPIO_TypeDef *GPIOx;         //端口号

    enum my_button_event event;

    my_button_callback cb;
};

uint8_t my_button_register(struct my_button *button);
uint8_t my_button_start(void);

uint8_t scanGPIOB_start();
#endif
