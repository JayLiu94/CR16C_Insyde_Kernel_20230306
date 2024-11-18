/* ----------------------------------------------------------------------------
 * MODULE PURINI.C
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
 * Contains initialization functions used during a reset.
 * Based on PC87570 code version 2.00.07[2667-21]-n
 *
 * Revision History (Started March 15, 2000 by GLP)
 *
 * EDH
 * 11 Dec 00   Support the fourth PS/2 port as port 4. It will be
 *             recognized as port 0 with Active PS/2 Multiplexing
 *             driver.
 * 13 Jan 01   Supported the Intelligent mouse function.
 *             If no Wheel mouse connected then response stadard mouse exist.
 *             If Wheel mouse connected then response wheel mouse exist.
 * 18 Jan 01   Allow OEM enable/disable PS/2 port4 support in OEMBLD.MAK.
 * 25 Jan 05   Removed switch for PS/2 Port4 supported.
 *             Added a switch for OEM to enable/disable PS/2 Port2 supported.
 * 13 Mar 06   Added for Extended cmd E9 to Enable/disable Aux Port
 * ------------------------------------------------------------------------- */

#define PURINI_C

#include "swtchs.h"
#include "purhook.h"
#include "types.h"
#include "purdat.h"
#include "ini.h"
#include "reginit.h"
#include "purini.h"
#include "ps2.h"
#include "host_if.h"
#include "oem.h"
#include "dev.h"
#include "scan.h"
#include "i2c.h"
#include "timers.h"
#include "purscn.h"
#include "purdev.h"
#include "puracpi.h"
#include "purmain.h"
#include "purfunct.h"
#include "staticfg.h"
#include "purxlt.h"
#include "proc.h"
#include "raminit.h"
#include "purext.h"
#include "regs.h"
#include "com_defs.h"

#if RAM_BASED_FLASH_UPDATE
extern void FLASH_UPDATE_init(void);
#else
#include "fu.h"
#endif

/* --- Prototypes for local routines. --- */
/* Reminder: static gives the routine file scope.  It
   will not be exported or defined as a global routine. */
static void cold_reset(void);
static void warm_reset(void);
static void general_reset1(void);
static void general_reset2(void);
void Init_SXI(void);
void CalibrateADC();
void InitADC( BYTE ChNum1, BYTE ChNum2, BYTE ChNum3);
//void Sample_ADC(BYTE VC1SELIN, BYTE VC2SELIN, BYTE VC3SELIN);
void LFCG_set_default(BYTE default_val);


/* ---------------------------------------------------------------
   Name: Reset_Initialization

   Initialize the registers and data variables.
   --------------------------------------------------------------- */
void Reset_Initialization(void)
{

#if RUN_FROM_MRAM
    // disable the FSPI signals, by setting the SPI_TRIS bit in DEVCNT register
    SET_BIT(DEVCNT, SPI_TRIS);
#endif

   WKEN2 = 0;  /* The debugger sets some bits in this register so clear it. */

   // if JBB after flash update do not init regs
   if (FLASH_jbb != 0xAA55)
   {
       Init_Regs();

#if !INCLUDE_CRISIS

// init Flash Update functionality
#if RAM_BASED_FLASH_UPDATE
   FLASH_UPDATE_init(); // ram version
#else // ROM BASED FLASH UPDATE
    FLASH_UPDATE_init((unsigned long)&BOOT_BLOCK_ADDRESS, 0);
#endif // RAM/ROM_BASED_FLASH_UPDATE

#endif // !INCLUDE_CRISIS
   }

   Icu_Init();
   Timers_Init();
   Host_If_Init();

   Scan_Init();

   if (OEM_Warm_Reset_Check())
   {
      Gen_Hookc_Warm_Reset_Begin();
      warm_reset();
      Service.bit.UNLOCK = 1; /* Enable all devices on a restart by
                                 posting unlock service request. */
      Gen_Hookc_Warm_Reset_End();
   }
   else
   {
      Gen_Hookc_Cold_Reset_Begin();
      cold_reset();

      // if JBB after flash update - skip Gen_Hookc_Cold_Reset_End()
      if (FLASH_jbb != 0xAA55)
      {
          Gen_Hookc_Cold_Reset_End();
      }
   }

   /* <<< V5.1 2001/2/3 Generate SMI/SCI for H/W dedicate pins. */

#if SXI_SUPPORTED
   Init_SXI();
#endif

   /* >>> V5.1 2001/2/3 added */
}

/* Initialization when power goes on. */
void cold_reset(void)
{

   BITS_8 leds;

   /* Initialize variables from RAM_Init. */
   /* (Update Ext_Cb3.byte a little further down.) */
   Ext_Cb2.byte = Ext_Cb2_Init;
   /* >>> Vincent 2003/3/10 Add. */
   Keypro_Stat.byte = 0;
   /* <<< End Add. */
   Save_Typematic = Save_Typematic_Init;
   Save_Kbd_State =
      (SAVE_KBD_STATE_INIT & ~maskLEDS) | (Save_Led_State_Init & maskLEDS);
   Wakeup1.byte = Wakeup1_Init;
   Wakeup2.byte = Wakeup2_Init;
   Wakeup3.byte = Wakeup3_Init;
   Mouse_State[0] = Mouse_State0_Init;
   Mouse_State[1] = Mouse_State1_Init;
   Mouse_State[2] = Mouse_State2_Init;


   Mouse_Cmd = 0;

   /* Shut off local keyboard LEDs.
      Initialize Led_Ctrl and Led_Data variables.
      Initialize Ext_Cb3 from RAM_Init
      Initialize LEDs. */
   Led_Ctrl.all = LED_CTRL_INIT;
   Led_Data.all = 0;
   Ext_Dat8(&Hif_Var[HIF_PM1], 0x97, Ext_Cb3_Init, HIF_PM1);

   Ext_Cb0.byte = EXT_CB0_INIT;
   Ps2_Ctrl.byte = PS2_CTRL_INIT;

   Gen_Info.byte = 0;   /* Clear general control. */

   Int_Var.Ticker_10 = 10;
   Int_Var.Scan_Lock = 0;/* Scanner transmission unlocked. */

   Flag.PASS_READY = 0; /* Password not loaded. */
   Int_Var.Ticker_100 = 50;

   // if JBB after flash update - (FLASH_jbb == 0xAA55) - do not inhibit scan transmission
   if (FLASH_jbb != 0xAA55)
   {
        Flag.SCAN_INH = 1;   /* Scanner transmission inhibited. */
   }

   Flag.VALID_SCAN = 0;

   Flag.NEW_PRI_K = 0;
   Flag.NEW_PRI_M = 0;

   Flag.LED_ON = 1;

   Ccb42.byte = CCB42_INIT;

   Sys_Funct_Init();

   Pass_On = 0;      /* Password enable send code. */
   Pass_Off = 0;     /* Password disable send code. */
   Pass_Make1 = 0;   /* Reject make code 1. */
   Pass_Make2 = 0;   /* Reject make code 2. */

   Aux_Status.Send_Flag = 0;
   Aux_Port_Ticker = 0;

   Aux_Port1.retry = 0;
   Aux_Port1.delay = 0;
   Aux_Port1.lock = 0;
   Aux_Port1.kbd = 0;
   Aux_Port1.valid = 0;
   Aux_Port1.ack = 0;
   Aux_Port1.disable = 0;

   Aux_Port1.sequence = IDLE;
   Aux_Port1.state_byte0 = 0;
   Aux_Port1.state_byte1 = 0;
   Aux_Port1.state_byte2 = 0;

   #ifndef AUX_PORT2_SUPPORTED
   #error AUX_PORT2_SUPPORTED switch is not defined.
   #elif AUX_PORT2_SUPPORTED

   Aux_Port2.retry = 0;
   Aux_Port2.delay = 0;
   Aux_Port2.lock = 0;
   Aux_Port2.kbd = 0;
   Aux_Port2.valid = 0;
   Aux_Port2.ack = 0;
   Aux_Port2.disable = 0;
   Aux_Port2.sequence = IDLE;
   Aux_Port2.state_byte0 = 0;
   Aux_Port2.state_byte1 = 0;
   Aux_Port2.state_byte2 = 0;

   #endif

   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #elif AUX_PORT3_SUPPORTED

    Aux_Port3.retry = 0;
    Aux_Port3.delay = 0;
    Aux_Port3.lock = 0;
    Aux_Port3.kbd = 0;
    Aux_Port3.valid = 0;
    Aux_Port3.ack = 0;
    Aux_Port3.disable = 0;
    Aux_Port3.sequence = IDLE;
    Aux_Port3.state_byte0 = 0;
    Aux_Port3.state_byte1 = 0;
    Aux_Port3.state_byte2 = 0;

   #endif
   MULPX.byte = 0;

   general_reset1();
   Setup_Ext_Cbx();
   general_reset2();

   Write_Inhibit_Switch_Bit(1);  /* Disable inhibit switch. */
   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   if ((Ps2_Ctrl.bit.K_AUTO_SWITCH) || (Ps2_Ctrl.bit.M_AUTO_SWITCH))
   {  /*
      If the autoswitching feature of the primary keyboard is
      enabled, the primary keyboard can be on Aux_Port1 or 2.
      There can be a secondary keyboard on the other port.

      If the autoswitching feature of the primary mouse is enabled
      the primary mouse can be on Aux_Port1, 2 or 3 (if supported).
      There can be a secondary mouse on the other port(s).

      Auxiliary Port 1 can only have a mouse connected to it.
      Assume that there are two keyboards and initialize the
      sequence to check the type of the devices on port 1 and 2. */

      #if AUX_PORT2_SUPPORTED
      Aux_Port2.valid = 0;
      Aux_Port2.sequence = TYPE_S1;
      #endif
      #if AUX_PORT3_SUPPORTED
      Aux_Port3.valid = 0;
      Aux_Port3.sequence = TYPE_S1;
      #endif


      Service.bit.UNLOCK = 1; /* Post unlock service request. */
   }
   #endif
   Enable_Timer_A();       /* Load Timer A if not already enabled. */
   #if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
   Reset_Int_Mouse();
   #endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
}

/* The power was on and reset was generated. */
void warm_reset(void)
{
   //Scan_Init();

   OEM_Write_Leds(Led_Data);  /* Restore LEDS to original state. */

   general_reset1();
   Setup_Ext_Cbx();
   general_reset2();

   /* Configure inhibit switch.  (Restart password.) */
   if (Gen_Info.bit.PASS_ENABLE) Write_Inhibit_Switch_Bit(0);
   else Write_Inhibit_Switch_Bit(1);

   if (Ccb42.bit.SYS_FLAG) Write_System_Flag(0);
   else Write_System_Flag(1);

   Pass_Buff_Idx = 0;         /* Clear password buffer index. */
}

/* ---------------------------------------------------------------
   Name: Setup_Ext_Cbx

   Setup resources (registers and variables) that are
   dependent on the value of Ext_Cb0 (Extended Control Byte 0).
   --------------------------------------------------------------- */
void Setup_Ext_Cbx(void)
{
   #ifndef AUX_PORT3_SUPPORTED
   #error AUX_PORT3_SUPPORTED switch is not defined.
   #endif

   #ifndef AUX_PORT2_SUPPORTED
   #error AUX_PORT2_SUPPORTED switch is not defined.
   #endif

   #if AUX_PORT2_SUPPORTED
   if (Ext_Cb0.bit.EN_AUX_PORT2)
   {
      /* Enable device on Auxiliary Port 2. */

      Aux_Port2.valid = 1;       /* Set valid transmission flag. */
      Lock_Line(AUX_PORT2_CODE); /* Lock auxiliary device. */
      Set_Aux_Port2_Dat();       /* Setup transmission state. */
      Enable_Serial_Irq(AUX_PORT2_CODE);/* Enable auxiliary device interrupt. */
      Aux_Port2.disable = 0;      /* Mark  2006/3/13 added for Extended cmd E9 to Enable Aux Port*/
   }
   else
   {
      /* Disable device on Auxiliary Port 2. */

      Aux_Port2.valid = 0;       /* Clear valid transmission flag. */
      Lock_Line(AUX_PORT2_CODE); /* Lock auxiliary device. */
      Set_Aux_Port2_Dat();
      Disable_Serial_Irq(AUX_PORT2_CODE);/* Disable aux device interrupt. */
      Aux_Port2.disable = 1;      /* Mark 2006/3/13 added for Extended cmd E9 to disable Aux Port*/
   }
   #endif

   if (Ext_Cb0.bit.PS2_AT)
   {
      /* Auxiliary device (mouse) is supported. */

      if (Ext_Cb0.bit.EN_AUX_PORT1)
      {
         /* Enable device on Auxiliary Port 1. */

         Aux_Port1.valid = 1;       /* Set valid transmission flag. */
         Lock_Line(AUX_PORT1_CODE); /* Lock auxiliary device. */
         Set_Aux_Port1_Dat();       /* Setup transmission state. */
         Enable_Serial_Irq(AUX_PORT1_CODE);/* Enable aux device interrupt. */
         Aux_Port1.disable = 0;      /* Mark 2006/3/13 added for Extended cmd E9 to enable Aux Port*/
      }
      else
      {
         /* Disable device on Auxiliary Port 1. */

         Aux_Port1.valid = 0;       /* Clear valid transmission flag. */
         Lock_Line(AUX_PORT1_CODE); /* Lock auxiliary device. */
         Set_Aux_Port1_Dat();
         Disable_Serial_Irq(AUX_PORT1_CODE);/* Disable aux device interrupt. */
         Aux_Port1.disable = 1;      /* Mark 2006/3/13 added for Extended cmd E9 to disable Aux Port*/
      }

      #if AUX_PORT3_SUPPORTED
      if (Ext_Cb0.bit.EN_AUX_PORT3)
      {
         /* Enable device on Auxiliary Port 3. */

         Aux_Port3.valid = 1;       /* Set valid transmission flag. */
         Lock_Line(AUX_PORT3_CODE); /* Lock auxiliary device. */
         Set_Aux_Port3_Dat();       /* Setup transmission state. */
         Enable_Serial_Irq(AUX_PORT3_CODE);/* Enable aux device interrupt. */
         Aux_Port3.disable = 0;      /* Mark 2006/3/13 added for Extended cmd E9 to enable Aux Port*/

      }
      else
      {
         /* Disable device on Auxiliary Port 3. */

         Aux_Port3.valid = 0;       /* Clear valid transmission flag. */
         Lock_Line(AUX_PORT3_CODE); /* Lock auxiliary device. */
         Set_Aux_Port3_Dat();
         Disable_Serial_Irq(AUX_PORT3_CODE);/* Disable aux device interrupt. */
         Aux_Port3.disable = 1;      /* Mark 2006/3/13 added for Extended cmd E9 to disable Aux Port*/
      }
      #endif

      /* Reflect 42 control byte because this is a PS/2. */
      Ccb42.bit.DISAB_AUX = 1;
      Ccb42.bit.INTR_AUX = 0;
   }
   else
   {
      /* Disable auxiliary device (mouse) support. */

      Ext_Cb0.bit.EN_AUX_PORT1 = 0;
      Aux_Port1.valid = 0;

      #if AUX_PORT3_SUPPORTED
       Ext_Cb0.bit.EN_AUX_PORT3 = 0;
       Aux_Port3.valid = 0;
      #endif
   }

   /* Setup Fast Gate A20 and Fast Reset options. */
   if (Ext_Cb3.bit.ENABLE_HRDW)
   {
      Enable_Fast_A20();
      Enable_Fast_Reset();
   }
   else
   {
      Disable_Fast_A20();
      Disable_Fast_Reset();
   }
}

/* ---------------------------------------------------------------
   Name: general_reset1, general_reset2

   Initialize things common to both Cold/Warm reset.

   --------------------------------------------------------------- */

static void general_reset1(void)
{
    SMALL index;

    Service.word = 0;          /* Clear service requests. */

    Gen_Timer = 0;             /* Clear general timer. */

    Timer_A.byte = 0;          /* Clear Timer A events. */
    Timer_A3.byte = 0;
    Timer_A4.byte = 0;
    Timer_A5.byte = 0;
    Timer_B.byte = 0;          /* Clear Timer B events. */
    Scanner_State.byte = 0;    /* Clear scanner state. */
    for (index = 0; index < HIF_CHNLS; index++)
    {
        Hif_Var[index].Cmd_Byte = 0;              /* Clear pending commands. */
        HIF_Response[index].byte = 0;     /* Clear response codes. */
    }
    /* Use Save_HIF to initialize the saved values
       of HIF_Response.byte and Cmd_Byte.  This is done
       after HIF_Response.byte and Cmd_Byte are initialized. */

    Init_Kbd();                /* Initialize internal (scanned) keyboard. */


    Flag.ENTER_LOW_POWER = 0;
    Int_Var.Low_Power_Mode = 0;
    Int_Var.Active_Aux_Port = 0;  /* No active auxiliary port. */

    #ifndef I2C_SUPPORTED
    #error I2C_SUPPORTED switch is not defined.
    #elif I2C_SUPPORTED
    for (index = 0; index < SMB_CHNLS; index++)
    {
       I2C_Init(index);
    }
    #if HID_OV_I2C_SUPPORTED
    HID_Init();
    HID_I2C_Init();
    #endif // HID_OV_I2C_SUPPORTED

    #endif

    #ifndef ACPI_SUPPORTED
    #error ACPI_SUPPORTED switch is not defined.
    #elif ACPI_SUPPORTED
     ACPI_Init();
    #endif

    for (index = 0; index < CAUSE_ARRAY_SIZE; index++)
    {
        /* Clear SCI and SMI cause flags. */
        Cause_Flags[index] = 0;
    }

    /* Clear buffered SCI and SMI causes. */
    SCI_Event_In_Index = 0;
    SCI_Event_Out_Index = 0;
    SMI_Event_In_Index = 0;
    SMI_Event_Out_Index = 0;
}

static void general_reset2(void)
{
   /* If internal keyboard (scanner) is enabled,
      then enable Any Key Interrupt. */
   Enable_Any_Key_Irq();

   Enable_Host_IBF_Irq();  /* Enable primary PC interface interrupt. */
   Enable_Host_IBF2_Irq(); /* Enable secondary PC interface interrupt. */
   Enable_Host_IBF3_Irq(); /* Enable secondary PC interface interrupt. */
}

#if SXI_SUPPORTED
void Init_SXI(void)
{
   SET_BIT(HIPM1IE, HSMIE | HSCIE);
   SET_BIT(HIIRQC, HIIRQC_IRQ11B);
   SET_BIT(HIPM1IE, IESMIE);	/* Enabl SMI output enable. */
   CLEAR_BIT(HIPM1IE, IESCIE);		/* Disable SCI output enable. */
   switch ((Cfg0E >> shiftSMI_OP_SELECT) & maskSMI_OP_SELECT)
   {
      case SXI_SET_HI:
      case SXI_PULSE_HI:
         /* SMI normal low and active high or high pulse. */
         CLEAR_BIT(HIPM1IC, SMIPOL);
         break;
      case SXI_SET_LO:
      case SXI_PULSE_LO:
         /* SMI normal high and active low or low pulse. */
         SET_BIT(HIPM1IC, SMIPOL);
         break;
   }

   CLEAR_BIT(HIPM1IE, IESMIE);	/* Disable SMI output enable. */
   SET_BIT(HIPM1IE, IESCIE);		/* Enable SCI output enable. */
   switch ((Cfg0E >> shiftSCI_OP_SELECT) & maskSCI_OP_SELECT)
   {
      case SXI_SET_HI:
      case SXI_PULSE_HI:
         /* SCI normal low and active high or high pulse. */
         CLEAR_BIT(HIPM1CTL, SCIPOL);
         break;

      case SXI_SET_LO:
      case SXI_PULSE_LO:
         /* SCI normal high and active low or low pulse. */
         SET_BIT(HIPM1CTL, SCIPOL);
         break;
   }
   CLEAR_BIT(HIPM1IE, IESCIE);
}
#endif

#if AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
void Reset_Int_Mouse (void)
{
   /* Initialize the Intelligent mouse control byte. */
   Int_Mouse.word = 0;
   Aux_Port1.intm   = 0; // Set port 1 device as standard mouse.
   Aux_Port1.intseq = 0; // Clear the port 1 Intelligent mouse command sequence done flag.
   Aux_Port1.intm5 = 0;

   #if AUX_PORT2_SUPPORTED
   Aux_Port2.intm   = 0; // Set port2 device as standard mouse.
   Aux_Port2.intseq = 0; // Clear the port2 Intelligent mouse command sequence done flag.
   Aux_Port2.intm5 = 0;
   #endif

   #if AUX_PORT3_SUPPORTED
   Aux_Port3.intm   = 0; // Set Port3 device as standard mouse.
   Aux_Port3.intseq = 0; // Clear the port3 Intelligent mouse command sequence done flag.
   Aux_Port3.intm5 = 0;
   #endif   /* AUX_PORT3_SUPPORTED */
}
#endif //AUX_PORT2_SUPPORTED || AUX_PORT3_SUPPORTED
