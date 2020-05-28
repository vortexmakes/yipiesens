/*
 *	ioports.h
 *	
 *	Routines for PORTS initialization.
 *
 *	Programmer: Darío S. Baliña.
 *	Date : 26/05/06.
 */

#ifndef __IOPORTS_H__
#define __IOPORTS_H__

#include "derivative.h"

/*********************************/
/* HARDWARE CONCTIONS DEFINITION */
/*********************************/
/* DATA_REGISTERS */
/* PORT A Data Register */
/* PORT B Data Register */
#define TPL_485DIR	(PTBD_PTBD2)
/* PORT C Data Register */
/* PORT D Data Register */
/* PORT E Data Register */
/*********************************/
/*********************************/

void ports_init(void);
	
#endif
