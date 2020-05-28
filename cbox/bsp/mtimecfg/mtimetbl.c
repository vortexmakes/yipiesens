/*
 * 	mtimetbl.c
 * 		Main TaBLe timer
 */

#include <stdio.h>

#include "mytypes.h"

#include "mtimedef.h"
#include "mtimetbl.h"
#include "watchdog.h"
#include "sleep.h"
#include "accel.h"
#include "grsisr.h"
#include "tplfsm.h"
#include "adc.h"

void
none( void )
{
}

static void(* const actions_2[])( void ) =
{
	check_watchdog, check_sleep, proc_accel, NULL
};

static void(* const actions_20[])( void ) =
{
	tplfsm_timer_isr, proc_adc, NULL
};

static void(* const actions_1000[])( void ) =
{
  grs_flow_isr, NULL
};

const TIMER_CHAIN timer_chain[] =
{
	{ 2000/MAIN_TIMER_TIME,		actions_2 		},
	{ 20000/MAIN_TIMER_TIME, 	actions_20 		},
	{ 1000000/MAIN_TIMER_TIME,	actions_1000	},
};


