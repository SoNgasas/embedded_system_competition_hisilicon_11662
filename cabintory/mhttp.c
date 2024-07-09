
#include "hi_stdlib.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "mhttp.h"
#define HTTPC_DEMO_RECV_BUFSIZE 64
#define SOCK_TARGET_PORT  80

#define MAX_RESPONSE_SIZE 512 // 假设HTTP响应不会超过0.5KB

/*static const char *Request = "GET / HTTP/1.1\r\n\
Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n\
Host: baidu.com\r\n\
Connection: close\r\n\
\r\n";*/

static const char *Request = "GET /rest/api3.do?api=mtop.common.getTimestamp HTTP/1.1\r\n\
Content-Type: application/json;charset=utf-8\r\n\
Host: api.m.taobao.com\r\n\
Connection: Keep Alive\r\n\
\r\n";

const char addr_ip[] = "140.205.162.6";
/*****************************************************************************
 * Func description: demo for http get action
 *****************************************************************************/
char response_buf[MAX_RESPONSE_SIZE] = {0}; // 初始化为空字符串
int response_len = 0;                       // 用于跟踪当前已存储的字符数
char date_buf[100]= {0};

void extract_date(const char *headers, char *date_buf, size_t buf_size)
{
    const char *start = strstr(headers, "Date:");
    if (!start)
    {
        date_buf[0] = '\0'; // 如果没有找到，则使date_buf为空字符串
        return;
    }

    // 跳过"Date:"字符串及其后的所有空白字符（空格、制表符等）
    while (*start && (*start == ' ' || *start == '\t' || *start == ':'))
        start++;

    // 现在start指向日期字符串的第一个非空白字符

    // 复制内容到date_buf，直到遇到换行符、回车符或达到buf_size-1
    const char *end = strchr(start, '\r');
    if (!end)
        end = strchr(start, '\n');
    if (!end)
        end = headers + strlen(headers); // 防止超出headers字符串的末尾

    size_t length = (size_t)(end - start);
    if (length >= buf_size)
        length = buf_size - 1; // 确保不会超出buf_size

    // 复制并添加空终止符
    memcpy(date_buf, start + 6, length - 6);
    date_buf[length] = '\0';
}

unsigned int http_clienti_get(void)
{
    struct sockaddr_in addr = {0};
    int s, r;
    char recv_buf[HTTPC_DEMO_RECV_BUFSIZE];
    addr.sin_family = AF_INET;
    addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
    addr.sin_addr.s_addr = inet_addr(addr_ip);
    s = socket(AF_INET, SOCK_STREAM, 0);
 
    if (s < 0)
    {
        return 1;
    }
    //printf("... allocated socket");
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        printf("... socket connect failed errno=%d", errno);
        lwip_close(s);
        return 1;
    }
    //printf("... connected");
    if (lwip_write(s, Request, strlen(Request)) < 0)
    {
        lwip_close(s);
        return 1;
    }
    //("... socket send success");
    struct timeval receiving_timeout;
    /* 5S Timeout */
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0)
    {
        printf("... failed to set socket receiving timeout");
        lwip_close(s);
        return 1;
    }
    //printf("... set socket receiving timeout success");
    /* Read HTTP response */
    int loop = 0;
    do
    {
        (void)memset_s(recv_buf, sizeof(recv_buf), 0, sizeof(recv_buf));
        r = lwip_read(s, recv_buf, sizeof(recv_buf) - 1);

        // 检查是否还有空间来存储新读取的数据
        if (response_len + r >= MAX_RESPONSE_SIZE - 1)
        {
            // 响应太大，无法存储。处理错误或截断。
            break;
        }
        memcpy(response_buf + response_len, recv_buf, r);
        response_len += r;

        // for (int i = 0; i < r; i++) {

        //     putchar(recv_buf[i]);
        // }
    loop++;
    } while (r > 0);
     printf("loop=%d\n", loop);
    response_len=0;
    extract_date(response_buf, date_buf, sizeof(date_buf));
    //printf("%s", date_buf);
    //printf("... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
    lwip_close(s);
    return 0;
}

void time_get_task(void)
{
    while (1)
    {
        http_clienti_get();
        osDelay(100);
        memset(response_buf, 0, sizeof(response_buf));
    }
}