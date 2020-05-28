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
#include "iic.h"
#include "adc.h"

void
platform_init(void)
{
	MCU_init();
	ports_init();
	adc_init();
	IIC_Init();
}

