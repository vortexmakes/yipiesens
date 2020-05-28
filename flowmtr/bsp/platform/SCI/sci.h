/*
 *	sci.h
 *	Routines handle SCI service
 *
 *	Programmer: Darío S. Baliña.
 *	Date : 29/05/06.
 */

#ifndef __SCI_H__
#define __SCI_H__

#include "mytypes.h"

typedef void(*SCI_CALLBACK)( MUInt );

#undef __8MHz__

#define	UC_B300		0xD55	// 0.0% error
#define	UC_B600		0x6AB	// 0.0% error
#define	UC_B1200	0x355	// 0.0% error
#define	UC_B2400	0x1AB	// 0.1% error
#define	UC_B4800	0xD5	// 0.2% error
#define	UC_B9600	0x6B	// 0.3% error
#define	UC_B1440	0x47	// 0.15% error
#define	UC_B19200	0x35	// 0.6% error
#define	UC_B38400	0x1B	// 1.2% error
#define	UC_B57600	0x12	// 1.2% error
#define	UC_B115200	0x09	// 1.2% error


void host_sci1_init( unsigned short baud_rate );
void host_sci1_setbd( unsigned short baud_rate );

void interrupt host_xmit_isr( void );
void interrupt host_rcv_isr( void );

#endif
