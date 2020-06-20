#ifndef  _TOOLAPP_H
#define  _TOOLAPP_H
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#define HIGHT                 1
#define LOW                   0
//触发器数组大小
#define TRLENGHT              10


typedef void(*TrlCallback)(uint8_t);
void SingleTrig(TrlCallback p, uint8_t val, uint8_t inval, uint8_t index);

#endif // ! _TOOLAPP_H
