/**
 * formats.c
 */


#include <stdio.h>
#include <stdarg.h>

#include "formats.h"
#include "serial.h"

static char buff[ MAX_CHARS_SERIAL ];

void
mprintf( MUInt ch, const char *fmt, ... )
{
	va_list args;
	int r;

	va_start( args, fmt );
	r = vsprintf( buff, fmt, args );
	va_end( args );
	r = put_string( ch, buff );
}
