/*
 *	adc.c
 *
 *	Routines for ADC Module.
 *
 */

#ifndef _ADC_H_
#define	_ADC_H_

#include "mytypes.h"

enum
{
	HUMEDITY1,
	HUMEDITY2,

	NUM_ADC_CH
};

/*
 *	adc_init: ADC module init and configuartion
 */
void adc_init( void );

/*
 * proc_adc: Called at Timer ISR, adquiere in order each
 * 			 declared ADC CH.
 */
void proc_adc( void );

/*
 *	read_adc: Performs one conversion on the chanel 
 *			  selected and return the adc value.
 */
unsigned char read_adc( MUInt ch );

#endif
