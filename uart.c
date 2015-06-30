#include <avr/io.h>
#include "uart.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>


void UARTInit() {
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

void UARTSendChar(char c) {
	if (c == '\n') {
		UARTSendChar('\r');
	}
	while (!(UCSR0A & _BV(UDRE0))){}	
	UDR0 = c;
};

void UARTSendStr(char * c) {
	
	while(*c != 0)
	{
		UARTSendChar(*c);
		c++;
	}
};

char UARTGetChar() {	
	while(!(UCSR0A & (1<<RXC0))){}
	return UDR0;
};