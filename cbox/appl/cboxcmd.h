/*
 * cboxcmd
 */

#ifndef __CBOXCMD_H__
#define __CBOXCMD_H__

enum
{
	ADDR_NORIA,
	ADDR_CAUDALIMETRO,
	NUM_SENSORS_BOX,
};

enum
{
	TPSENS_NULL_CMD,

	CBOX_NULL = TPSENS_NULL_CMD,
	CBOX_READ_ALL,		/* Reads all CBOX parameters */

	NUM_CBOX_CMDS
};

#endif
