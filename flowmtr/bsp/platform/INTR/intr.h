/*
 * 	intr.h
 */


#ifndef __INTR_H__
#define __INTR_H__

#define INTR_CRITICAL_METHOD	1
#define INTR_MAX_NESTING_CSECT	8

/*
 * functions to protect critical sections
 */
void init_critical( void );
void enter_critical( void );
void exit_critical( void );


#include <hidef.h>
#include "derivative.h"

/*
 * 	Enable and disable general interrupts.
 */
#define enable_interrupt()		EnableInterrupts
#define disable_interrupt()		DisableInterrupts

/*
 * MCU reset
 */
#define _reset_mcu() 			{asm bgnd;}

/*
 * feed hardware watchdog
 */
#define _kick_hw_watchdog()		__RESET_WATCHDOG()


#endif
