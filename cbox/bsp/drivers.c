/*
 *	drivers.c
 *
 *	Programmer: Dar�o S. Bali�a.
 *	Date : 26/05/06.
 */

#include "drivers.h"
#include "platform.h"

#include "maintime.h"
#include "sleep.h"
#include "sstack.h"

void
drivers_init(void)
{
    platform_init();

	init_sstack();

	init_main_timer_interrupt();

}
