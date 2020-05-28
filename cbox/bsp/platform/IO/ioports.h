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
#define SA0_PIN		(PTAD_PTAD1)
/* PORT B Data Register */
#define TPL_485DIR	(PTBD_PTBD2)
#define HSENS_CTRL	(PTBD_PTBD3)
#define LED			(PTBD_PTBD5)
/* PORT C Data Register */
#define BOARD_ID0	(PTCD_PTCD0)
#define BOARD_ID1	(PTCD_PTCD1)
#define BOARD_ID2	(PTCD_PTCD2)
#define BOARD_ID3	(PTCD_PTCD3)
/* PORT D Data Register */
/* PORT E Data Register */
/*********************************/
/*********************************/

void ports_init(void);
	
#endif
