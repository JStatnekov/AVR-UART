

#include <avr/io.h>
#include "../LCDControl.h"
#include "uart.h"

int TestUART(void){
	
	LCDInit();
	UARTInit();
	DDRD |= (1<<PORTD3);
	
	UARTSendStr("Hello\n");
	LCDWriteStr("waiting...",0);
	
	uint8_t columnPosition=0, rowPosition=0;
	char input;
	while(1) {
		
		LCDSetTextCursorPosition(rowPosition, columnPosition++);
		if(columnPosition > 18)
		{
			rowPosition = (rowPosition+1)%8;
			columnPosition = 0;
		}
		
		input = UARTGetChar();
		PORTD ^= (1<<PORTD3);//we flash the light every time we recieve a character
		
		UARTSendStr("You wrote ");
		UARTSendChar(input);
		UARTSendChar('\n');
		LCDWriteChar(input,0,1);		
	}
};
