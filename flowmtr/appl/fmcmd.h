/*
 * fmcmd
 */

#ifndef __FMCMD_H__
#define __FMCMD_H__

enum
{
	ADDR_NORIA,
	ADDR_CAUDALIMETRO,
	NUM_SENSORS_BOX,
};

enum
{
	TPSENS_NULL_CMD,

	FMTR_NULL = TPSENS_NULL_CMD,
	FMTR_READ_ALL,		/* Reads all FlowMeter parameters */

	NUM_FMTR_CMDS
};

#endif
