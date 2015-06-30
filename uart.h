

#include <stdio.h>//we need this for the FILE

#ifndef UART_H__
#define UART_H__

void UARTInit();
void UARTSendChar(char c);
void UARTSendStr(char * c);
char UARTGetChar();

#endif /* UART_H_ */