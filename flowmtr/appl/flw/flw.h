/*
 *	file: flw.h
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file flw.h
 *	\brief
 */


#ifndef __FLW_H__
#define __FLW_H__


#include "fsm.h"


/**
 * 	Debug activation.
 */

#define FLW_DBG_EN		0


/**
 * 	Debug macro.
 */

#if FLW_DBG_EN == 1
	#include <stdio.h>
	extern FILE *flwsig, *flwout;
	#define flwprint(x)	\
				do{ \
					fprintf (x); \
					fprintf (x); \
				} while( 0 )
#else
	#define flwprint(x);
#endif


/**
 * 	If this option is enabled the pulse per second will be averaged.
 */

#define PPS_AVERAGED	0


/**
 * 	Periodic timer. It is used for updating the pulses per cycle of each 
 * 	signal pair.
 */
#ifdef __PERIODIC_TIMER__
#define TPER_S1S2		&timers[ 0 ]
#define TPER_S3S4		&timers[ 1 ]
#endif


/**
 *	Oneshot timer for signal pairs.
 */
#ifdef __PERIODIC_TIMER__
#define TOS_S1S2		&timers[ 2 ]
#define TOS_S3S4		&timers[ 3 ]
#else
#define TOS_S1S2		&timers[ 0 ]
#define TOS_S3S4		&timers[ 1 ]
#endif


/** 
 * 	Number initial of ticks for periodic timer expiration.
 */

#define TPD_TICK		200


/**
 *	Number of ticks for all timer expirations after the first 
 *	(expiration period). A zero for this parameter makes the 
 * 	timer a one-shot timer, otherwise, for periodic timers, 
 * 	any value in range.
 */

#define TPD_PRD			200


/**
 * 	Maximum time [in ticks] between the S1/S3 rising edge and 
 * 	S2/S4 falling edge.
 */

#define TAMAX			30


/**
 * 	Maximum time [in ticks] between the S2/S4 rising edge and 
 * 	S1/S3 falling edge.
 */

#define TBMAX			50


/**
 * 	Maximum pulse width [in ticks].
 */

#define TWMAX			20


/**
 * 	Indicates the flow direction.
 */

enum
{
	REVERSE, FORWARD
};


/**
 * 	State machine ID.
 */

enum
{
	S1S2, S3S4,
	NUM_SIG_PAIRS
};


/**
 * 	Event signal.
 */

enum
{
	SIG_00,
	SIG_01,
	SIG_10,
	SIG_11,
	TOUT
};


extern DCLR_STATE( a0 );
extern DCLR_STATE( a1 );
extern DCLR_STATE( a2 );
extern DCLR_STATE( b0 );
extern DCLR_STATE( b1 );
extern DCLR_STATE( c0 );
extern DCLR_STATE( c1 );
extern DCLR_STATE( g0 );
extern DCLR_STATE( g1 );
extern DCLR_STATE( h0 );
extern DCLR_STATE( h1 );
extern DCLR_STATE( i0 );
extern DCLR_STATE( i1 );
extern DCLR_STATE( j0 );
extern DCLR_STATE( j1 );
extern DCLR_STATE( k0 );
extern DCLR_STATE( k1 );
extern DCLR_STATE( m0 );
extern DCLR_STATE( m1 );


/**
 * 	\brief
 * 	Retrieve the pulse per seconds in a safe manner.
 *
 * 	\note 
 * 	Should be protected the shared data.
 *
 * 	\param fsm		state machine ID.
 */

unsigned char flw_get_pps( MUInt fsm );


/**
 * 	\brief
 * 	Retrieve the flow direction in a safe manner.
 *
 * 	\note 
 * 	Should be protected the shared data.
 *
 * 	\param fsm		state machine ID.
 */

unsigned char flw_get_flowdir( MUInt fsm );


/**
 * 	\brief
 *  Initialize the flowmeter state machines.
 */

void flw_init( void );


/**
 * 	\brief
 * 	For each signal pair processes the occurred event (signal change 
 * 	or timeout) every time that it is invoked.
 *
 * 	\param fsm		state machine ID. The valid values are S1S2, and S3S4.
 * 	\param sig		contains the signal pair and can have the following 
 * 					values: SIG_00, SIG_01, SIG_10,	or SIG_11.
 */

void flw_process( MUInt fsm, unsigned char sig );


#endif
