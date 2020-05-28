/*
 * humsens.h
 */

#ifndef __HUMSENS_H__
#define __HUMSENS_H__

#include "adc.h"
#include "mytypes.h"

uchar get_humedity_sensor( void );
void proc_humsens( ushort gflow );


#endif
