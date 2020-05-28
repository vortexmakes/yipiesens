/*
 *	file: flwact.c
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file flwact.c
 *	\brief
 */


#include "fsm.h"
#include "flw.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>


static
const TRN_T *
set_flow_dir( FSM_T *fsm )
{
	const TRN_T *s;

	if( fsm->hf1 == 1 && fsm->hf2 == 1 )
	{
		fsm->pps = fsm->ppc = 0;
		if( fsm->ta >= fsm->tb )
		{
			fsm->flowdir = REVERSE;
			s = g1;
		}
		else
		{
			fsm->flowdir = FORWARD;
			s = g0;
		}
	}
	else
		s = CS(0);
	return s;
}


void 
sig_init( FSM_T *fsm )
{
}


const 
struct trn_t *
start_ta( FSM_T *fsm, EVT_T e )
{
	flwprint((" "__FUNCTION__"" ));
	timer_start( fsm->tos, TAMAX, 0 );
	return HANDLED;
}


const 
struct trn_t *
reset_ta( FSM_T *fsm, EVT_T e )
{
	flwprint((" "__FUNCTION__"" ));
	timer_stop( fsm->tos );
	return HANDLED;
}


const 
struct trn_t *
stop_hf1( FSM_T *fsm, EVT_T e )
{
	const TRN_T *s;

	fsm->ta = TAMAX - TIMER_GET( fsm->tos );
	timer_stop( fsm->tos );
	fsm->hf1 = 1;

	flwprint((" "__FUNCTION__": ta=%d", fsm->ta ));
	s = set_flow_dir( fsm );
	return s == CS(0) ? a1 : s;
}


const 
struct trn_t *
stop_hf2( FSM_T *fsm, EVT_T e )
{
	const TRN_T *s;

	fsm->tb = TAMAX - TIMER_GET( fsm->tos );
	timer_stop( fsm->tos );
	fsm->hf2 = 1;

	flwprint(( " "__FUNCTION__": tb=%d", fsm->tb ));
	s = set_flow_dir( fsm );
	return s == CS(0) ? a0 : s;
}


const 
struct trn_t *
start_rev_cyc( FSM_T *fsm, EVT_T e )
{
	flwprint(( " "__FUNCTION__": dir=%s", fsm->flowdir ? "FORWARD":"REVERSE" ));
	timer_start( fsm->tos, TBMAX, 0 );
	return HANDLED;
}


const 
struct trn_t *
start_dir_cyc( FSM_T *fsm, EVT_T e )
{
	flwprint(( " "__FUNCTION__": dir=%s", fsm->flowdir ? "FORWARD":"REVERSE" ));
	timer_start( fsm->tos, TAMAX, 0 );
	return HANDLED;
}


const 
struct trn_t *
start_pulse( FSM_T *fsm, EVT_T e )
{
	flwprint(( " "__FUNCTION__"" ));
	timer_start( fsm->tos, TWMAX, 0 );
	return HANDLED;
}


const 
struct trn_t *
reset_cyc( FSM_T *fsm, EVT_T e )
{
	flwprint(( " "__FUNCTION__"" ));
	timer_stop( fsm->tos );
	return HANDLED;
}


const 
struct trn_t *
inc_pulse( FSM_T *fsm, EVT_T e )
{
	timer_stop( fsm->tos );
	++fsm->ppc;
	flwprint(( " "__FUNCTION__": ppc=%d", fsm->ppc ));
	return HANDLED;
}
