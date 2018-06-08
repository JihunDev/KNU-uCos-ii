/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                                          AVR Specific code
*
* File         : INCLUDES.H
* By           : Ole Saether
* Port Version : V1.01
*
* Modifications by Julius Luukko 07-21-2003 (Julius.Luukko@lut.fi) to get this compiled with 
* uC/OS-II 2.52 and 2.70.
*
*********************************************************************************************************
*/

#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

/* If you have uC/OS-II version below 2.70 uncomment the includes for os_cpu.h and os_cfg.h */

//#include  "C:\myMP3\Ports\os_cpu.h"
#include  <os_cpu.h>
//#include "C:\cvavreval\inc\mega128.h"
//#include  "os_cpu.h"
#include  "os_cfg.h"
#include  <ucos_ii.h>

/* ISR support macros */
#include <avr_isr.h>


#define  COLOR_BLACK     0                       /* VT102 Color Codes                                  */
#define  COLOR_RED       1
#define  COLOR_GREEN     2
#define  COLOR_BROWN     3
#define  COLOR_BLUE      4
#define  COLOR_MAGENTA   5
#define  COLOR_CYAN      6
#define  COLOR_WHITE     7

#define BAUD_RATE		19200
#define BAUD_RATE_L		(CPU_CLOCK_HZ / (16l * BAUD_RATE)) - 1
#define BAUD_RATE_H		((CPU_CLOCK_HZ / (16l * BAUD_RATE)) - 1) >> 8


#endif
