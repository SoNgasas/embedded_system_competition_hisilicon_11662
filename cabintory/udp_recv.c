#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"


#include "hal_bsp_wifi.h"
#include "wifi_device.h"
#include "lwip/sockets.h"

// #define WIFI_SSID "sldcomputer"
// #define WIFI_PASSWORD "sldnb666"
// #define HOSTNAME "itcast"
float udp_recv_temperature = 0;
float udp_recv_humidity = 0;

void udp_recv_task(void) {
    //WiFi_connectHotspots(WIFI_SSID, WIFI_PASSWORD);
    // udp create
    printf("udp_recv_task\r\n");
    int sock_fd;
    int ret;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("sock_fd create error\r\n");
        return;
    }

    // config receive addr
    struct sockaddr_in recvfrom_addr;
    socklen_t recvfrom_addr_len = sizeof(recvfrom_addr);

    memset((void *) &recvfrom_addr, 0, recvfrom_addr_len);
    recvfrom_addr.sin_family = AF_INET;
    recvfrom_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvfrom_addr.sin_port = htons(114514);

    // bind receive addr
    // bind
    ret = bind(sock_fd, (struct sockaddr *) &recvfrom_addr, recvfrom_addr_len);
    if (ret == -1) {
        perror("bind error\r\n");
        return;
    }

    char recv_buf[1024];
    int recv_len;
    while (1) {
        struct sockaddr_in sender_addr;
        int sender_addr_len;

        recv_len = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &sender_addr,
                            sender_addr_len);

        if (recv_len <= 0) {
            continue;
        }

        char recv_data[recv_len];
        memcpy(recv_data, recv_buf, recv_len);
        sscanf(recv_data, "Temperature: %f, Humidity: %f", &udp_recv_temperature, &udp_recv_humidity);
        printf("len: %d data: %s\r\n", recv_len, recv_data);
    }
}
