/*
 * flowmtr.c
 */

#include "flowmtr.h"
#include "sleep.h"
#include "formats.h"
#include "tplink.h"
#include "reset.h"
#include "fmcmd.h"
#include "ioports.h"
#include "flw.h"

#ifndef __SIMULATE_CBOX__
typedef struct
{
	uchar f1;
	uchar f1dir;
	uchar f2;
	uchar f2dir;
}FLOWMTR_ST;

static FLOWMTR_ST fm;

#else

typedef struct
{
	ushort hoard;	// grain hoard.
	ushort pqty;     // pails quantity.
	ushort flow;	// grain volume by second.
} GRSENS_T;

typedef struct
{
	ushort x;
	ushort y;
	ushort z;
	uchar m;
}ACCEL_T;

typedef struct
{
	uchar cmd;
	uchar m;
	GRSENS_T h;
	ACCEL_T a;
	uchar hum;
}CBOX_ST;

static CBOX_ST cb;
#endif

void
send_flow( void )
{
#ifndef __SIMULATE_CBOX__
	fm.f1 = flw_get_pps( S1S2 );
	fm.f1dir = flw_get_flowdir( S1S2 );
	fm.f2 = flw_get_pps( S3S4 );
	fm.f2dir = flw_get_flowdir( S3S4 );

	if( tplink_send_frame( (uchar *)&fm, sizeof(fm) ) < 0 )
		reset_now();
#else

/*
 * Transmite como si fuera CBOX
 */
	cb.cmd = FMTR_READ_ALL;
	cb.a.x = flw_get_pps( S1S2 );
	cb.a.y = flw_get_pps( S3S4 );

	if( flw_get_flowdir( S1S2 ) == 1 )
		cb.a.x |= 0x80;

	if( flw_get_flowdir( S3S4 ) == 1 )
		cb.a.y |= 0x80;

	if( tplink_send_frame( (uchar *)&cb, sizeof(cb) ) < 0 )
		reset_now();
#endif
}

void
tplink_onrcv( TPLFRM_T	*p )
{
	switch( *(uchar *)(p->pload) )
	{
		case FMTR_READ_ALL:
			send_flow();
			break;
		case FMTR_NULL:
		default:
			break;
	}
}

void
tplink_ontout( void )
{

}


#define s1s2_sig()	(PTAD & 0x03)
#define s3s4_sig()	((PTAD & 0x0C)>>2)

void
flwmtr_proc( void )
{
	flw_process( S3S4, s3s4_sig() );
	flw_process( S1S2, s1s2_sig() );
}
