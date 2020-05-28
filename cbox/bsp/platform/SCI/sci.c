/*
 *	sci.c
 *	Routines handle SCI service
 *
 *	Programmer: Darío S. Baliña.
 *	Date : 29/05/06.
 */

#include "derivative.h"
#include "mytypes.h"
#include "sci.h"
#include "tplfsm.h"


#define OVRN	0x08
#define NF		0x04
#define FE		0x02
#define PE		0x01

void 
host_sci1_init( unsigned short baud_rate )
{
	unsigned short *pbaud;
	
	pbaud = (unsigned short*)&SCIBDH;
	
	SCIC2_RIE = 0;

	SCIC1 = 0;
	SCIC2_TE = 0;
	SCIC2_RE = 0;

	*pbaud = baud_rate;

	SCIC2_TE = 1;
	SCIC2_RE = 1;
	SCIC2_RIE = 1;
}

void
host_sci1_setbd( unsigned short baud_rate )
{
  while( SCIC2_TIE || !SCIS1_TC );
  
  host_sci1_init( baud_rate );
}

void
interrupt
host_rcv_isr( void )
{
	volatile MUInt data;

	if( SCIS1 & (OVRN|PE|FE|NF))
	{
		data = SCID;
		return;
	}
	tplfsm_rcv_isr( SCID );
}

void
interrupt
host_xmit_isr( void )
{
	SCIS1 = SCIS1;
	
	tplfsm_xmit_isr();
}


