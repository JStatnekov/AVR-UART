#ifndef UART_H__
#define UART_H__

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

void UARTInitBlocking();
void UARTClearRecieveBufferBlocking();
char UARTGetCharBlocking();

#define UARTBufferSize 32
void UARTAsynchInit();
char UARTAsynchReadFromBuffer();//this returns 0 when all of the active buffer has been read

void UARTSendCharBlocking(char c);
void UARTSendStrBlocking(char * c);

uint8_t UARTIsBufferFull();

typedef struct {
	uint8_t frameError, //Frame Error. Set when next byte in the UDR0 register has a framing error
	dataOverRun, //Data OverRun. Set when the UDR0 was not read before the next frame arrived
	parityError,	//USART Parity Error. Set when next frame in the UDR0 has a parity error
	bufferOverrun;
} UARTErrors;

UARTErrors GetErrors();

#endif /* UART_H_ */