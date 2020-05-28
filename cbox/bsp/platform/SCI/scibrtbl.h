/*
 *	scibrtbl.h
 */

#ifndef __SCIBRTBLE_H__
#define __SCIBRTBLE_H__

enum
{
	B300, B600, B1200, B2400, B4800, B9600, B19200,
	B38400, B57600, B115200,
	NUM_BAUD
};

extern const unsigned short sci_br_tbl[NUM_BAUD];

#endif