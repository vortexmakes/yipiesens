/*
 * humsens.c
 */

#include "adc.h"
#include "ioports.h"
#include "settings.h"

#define humsens_ctrl(x)		(HSENS_CTRL=x)
//#define humsens_ctrl(x)		

enum
{
	HUMSENS_OFF, HUMSENS_STARTING,
	HUMSENS_ON, HUMSEN_STOPING
}HUMS_STATES;

enum
{
	HOFF_CTRL, HON_CTRL
}HUMS_CTRL;

static MUInt hums_st = HUMSENS_OFF;
static MUInt hums_delay = 0;

uchar
get_humedity_sensor( void )
{
	if( hums_st != HUMSENS_ON )
		return 0;
	return read_adc( HUMEDITY1 );
}

void
proc_humsens( ushort gflow )
{
	switch( hums_st )
	{
		case HUMSENS_OFF:
			if( gflow != 0 )
			{
				hums_st = HUMSENS_STARTING;
				hums_delay = (HUMSENS_ON_DELAY-1);
			}
			break;
		case HUMSENS_STARTING:
			if( hums_delay && --hums_delay == 0 )
			{
				hums_st = HUMSENS_ON;
				humsens_ctrl( HON_CTRL );
			}
			break;
		case HUMSENS_ON:
			if( gflow == 0 )
			{
				hums_st = HUMSEN_STOPING;
				hums_delay = (HUMSENS_OFF_DELAY-1);
			}
			break;
		case HUMSEN_STOPING:
			if( hums_delay && --hums_delay == 0 )
			{
				hums_st = HUMSENS_OFF;
				humsens_ctrl( HOFF_CTRL );
			}
			break;
		default:
			hums_st = HUMSENS_OFF;
			humsens_ctrl( HOFF_CTRL );
			break;
	}
}
