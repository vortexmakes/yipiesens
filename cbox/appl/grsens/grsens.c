/*
 * grsens.c
 * 
 * Grain Sensor Sampler 
 *
 * Author: Dario Baliña, Company: Vortex Technologies
 * email: dariosb@gmail.com
 *
 * Client: Yipies 
 */

#include "mytypes.h"
#include "grsens.h"
#include "grsisr.h"
#include "intr.h"
#include "settings.h"
#include "humsens.h"

#define MAX_SAMPLE_VALUE	150
#define OVRN_THR			( 0xFFFF - MAX_SAMPLE_VALUE )
#define HOARD_OVRRUN		0xFFFF
#define PAILS_OVRRUN		0xFF

static ushort gsum;			// actual grain accumulation
static ushort pails;		// actual pail count
static ushort favrg;		// actual flow average
static ushort fsum_last;	// actual flow average
static ushort fsum;			// actual grain accumulation for flow calculation

/*
 * grs_init:
 *
 *	Grain sensor sampling software initialization
 *	It must be called befor enabling interrupts.
 */
void
grs_init( void )
{
//	grs_init_hdl(); descomentar luego de implementar

}

/*
 * grs_read:
 *
 *	fills GRSENS_T struct pointed by p with:
 *	hoard: total grain hoarded up since last read.
 *	pqty: pails quantity since last read request.
 *	flow: grain volume by period of time, usually 1 sec, 
 *	it depends of grs_flow_isr calling tick.
 *
 *	Calling to grs_read triggers next adquisition.
 *
 */
void
grs_read( GRSENS_T *g )
{
 	enter_critical();
	
	g->hoard = gsum;
	gsum = 0;

	g->pqty = pails;
	pails = 0;

	g->flow = favrg;

	exit_critical();
}


/************** ISR's functions *************/

/*
 * grs_sample_isr:
 *
 * 	Called by each sample capture
 * 	If the amount of grain hoard reach to
 * 	OVRN_THR, the accumulation is stoped until 
 * 	next grs_read call.
 */
void
grs_sample_isr( uchar sample )
{
	if( sample <= GRNOISE_THRESHOLD )
		return;

	fsum += sample;

	if( gsum < OVRN_THR )
	{
		gsum += sample;
		++pails;
	}
	else if( sample >= MAX_SAMPLE_VALUE )
	{
		gsum = HOARD_OVRRUN;
		pails = PAILS_OVRRUN;
	}
}

/*
 * grs_flow_isr:
 *
 * 	Called between one flow calculation
 * 	period of time( 1 sec ).
 */
void
grs_flow_isr( void )
{
	favrg = (fsum_last + fsum) >> 1;
	fsum_last = fsum;
	fsum = 0;

	proc_humsens( favrg );
}
