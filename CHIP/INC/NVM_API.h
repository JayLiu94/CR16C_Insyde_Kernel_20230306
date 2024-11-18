/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2006-2010 by Nuvoton Technology Corporation                                              */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    nvm_api.h                                                                                            */
/*            This file contains API to NVM functions                                                      */
/* Project:                                                                                                */
/*            EC FW                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

#include "defs_errors.h"
#include "types.h"






typedef BYTE (*NVM_Read_func_ptr)(int Offset);
#define NVM_Read_ADDR                       (*(volatile unsigned long *)0x68)
#define NVM_Read(Offset)                    ((NVM_Read_func_ptr) NVM_Read_ADDR)(Offset)


typedef DEFS_STATUS (*NVM_Write_func_ptr)(int Offset, BYTE Data);
#define NVM_Write_ADDR                      (*(volatile unsigned long *)0x6C)
#define NVM_Write(Offset, Data)             ((NVM_Write_func_ptr) NVM_Write_ADDR)(Offset, Data)


typedef void (*NVM_SetReadProtect_func_ptr)(DWORD ReadMask);
#define NVM_SetReadProtect_ADDR             (*(volatile unsigned long *)0x70)
#define NVM_SetReadProtect(ReadMask)        ((NVM_SetReadProtect_func_ptr) NVM_SetReadProtect_ADDR)(ReadMask)


typedef DWORD (*NVM_GetReadProtect_func_ptr)();
#define NVM_GetReadProtect_ADDR             (*(volatile unsigned long *)0x74)
#define NVM_GetReadProtect()                ((NVM_GetReadProtect_func_ptr) NVM_GetReadProtect_ADDR)()


typedef void (*NVM_SetWriteProtect_func_ptr)(DWORD WriteMask);
#define NVM_SetWriteProtect_ADDR            (*(volatile unsigned long *)0x78)
#define NVM_SetWriteProtect(WriteMask)      ((NVM_SetWriteProtect_func_ptr) NVM_SetWriteProtect_ADDR)(WriteMask)


typedef DWORD (*NVM_GetWriteProtect_func_ptr)();
#define NVM_GetWriteProtect_ADDR            (*(volatile unsigned long *)0x7C)
#define NVM_GetWriteProtect()               ((NVM_GetWriteProtect_func_ptr) NVM_GetWriteProtect_ADDR)()


typedef DEFS_STATUS (*NVM_SetUnLockValue_func_ptr)(DWORD UnLockValue);
#define NVM_SetUnLockValue_ADDR             (*(volatile unsigned long *)0x80)
#define NVM_SetUnLockValue(UnLockValue)     ((NVM_SetUnLockValue_func_ptr) NVM_SetUnLockValue_ADDR)(UnLockValue)


typedef DEFS_STATUS (*NVM_UnLock_func_ptr)(DWORD UnLockValue);
#define NVM_UnLock_ADDR                     (*(volatile unsigned long *)0x84)
#define NVM_UnLock(UnLockValue)             ((NVM_UnLock_func_ptr) NVM_UnLock_ADDR)(UnLockValue)


typedef DEFS_STATUS (*NVM_SetPowerMode_func_ptr)(BYTE PowerMode);
#define NVM_SetPowerMode_ADDR               (*(volatile unsigned long *)0x88)
#define NVM_SetPowerMode(PowerMode)         ((NVM_SetPowerMode_func_ptr) NVM_SetPowerMode_ADDR)(PowerMode)


typedef DEFS_STATUS (*NVM_DisableJtagPermanently_func_ptr)();
#define NVM_DisableJtagPermanently_ADDR     (*(volatile unsigned long *)0x8C)
#define NVM_DisableJtagPermanently()        ((NVM_DisableJtagPermanently_func_ptr) NVM_DisableJtagPermanently_ADDR)()


typedef BYTE (*NVM_JtagIsEnabled_func_ptr)();
#define NVM_JtagIsEnabled_ADDR              (*(volatile unsigned long *)0x90)
#define NVM_JtagIsEnabled()                 ((NVM_JtagIsEnabled_func_ptr) NVM_JtagIsEnabled_ADDR)()

















