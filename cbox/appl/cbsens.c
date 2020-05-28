/*
 * cbsens.c
 */

#include "cbsens.h"
#include "grsens.h"
#include "accel.h"
#include "sleep.h"
#include "formats.h"
#include "humsens.h"
#include "movdet.h"
#include "tplink.h"
#include "reset.h"
#include "cboxcmd.h"
#include "ioports.h"

#define _NORIA_SENSOR_
#define _ACCEL_MOTION_DETECT_
#define _INCLINOMETER_
#define _HUMEDITY_DETECT_

//#define __CHANGE_HUM2_FOR_ACCCELX__

static CBOX_ST cb;

#define h2ns(x);

void set_responce( CBOX_ST *p );

void
tplink_onrcv( TPLFRM_T	*p )
{
	switch( *(uchar *)(p->pload) )
	{
		case CBOX_READ_ALL:

			cb.cmd = CBOX_READ_ALL;
			grs_read( &cb.h );
			get_accel_data( &cb.a );
			cb.m = get_movdet();
			cb.hum = get_humedity_sensor();
			
			h2ns( &cb.h.hoard );
			h2ns( &cb.h.pqty );
			h2ns( &cb.h.flow );

			h2ns( &cb.a.x );
			h2ns( &cb.a.y );
			h2ns( &cb.a.z );

			LED ^= 1;

#ifdef __CHANGE_HUM2_FOR_ACCCELX__
			cb.a.x = read_adc( HUMEDITY2 );
#endif
			set_responce( &cb );

#if 0
			if( tplink_send_frame( (uchar *)&cb, sizeof(cb) ) < 0)
				reset_now();
#endif
			break;
		case CBOX_NULL:
		default:
			break;
	}
}

void
tplink_ontout( void )
{

}

