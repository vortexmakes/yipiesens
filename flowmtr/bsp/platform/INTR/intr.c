/*
 * 	intr.c
 */


#include "intr.h"


static unsigned int ccr_sp;
static unsigned char ccr_sp_mem[ INTR_MAX_NESTING_CSECT ];


void
init_critical( void )
{
	asm ldhx @ccr_sp_mem;
	asm	sthx ccr_sp;
}


void 
enter_critical( void )
{
	asm tpa;			/* transfer CCR to A */
	asm sei;			/* disable interrupts */
	asm ldhx ccr_sp;
	asm sta  ,x;		/* save old interrupt status */
	asm	aix	 #1;
	asm	sthx ccr_sp;
}


void 
exit_critical( void )
{
	asm ldhx ccr_sp;
	asm	aix	 #-1;
	asm	sthx ccr_sp;
	asm lda  ,x;		/* get old interrupt status */
	asm tap;			/* transfer A to CCR */
}


