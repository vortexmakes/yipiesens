/*
 * tplhal.c
 */

#include "tplhal.h"
#include "scibrtbl.h"
#include "settings.h"
#include "tplfsm.h"
#include "intr.h"
#include "ioports.h"



/*
 * Public Functions
 */

void
init_tpl_hal( void )
{
	host_sci1_init( sci_br_tbl[BAUD_RATE_DFT] );
	TPL_485DIR = 0;
}

void
tpl_choke_xmit( uchar data )
{
	uchar i = 10;

	enter_critical();
	SCIC2_RE = 0;
	SCIC2_RIE = 0;

	while(--i)
		TPL_485DIR = 1;

	SCID = (unsigned char)data;
	SCIC2_TIE = 1;
	exit_critical();
}

void
tpl_xmit( uchar data )
{
	enter_critical();
	SCID = (unsigned char)data;
	exit_critical();
}

void
tpl_stop_xmit( void )
{
	enter_critical();
	SCIC2_TIE = 0;	
	SCIC2_TCIE = 1;
	exit_critical();
}

void
tpl_eoftx( void )
{
	enter_critical();
	SCIC2_TCIE = 0;

	TPL_485DIR = 0;

	SCIC2_RIE = 1;
	SCIC2_RE = 1;
	exit_critical();
}	
