#ifndef __HX711_H
#define __HX711_H

// #include "sys.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include <stdint.h>

#define HX711_SCK HI_IO_NAME_GPIO_9
#define HX711_DOUT HI_IO_NAME_GPIO_10


void Init_HX711pin(void);
uint32_t HX711_Read(void);
void Get_Maopi(void);
void Get_Weight(void);
//int32_t HX711_Get_Weight(void);//此函数为使用例程，仅供参考

extern uint32_t Weight_Maopi;
extern int32_t Weight_Shiwu;
extern float GapValue;

#endif

