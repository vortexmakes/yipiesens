/*
 * main.c
 */

#include <hidef.h>
#include "derivative.h"

#include "initsys.h"
#include "mytypes.h"
#include "watchdog.h"
#include "flw.h"
#include "tplink.h"
#include "sleep.h"

void send_flow(void);

void
main( void )
{
	init_system();

	for(;;)
		kick_watchdog();
}
