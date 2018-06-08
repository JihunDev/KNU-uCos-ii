#include "uart.h"
#include "includes.h"

///////////////////////////LCD/////////////////////////
#include <avr/io.h>
/*
**************************************************************************************************************
*                                               CONSTANTS
**************************************************************************************************************
*/

#define  TASK_STK_SIZE  OS_TASK_DEF_STK_SIZE            /* Size of each task's stacks (# of bytes)          */
#define  N_TASKS        3                               /* Number of identical tasks                        */
#define  MAX            61                               /* Number of identical tasks                        */
///////////////////////PORT/////////////////////////////
#define DDR_LED				DDRC
#define PORT_LED				PORTC
#define PIN_LED				PINC

#define DDR_KEY				DDRE
#define PORT_KEY				PORTE
#define PIN_KEY				PINE
//////////////////////key/////////////////////////////

//////////////////////////////////////////////////////
/* #define UART_TX_BUF_SIZE 512 */
#define UART_TX_BUF_SIZE 256
//	    RS  - PD0
//	    R/W - PD1
//	    EN  - PD2
//      DB4  - PD4
//	    DB5  - PD5
//	    DB6  - PD6
//	    DB7  - PD7

#define FUNCSET       0x28	// Function Set
#define ENTMODE	      0x06	// Entry Mode Set
#define ALLCLR        0x01	// All Clear
#define DISPON        0x0c 	// Display On
#define LINE1 0x80  //ù��° ������ DDRAM address : 0x00 -> 0x80+0x00
#define LINE2 0xC0  //�ι�° ������ DDRAM address : 0x40 -> 0x80+0x40

void LCD_init(void);
void LCD_String(char *);
void Busy(void);
void Command(unsigned char);
void Data(unsigned char);
void delay_us(unsigned char);
void delay_ms(unsigned char);


/*
**************************************************************************************************************
*                                               VARIABLES
**************************************************************************************************************
*/

OS_STK          TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                     */
OS_STK          TaskStartStk[TASK_STK_SIZE];
char            TaskData[N_TASKS];                      /* Parameters to pass to each task                  */

OS_EVENT *sem;		/* �������� ������ */
OS_EVENT *mutex;		/* ���ؽ� ������ */
INT8U resourceMutex;		/* �����ڿ� */


OS_EVENT       *DispStrSem;
INT32U          RndNext;                                /* Used by random generator                         */
INT8U           UartTxBuf[UART_TX_BUF_SIZE];            /* UART transmit buffer                             */
INT16U          UartTxRdPtr;                            /* UART transmit buffer read pointer                */
INT16U          UartTxWrPtr;                            /* UART transmit buffer write pointer               */
INT16U          UartTxCount;                            /* Number of characters to send                     */
OS_EVENT       *UartTxSem;

/////////////////////////////LCD///////////////////////
INT8U    T3=0;  
INT8U    T4=0;  
INT8U    T5=0; 
INT8U    T6=0;
INT8U    T7=0;
INT8U    T8=0;
INT8U	 n1=0;
INT8U    n2=0;
INT8U    set=0;
INT8U    set1=0; 
/////////////////////////////LCD///////////////////////


/*
**************************************************************************************************************
*                                           FUNCTION PROTOTYPES
**************************************************************************************************************
*/
/* �½�ũ �Լ� */
void TaskPrio3(void *data);
void TaskPrio4(void *data);
void TaskPrio5(void *data);

//task_func task_list[N_TASKS] = {TaskPrio3, TaskPrio4, TaskPrio5};


//void  Task(void *data);                                 /* Function prototypes of tasks                     */
void  TaskStart(void *data);                            /* Function prototypes of Startup task              */
void init_tick(void);


/*
**************************************************************************************************************
*                                                MAIN
**************************************************************************************************************
*/
/////////////////���ͷ�Ʈ/////////////////////////////
void  init_ext_int(void)  // �̰� ���ٳְ� �� ���ι� ���� ��ġ�ؾߵǰ�
{
	EIMSK = (1<<INT4)|(1<<INT5);				// ���ͷ�Ʈ 4, 5��  Enable ��Ʈe 4,5
	// ���ͷ�Ʈ 0~3 ��Ʈ B, ���ͷ�Ʈ 4~7 ��Ʈ E
	EICRB = (1<<ISC40)|(1<<ISC41)|(1<<ISC50)|(1<<ISC51);	// ���ͷ�Ʈ 4, 5�� ��¿���
}
SIGNAL(SIG_INTERRUPT4) // 
{
	set1 = (set1+1)%3;
	if(set1 == 1)
	{
	OSTaskResume(5);
	}
	else if(set1 == 2)
	{
	OSTaskResume(4);
	}
	
}

SIGNAL(SIG_INTERRUPT5) //
{
	set = (set+1)%3;
	if(set == 1)
	{
	OSTaskSuspend(5);
	}
	else if(set == 2)
	{
	OSTaskResume(5);
	}
	
}

/////////////////////////////////////////////
int main (void)
{  
  port_init();
  LCD_init();
  init_ext_int();  // �� �ְ�
  OSInit();
  INT8U err;
///////////////////////////////////LCD////////////////////////////////
    Command(LINE1);
   	LCD_String("TIME");	// �ι�° ���ο� ���	 
/////////////////////////////////////LCD///////////////////////////////////
	//DDRG = 0x03;
	//PORTG = 0x00;

  sem  = OSSemCreate(1);                        /* Random number semaphore                          */
  mutex = OSMutexCreate(2, &err);	/* ���ؽ� ���� */
  DispStrSem = OSSemCreate(1);                        /* Display string semaphore                         */
  UartTxSem  = OSSemCreate(UART_TX_BUF_SIZE);         /* Initialize Uart transmit buffer semaphore        */

    OSTaskCreate(TaskStart, (void *)0, (void *)&TaskStartStk[TASK_STK_SIZE - 1], 0);

  RndNext = 1;                                        /* set random generator seed to 1                   */
  OSStart();                                          /* Start multitasking                               */
  return 0;
}

/*
**************************************************************************************************************
*                                              STARTUP TASK
**************************************************************************************************************
*/

void TaskStart (void *data)
{
  //INT8U   i;
  //char    s[10];
  char    buf[16];	
  data = data;                                        /* Prevent compiler warning                         */


  /* 
   * Enabling of timer interrupt is moved from AvrInit() to here as suggested by to book
   * JLu 01/2003
   */

  OS_ENTER_CRITICAL();
/*   TCCR0=0x05;                                 /\* on some processors other than mega128 *\/ */
  TCCR0=0x07;                                         /* Set TIMER0 prescaler to CLK/1024                 */
  TIMSK=_BV(TOIE0);                                   /* Enable TIMER0 overflow interrupt                 */
  TCNT0=256-(CPU_CLOCK_HZ/OS_TICKS_PER_SEC/1024);     /* Set the counter initial value                    */
  OS_EXIT_CRITICAL();

  OSStatInit();                                       /* Initialize uC/OS-II's statistics                 */
    
  init_tick();	/* Ÿ�̸� �ʱ�ȭ �� Ȱ��ȭ */
           
	/* N_TASKS��ŭ �½�ũ ����� */	  	
	OSTaskCreate(TaskPrio3, (void *)&TaskData[0],  (void *)&TaskStk[0][TASK_STK_SIZE - 1], 5);
	OSTaskCreate(TaskPrio4, (void *)&TaskData[1],  (void *)&TaskStk[1][TASK_STK_SIZE - 1], 4);
	OSTaskCreate(TaskPrio5, (void *)&TaskData[2],  (void *)&TaskStk[2][TASK_STK_SIZE - 1], 3);

  for (;;) {
    OSCtxSwCtr = 0;
    OSTimeDlyHMSM(0, 0, 1, 0);                        /* Wait one second                                  */
//////////////////////////LCD///////////////////////////	
	sprintf(buf,"  %d  %d  %d",T3,T4,T5);
    Command(LINE2);
   	LCD_String(buf);	// �ι�° ���ο� ���			
//////////////////////////LCD///////////////////////////

  }
}

/*
**************************************************************************************************************
*                                                  TASKS
**************************************************************************************************************
*/

void TaskPrio3(void *data)//��ư5
{
  INT8U err;
  OSTaskSuspend(OS_PRIO_SELF);
  OSSemPend(sem, 0, &err);
  delay_ms(100);
  OSMutexPend(mutex, 0, &err); 
  delay_ms(100);
  //T5=0,T4=0,T3=0;
	for(;;)
	{
		T5++;
		delay_ms(100);
		
			if(T5 == 60)
			{
			
			T5 = 0;
			T4++;
			}
			if(T4 == 3)
			{
			OSMutexPost(mutex);
			delay_ms(100);
			}
			if(T4 == 60)
			{
			T4 = 0;
			T3++;
			}
		
			if(T3 == 13)
			{
			T3 = 1;
			}
			if(T4 == 3)
			{
			OSSemPost(sem);
			}
		
	}
}

void TaskPrio4(void *data)//�ð�4
{

	INT8U err;
	INT8U i=0;
	OSTaskSuspend(OS_PRIO_SELF);
	OSSemPend(sem, 0, &err);
    OSMutexPend(mutex, 0, &err);
    T5=0,T4=0,T3=0;
	for( resourceMutex = i ; resourceMutex < MAX ; resourceMutex++)
	{				
		T5 = resourceMutex;
		delay_ms(100);
			
			if(resourceMutex ==60)
			{
			resourceMutex =0;
			delay_ms(100);
			T4++;
			OSSemPost(sem);
			}
			if(T4 == 3)
			{
			T4 = 0;
			OSTaskSuspend(5);
			delay_ms(200);
			OSMutexPost(mutex);
			delay_ms(200);
			OSTaskSuspend(4);
			delay_ms(200);
			}
			if(T3 == 13)
			{
			T3 = 1;
			}
				
	}
	
}

void TaskPrio5(void *data)  //ffff
{  
   OSTaskSuspend(OS_PRIO_SELF);
    
}


void init_tick(void)
{
	OS_ENTER_CRITICAL();
	
	// clk/64
	TCCR0 = (1 << CS02) | (0 << CS01) | (0 << CS00);							
	TCNT0 = 256 - (CPU_CLOCK_HZ/OS_TICKS_PER_SEC/64);         			                             

 
	TIMSK = (1 << TOIE0);                                 
	
	OS_EXIT_CRITICAL();
}


void port_init(void)
{
 DDRA = 0x00;  //LCD ��Ʈ�� ��ȣ(PA2=LCD_EN, PA1=RW, PA0=RS)
 PORTA = 0x00; 
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xff;
 PORTD = 0x00;
 DDRD  = 0xff;
 PORTE = 0x00;
 DDRE  = 0x00;
 DDRF = 0x00;  //LCD ������ ����
 PORTF = 0x00;
 PORTG = 0x00;
 DDRG  = 0x03;
}

void delay(int dd){
    int i,j;
    
    for(i=0 ; i<1000 ; i++){
        for(j=0 ; j<dd ; j++){
        }
    }
}
// LCD �ʱ�ȭ  
void LCD_init(void)
{       
	DDRD = 0xFF;			// ��Ʈ D ��� ����
	PORTD &= 0xFB;			//E = 0;
	              
	delay_ms(15);
	Command(0x20);
	delay_ms(5);
	Command(0x20);
	delay_us(100);
	Command(0x20);
	Command(FUNCSET);
	Command(DISPON);
	Command(ALLCLR);
	Command(ENTMODE);
}

// ���ڿ� ��� �Լ�
void LCD_String(char str[])
{
	char *pStr=0;
	
	pStr = str;	
	while(*pStr) Data(*pStr++);
}	                                 

// �ν�Ʈ���� ���� �Լ�
void Command(unsigned char byte)
{
	Busy();

	// �ν�Ʈ���� ���� ����Ʈ
	PORTD = (byte & 0xF0);		// ������	
	PORTD &= 0xFE;			//RS = 0;           
	PORTD &= 0xFD;			//RW = 0;  
	delay_us(1);     
	PORTD |= 0x04;			//E = 1;
	delay_us(1);
	PORTD &= 0xFB;			//E = 0;

	// �ν�Ʈ���� ���� ����Ʈ
        PORTD = ((byte<<4) & 0xF0);	// ������
	PORTD &= 0xFE;			//RS = 0;
	PORTD &= 0xFD;			//RW = 0;
	delay_us(1);     	
	PORTD |= 0x04;			//E = 1;
	delay_us(1);
	PORTD &= 0xFB;			//E = 0;
}
                    
//������ ���� �Լ�
void Data(unsigned char byte)
{
	Busy();
        
	// ������ ���� ����Ʈ
	PORTD = (byte & 0xF0);		// ������
	PORTD |= 0x01;			//RS = 1;
	PORTD &= 0xFD;			//RW = 0;
	delay_us(1);     	
	PORTD |= 0x04;			//E = 1;
	delay_us(1);
	PORTD &= 0xFB;			//E = 0;

	// ������ ���� ����Ʈ
	PORTD = ((byte<<4) & 0xF0);  	// ������
	PORTD |= 0x01;			//RS = 1;
	PORTD &= 0xFD;			//RW = 0;     
	delay_us(1);     	
	PORTD |= 0x04;			//E = 1;
	delay_us(1);
	PORTD &= 0xFB;			//E = 0;
}

// Busy Flag Check -> �Ϲ����� BF�� üũ�ϴ� ���� �ƴ϶�
// ������ �ð� ������ �̿��Ѵ�.
void Busy(void)
{
	delay_ms(2);
}

void delay_us(unsigned char time_us)
{
  register unsigned char i;
  
  for(i=0; i < time_us; i++)
  {
    asm volatile("PUSH R0");
    asm volatile("POP R0");
    asm volatile("PUSH R0");
    asm volatile("POP R0");
    asm volatile("PUSH R0");
    asm volatile("POP R0");
  }
}        

void delay_ms(unsigned char time_ms)
{
  register unsigned char i;
  
  for(i=0; i < time_ms; i++)
  {
    delay_us(250);
    delay_us(250);
    delay_us(250);
    delay_us(250);
  }
}  


