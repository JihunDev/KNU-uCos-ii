
#include "includes.h"

#ifndef _UART_H_
#define _UART_H_


void init_uart(void)
{
	// USART 초기화
	UCSR0A = 0x0;
	UCSR0B = 0x08;    // 송신 인에이블 TXEN = 1
	UCSR0C = 0x06;    // 비동기 데이터 8비트 모드       
	UBRR0H = 0;		// X-TAL = 16MHz 일때, BAUD = 9600
    UBRR0L = 103;
}

// 1 byte 전송 함수
void PutChar (unsigned char data)
{
	while(!(UCSR0A & 0x20));	// UDRE0 = 1 ?
	UDR0 = data;	               	// USART0 I/O 레지스터에 데이터 전송
}

// 한 바이트 수신                 
char Getch(void)
{      
	while(!(UCSR0A & 0x80));	// RXC = 1 ?
	return UDR0;	  	        // USART I/O 레지스터 리턴
}


void PutString (const char *s)
{
    while (*s != '\0') {
        PutChar(*s++);
    }
}

// 문자열 전송 함수
void PutString2 (unsigned char *str, unsigned char len)
{
	int i;
	
	for (i = 0; i < len; i++)
	{
		if (!(*(str + i)))
		{
			break;
		}
		
		PutChar (*(str + i));
	}
}

void PutDec (INT8U x2)
{
    INT8U x0;
    INT8U x1;

    x0  = (x2 % 10);
    x2 /= 10;
    x1  = (x2 % 10);
    x2 /= 10;
    if (x2) {
        PutChar(x2 + '0');
    }
    if (x1 || x2) {
        PutChar(x1 + '0');
    }
    PutChar(x0 + '0');

}


#endif
