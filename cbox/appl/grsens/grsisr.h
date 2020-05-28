/*
 * grsisr.h
 *
 * Grain Sensor Sampler ISR Routines interface
 *
 * Author: Dario Baliña, Company: Vortex Technologies
 * email: dariosb@gmail.com
 *
 * Client: Yipies
 */

#ifndef __GRSISR_H__
#define __GRSISR_H__

#include "mytypes.h"

/*
 * grs_sample_isr:
 *
 * 	Called by each sample capture
 * 	If the amount of grain hoard reach to
 * 	OVRN_THR, the accumulation is stoped until 
 * 	next grs_read call.
 */
void
grs_sample_isr( uchar sample );

/*
 * grs_flow_isr:
 *
 * 	Called between one flow calculation 
 * 	period of time( 1 sec ).
 */
void grs_flow_isr( void );

#endif
