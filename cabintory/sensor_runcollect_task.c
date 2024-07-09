/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., Ltd
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "sensor_runcollect_task.h"

#include "sys_config.h"

// #include "hcsr04.h"
// #include "gy906.h"
#include "HX711.h"
#include "uart_control.h"
#include "basket.h"
#include "mhttp.h"
#include "udp_recv.h"
msg_data_t sys_msg_data = {0}; // 传感器的数据
static uint8_t fan_gif_index = 0;
#define FAN_GIF_INDEX_MAX 3

int32_t Weight_Result;


#define TASK_DELAY_TIME (1000 * 1000) // us
#define COEFFICIENT_10 10
#define COEFFICIENT_100 100
#define COEFFICIENT_1000 1000

#define SEC_MAX 60
#define MIN_MAX 60
#define HOUR_MAX 24

/**
 * @brief  传感器采集任务
 * @note
 * @retval None
 */


void sensor_collect_task(void)
{   
    //请先初始化
    uint8_t displayBuff[20] = {0};

    Init_HX711pin();

    while (1) {
        // 采集传感器的值
        //SHT20_ReadData(&temperature, &humidity);
        
        // sys_msg_data.temperature = (float)gy906_get();
        // sys_msg_data.distance = (float)GetDistance();
        sys_msg_data.temperature = udp_recv_temperature;
        sys_msg_data.humidity = udp_recv_humidity;
        sys_msg_data.modeswitch = 1;
        sys_msg_data.totalweight = Weight_Result;
        usleep(2*TASK_DELAY_TIME);//每隔2s采集一次数据
        }
}


//以下内容是一坨乱七八糟的代码，不知道是什么鬼
int32_t Weight_last=0;
int32_t Weight_new=0;
int32_t Weight_diff=0;
int32_t Weight_stable_last=0;
int32_t Weight_stable_new=0;
int32_t Weight_stable_diff=0;


void sensor_run_task(void)
{
   Init_HX711pin();
	Get_Maopi();
	Get_Weight();
	int32_t nothing_weight = Weight_Shiwu;//去皮后的Weight_Shiwu不为0或太大，看一看空载时的重量，这个值就是nothing_weight

	printf("nothing_weight %d\r\n", nothing_weight);
	// printf("Weight_Maopi = %d\r\n", Weight_Maopi);
	while (1)
	{
		Get_Weight();
		Weight_last=Weight_new;
		Weight_new=Weight_Shiwu-nothing_weight;
		printf("Weight_Shiwu = %d\r\n", Weight_new);//减去空载时的重量，就是净重
		printf("recognitionResult = %s\r\n",recognitionResult);
		printf("time now= %s\r\n",date_buf);
		Weight_diff=Weight_new-Weight_last;
		if(Weight_diff<5&&Weight_diff>-5)
		{
			Weight_stable_last=Weight_stable_new;
			Weight_stable_new=Weight_new;
			Weight_stable_diff=Weight_stable_new-Weight_stable_last;
			printf("Weight_stable_new = %d\r\nWeight_stable_diff = %d\r\n", Weight_stable_new,Weight_stable_diff);
			if(strlen(recognitionResult)!=0 && Weight_stable_diff>10 ){//先识别到物品，再称重，且重量变化大于10g
				//先查菜篮子中有没有recognitionResult，有的话就修改重量，没有的话就添加
				int recognitionResultWeight = queryBasketWeight(basketList, (char *)recognitionResult);
				if (recognitionResultWeight != -1)
				{
					printf("The weight of recognitionResult is: %d\n", recognitionResultWeight);
					// int tempWeight = recognitionResultWeight + Weight_stable_diff;
					// //如果新重量小于10g，就删除
					// if (tempWeight < 10)
					// {
					// 	printf("The weight of recognitionResult is less than 10g, delete it.\n");
					// 	deleteBasketNode(&basketList, recognitionResult);
					// }
					// else
					// {
					// 	modifyBasketWeight(&basketList, recognitionResult, recognitionResultWeight + Weight_stable_diff);
					// }
					modifyBasketWeight(&basketList, recognitionResult, recognitionResultWeight + Weight_stable_diff);
				}
				else
				{
					printf("recognitionResult not found in the basket.\n");
					addToBasketList(&basketList, recognitionResult, Weight_stable_diff,date_buf);
				}
				//buffer用完清除好习惯
				memset(recognitionResult, 0, 5);
				memset(uartReadBuff, 0, UART_BUFF_SIZE);
			}
			if(strlen(recognitionResult)==0 && Weight_stable_diff<-30){//先取出物品，再识别到物品，且重量变化大于10g
				//先查菜篮子中有没有recognitionResult，有的话就修改重量，没有的话就添加
				while(strlen(recognitionResult)==0){
					osDelay(50);
				}
				int recognitionResultWeight = queryBasketWeight(basketList, (char *)recognitionResult);
				if (recognitionResultWeight != -1)
				{
					printf("The weight of recognitionResult is: %d\n", recognitionResultWeight);
					modifyBasketWeight(&basketList, recognitionResult, recognitionResultWeight + Weight_stable_diff);
				}
				else
				{
					printf("recognitionResult not found in the basket.\n");
					addToBasketList(&basketList, recognitionResult, Weight_stable_diff,date_buf);
				}
				//buffer用完清除好习惯
				memset(recognitionResult, 0, 5);
				memset(uartReadBuff, 0, UART_BUFF_SIZE);
			}
		}
		basketList = deleteLightBaskets(basketList);
		if(Weight_stable_new<20){
			//清空菜篮子
			freeBasketList(basketList);
			basketList = NULL;
		}
		printBasketList(basketList);
		osDelay(50);
	}

}
