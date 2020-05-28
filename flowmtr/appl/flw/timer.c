/*
 *	file: timer.c
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file timer.c
 *	\brief
 */


#include "mytypes.h"
#include "timer.h"
#include "flw.h"


TIM_T timers[ TIMER_MAX_NUM_TIMERS ];


void
timer_stop( TIM_T *t )
{
	t->ntick = 0;
	t->ovfw = 0;
}


void
timer_start( TIM_T *t, TICK_T itick, TICK_T prd )
{
	t->ntick = itick;
	t->prd = prd;
	t->used = 1;
	t->ovfw = 0;
}

#ifdef __PERIODIC_TIMER__
void
timer_periodic_tick( MUInt fsm )
{
	TIM_T *t;
	FSM_T *pf;

	pf = verify_fsm( fsm );

	t = pf->tper;

	if( t->used && t->ntick && --t->ntick == 0 )
	{
		t->ovfw = 1;
		t->ntick = t->prd != 0 ? t->prd : 0;
	}
}
#endif


void
timer_oneshot_tick( MUInt fsm )
{
	TIM_T *t;
	FSM_T *pf;

	pf = verify_fsm( fsm );
	t = pf->tos;
	if( t->used && t->ntick && --t->ntick == 0 )
	{
		t->ovfw = 1;
		t->ntick = t->prd != 0 ? t->prd : 0;
	}
}
