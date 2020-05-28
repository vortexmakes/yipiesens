/*
 *	file: timer.h
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file timer.h
 *	\brief
 */


#ifndef __TIMER_H__
#define __TIMER_H__


#include "mytypes.h"

#ifdef __PERIODIC_TIMER__
#define TIMER_MAX_NUM_TIMERS		4
#else
#define TIMER_MAX_NUM_TIMERS		2
#endif

/**
 * 	\brief
 * 	This data type defines the dynamic range of the time delays measured in 
 * 	clock ticks (maximum number of ticks).
 */

typedef unsigned short TICK_T;


/**
 * 	\brief
 * 	Defines the data structure used to maintain information that allows the 
 * 	timer-handling facility to update and expire software timers. 
 */

typedef struct
{
	/** 
	 * 	Usage indication.
	 */

	unsigned used	:1;


	/** 
	 * 	Overflow flag.
	 */

	unsigned ovfw	:1;

	
	/** 
	 * 	Tick-down counter.
	 */

	TICK_T ntick;


	/** 
	 * 	Number of ticks for all timer expirations after the first 
	 * 	(expiration period).
	 */

	TICK_T prd;
} TIM_T;


extern TIM_T timers[ TIMER_MAX_NUM_TIMERS ];


#define TIMER_CLEAR( t )			(t)->ovfw = 0
#define TIMER_IS_OVERFLOW( t )		((t)->ovfw == 1)
#define TIMER_GET( t )				((t)->ntick)

void timer_clear( TIM_T *t );
void timer_stop( TIM_T *t );
void timer_start( TIM_T *t, TICK_T itick, TICK_T prd );
void timer_periodic_tick( MUInt fsm );
void timer_oneshot_tick( MUInt fsm );

#endif
