/*
 *	file: flw.c
 *	Last updated for version: 1.0.00
 *	Date of the last update:  Feb 23, 2012
 */

/**
 * 	\file flw.c
 *	\brief
 */


#include "fsm.h"
#include "flw.h"
#include "flwact.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>


#if PPS_AVERAGED == 0
	#define SET_PPS() \
				pf->pps = pf->ppc; \
				pf->ppc = 0
#else
	#define SET_PPS() \
				pf->pps = (pf->ppc + pf->pps)/2; \
				pf->ppc = 0
#endif

DCLR_STATE( a0 );
DCLR_STATE( a1 );
DCLR_STATE( a2 );
DCLR_STATE( b0 );
DCLR_STATE( b1 );
DCLR_STATE( c0 );
DCLR_STATE( c1 );
DCLR_STATE( g0 );
DCLR_STATE( g1 );
DCLR_STATE( h0 );
DCLR_STATE( h1 );
DCLR_STATE( i0 );
DCLR_STATE( i1 );
DCLR_STATE( j0 );
DCLR_STATE( j1 );
DCLR_STATE( k0 );
DCLR_STATE( k1 );
DCLR_STATE( m0 );
DCLR_STATE( m1 );


STATE( a0 )
{
	TRN( SIG_01, 	CA(0), 			a0 ),
	TRN( SIG_11, 	start_ta, 		b0 ),

	TRN_END
};


STATE( a1 )
{
	TRN( SIG_10, 	CA(0), 			a1 ),
	TRN( SIG_11, 	start_ta, 		b1 ),

	TRN_END
};


STATE( a2 )
{
	TRN( SIG_01, 	CA(0), 			a0 ),
	TRN( SIG_10, 	CA(0), 			a1 ),

	TRN_END
};


STATE( b0 )
{
	TRN( SIG_01, 	CA(0), 			a0 ),
	TRN( SIG_10, 	stop_hf1, 		NULL ),
	TRN( TOUT, 		CA(0), 			c0 ),

	TRN_END
};


STATE( b1 )
{
	TRN( SIG_01, 	stop_hf2, 		NULL ),
	TRN( SIG_10, 	CA(0), 			a1 ),
	TRN( TOUT, 		CA(0), 			c1 ),

	TRN_END
};


STATE( c0 )
{
	TRN( SIG_01, 	CA(0), 			a0 ),
	TRN( SIG_10, 	CA(0), 			a1 ),

	TRN_END
};


STATE( c1 )
{
	TRN( SIG_01, 	CA(0), 			a0 ),
	TRN( SIG_10, 	CA(0), 			a1 ),

	TRN_END
};


STATE( g0 )
{
	TRN( SIG_01, 	CA(0), 			h0 ),
	TRN_END
};


STATE( g1 )
{
	TRN( SIG_10, 	CA(0), 			h1 ),
	TRN_END
};


STATE( h0 )
{
	TRN( SIG_11, 	start_dir_cyc, 	i0 ),
	TRN_END
};


STATE( h1 )
{
	TRN( SIG_11, 	start_rev_cyc, 	i1 ),
	TRN_END
};


STATE( i0 )
{
	TRN( SIG_01, 	reset_cyc, 		h0 ),
	TRN( SIG_10, 	start_pulse, 	j0 ),
	TRN( TOUT, 		CA(0), 			m0 ),

	TRN_END
};


STATE( i1 )
{
	TRN( SIG_01, 	start_pulse, 	j1 ),
	TRN( SIG_10, 	reset_cyc, 		h1 ),
	TRN( TOUT, 		CA(0), 			m1 ),

	TRN_END
};


STATE( j0 )
{
	TRN( SIG_11, 	inc_pulse, 		k0 ),
	TRN( TOUT, 		CA(0), 			m0 ),

	TRN_END
};


STATE( j1 )
{
	TRN( SIG_11, 	inc_pulse, 		k1 ),
	TRN( TOUT, 		CA(0), 			m1 ),

	TRN_END
};


STATE( k0 )
{
	TRN( SIG_01, 	CA(0), 			h0 ),
	TRN_END
};


STATE( k1 )
{
	TRN( SIG_10, 	CA(0), 			h1 ),
	TRN_END
};


STATE( m0 )
{
	TRN( SIG_01, 	CA(0), 			h0 ),
	TRN_END
};


STATE( m1 )
{
	TRN( SIG_10, 	CA(0), 			h1 ),
	TRN_END
};

	
FSM_T fsms[] = 
{
#ifdef __PERIODIC_TIMER__
	{ a2, NULL, sig_init, SIG_11, 0, 0, 0, 0, S1S2, TOS_S1S2, TPER_S1S2, FORWARD, 0, 0 },
	{ a2, NULL, sig_init, SIG_11, 0, 0, 0, 0, S3S4, TOS_S3S4, TPER_S3S4, FORWARD, 0, 0 }
#else
	{ a2, NULL, sig_init, SIG_11, 0, 0, 0, 0, S1S2, TOS_S1S2, FORWARD, 0, 0 },
	{ a2, NULL, sig_init, SIG_11, 0, 0, 0, 0, S3S4, TOS_S3S4, FORWARD, 0, 0 }
#endif
};


unsigned char 
flw_get_pps( MUInt fsm )
{
	FSM_T *pf;
	unsigned char pps;

	pf = verify_fsm( fsm );
	/* enter_critical() */
#ifndef __PERIODIC_TIMER__
	SET_PPS();
#endif
	pps = pf->pps;
	/* exit_critical() */
	return pps;
}


unsigned char 
flw_get_flowdir( MUInt fsm )
{
	FSM_T *pf;
	unsigned char flowdir;

	pf = verify_fsm( fsm );
	/* enter_critical() */
	flowdir = (unsigned char)(pf->flowdir);
	/* exit_critical() */
	return flowdir;
}


/*
 * 	per tick:
 *
 * 	timer_periodic_tick();
 * 	flw_process( S1S2, sig_s1s2 );
 * 	flw_process( S3S4, sig_s3s4 );
 */

void
flw_process( MUInt fsm, unsigned char sig )
{
	EVT_T e;
	FSM_T *pf;

	pf = verify_fsm( fsm );
#ifdef __PERIODIC_TIMER__
	timer_periodic_tick( fsm );
#endif
	timer_oneshot_tick( fsm );

	e = ( pf->lsig == sig && TIMER_IS_OVERFLOW( pf->tos ) )? TOUT:sig;
	pf->lsig = sig;
	TIMER_CLEAR( pf->tos );
	fsm_dispatch( fsm, e );

#ifdef __PERIODIC_TIMER__
	if( TIMER_IS_OVERFLOW( pf->tper ) )
	{
		TIMER_CLEAR( pf->tper );
		SET_PPS();
		flwprint(( " -------------------------- Pulses per second=%d", pf->pps ));
	}
#endif
}


void
flw_init( void )
{
#ifdef __PERIODIC_TIMER__
	timer_start( TPER_S1S2, TPD_TICK, TPD_PRD );
	timer_start( TPER_S3S4, TPD_TICK, TPD_PRD );
#endif
	fsm_init( S1S2 );
	fsm_init( S3S4 );
}
