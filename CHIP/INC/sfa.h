/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2013 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   sfa.h  (Nuvoton fpu)                                                                                  */
/*            This file contains Nuvoton Sensor Fusion accelerator interface                               */
/* Project:                                                                                                */
/*            Sensor Hub                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _HW_FPU_H
#define _HW_FPU_H


/*---------------------------------------------------------------------------------------------------------*/
/* SFA_REVISION_T : defines SFA revision. Rider12A1 has SFA revision A1. Rider12B1 has SFA revision B1.    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	A1 = 0x00,
	B1 = 0x01
} SFA_REVISION_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SFA_init                                                                               */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  initialize the Sensor Fusion accelerator. The defautl rounding mode                    */
/*                  is Round to Nearest even.                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SFA_init(SFA_REVISION_T rev);


#endif

