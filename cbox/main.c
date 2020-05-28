/*
 * main.c
 */

#include <hidef.h>
#include "derivative.h"

#include "initsys.h"
#include "mytypes.h"
#include "watchdog.h"
#include "cbsens.h"
#include "tplink.h"
#include "intr.h"
#include "sleep.h"

static CBOX_ST xmit_cb;
static CBOX_ST *pxmit;

void
set_responce( CBOX_ST *p )
{
	xmit_cb = *p;
	pxmit = &xmit_cb;
}

void
chk_resp_and_xmit( void )
{

	enter_critical();
	
	if( pxmit == NULL )
	{
		exit_critical();
		return;
	}
	
	exit_critical();
	sleep( 50 / SLEEP_BASE );

	enter_critical();
	tplink_send_frame( (uchar *)pxmit, sizeof(CBOX_ST) );
	pxmit = NULL;
	exit_critical();

}


void
main( void )
{
	init_system();

	for(;;)
	{
		kick_watchdog();
		chk_resp_and_xmit();	
	}
}
