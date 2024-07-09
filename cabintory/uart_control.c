/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
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

#include "iot_gpio_ex.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_uart.h"
#include "hi_uart.h"
#include "iot_watchdog.h"
#include "iot_errno.h"

#include "uart_control.h"

typedef struct
{
    char id[5];
    int probability;
} RecognitionResult;

unsigned char uartReadBuff[UART_BUFF_SIZE] = {0};
unsigned char recognitionResult[5] = {0};
char *id[5] = {0};
void Uart1GpioInit(void)
{
    IoTGpioInit(IOT_IO_NAME_GPIO_0);
    // 设置GPIO0的管脚复用关系为UART1_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_0, IOT_IO_FUNC_GPIO_0_UART1_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_1);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_1, IOT_IO_FUNC_GPIO_1_UART1_RXD);
}

void Uart1Config(void)
{
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(HI_UART_IDX_1, &uart_attr);
    if (ret != IOT_SUCCESS)
    {
        printf("Init Uart1 Falied Error No : %d\n", ret);
        return;
    }
}



void UartTask(void)
{
    IoTWatchDogDisable();
    // const char *data = "Hello OpenHarmony !!!\n";
    
    uint32_t len = 0;
    char id[5] = {0};    // 旧的
    char preid[5] = {0}; // 最新获取
    // 对UART1的一些初始化 Some initialization of UART1
    Uart1GpioInit();
    // 对UART1参数的一些配置 Some configurations of UART1 parameters
    Uart1Config();

    while (1)
    {
        memset(uartReadBuff, 0, UART_BUFF_SIZE);
        // 通过UART1 接收数据 Receive data through UART1
        len = IoTUartRead(HI_UART_IDX_1, uartReadBuff, UART_BUFF_SIZE);
        if (len > 0)
        {
            // printf("Uart Read Data is: [ %d ] %s check \r\n", count, uartReadBuff);
            strcpy(preid,uartReadBuff);
            preid[4] = '\0'; // 确保字符串以\0 结束

            // printf("preid is %s\n id is %s\n", preid,id);
            if (strcmp(preid, id) != 0)
            {
                //("Uart Read Data is: [ %d ] %s check \r\n", count, preid);
                strcpy(id, preid);
            }
            else
            {
                strcpy(recognitionResult, preid);
                // printf("stable id is %s\n", preid);
            }
        }
        // memset(uartReadBuff, 0, UART_BUFF_SIZE);
        // // 通过UART1 接收数据 Receive data through UART1
        // len = IoTUartRead(HI_UART_IDX_1, uartReadBuff, UART_BUFF_SIZE);
        // if (len > 0)
        // {
        //     // 把接收到的数据打印出来 Print the received data
        //     char *preid = getMaxProbabilityResult(uartReadBuff); // 获取现在的id，根据概率最大
        //     preid[4] = '\0';                                     // 确保字符串以\0 结束
        //     if (strlen(preid) == 4)
        //     {
        //         if (strcmp(preid, id) != 0)//输出识别稳定的结果
        //         {
        //             //("Uart Read Data is: [ %d ] %s check \r\n", count, preid);
        //             strcpy(id, preid);
        //         }
        //         else
        //         {
        //             strcpy(recognitionResult, preid);
        //             printf("stable recognitionResult is %s\n", recognitionResult);
        //         }
        //     }
        //     usleep(U_SLEEP_TIME);
        //     count++;
        // }
    }
}

// void UartExampleEntry(void)
// {
//     osThreadAttr_t attr;

//     attr.name = "UartTask";
//     attr.attr_bits = 0U;
//     attr.cb_mem = NULL;
//     attr.cb_size = 0U;
//     attr.stack_mem = NULL;
//     attr.stack_size = 5 * 1024; // 任务栈大小*1024 stack size 5*1024
//     attr.priority = osPriorityNormal;

//     if (osThreadNew((osThreadFunc_t)UartTask, NULL, &attr) == NULL) {
//         printf("[UartTask] Failed to create UartTask!\n");
//     }

// }
