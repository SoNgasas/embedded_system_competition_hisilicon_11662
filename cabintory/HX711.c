
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include "HX711.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"

uint32_t HX711_Buffer;
uint32_t Weight_Maopi;
int32_t Weight_Shiwu;
uint8_t Flag_Error = 0;
IotGpioValue HX711_DOUT_val;

#define GapValue (106.5*2)

void Init_HX711pin(void)
{
	IoTGpioInit(HX711_SCK);
	IoTGpioInit(HX711_DOUT);
	IoTGpioSetDir(HX711_SCK, IOT_GPIO_DIR_OUT);
	IoTGpioSetDir(HX711_DOUT, IOT_GPIO_DIR_IN);
	IoSetFunc(HX711_SCK, IOT_IO_FUNC_GPIO_9_GPIO);
	IoSetFunc(HX711_DOUT, IOT_IO_FUNC_GPIO_10_GPIO);
	IoTGpioSetOutputVal(HX711_SCK, IOT_GPIO_VALUE0);
	IoTGpioSetOutputVal(HX711_DOUT, IOT_GPIO_VALUE0);
	printf("Init_HX711pin init success!\r\n");
}

uint32_t HX711_Read(void)
{
	// printf("HX711_Read START!\r\n");
	unsigned long count;
	unsigned char i;
	count = 0;
	IoTGpioSetOutputVal(HX711_DOUT, IOT_GPIO_VALUE1);
	usleep(1);
	IoTGpioSetOutputVal(HX711_SCK, IOT_GPIO_VALUE0);

	while (HX711_DOUT_val != 0)
	{
		IoTGpioGetInputVal(HX711_DOUT, &HX711_DOUT_val);
	}

	for (i = 0; i < 24; i++)
	{
		IoTGpioSetOutputVal(HX711_SCK, IOT_GPIO_VALUE1);
		count = count << 1;
		usleep(0.5);
		IoTGpioSetOutputVal(HX711_SCK, IOT_GPIO_VALUE0);
		IoTGpioGetInputVal(HX711_DOUT, &HX711_DOUT_val);
		if (HX711_DOUT_val)count++;
		usleep(0.5);
	}

	// printf("count = %d\r\n", count);
	IoTGpioSetOutputVal(HX711_SCK, IOT_GPIO_VALUE1);
	count = count ^ 0x800000;
	usleep(1);

	IoTGpioSetOutputVal(HX711_SCK, IOT_GPIO_VALUE0);
	return (count);
}

void Get_Maopi(void)
{
	Weight_Maopi = HX711_Read();
}

void Get_Weight(void)
{
	//printf("Get_Weight START!\r\n");
	HX711_Buffer = HX711_Read();
	if (HX711_Buffer > Weight_Maopi)
	{
		Weight_Shiwu = HX711_Buffer;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;

		Weight_Shiwu = (int32_t)((float)Weight_Shiwu / GapValue);
	}
}

//此函数为使用例程，仅供参考
// void HX711_Get_Weight()
// {
// 	// printf("Enter task1()!\r\n");
// 	// Init_HX711pin();
// 	Get_Maopi();
// 	Get_Weight();
// 	int32_t nothing_weight = Weight_Shiwu;//去皮后的Weight_Shiwu不为0或太大，看一看空载时的重量，这个值就是nothing_weight
// 	Get_Weight();


// 	while (1)
// 	{
// 		Get_Weight();
// 		Weight_Result= Weight_Shiwu-nothing_weight;//减去空载时的重量，就是净重
// 		osDelay(100);
// 	}
//}

#define TASK_STACK_SIZE (1024 * 10)
