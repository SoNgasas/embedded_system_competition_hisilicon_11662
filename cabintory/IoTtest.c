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

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "cJSON.h"
#include "hi_nv.h"
#include "sys_config.h"

#include "mqtt_send_task.h"
#include "mqtt_recv_task.h"
#include "sensor_runcollect_task.h"
#include "uart_control.h"
#include "mhttp.h"
#include "udp_recv.h"
#include "hal_bsp_wifi.h"
#include "hal_bsp_mqtt.h"
#define TASK_STACK_SIZE (20 * 1024)
#define TASK_INIT_DELAY 1 // s

osThreadId_t mqtt_send_task_id;      // mqtt 发布数据任务ID
osThreadId_t mqtt_recv_task_id;      // mqtt 接收数据任务ID
osThreadId_t onlinetime_getid;       // mqtt 订阅数据任务ID
osThreadId_t sensor_collect_task_id; // 传感器采集任务ID
osThreadId_t sensor_run_task_id;     // 传感器运行任务ID
osThreadId_t UartTaskid;
void CleanRecognitionResult(void)
{
    while (1)

    {   memset(recognitionResult, 0, 5);
        printf("recognitionResult is cleaned!\n");
        osDelay(500);
    }
    
}

int mqtt_init(void)
{
    // 连接MQTT服务器
    while (MQTTClient_connectServer(SERVER_IP_ADDR, SERVER_IP_PORT) != WIFI_SUCCESS) {
        printf("mqttClient_connectServer\r\n");

        sleep(TASK_INIT_DELAY);

    }
    printf("mqttClient_connectServer\r\n");

    sleep(TASK_INIT_DELAY);

    // 初始化MQTT客户端
    while (MQTTClient_init(MQTT_CLIENT_ID, MQTT_USER_NAME, MQTT_PASS_WORD) != WIFI_SUCCESS) {
        printf("mqttClient_init\r\n");

        sleep(TASK_INIT_DELAY);

    }
    printf("mqttClient_init\r\n");

    sleep(TASK_INIT_DELAY);

    // 订阅MQTT主题
    while (MQTTClient_subscribe(MQTT_TOPIC_SUB_COMMANDS) != 0) {
        printf("mqttClient_subscribe\r\n");

        sleep(TASK_INIT_DELAY);
        //SSD1306_CLS(); // 清屏
    }
    printf("mqttClient_subscribe\r\n");

    sleep(TASK_INIT_DELAY);
    //SSD1306_CLS(); // 清屏

    return 0;
}
static void cabintory(void)
{
    printf("Enter cabintory()!\r\n");
    p_MQTTClient_sub_callback = &mqttClient_sub_callback;
   
    if (WiFi_connectHotspots(WIFI_SSID, WIFI_PAWD) != WIFI_SUCCESS) {
        printf("[error] connectWiFiHotspots\r\n");
    }

    mqtt_init();
  
    osThreadAttr_t options;
    options.name = "mqtt_send_task";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = TASK_STACK_SIZE;
    options.priority = osPriorityNormal;

    mqtt_send_task_id = osThreadNew((osThreadFunc_t)mqtt_send_task, NULL, &options);
    if (mqtt_send_task_id != NULL) {
        printf("ID = %d, Create mqtt_send_task_id is OK!\r\n", mqtt_send_task_id);
    }

    options.name = "mqtt_recv_task";
    mqtt_recv_task_id = osThreadNew((osThreadFunc_t)mqtt_recv_task, NULL, &options);
    if (mqtt_recv_task_id != NULL) {
        printf("ID = %d, Create mqtt_recv_task_id is OK!\r\n", mqtt_recv_task_id);
    }

    options.stack_size = 5*1024;
    options.name = "onlinetime_get_task";
    onlinetime_getid = osThreadNew((osThreadFunc_t)time_get_task, NULL, &options);

 

    options.name = "udp_recv";

    options.stack_size = 1024 * 4;

    if (osThreadNew((osThreadFunc_t) udp_recv_task, NULL, &options) == NULL) {
        printf("Create udp recv task Failed!\r\n");
    }
    options.priority = osPriorityNormal1;
    options.stack_size= 5* 1024;
    options.name = "sensor_run_task";
    sensor_run_task_id = osThreadNew((osThreadFunc_t)sensor_run_task, NULL, &options);
    if (sensor_run_task_id != NULL) {
        printf("ID = %d, Create sensor_run_task_id is OK!\r\n", sensor_run_task_id);
    }
    options.priority = osPriorityNormal;
    options.stack_size = 5 * 1024;
    options.name = "sensor_collect_task";
    sensor_collect_task_id = osThreadNew((osThreadFunc_t)sensor_collect_task, NULL, &options);
    if (sensor_collect_task_id != NULL) {
        printf("ID = %d, Create sensor_collect_task_id is OK!\r\n", sensor_collect_task_id);
    }

    
    options.stack_size = 5 * 1024;
    options.name = "UartTask";
    options.stack_size = 5 * 1024; // 任务栈大小*1024 stack size 5*1024
    UartTaskid = osThreadNew((osThreadFunc_t)UartTask, NULL, &options);
    if (UartTaskid == NULL) {
        printf("[UartTask] Failed to create UartTask!\n");
    }
    options.stack_size =  1024;
    options.name = "cleanrecognitionResult";
    if (osThreadNew((osThreadFunc_t)CleanRecognitionResult, NULL, &options) == NULL) {
        printf("Create cleanrecognitionResult Failed!\r\n");
    }
}

SYS_RUN(cabintory);
