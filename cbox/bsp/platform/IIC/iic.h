/***********************************************************************************************\
* Freescale MMA8451,2,3Q Driver
*
* Filename: iic.h
*
*
* (c) Copyright 2010, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/
#ifndef _IIC_H_
#define _IIC_H_

/***********************************************************************************************\
* Public macros
\***********************************************************************************************/

/***********************************************************************************************
**
**  Inter-Integrated Circuit (IIC)
**
**  0x0030  IICA      IIC Address Register
**  0x0031  IICF      IIC Frequency Divider Register
**  0x0032  IICC1     IIC Control Register 1
**  0x0033  IICS      IIC Status Register
**  0x0034  IICD      IIC Data I/O Register
**  0x0035  IICC2     IIC Control Register 2
**
**  IIC target bit rate = 800K
**  MCU bus frequency = 16.384MHz
**
**  IIC MULT = 0 ;  hence mul = 1
**  IIC ICR  = 0 ;  hence scl = 20
**
**  IIC bit rate = bus / (mul * scl)
**               = 16.384MHz / (1 * 20)
**               = 819.2kHz
**
**    In addtion, SDA Hold Value = 7
**                SCL Start Hold Value = 6
**                SDA Stop Hold Value = 11
**
**    Hence,
**      SDA hold time = (1/bus) * mul * SDA Hold Value
**                    = 61.03ns * 1 * 7
**                    = 427.21ns
**
**      SCL Start hold time = (1/bus) * mul * SCL Start Hold Value
**                          = 61.03ns * 1 * 6
**                          = 366.18ns
**
**      SDA Stop hold time  = (1/bus) * mul * SDA Stop Hold Value
**                          = 61.03ns * 1 * 11
**                          = 671.33ns
*/

#define init_IICA     0x00

#ifdef __S08SH__
#define init_IICF     0b00000000
/*                      00000000 = reset
**                      ||||||||
**                      ||++++++-- ICR      =0  : scl = 20
**                      ++-------- MULT     =0  : mul = 1
*/
#endif

#define init_IICC1    0b10000000
/*                      00000000 = reset
**                      ||||||xx
**                      |||||+---- RSTA
**                      ||||+----- TXAK     =0  : send ACK
**                      |||+------ TX       =0  : Receive mode
**                      ||+------- MST      =0  : Initial state for Master mode
**                      |+-------- IICIE    =0  : IIC interrupts disabled
**                      +--------- IICEN    =1  : IIC module enabled
*/

#define init_IICC2    0b00000000
/*                      00000000 = reset
**                      ||xxx|||
**                      ||   +++-- AD10-8   =0  : upper 3 bits of 10-bit slave address
**                      |+-------- ADEXT    =0  : 7-bit address scheme
**                      +--------- GCAEN    =0  : General call address disabled
*/


/***********************************************************************************************\
* Public type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public prototypes
\***********************************************************************************************/

void IIC_Init( void );
void IIC_RegWrite(byte address, byte reg,byte val);
byte IIC_RegRead(byte address, byte reg);
void IIC_RegWriteN(byte address, byte reg1,byte N,byte *array);
void IIC_RegReadN(byte address, byte reg1,byte N,byte *array);


#endif  /* _IIC_H_ */
