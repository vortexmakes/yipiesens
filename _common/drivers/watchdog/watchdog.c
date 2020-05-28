/*
 *	watchdog.c
 *
 *	Programmer: Darío S. Baliña.
 */
#include "mytypes.h"
#include "watchdog.h"
#include "reset.h"
#include "intr.h"
#include "sstack.h"

#define BWATCHDOG_TICK	(WATCHDOG_TICK/WATCHDOG_BASE)

extern char __SEG_START_SSTACK[];

static unsigned short watchdog_tick = BWATCHDOG_TICK;
unsigned char *psstack = (unsigned char *)(__SEG_START_SSTACK);

/*
 * init_sstack:
 */
void
init_sstack( void )
{
	*psstack = SSTACK_VALUE;
}

/* 
 * reset_now: If in debugg mode stop the execution,
 *				 resets the MCU in other case.
 */
void
reset_now( void )
{
	disable_interrupt();
	_reset_mcu();
}

/*
 *	kick_watchdog: Kicks The Watchdog Counter
 */
void
kick_watchdog( void )
{
	enter_critical();
	watchdog_tick = BWATCHDOG_TICK;
	exit_critical();
	_kick_hw_watchdog();
}

/*
 *	check_watchdog: Checks The Watchdog Counter, if reach to 0 reset.
 */
void
check_watchdog( void )
{
	if( !--watchdog_tick )
		reset_now();
	if( *psstack != SSTACK_VALUE )
		reset_now();
}
