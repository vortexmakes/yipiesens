/*
 *	adc.c
 *
 *	Routines for ADC Module.
 *
 */

#include "derivative.h"
#include "adc.h"
#include "mytypes.h"

typedef struct
{
	uchar ch_mask;
	uchar value;
}ADC_CH_ST;

static ADC_CH_ST adcs[ NUM_ADC_CH ] =
{
	{ 0x00,	0 },
	{ 0x08, 0 }
};

static MUInt pch;

/*
 *	adc_init: ADC module init and configuartion
 */
void
adc_init( void )
{
	pch = 0;
	ADCSC1 = adcs[0].ch_mask;
}

void
proc_adc( void )
{
	adcs[pch].value = ADCRL;

	if( ++pch >= NUM_ADC_CH )
		pch = 0;

	ADCSC1 = adcs[pch].ch_mask;
}

/*
 *	read_adc: Performs one conversion on the chanel 
 *			  selected and return the adc value.
 */
unsigned char
read_adc( MUInt ch )
{
	return adcs[ch].value;
	//while(!ADCSC1_COCO );
}

