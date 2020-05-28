/*
 * movdet.c
 */

#include "mytypes.h"
#include "settings.h"
#include "accel.h"
#include "ioports.h"
#include "formats.h"

#define amov_ctrl(x);		//(MOVE_DETECT=x) uncoment to log moving with led

enum
{
	AMOV_STILL, AMOV_TRIGGERED,
	AMOV_WSTILL
}AMOV_STATES;

enum
{
	MOFF_CTRL, MON_CTRL
}AMOV_CTRL;

static MUInt amov_st = AMOV_STILL;
static MUInt mov_hist = AMOV_STILL;
static ushort amov_delay = 0;
static short delta;

#ifdef _MOVEDET_SIMPLE_AXIS_
static ushort acc_old = 0;
#else
static ushort x_old = 0;
static ushort y_old = 0;
static ushort z_old = 0;
#endif

uchar
get_movdet( void )
{
  MUInt ret;
  
  ret = mov_hist;
  
  if( amov_st == AMOV_STILL )
    mov_hist = AMOV_STILL;
  
	return ret != AMOV_STILL;
}

short
calc_accel_delta( ushort old, ushort *new )
{
	short temp;

	if( *new > 0x1FFF )
		*new = (~(*new) + 1) & 0x3FFF;

	temp = old - *new;

	return (temp > 0) ? temp : -temp;
}

void
#ifdef _MOVEDET_SIMPLE_AXIS_
proc_accel_movdet( ushort axis )
#else
proc_accel_movdet( ushort x, ushort y, ushort z )
#endif
{
#ifdef _MOVEDET_SIMPLE_AXIS_
	delta = calc_accel_delta( accel_old, &axis );
#else
	delta = calc_accel_delta( x_old, &x );
	delta += calc_accel_delta( y_old, &y );
	delta += calc_accel_delta( z_old, &z );
#endif

	switch( amov_st )
	{
		case AMOV_STILL:
			if( delta > MOVE_DET_THR )
			{
				mov_hist = amov_st = AMOV_TRIGGERED;
				amov_ctrl( MON_CTRL );
			}
			break;
		case AMOV_TRIGGERED:
			if( delta < MOVE_DET_THR )
			{
				amov_st = AMOV_WSTILL;
				amov_delay = (MOVE_DET_DELAY/ACCEL_TICK);
			}
			break;
		case AMOV_WSTILL:
			if( delta > MOVE_DET_THR )
				amov_delay = (MOVE_DET_DELAY/ACCEL_TICK);

			if( amov_delay && --amov_delay == 0 )
			{
				amov_st = AMOV_STILL;
				amov_ctrl( MOFF_CTRL );
			}
			break;
		default:
			amov_st = AMOV_STILL;
			amov_ctrl( MOFF_CTRL );
			break;
	}

#ifdef _MOVEDET_SIMPLE_AXIS_
	acc_old = axis;
#else
	x_old = x;
	y_old = y;
	z_old = z;
#endif
}

