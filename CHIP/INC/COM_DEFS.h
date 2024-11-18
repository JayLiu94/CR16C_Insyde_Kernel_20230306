/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     com_defs.h - Common definitions to be used with all C files
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#ifndef COM_DEFS_H
#define COM_DEFS_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include    <asm.h>

/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/

#define    TRUE            1
#define    FALSE           0

#ifndef    NULL
#define    NULL            0
#endif     // NULL

#define    hw_char         *(volatile unsigned char *)
#define    hw_short        *(volatile unsigned short *)


typedef    void (*handler_t)(void);

typedef    int             bool;

#define    BIT(X)          (1 << (X))

#define    PSR_E_BIT_MASK  BIT(9)
#define    PSR_I_BIT_MASK  BIT(11)

#define    ATOMIC_OP(exp)  do { _di_(); exp; _ei_(); } while (0)

#ifdef DEBUG
#define ASSERT(cond)  {if(!(cond)) __asm__("excp bpt"); }
#else
#define ASSERT(cond)
#endif
/*------------------------------------------------------------------------------
 * BIT operation macros
 *----------------------------------------------------------------------------*/

#define CPL_BIT(reg, bits)       ((reg) ^= bits)
#define SET_BIT(reg, bits)       ((reg) |= bits)
#define MASK_SET_BIT(reg, mask, set) ((reg) = (reg & ~mask) | set)
#define CLEAR_BIT(reg, bits)     ((reg)&= ~bits)
#define READ_BIT(reg, bit)      ((reg  & bit) ? 1 : 0)
#define IS_BIT_SET(reg, bit)    ((reg & bit) ? 1 : 0)
#define IS_BIT_CLEAR(reg, bit)  ((reg & bit) ? 0 : 1)


/*------------------------------------------------------------------------------
 * FIELD operation macros
 *----------------------------------------------------------------------------*/

// Get contents of "field" from "reg"
#define GET_FIELD(reg, field)   \
    GET_FIELD_SP(field##_S,field##_P,reg)

// Set contents of "field" in "reg" to value"
#define SET_FIELD(reg, field, value)    \
    SET_FIELD_SP(field##_S,field##_P,reg, value)

// Auxiliary macro: Get contents of field using size and position
#define GET_FIELD_SP(size, position, reg)    ((reg>>position)&((1<<size)-1))

// Auxiliary macro: Set contents of field using fields size and position
#define SET_FIELD_SP(size, position, reg ,value)    \
    (reg = (reg &(~((((DWORD)1<<size)-1)<<position)))|((DWORD)value<<position))

/*------------------------------------------------------------------------------
 * MASK operation macros
 *----------------------------------------------------------------------------*/
#define _set_i_bit_() \
  do \
  { \
    unsigned int tpsr = 0; \
    _spr_("psr", tpsr); \
    tpsr |= 0x0800;    \
    _lpr_("psr",tpsr); \
  } while(0)


#define MAX(a, b)            ( (a)>(b) ? (a) : (b) )      /* calculate maximum value                       */
#define MIN(a, b)            ( (a)<(b) ? (a) : (b) )      /* calculate minimum value                       */
#define ABS(a)               ( (a)>(0) ? (a) : -(a) )     /* calculate absolute value                      */
#define MOD(a,b)             ((a) - ((a)/(b))*(b))        /* calculate modulus                             */


#define LSN(u8)         ((BYTE)((BYTE)u8 & 0x0F))
#define MSB0(u32)       ((BYTE)(((DWORD)(u32) & 0xFF000000) >> 24))

#define _STRINGX(x)             #x

#define STRINGX(x)              _STRINGX(x)

/*---------------------------------------------------------------------------------------------------------*/
/* Calculate size of statically declared array                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)           (sizeof(a) / sizeof((a)[0]))
#endif

#endif /* COM_DEFS_H */


