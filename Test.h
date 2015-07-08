

#include <avr/io.h>
#include "../LCDControl.h"
#include "uart.h"

int TestUART(){
	
	LCDInit();
	UARTInitBlocking();
	DDRD |= (1<<PORTD3);
	
	UARTSendStrBlocking("Hello\n");
	LCDWriteStr("waiting...",0);
	uint8_t firstTimeThrough = 1;
	
	uint8_t columnPosition=0, rowPosition=0;
	char input;
	while(1) {
		
		LCDSetTextCursorPosition(rowPosition, columnPosition++);
		if(columnPosition > 18)
		{
			rowPosition = (rowPosition+1)%8;
			columnPosition = 0;
		}
		
		input = UARTGetCharBlocking();
		PORTD ^= (1<<PORTD3);//we flash the light every time we receive a character
		
		UARTSendStrBlocking("You wrote ");
		UARTSendCharBlocking(input);
		UARTSendCharBlocking('\n');
		if(firstTimeThrough)
		{
			firstTimeThrough = 0;
			LCDClear();
		}
		
		LCDWriteChar(input,0,1);		
	}
};



int TestUARTAsynch(){
	
	LCDInit();
	UARTAsynchInit();
	
	DDRD |= (1<<PORTD3);
	PORTD ^= (1<<PORTD3);
	
	UARTSendStrBlocking("Hello\n");
	
	uint8_t firstTimeThrough = 1;
	
	uint8_t columnPosition=0, rowPosition=0;
	char input;
	while(1) {
			
		char * buffer = GetBuffer();
			
		LCDSetTextCursorPosition(0, 0);	
		LCDWriteStr("BuffAddr ",0);
		LCDWriteSmallNumberAsString((uintptr_t)buffer);
		LCDSetTextCursorPosition(1, 0);
		LCDWriteStr("ReadAddr ",0);	
		LCDWriteSmallNumberAsString((uintptr_t)GetLastReadCharPtr());
		LCDSetTextCursorPosition(2, 0);
		LCDWriteStr("RecAddr  ",0);
		LCDWriteSmallNumberAsString((uintptr_t)GetLastRecievedCharPtr());
		
		LCDSetTextCursorPosition(3, 0);
		for(int i=0; i <UARTBufferSize;i++){
			LCDWriteChar(buffer[i],0,1);
			if(i % 16 == 0)//16 chars per line
			{
				LCDSetTextCursorPosition(3 + i/16,0);
			}
		}
		
		//we wait until the buffer is full		
		if(UARTIsBufferFull())
		{
			UARTSendStrBlocking("You wrote ");
			do{
				input = UARTAsynchReadFromBuffer();
				
				UARTSendCharBlocking(input);
				if(firstTimeThrough)
				{
					firstTimeThrough = 0;
					LCDClear();
				}
		}while(input != 0);
		UARTSendCharBlocking('\n');
		}
	}
};