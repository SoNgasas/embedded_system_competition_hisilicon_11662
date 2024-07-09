#ifndef UART_CONTROL_H
#define UART_CONTROL_H
#define UART_BUFF_SIZE 100
#define U_SLEEP_TIME   100000
void Uart1GpioInit(void);
void Uart1Config(void);
void UartTask(void);

extern unsigned char uartReadBuff[];
extern unsigned char recognitionResult[];
#endif