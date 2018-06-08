//PORTA : RS:D0 RW:D1 E:D2
//PORTF : DATA

//ICC-AVR application builder : 2005-11-11 ¿ÀÈÄ 12:05:38
// Target : M128
// Crystal: 16.000Mhz

#include <iom128v.h>
#include <macros.h>

#define ON 1
#define OFF 0
#define DELAYTIME 10

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xFF;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0xFF;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0xFF;
 PORTG = 0x00;
 DDRG  = 0x00;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts
 XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 SEI(); //re-enable interrupts
 //all peripherals are now initialized
}
void delay(int dd){
    int i,j;
    
    for(i=0 ; i<1000 ; i++){
        for(j=0 ; j<dd ; j++){
        }
    }
}

// RS : PORTA D0
void LCD_RS(int onoff){
    if(onoff == ON)
        PORTA = PORTA | 0x01;
    else
        PORTA = PORTA & 0xFE;
}
// RW : PORTA D1
void LCD_RW(int onoff){
    if(onoff == ON)
        PORTA = PORTA | 0x02;
    else
        PORTA = PORTA & 0xFD;
}
// RS : PORTA D2
void LCD_E(int onoff){
    if(onoff == ON)
        PORTA = PORTA | 0x04;
    else
        PORTA = PORTA & 0xFB;
}

void E_Pulse(void){
	 char i;
	 LCD_E(ON);
	 delay(DELAYTIME*10);
	 LCD_E(OFF);
	 delay(DELAYTIME*10);
}

void write_char(char s){
	 LCD_RS(ON);
	 delay(DELAYTIME);		 
	 LCD_RW(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
 	 delay(DELAYTIME);
	 PORTF = s;
	 delay(DELAYTIME);
	 LCD_E(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
	 delay(DELAYTIME);
}	

void clrscr(){
	 LCD_RS(OFF);
	 delay(DELAYTIME);		 
	 LCD_RW(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
 	 delay(DELAYTIME);
	 PORTF = 0x01;
	 delay(DELAYTIME);
	 LCD_E(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
	 delay(DELAYTIME);
}

void returnhome(){
	 LCD_RS(OFF);
	 delay(DELAYTIME);		 
	 LCD_RW(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
 	 delay(DELAYTIME);
	 PORTF = 0x02;
	 delay(DELAYTIME);
	 LCD_E(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
	 delay(DELAYTIME);
}

void LCDONOFF(int onoff){
	 LCD_RS(OFF);
	 delay(DELAYTIME);		 
	 LCD_RW(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
 	 delay(DELAYTIME);
	 if(onoff == ON){
	 	 PORTF = 0x0F;
	 }else{
	 	 PORTF = 0x08;
	 }
	 delay(DELAYTIME);
	 LCD_E(OFF);
	 delay(DELAYTIME);
	 LCD_E(ON);
	 delay(DELAYTIME);
}


void lcd_disp(char x, char y){
LCD_RS(OFF);
LCD_RW(OFF);
if(y==1)y++;
else if(y==2) y--;
if(y<=1){
        PORTF=y*20+x+0x80;
}
else{
        y=2;
        PORTF=y*20+x+0xc0;
}
E_Pulse();
}

void write_lcd(char x, char y, char *str){
lcd_disp(x,y);
while(*str) write_char(*str++);
}

void main(void){
	 int i;
     init_devices();
     returnhome();
	 clrscr();
	 while(1){
	    LCDONOFF(ON);
		delay(1000);
	 	write_char('s');
		LCDONOFF(OFF);
		delay(1000);

	 }
}

/*         while(1){
                 write_lcd(0,0,"Hello");
                 delay(300);
//                 clrscr();
                 write_lcd(0,0,"Test");
                 delay(300);
//                 clrscr();
        }         
}*/
