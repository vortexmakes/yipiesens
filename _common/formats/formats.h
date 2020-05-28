/**
 * formats.h
 */
#ifndef __FORMATS_H__
#define __FORMATS_H__

#include "mytypes.h"

#define MAX_CHARS_SERIAL	48

void mprintf( MUInt ch, const char *fmt, ... );

#ifdef __DEBUG_SERIAL__
#define dprintf(x)	mprintf##x
#else
#define dprintf(x)
#endif

#endif
