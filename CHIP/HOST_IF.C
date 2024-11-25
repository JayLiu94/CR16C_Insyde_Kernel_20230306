/*-----------------------------------------------------------------------------
 * MODULE HOST_IF.C
 *
 * Copyright(c) 1996-2010, Insyde Software Corporation. All Rights Reserved.
 *
 * You may not reproduce, distribute, publish, display, perform, modify, adapt,
 * transmit, broadcast, present, recite, release, license or otherwise exploit
 * any part of this publication in any form, by any means, without the prior
 * written permission of Insyde Software Corporation.
 *
 *-----------------------------------------------------------------------------
 *
 * Revision History (Started June 13, 1996 by GLP)
 *
 * 27 Feb 98   Removed references to EMUL.
 * 24 Apr 98   Changed Aux_Error_To_Host.
 * 08 Jul 98   Added SENDING_SCAN_RC handling to Data_To_Host.
 * 11 Sep 98   Changed for smaller code space.
 * 19 Aug 99   Updated copyright.
 * 20 Jan 00   Support for Active PS/2 Multiplexing (APM) is
 *             marked as V21.1.
 * 15 Feb 05   Fixed PS/2 device lock up issue.
 *             The OBF and IRQ1B/IRQ12B always set as 1. The serial
 *             IRQ1/IRQ12 disappeared on scope. The sloution was
 *             clear the HICTRL_OBFKIE/HICTRL_OBFMIE before write
 *             data to HIKDO/HIMDO.
 * ------------------------------------------------------------------------- */

#define HOST_IF_C

#include "swtchs.h"
#include "types.h"
#include "com_defs.h"
#include "host_if.h"
#include "purdat.h"
#include "ps2.h"
#include "cr_uart.h"
#if ESPI_SUPPORTED
#include "espi.h"
#endif /* ESPI_SUPPORTED */

#ifndef HOST_IF_MSGS
#error HOST_IF_MSGS switch is not defined.
#endif

/* ---------------------------------------------------------------
   Name: Data_To_Host

   Clears error bits in the Host Interface status port and
   sends a command response byte or a byte of keyboard data
   to the Host.  Generates Host IRQ1 if keyboard interrupts
   are enabled in controller command byte.

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
void Data_To_Host(BYTE data)
{
   BYTE temp;

   #if HOST_IF_MSGS
    Message("0x%X>Host\n", (int) data);
   #endif
    DEBUG_PRINTF("KB 0x%02x>Host\n", data);

   /* Clear error and aux device bits from status port. */
   CLEAR_BIT(HIKMST, (maskSTATUS_PORT_PARITY  |  /* Parity error bit. */
               maskSTATUS_PORT_GENTMO  |  /* General time out error bit. */
               maskSTATUS_PORT_SENDTMO |  /* Send time out error bit for AT. */
               maskSTATUS_PORT_AUXOBF));   /* Aux device bit for PS/2. */

   /*
   If Ext_Cb3.bit.DIAG_MODE is set, diagnostic mode has been selected
   for the internal keyboard (scanner).  In this case,
   Flag.SENDING_SCAN_RC is set when the RC (row/column) data from the
   internal keyboard is to be sent to the Host.  The data is sent
   without an interrupt.  The data from the external keyboard, however,
   may be sent with an interrupt.  This allows the configuration utility
   to determine if the data is coming from the internal or external
   keyboard because external codes are intercepted by INT9, while the
   internal keyboard's row/column data needs to be polled.  This way,
   the configuration utility can use the external keyboard for control
   of the application, and the internal keyboard to assign matrix tables. */

   //temp = HICTRL;
   temp = HICTRL & ~(HICTRL_OBFKIE);
   HICTRL = temp;
   if ( (Ccb42.bit.INTR_KEY == 0) || (Flag.SENDING_SCAN_RC == 1) )
   {  /*
      Disable IRQ1 generation so Host will not be sent an
      IRQ1 with the data.  It is assumed that when the
      interrupt is disabled, the interrupt signal stays low. */

      //temp &= ~(HICTRL_OBFKIE);
      CLEAR_BIT(temp, HICTRL_OBFKIE);
   }
   else
   {  /*
      Enable IRQ1 generation so Host
      will be sent an IRQ1 with the data. */

      //temp |= HICTRL_OBFKIE;
      SET_BIT(temp, HICTRL_OBFKIE);

   }
   HICTRL = temp;

   HIKDO = data;  /* Put data in output buffer. */

   Flag.SENDING_SCAN_RC = 0;
}

/* ---------------------------------------------------------------
   Name: Error_To_Host

   Sets appropriate error bits in the Host Interface status port and
   sends a byte of data to the Host.  The error bits for the status
   register will be modified before writing them if the controller is
   in PS/2 compatible mode.  Generates Host IRQ1 if keyboard
   interrupts are enabled in controller command byte.

   Input:
      error bits for status register in AT compatible mode,
      data to send to Host.
   --------------------------------------------------------------- */
void Error_To_Host(BYTE data, BYTE error_bits)
{
   BYTE temp;
    DEBUG_PRINTF("KB 0x%02x, Err 0x%02x>Host\n", data, error_bits);

   if (IS_BIT_SET(HIKMST, maskSTATUS_PORT_ENABLE))
   {
      /* The inhibit switch is not on.  The keyboard is enabled. */

      if (Ext_Cb0.bit.PS2_AT)
      {
         /* PS/2 mode.  Convert status bits
            from AT format to PS/2 format. */

         /* If a send time out error is setup, set
            the general time out error bit.  The
            PS/2 mode does not have a send time out bit. */
         if (error_bits & maskSTATUS_PORT_SENDTMO)
            error_bits |= maskSTATUS_PORT_GENTMO;

         /* Clear Aux device bit (because the data
            is from the keyboard, not the aux device). */
         error_bits &= ~maskSTATUS_PORT_AUXOBF;
      }

      /* Clear error and aux device bits and
         merge new error bits in status port. */
      //HIKMST = (HIKMST & ~(maskSTATUS_PORT_PARITY  |  /* Parity error bit. */
      //                     maskSTATUS_PORT_GENTMO  |  /* General time out error bit. */
      //                     maskSTATUS_PORT_SENDTMO |  /* Send time out error bit for AT. */
      //                     maskSTATUS_PORT_AUXOBF))   /* Aux device bit for PS/2. */
      //
      //         | error_bits;
      MASK_SET_BIT(HIKMST, (maskSTATUS_PORT_PARITY |  /* Parity error bit. */
                           maskSTATUS_PORT_GENTMO  |  /* General time out error bit. */
                           maskSTATUS_PORT_SENDTMO |  /* Send time out error bit for AT. */
                           maskSTATUS_PORT_AUXOBF),   /* Aux device bit for PS/2. */
                           error_bits);


//      temp = HICTRL;
      temp = HICTRL & ~(HICTRL_OBFKIE);
      HICTRL = temp;
      if (Ccb42.bit.INTR_KEY)
      {  /*
         Enable IRQ1 generation so Host
         will be sent an IRQ1 with the data. */

         //temp |= HICTRL_OBFKIE;
         SET_BIT(temp, HICTRL_OBFKIE);
      }
      else
      {  /*
         Disable IRQ1 generation so Host will not be sent an
         IRQ1 with the data.  It is assumed that when the
         interrupt is disabled, the interrupt signal stays low. */

         //temp &= ~(HICTRL_OBFKIE);
         CLEAR_BIT(temp, HICTRL_OBFKIE);
      }
      HICTRL = temp;

      HIKDO = data;  /* Put data in output buffer. */
   }
}

/* ---------------------------------------------------------------
   Name: Aux_Data_To_Host

   Send auxiliary device (mouse) data to the Host.

   Clears error bits in the Host Interface status port and
   sends a byte of auxiliary device (mouse) data to the
   Host.  Generates Host IRQ12 if auxiliary device (mouse)
   interrupts are enabled in controller command byte.

   Input:
      data to send to Host.
   --------------------------------------------------------------- */
void Aux_Data_To_Host(BYTE data)
{
   BYTE temp;

    DEBUG_PRINTF("Aux 0x%02x>Host\n", data);

   /* Clear error bits and set aux device bit in status port. */
   //HIKMST = (HIKMST & ~(maskSTATUS_PORT_PARITY  |  /* Parity error bit. */
   //                     maskSTATUS_PORT_GENTMO  |  /* General time out error bit. */
   //                     maskSTATUS_PORT_SENDTMO))  /* Send time out error bit for AT. */
   //
   //          | maskSTATUS_PORT_AUXOBF;             /* Aux device for PS/2. */
   MASK_SET_BIT(HIKMST, (BYTE) (maskSTATUS_PORT_PARITY  |  /* Parity error bit. */
                        maskSTATUS_PORT_GENTMO  |  /* General time out error bit. */
                        maskSTATUS_PORT_SENDTMO),  /* Send time out error bit for AT. */
                        maskSTATUS_PORT_AUXOBF);             /* Aux device for PS/2. */

   if (MULPX.bit.Multiplex)
   {
      HIKMST = (HIKMST & ~(maskSTATUS_PORT_SYSTEM)) |
                  ((MULPX.byte & 0x03) << 6);
   }

   //temp = HICTRL;
   temp = HICTRL & ~(HICTRL_OBFMIE);
   HICTRL = temp;
   if (Ccb42.bit.INTR_AUX)
   {  /*
      Enable IRQ12 generation so Host
      will be sent an IRQ12 with the data. */

      //temp |= HICTRL_OBFMIE;
      SET_BIT(temp, HICTRL_OBFMIE);
   }
   else
   {  /*
      Disable IRQ12 generation so Host will not be sent an
      IRQ12 with the data.  It is assumed that when the
      interrupt is disabled, the interrupt signal stays low. */

      //temp &= ~(HICTRL_OBFMIE);
      CLEAR_BIT(temp, HICTRL_OBFMIE);
   }
   HICTRL = temp;

   HIMDO = data;  /* Put data in mouse output buffer. */
}

/* ---------------------------------------------------------------
   Name: Aux_Error_To_Host

   Send auxiliary device (mouse) error data to the Host.

   Sets appropriate error bits in the Host Interface status port
   and sends a byte of auxiliary device (mouse) data to the Host.
   Generates Host IRQ12 if auxiliary device (mouse) interrupts are
   enabled in controller command byte.

   This routine should only be used if the controller is in PS/2
   compatible mode.

   Input:
      error bits for status register in AT compatible mode,
      data to send to Host.
   --------------------------------------------------------------- */
void Aux_Error_To_Host(BYTE data, BYTE error_bits)
{
   BYTE temp;

    DEBUG_PRINTF("Aux 0x%02x, Err 0x%02x>Host\n", data, error_bits);

   temp = HIKMST;
   if (IS_BIT_SET(temp, maskSTATUS_PORT_ENABLE))
   {
      /* The inhibit switch is not on.  The keyboard is enabled. */

      if (Ext_Cb0.bit.PS2_AT)
      {
         /* PS/2 mode.  Convert status bits
            from AT format to PS/2 format. */

         /* If a send time out error is setup, set
            the general time out error bit.  The
            PS/2 mode does not have a send time out bit. */
         if (IS_BIT_SET(error_bits, maskSTATUS_PORT_SENDTMO))
            SET_BIT(error_bits, maskSTATUS_PORT_GENTMO);
      }

      /* Clear error bits, set aux device bit,
         and merge new error bits in status port. */
      CLEAR_BIT(temp, (maskSTATUS_PORT_PARITY  | /* Parity error bit. */
                maskSTATUS_PORT_GENTMO  | /* General time out error bit. */
                maskSTATUS_PORT_SENDTMO)); /* Send time out error bit for AT. */


      //temp |= maskSTATUS_PORT_AUXOBF;
      SET_BIT(temp, maskSTATUS_PORT_AUXOBF);
      if (MULPX.bit.Multiplex)
      {
         HIKMST = (temp | maskSTATUS_PORT_SYSTEM |
                        ((MULPX.byte & 0x03) << 6));

         if (Gen_Timer != 0)
         {
            data	= AUX_SEND_ERROR;
         }
         else if (!Parity_Is_Ok())
         {
            data = AUX_RECEIVE_ERROR;
         }
         else
         {
            data = AUX_REMOVED;
         }
      }
      else
      {
         HIKMST = temp | error_bits;
      }

      //temp = HICTRL;
      temp = HICTRL & ~(HICTRL_OBFMIE);

      HICTRL = temp;
      if (Ccb42.bit.INTR_AUX)
      {  /*
         Enable IRQ12 generation so Host
         will be sent an IRQ12 with the data. */

         //temp |= HICTRL_OBFMIE;
         SET_BIT(temp, HICTRL_OBFMIE);
      }
      else
      {  /*
         Disable IRQ12 generation so Host will not be sent an
         IRQ12 with the data.  It is assumed that when the
         interrupt is disabled, the interrupt signal stays low. */

         //temp &= ~(HICTRL_OBFMIE);
         CLEAR_BIT(temp, HICTRL_OBFMIE);
      }
      HICTRL = temp;

      HIMDO = data;  /* Put data in mouse output buffer. */
   }
}

/* ---------------------------------------------------------------
   Name: Host_If_Init

   Initialize Host interface registers.
   --------------------------------------------------------------- */
void Host_If_Init(void)
{
    #if ESPI_SUPPORTED
    eSPI_Init();
    #endif /* ESPI_SUPPORTED */
	/* Keyboard and Mouse interrupts (IRQ1 and IRQ12)
      are enabled or disabled by a command from the
      Host.  IRQ1 and IRQ12 generation will be disabled
      here so Host will not be sent an IRQ1 or IRQ12
      when data is sent through the Host interface.

      The Power Management interrupt (IRQ11) is an OEM
      defined interrupt.  It will be disabled here. */

   /* Set so that:
      IRQ 1, 11, and 12 bits will be 0 if IRQ generation is
      turned off, level interrupt is selected, standard ISA
      polarity, and open drain type interrupt signals. */
   HIIRQC = 0;

   /* Init HICTRL:
      Disable OBF kbd, mouse, and PM interrupts (IRQ1, IRQ12 and IRQ11).
      Disable IBF kbd/mouse and PM core interrupts.
      Disable OBE kbd/mouse and PM core interrupts. */
   HICTRL = 0;

   /*
   Initialize the Host interface status port.  Enable the
   keyboard (set the inhibit switch bit to off, disable the
   password) to allow the Host to run. */

   HIKMST = maskSTATUS_PORT_ENABLE;

}
