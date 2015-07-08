#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

static volatile char UARTRecieveBuffer[UARTBufferSize];
//these are the buffer locations
static volatile char* UARTLastReadCharPtr;
static volatile char* UARTLastRecievedCharPtr;
static volatile UARTErrors UARTLastRecieveErrors;
static volatile uint8_t UARTBufferIsFull = 0;


void UARTClearRecieveBufferBlocking()
{
	UCSR0B &= ~(1<<RXEN0);
	UCSR0B |= (1<<RXEN0);
};

void UARTInitBlocking() {
	UBRR0H = UBRRH_VALUE;//these are setting the baud rate
	UBRR0L = UBRRL_VALUE;
	
	#if USE_2X
	UCSR0A |= (1<<U2X0);
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif
	
//USART Control and Status Registers
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);   //turn on the RX and TX pins
	//this is number of bits per message and is 5 + a 2bit number that takes up UCSZ00 and 1
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); //8 bit data
};

void UARTAsynchInit()
{
	UARTLastReadCharPtr = UARTRecieveBuffer;
	UARTLastRecievedCharPtr = UARTRecieveBuffer;

	UARTInitBlocking();
	UCSR0B |= (1<<RXCIE0);//UDRIE0
	sei();
};

void UARTSendCharBlocking(char c) {
	if (c == '\n') {
		UARTSendCharBlocking('\r');
	}
	while (!(UCSR0A & _BV(UDRE0))){}	
	UDR0 = c;
};

void UARTSendStrBlocking(char * c) {
	while(*c != 0)
	{
		UARTSendCharBlocking(*c);
		c++;
	}
};

char UARTGetCharBlocking() {	
	while(!(UCSR0A & (1<<RXC0))){}
	return UDR0;
};

//this returns 0 when all of the active buffer has been read
char UARTAsynchReadFromBuffer()
{
	char data = 0;
	if(UARTLastReadCharPtr != UARTLastRecievedCharPtr)
	{
		UARTBufferIsFull = 0;
		data = *UARTLastReadCharPtr;
		UARTLastReadCharPtr++;
		
		if(UARTLastReadCharPtr == UARTRecieveBuffer+UARTBufferSize)
		{
			UARTLastReadCharPtr = UARTRecieveBuffer; //circular buffer
		}
	}
	
	return data;
};

UARTErrors GetErrors()
{
	return UARTLastRecieveErrors;
};

ISR(USART_RX_vect){
	*UARTLastRecievedCharPtr = UDR0;
	
	UARTLastRecievedCharPtr++;
	if(UARTLastRecievedCharPtr == UARTRecieveBuffer+UARTBufferSize)
	{
		UARTLastRecievedCharPtr = UARTRecieveBuffer; //circular buffer
	}
	//extract status from the status register
	UARTLastRecieveErrors.frameError = (1<<FE0) & UCSR0A;
	UARTLastRecieveErrors.dataOverRun = (1<<DOR0) & UCSR0A;
	UARTLastRecieveErrors.parityError = (1<<UPE0) & UCSR0A;
	UARTLastRecieveErrors.bufferOverrun = (UARTLastRecievedCharPtr == UARTLastReadCharPtr);//this last line means that the write head has caught up to the read head
	
	
	char * tempUARTLastRecievedCharPtr = UARTLastRecievedCharPtr+1 == UARTRecieveBuffer+UARTBufferSize ? UARTRecieveBuffer : UARTLastRecievedCharPtr+1;
	UARTBufferIsFull = tempUARTLastRecievedCharPtr == UARTLastReadCharPtr;
};

uint8_t UARTIsBufferFull()
{
	return UARTBufferIsFull;
};

//these last 3 methods are for mainly for testing and POC work
volatile char * GetLastReadCharPtr()
{
	return UARTLastReadCharPtr;
};

volatile char * GetLastRecievedCharPtr()
{
	return UARTLastRecievedCharPtr;
};

volatile char * GetBuffer()
{
	return UARTRecieveBuffer;
};



