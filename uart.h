
#include "includes.h"

#ifndef _UART_H_
#define _UART_H_


void init_uart(void)
{
	// USART �ʱ�ȭ
	UCSR0A = 0x0;
	UCSR0B = 0x08;    // �۽� �ο��̺� TXEN = 1
	UCSR0C = 0x06;    // �񵿱� ������ 8��Ʈ ���       
	UBRR0H = 0;		// X-TAL = 16MHz �϶�, BAUD = 9600
    UBRR0L = 103;
}

// 1 byte ���� �Լ�
void PutChar (unsigned char data)
{
	while(!(UCSR0A & 0x20));	// UDRE0 = 1 ?
	UDR0 = data;	               	// USART0 I/O �������Ϳ� ������ ����
}

// �� ����Ʈ ����                 
char Getch(void)
{      
	while(!(UCSR0A & 0x80));	// RXC = 1 ?
	return UDR0;	  	        // USART I/O �������� ����
}


void PutString (const char *s)
{
    while (*s != '\0') {
        PutChar(*s++);
    }
}

// ���ڿ� ���� �Լ�
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
