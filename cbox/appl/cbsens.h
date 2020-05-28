/*
 * cbsens.h
 */

#ifndef __CBSENS_H__
#define __CBSENS_H__

#include "grsens.h"
#include "accel.h"

typedef struct
{
	uchar cmd;
	uchar m;
	GRSENS_T h;
	ACCEL_T a;
	uchar hum;
}CBOX_ST;

void do_cbsens( void );

#endif
