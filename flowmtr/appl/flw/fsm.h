/*
 *	file: fsm.h
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file fsm.h
 *	\brief
 */


#ifndef __FSM_H__
#define __FSM_H__


#include "mytypes.h"
#include "timer.h"


#define ANY					0xFF
#define DCLR_STATE(s)		const struct trn_t s[]
#define STATE(s)			const TRN_T s[]=
#define TRN(e,a,n)			{(e),(a),(n)}
#define TRN_NONE(a,n)		{ANY,(a),(n)}
#define TRN_END				{ANY,CA(0),CS(0)}
#define CI( p )				((INIT_T)(p))
#define CA( p )				((ACT_T)(p))
#define CT( p )				((TRN_T*)(p))
#define CS( p )				((TRN_T*)(p))
#define HANDLED				NULL


typedef MUInt EVT_T;
typedef void (*INIT_T)( struct fsm_t *fsm );
typedef const struct trn_t *(*ACT_T)( struct fsm_t *fsm, EVT_T e );


typedef struct trn_t
{
	EVT_T event;
	ACT_T action;
	const struct trn_t *next;
} TRN_T;


typedef struct fsm_t
{
	const TRN_T *istate;	/* initial state */
	const TRN_T *state;		/* current state */
	INIT_T init_action;		/* initial action */
	unsigned char lsig;		/* last value of signal pair */
	unsigned char ppc;		/* pulses per cycle */
	unsigned char pps;		/* pulses per second */
	unsigned hf1:1;			/* half cycle 1 complete. Used in startup time. */
	unsigned hf2:1;			/* half cycle 2 complete. Used in startup time. */
	unsigned sp:1;			/* signal pair id */ 
	TIM_T *tos;				/* oneshot timer */
#ifdef __PERIODIC_TIMER__
	TIM_T *tper;			/* periodic timer */
#endif
	unsigned flowdir:1;		/* flow direction */ 
	TICK_T ta;				/* tA time. Used in startup time. */
	TICK_T tb;				/* tB time. Used in startup time.  */
} FSM_T;


/**
 *	Finite state machine list.
 */

extern FSM_T fsms[ 2 ];


/**
 * 	\brief
 * 	Retrieve a pointer to finite state machine oobject.
 *
 * 	\param fsm		state machine ID.
 */

FSM_T *verify_fsm( MUInt fsm );


/**
 * 	\brief
 * 	Initialize a previously created state machine.
 *
 * 	\param fsm		state machine ID.
 */

void fsm_init( MUInt fsm );


/**
 * 	\brief
 * 	Process event.
 *
 * 	\param fsm		state machine ID.
 * 	\param e		event.
 */

void fsm_dispatch( MUInt fsm, EVT_T e );


#endif
