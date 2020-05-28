/*
 * accel.h
 */

#ifndef __ACCEL_H__
#define __ACCEL_H__

#include "mytypes.h"
#include "mma845x.h"
#include "settings.h"

#if	ACCEL_FULL_SCALE_DFT == FULL_SCALE_2G
	#define ACCEL1G_RES		_2G_COUNT_BY_G
#elif ACCEL_FULL_SCALE_DFT == FULL_SCALE_4G
	#define ACCEL1G_RES		_4G_COUNT_BY_G
#elif ACCEL_FULL_SCALE_DFT == FULL_SCALE_8G
	#define ACCEL1G_RES		_8G_COUNT_BY_G
#endif

#define ACCEL_TICK		2

typedef struct
{
	ushort x;
	ushort y;
	ushort z;
	uchar m;
}ACCEL_T;

void init_accelerometer( void );
void show_accel_data(void);
void get_accel_data( ACCEL_T *p );

void dump_accel_regs( void );
void accel_calibrate(void);

void proc_accel( void );

#endif
