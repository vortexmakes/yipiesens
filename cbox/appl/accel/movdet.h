/*
 * movdet.h
 */

#ifndef __MOVDET_H__
#define __MOVDET_H__

#include "mytypes.h"

uchar get_movdet( void );

#ifdef _MOVEDET_SIMPLE_AXIS_
void proc_accel_movdet( ushort axis );
#else
void proc_accel_movdet( ushort x, ushort y, ushort z );
#endif

#endif
