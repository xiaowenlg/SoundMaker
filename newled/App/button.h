/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
/* GPIO����ģʽ
��1��GPIO_Mode_AIN ģ������
��2��GPIO_Mode_IN_FLOATING ��������
��3��GPIO_Mode_IPD ��������
��4��GPIO_Mode_IPU ��������
��5��GPIO_Mode_Out_OD ��©���
��6��GPIO_Mode_Out_PP �������
��7��GPIO_Mode_AF_OD ���ÿ�©���
��8��GPIO_Mode_AF_PP �����������
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
    uint16_t pin;                 //���ź�
	GPIO_TypeDef *GPIOx;         //�˿ں�

    enum my_button_event event;

    my_button_callback cb;
};

uint8_t my_button_register(struct my_button *button);
uint8_t my_button_start(void);

uint8_t scanGPIOB_start();
#endif
