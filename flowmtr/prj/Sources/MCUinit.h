/*
** ###################################################################
**     This code is generated by the Device Initialization Tool.
**     It is overwritten during code generation.
**     USER MODIFICATION ARE PRESERVED ONLY INSIDE EXPLICITLY MARKED SECTIONS.
**
**     Project   : flowmtr
**     Processor : MC9S08SH8CTJ
**     Version   : Component 01.007, Driver 01.06, CPU db: 3.00.062
**     Datasheet : MC9S08SH8 Rev. 3 6/2008
**     Date/Time : 06/07/2012, 05:59 p.m.
**     Abstract  :
**         This module contains device initialization code 
**         for selected on-chip peripherals.
**     Contents  :
**         Function "MCU_init" initializes selected peripherals
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################
*/

#ifndef __flowmtr_H
#define __flowmtr_H 1

/* Include shared modules, which are used for whole project */


/* User declarations and definitions */
/*   Code, declarations and definitions here will be preserved during code generation */
/* End of user declarations and definitions */

#ifdef __cplusplus
extern "C" {
#endif
extern void MCU_init(void);
#ifdef __cplusplus
}
#endif
/*
** ===================================================================
**     Method      :  MCU_init (component MC9S08SH8_20)
**
**     Description :
**         Device initialization code for selected peripherals.
** ===================================================================
*/



__interrupt void isrVmtim(void);
/*
** ===================================================================
**     Interrupt handler : isrVmtim
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void host_xmit_isr(void);
/*
** ===================================================================
**     Interrupt handler : host_xmit_isr
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void host_rcv_isr(void);
/*
** ===================================================================
**     Interrupt handler : host_rcv_isr
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



__interrupt void isrVscierr(void);
/*
** ===================================================================
**     Interrupt handler : isrVscierr
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/






/* END flowmtr */

#endif
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.07 [04.34]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
