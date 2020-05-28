/*
 *	watchdog.h
 *
 *	Routines for Watchdog Module.
 *
 */

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#define WATCHDOG_BASE 2

/*
 *	WATCHDOG_TICK: Defines The Watchdog alive time in msecs.
 */
#define WATCHDOG_TICK	2000

/*
 *	kick_watchdog: Kicks The Watchdog Counter
 */
void kick_watchdog( void );

/*
 *	check_watchdog: Checks The Watchdog Counter, if reach to 0 reset.
 */
void check_watchdog( void );

#endif
