/*
 *	platform.c
 *
 *	Routines for Platform Initialization.
 *
 *	Programmer: Dar�o S. Bali�a.
 */

#include "derivative.h"
#include "MCUinit.h"
#include "platform.h"
#include "ioports.h"


void
platform_init(void)
{
	MCU_init();
	ports_init();
}

