/*
 * grsens.h
 * 
 * Grain Sensor Sampler 
 *
 * Author: Dario Baliña, Company: Vortex Technologies
 * email: dariosb@gmail.com
 *
 * Client: Yipies 
 */

#ifndef __GRSENS_H__
#define __GRSENS_H__

#include "mytypes.h"

typedef struct
{
	ushort hoard;	// grain hoard.
	ushort pqty;     // pails quantity.
	ushort flow;	// grain volume by second.
} GRSENS_T;

/*
 * grs_init:
 *
 *	Grain sensor sampling software initialization
 *	It must be called befor enabling interrupts.
 */
void grs_init( void );

/*
 * grs_read:
 *
 *	fills GRSENS_T struct pointed by p with:
 *	hoard: total grain hoarded up since last sample request.
 *	pqty: pails quantity since last sample request.
 *	flow: grain volume by period second.
 *
 *	Calling to grs_read triggers next adquisition.
 */
void grs_read( GRSENS_T *p );


#endif
