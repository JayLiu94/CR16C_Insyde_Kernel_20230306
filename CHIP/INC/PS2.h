/*-----------------------------------------------------------------------------
 * MODULE PS2.H
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
 * PS/2 (serial) devices.
 *
 * Revision History (Started July 10, 1996 by GLP)
 *
 * 27 Feb 98   Removed references to EMUL.
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#ifndef PS2_H
#define PS2_H

#include "swtchs.h"
#include "types.h"
#include "com_defs.h"

#ifndef AUX_PORT3_SUPPORTED
#error AUX_PORT3_SUPPORTED switch is not defined.
#endif

#ifndef AUX_PORT2_SUPPORTED
#error AUX_PORT2_SUPPORTED switch is not defined.
#endif

#if AUX_PORT3_SUPPORTED
 extern AUX_PORT * const aux_pntr_table[3];
#elif AUX_PORT2_SUPPORTED
 extern AUX_PORT * const aux_pntr_table[2];
#else
 extern AUX_PORT * const aux_pntr_table[1];
#endif

void Ps2_Init(BYTE pscon);
void Enable_Serial_Irq(SMALL port);
void Disable_Serial_Irq(SMALL port);
FLAG Parity_Is_Ok(void);

/* The auxiliary keyboard is usually connected to Auxiliary Port 1.
   The external mouse is usually connected to Auxiliary Port 2.
   The internal (non-removable) mouse is connected to Auxiliary Port 3.
   The internal keyboard (scanner) is not connected to a PS/2 port.
   It is defined to be a number not used by the other PS/2 devices.
   It is defined in SCAN.H using NEXT_PORT.

   The values for AUX_PORT1_CODE through AUX_PORT3_CODE are selected
   so 0 can be used for "no port" and the other values can fit into
   two bits. */
#define AUX_PORT1_CODE 1
#define AUX_PORT2_CODE 2
#define AUX_PORT3_CODE 3
#define NEXT_PORT 3

/* Serial send/receive timeouts for auxiliary keyboard and auxiliar
   device (mouse).  These are multiples of the Timer A "base time"
   which should be set for 1 millisecond.  The timeout values are
   based on the IBM AT keyboard technical specifications, but some
   are set to allow more time if compatibility problems are not an
   issue. */

#define SEND_TMO    60 /* Time allowed for device to start clocking
                          data after a request to send is generated. */
#define ACK_TMO     25 /* 25 mS (IBM - 25 mS) */
#define RECEIVE_TMO  3 /*  3 mS (IBM -  2 mS) */
#define TRANSMIT_TMO 3 /*  3 mS (IBM -  2 mS) */

/* The following are multiples of the Timer B "base
   time" which should be set for 150 microseconds. */
#define RESPONSE_TIME  9  /* 9 Timer_B intervals required to transfer. */

/* ---------------------------------------------------------------
   Name: Enable_Serial_Irq

   Enable interrupt request so that an interrupt will occur
   when data is being transferred to/from a serial device.

   C prototype:
      void Enable_Serial_Irq(SMALL port);

   Port is AUX_PORT1_CODE, AUX_PORT2_CODE, or AUX_PORT3_CODE.
   --------------------------------------------------------------- */
void Enable_Serial_Irq(SMALL port);

/* ---------------------------------------------------------------
   Name: Disable_Serial_Irq

   Disable interrupt request so that an interrupt will not
   occur when data is being transferred to/from a serial device.

   C prototype:
      void Disable_Serial_Irq(SMALL port);

   Port is AUX_PORT1_CODE, AUX_PORT2_CODE, or AUX_PORT3_CODE.
   --------------------------------------------------------------- */
void Disable_Serial_Irq(SMALL port);

/* ---------------------------------------------------------------
   Name: Serial_Irq_Is_Enabled

   Reads a serial device's interrupt enable status.

   C prototype:
      FLAG Serial_Irq_Is_Enabled(SMALL port);

   Port is AUX_PORT1_CODE, AUX_PORT2_CODE, or AUX_PORT3_CODE.
   --------------------------------------------------------------- */
FLAG Serial_Irq_Is_Enabled(SMALL port);

/* ---------------------------------------------------------------
   Name: Clr_Aux_Port1_Clk,     Clr_Aux_Port2_Clk,
        Clr_Aux_Port3_Clk
         Set_Aux_Port1_Clk,     Set_Aux_Port2_Clk,
         Set_Aux_Port3_Clk
         Get_Aux_Port1_Clk_In,  Get_Aux_Port2_Clk_In,
         Get_Aux_Port3_Clk_In
         Get_Aux_Port1_Clk_Out, Get_Aux_Port2_Clk_Out

   Lower/raise/read auxiliary port 1, 2, 3 or 4 clock line.

   C prototypes:
      void Clr_Aux_Port1_Clk(void);
      void Clr_Aux_Port2_Clk(void);
      void Clr_Aux_Port3_Clk(void);

      void Set_Aux_Port1_Clk(void);
      void Set_Aux_Port2_Clk(void);
      void Set_Aux_Port3_Clk(void);

      FLAG Get_Aux_Port1_Clk_In(void);
      FLAG Get_Aux_Port2_Clk_In(void);
      FLAG Get_Aux_Port3_Clk_In(void);

      FLAG Get_Aux_Port1_Clk_Out(void);
      FLAG Get_Aux_Port2_Clk_Out(void);
   --------------------------------------------------------------- */
#define Clr_Aux_Port1_Clk() (CLEAR_BIT(PSOSIG, PSOSIG_CLK1))
#define Clr_Aux_Port2_Clk() (CLEAR_BIT(PSOSIG, PSOSIG_CLK2))
#define Clr_Aux_Port3_Clk() (CLEAR_BIT(PSOSIG, PSOSIG_CLK3))

#define Set_Aux_Port1_Clk() (SET_BIT(PSOSIG,  PSOSIG_CLK1))
#define Set_Aux_Port2_Clk() (SET_BIT(PSOSIG,  PSOSIG_CLK2))
#define Set_Aux_Port3_Clk() (SET_BIT(PSOSIG,  PSOSIG_CLK3))

#define Get_Aux_Port1_Clk_In() (IS_BIT_SET(PSISIG, PSISIG_RCLK1))
#define Get_Aux_Port2_Clk_In() (IS_BIT_SET(PSISIG, PSISIG_RCLK2))
#define Get_Aux_Port3_Clk_In() (IS_BIT_SET(PSISIG, PSISIG_RCLK3))

#define Get_Aux_Port1_Clk_Out() (IS_BIT_SET(PSOSIG, PSOSIG_CLK1))
#define Get_Aux_Port2_Clk_Out() (IS_BIT_SET(PSOSIG, PSOSIG_CLK2))
/* ---------------------------------------------------------------
   Name: Clr_Aux_Port1_Dat, Clr_Aux_Port2_Dat, Clr_Aux_Port3_Dat
         Set_Aux_Port1_Dat, Set_Aux_Port2_Dat, Set_Aux_Port3_Dat
         Get_Aux_Port1_Dat_In, Get_Aux_Port2_Dat_In, Get_Aux_Port3_Dat_In
         Get_Aux_Port1_Dat_Out, Get_Aux_Port2_Dat_Out

   Lower/raise/read auxiliary port 1, 2 or 3 data line.

   C prototypes:
      void Clr_Aux_Port1_Dat(void);
      void Clr_Aux_Port2_Dat(void);
      void Clr_Aux_Port3_Dat(void);

      void Set_Aux_Port1_Dat(void);
      void Set_Aux_Port2_Dat(void);
      void Set_Aux_Port3_Dat(void);

      FLAG Get_Aux_Port1_Dat_In(void);
      FLAG Get_Aux_Port2_Dat_In(void);
      FLAG Get_Aux_Port3_Dat_In(void);

      FLAG Get_Aux_Port1_Dat_Out(void);
      FLAG Get_Aux_Port2_Dat_Out(void);
   --------------------------------------------------------------- */
#define Clr_Aux_Port1_Dat() (CLEAR_BIT(PSOSIG, PSOSIG_WDAT1))
#define Clr_Aux_Port2_Dat() (CLEAR_BIT(PSOSIG, PSOSIG_WDAT2))
#define Clr_Aux_Port3_Dat() (CLEAR_BIT(PSOSIG, PSOSIG_WDAT3))

#define Set_Aux_Port1_Dat() (SET_BIT(PSOSIG, PSOSIG_WDAT1))
#define Set_Aux_Port2_Dat() (SET_BIT(PSOSIG, PSOSIG_WDAT2))
#define Set_Aux_Port3_Dat() (SET_BIT(PSOSIG, PSOSIG_WDAT3))

#define Get_Aux_Port1_Dat_In()  (IS_BIT_SET(PSISIG, PSISIG_RDAT1))
#define Get_Aux_Port2_Dat_In()  (IS_BIT_SET(PSISIG, PSISIG_RDAT2))
#define Get_Aux_Port3_Dat_In()  (IS_BIT_SET(PSISIG, PSISIG_RDAT3))

#define Get_Aux_Port1_Dat_Out() (IS_BIT_SET(PSOSIG, PSOSIG_WDAT1))
#define Get_Aux_Port2_Dat_Out() (IS_BIT_SET(PSOSIG, PSOSIG_WDAT2))

/* ---------------------------------------------------------------
   Name: Int_Lock_Aux_Port1,
         Int_Lock_Aux_Port2,
         Int_Lock_Aux_Port3

   Lock auxiliary device.  Device locking is required to block
   multiple transmissions of devices through the port 60 PC
   interface.  This function locks out the device on the named port.

   Clear any pending interrupt from lowering the clock line.

   The Int_ prefix signifies that these routines are called
   from an interrupt routine.  These macros are used instead
   of "lock" subroutines because the function must execute
   as quickly as possible.
   --------------------------------------------------------------- */
 /* For hardware shift mechanism mode. */
#define Int_Lock_Aux_Port1() Aux_Port1.lock = 1; Clr_Aux_Port1_Clk()
#define Int_Lock_Aux_Port2() Aux_Port2.lock = 1; Clr_Aux_Port2_Clk()
#define Int_Lock_Aux_Port3() Aux_Port3.lock = 1; Clr_Aux_Port3_Clk()

/* ---------------------------------------------------------------
   Name: Int_Lock_Alt_Aux1, Int_Lock_Alt_Aux2, Int_Lock_Alt_Aux3, Int_Lock_Alt_Aux4

   Device locking is required to block multiple transmissions
   of devices through the port 60 PC interface.  This function
   locks out all the devices that are NOT the same as the port
   name.

   The Int_ prefix signifies that these routines are called
   from an interrupt routine.  These macros are used instead
   of "lock" subroutines because the function must execute
   as quickly as possible.  They are invoked by the interrupt
   routines when the first data bit is received from an
   auxiliary port.  The overhead of the call and return adds
   too much time to the execution time when using the
   equivalent subroutines.
   --------------------------------------------------------------- */
#if AUX_PORT3_SUPPORTED
  /* Lock Auxiliary Port 2, 3. */
  #define Int_Lock_Alt_Aux1() Aux_Port2.lock = 1; Clr_Aux_Port2_Clk(); \
                              Aux_Port3.lock = 1; Clr_Aux_Port3_Clk()

  /* Lock Auxiliary Port 1, 3. */
  #define Int_Lock_Alt_Aux2() Aux_Port1.lock = 1; Clr_Aux_Port1_Clk(); \
                              Aux_Port3.lock = 1; Clr_Aux_Port3_Clk()
  /* Lock Auxiliary Port 1, 2. */
  #define Int_Lock_Alt_Aux3() Aux_Port1.lock = 1; Clr_Aux_Port1_Clk(); \
                              Aux_Port2.lock = 1; Clr_Aux_Port2_Clk()

#else
   /* Lock Auxiliary Port 2 and 1. */
   #if AUX_PORT2_SUPPORTED
   #define Int_Lock_Alt_Aux1() Aux_Port2.lock = 1; Clr_Aux_Port2_Clk()
   /* Lock Auxiliary Port 1 and 1. */
   #define Int_Lock_Alt_Aux2() Aux_Port1.lock = 1; Clr_Aux_Port1_Clk()
   #else
   #define Int_Lock_Alt_Aux1()
   #endif
#endif

/* ---------------------------------------------------------------
   Name: AT_Read_Test, PS2_Read_Test

   Read Test Inputs.

   C prototypes:
      WORD AT_Read_Test(void);
      WORD PS2_Read_Test(void);

   Returns: Byte value in lower portion of WORD returned.

            Bit   AT_Read_Test            PS2_Read_Test
            ---   ------------            -------------
            7-2   Reserved                Reserved
             0    Keyboard Clock Input    Keyboard Clock Input
             1    Keyboard Data Input     Auxiliary Clock Input
   --------------------------------------------------------------- */
#define AT_Read_Test()  ((Get_Aux_Port1_Dat_In() << 1) | Get_Aux_Port1_Clk_In())
#define PS2_Read_Test() ((Get_Aux_Port2_Clk_In() << 1) | Get_Aux_Port1_Clk_In())

/* ---------------------------------------------------------------
   Name: Start_Aux_Port1_Transmit,
         Start_Aux_Port2_Transmit,
         Start_Aux_Port3_Transmit

   Start transmission to auxiliary device.

   The device has been setup to receive the data by lowering
   its clock line and pausing to allow the device to recognize
   that it has been inhibited.

   C prototypes:
      void Start_Aux_Port1_Transmit(BYTE data);
      void Start_Aux_Port2_Transmit(BYTE data);
      void Start_Aux_Port3_Transmit(BYTE data);

   Input: data contains the data to be sent to the auxiliary device.
   --------------------------------------------------------------- */
 /* For hardware shift mechanism mode. */
 #define Start_Aux_Port1_Transmit(data) SET_BIT(PSIEN, PSIEN_SOTIE); \
            SET_BIT(PSCON, PSCON_XMT); PSDAT = data; Set_Aux_Port1_Clk()
 #define Start_Aux_Port2_Transmit(data) SET_BIT(PSIEN, PSIEN_SOTIE); \
            SET_BIT(PSCON, PSCON_XMT); PSDAT = data; Set_Aux_Port2_Clk()
 #define Start_Aux_Port3_Transmit(data) SET_BIT(PSIEN, PSIEN_SOTIE); \
            SET_BIT(PSCON, PSCON_XMT); PSDAT = data; Set_Aux_Port3_Clk()

/* ---------------------------------------------------------------
   Name: Setup_Aux_Port1_Receive,
         Setup_Aux_Port2_Receive,
         Setup_Aux_Port3_Receive

   Setup to receive from auxiliary ports.
   Setup to allow auxiliary devices to send.

   C prototypes:
      void Setup_Aux_Port1_Receive(void);
      void Setup_Aux_Port2_Receive(void);
      void Setup_Aux_Port3_Receive(void);
   --------------------------------------------------------------- */
 /* For hardware shift mechanism mode. */
 #define Setup_Aux_Port1_Receive() SET_BIT(PSIEN, PSIEN_SOTIE); \
                    CLEAR_BIT(PSCON, PSCON_XMT); Set_Aux_Port1_Clk()
 #define Setup_Aux_Port2_Receive() SET_BIT(PSIEN, PSIEN_SOTIE); \
                    CLEAR_BIT(PSCON, PSCON_XMT); Set_Aux_Port2_Clk()
 #define Setup_Aux_Port3_Receive() SET_BIT(PSIEN, PSIEN_SOTIE); \
                    CLEAR_BIT(PSCON, PSCON_XMT); Set_Aux_Port3_Clk()

/* ---------------------------------------------------------------
   Name: Make_Aux_Pntr

   Get a pointer to Aux_Port1, Aux_Port2, or Aux_Port3.

   Input: port - AUX_PORT1_CODE, AUX_PORT2_CODE, or AUX_PORT3_CODE

   C prototype:
      AUX_PORT * Make_Aux_Pntr(SMALL port);
   --------------------------------------------------------------- */
#define Make_Aux_Pntr(port) aux_pntr_table[port - AUX_PORT1_CODE]

#endif /* ifndef PS2_H */

