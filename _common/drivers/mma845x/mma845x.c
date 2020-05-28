/***********************************************************************************************\
* Freescale MMA8451,2,3Q Driver
*
* Filename: mma845x.c
*
*
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/

#include "system.h"
#include "formats.h"

/***********************************************************************************************\
* Private macros
\***********************************************************************************************/

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

extern byte SlaveAddressIIC;

#pragma DATA_SEG DEFAULT

/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
* Put MMA845xQ into Active Mode
\*********************************************************/
void MMA845x_Active ()
{
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (IIC_RegRead(SlaveAddressIIC, CTRL_REG1) | ACTIVE_MASK));
}

/*********************************************************\
* Put MMA845xQ into Standby Mode
\*********************************************************/
void MMA845x_Standby (void)
{
  byte n;
  /*
  **  Read current value of System Control 1 Register.
  **  Put sensor into Standby Mode.
  **  Return with previous value of System Control 1 Register.
  */
  n = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, n & ~ACTIVE_MASK);
}

/*********************************************************\
* Initialize MMA845xQ
\*********************************************************/
void MMA845x_Init ( byte full_scale )
{
  MMA845x_Standby();
  /*
  **  Configure sensor for:
  **    - Sleep Mode Poll Rate of 50Hz (20ms)
  **    - System Output Data Rate of 800Hz (1.25ms)
  **    - Full Scale of +/-2g
  **    - Motion Detection Latch in XYZ-axis
  **    - Motion Detection Threshold > 3g => 3/0.063 = 24 (0x18)
  **    - Debouncing of 100 steps at 1.25ms (ODR) -> 125ms
  */
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, /*FREAD_MASK|*/LNOISE_MASK|ASLP_RATE_20MS+DATA_RATE_1250US);
  IIC_RegWrite(SlaveAddressIIC, XYZ_DATA_CFG_REG, full_scale );
#ifdef ACCEL_MOTION_DETECT
  IIC_RegWrite(SlaveAddressIIC, FF_MT_CFG_1_REG, 
                                ELE_MASK|OAE_MASK|/*ZEFE_MASK|*/YEFE_MASK|XEFE_MASK);  
  IIC_RegWrite(SlaveAddressIIC, FT_MT_THS_1_REG, DBCNTM_MASK|4);
  IIC_RegWrite(SlaveAddressIIC, FF_MT_COUNT_1_REG, 0x64);                
#endif
                              
}

/***********************************************************************************************\
* Private functions
\***********************************************************************************************/
