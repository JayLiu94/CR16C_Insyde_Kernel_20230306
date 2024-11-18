/*------------------------------------------------------------------------------
 * Copyright (c) 2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     sib.c - SuperI/O Internal Bus (SIB) driver
 *
 * Project:
 *     Driver Set for Nuvoton Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

// ******************************************************************************
// *                                                                            *
// * Public Prototypes                                                          *
// *                                                                            *
// * Name       Description                                                     *
// * ------------------ ------------------------------------------------------- *
// * SIB_set_lock   lock Host-controlled module.                                *
// * SIB_clear_lock un-lock Host-controlled module.                             *
// * SIB_get_violation  check lock Host-controlled module                       *
// * SIB_read_reg   read Host-controlled device register value.                 *
// * SIB_write_reg  write data to Host-controlled device register.              *
// *                                                                            *
// * Requirements/Functional specifications references:                         *
// *                                                                            *
// * Development History                                                        *
// *                                                                            *
// * Data   Version     Description                                             *
// * ---------- --------------- ----------------------------------------------- *
// * 2007/01/24 0.87        First release                                       *
// *                                                                            *
// ******************************************************************************

/* include global parameters */
#include "types.h"
#include "sib.h"
#include "regs.h"

// ******************************************************************************
// * Function Name : SIB_set_lock                                               *
// *                                                                            *
// * Purpose : This function locks Host access to Host-Controlled modules       *
// *        /device in order to enable Core access to this device.              *
// *                                                                            *
// * Algorithm : send lock command to Host controlled module.                   *
// *                                                                            *
// *    Argument List -                                                         *
// *                                                                            *
// *    Name        I/O TYPE    Description                                     *
// *    --------------- ------- ------- --------------------------------------  *
// *    device      I   ushort  selects the device to lock host access          *
// *                                                                            *
// *    Return value -                                                          *
// *                                                                            *
// ******************************************************************************
void SIB_set_lock(SIB_device_t device) {

    /* local variables and initialize */
    WORD host_locks;

    host_locks = LKSIOHA;   // read lock host access register
    host_locks |= device;   // add lock for 'device'
    LKSIOHA = host_locks;   // write back lock host access register
}

// ******************************************************************************
// * Function Name : SIB_clear_lock                                             *
// *                                                                            *
// * Purpose : This function clears the lock of Host access to Host-Controlled  *
// *        modules/device.                                                     *
// *                                                                            *
// * Algorithm : send un-lock command to Host controlled module.                *
// *                                                                            *
// *    Argument List -                                                         *
// *                                                                            *
// *    Name        I/O TYPE    Description                                     *
// *    --------------- ------- ------- --------------------------------------  *
// *    device      I   ushort  selects the device to clear lock of             *
// *                    host access                                             *
// *                                                                            *
// *    Return value -                                                          *
// *                                                                            *
// ******************************************************************************
void SIB_clear_lock(SIB_device_t device) {

    /* local variables and initialize */
    WORD host_locks;

    host_locks = LKSIOHA;   // read lock host access register
    host_locks &= ~device;  // remove lock for 'device'
    LKSIOHA = host_locks;   // write back lock host access register
}

// ******************************************************************************
// * Function Name : SIB_get_violation                                          *
// *                                                                            *
// * Purpose : This function returns the Host access violations for Host-       *
// *        Controlled modules with Host lock access.                           *
// *                                                                            *
// * Algorithm : check lock device name.                                        *
// *                                                                            *
// *    Argument List -                                                         *
// *                                                                            *
// *    Name        I/O TYPE    Description                                     *
// *    --------------- ------- ------- --------------------------------------  *
// *                                                                            *
// *    Return value -                                                          *
// *        ushort    - access lock violation register                          *
// *                                                                            *
// ******************************************************************************
WORD SIB_get_violation()  {
    return SIOLV;       // return access lock violation register
}

// ******************************************************************************
// * Function Name : SIB_read_reg                                               *
// *                                                                            *
// * Purpose : This function implements core access to Host-Controlled modules  *
// *        reading protocol.                                                   *
// *                                                                            *
// * Algorithm : read Host-Controlled module value.                             *
// *                                                                            *
// *    Argument List -                                                         *
// *                                                                            *
// *    Name        I/O TYPE    Description                                     *
// *    --------------- ------- ------- --------------------------------------  *
// *    device      I   ushort  selects the device the core requests to         *
// *                    access                                                  *
// *    offset      I   uchar   offset of the wanted register in the            *
// *                    above device                                            *
// *    value       O   uchar   pointer where the read data should be           *
// *                    stored                                                  *
// *                                                                            *
// *    Return value -                                                          *
// *                                                                            *
// ******************************************************************************

void SIB_read_reg(SIB_device_t device,BYTE offset,BYTE* value) {

    SET_BIT(SIBCTRL, CSAE);         // set SIBCTRL.CSAE
    while (IS_BIT_SET(SIBCTRL, CSRD));  // verify that SIBCTRL.CSRD and SIBCTRL.CSWR are cleared
    while (IS_BIT_SET(SIBCTRL, CSWR));
    CRSMAE = (WORD)device;        // select the module to be accessed (CRSMAE register)
    SET_FIELD(IHIOA, OFFSET, offset);   // specify the offset of the register in the module (IHIOA register)
    SET_BIT(SIBCTRL, CSRD);         // write 1 to SIBCTRL.CSRD
    while (IS_BIT_SET(SIBCTRL, CSRD));  // read SIBCTRL.CSRD until it 0
    (*value) = IHD;             // read the data from IHD register
}

// ******************************************************************************
// * Function Name : SIB_write_reg                                              *
// *                                                                            *
// * Purpose : This function implements core access to Host-Controlled modules  *
// *        writing protocol.                                                   *
// *                                                                            *
// * Algorithm : check lock device name.                                        *
// *                                                                            *
// *    Argument List -                                                         *
// *                                                                            *
// *    Name        I/O TYPE    Description                                     *
// *    --------------- ------- ------- --------------------------------------  *
// *    device      I   ushort  selects the device the core requests to         *
// *                    access                                                  *
// *    offset      I   uchar   offset of the wanted register in the            *
// *                    above device                                            *
// *    value       I   uchar   data to be written                              *
// *                                                                            *
// *    Return value -                                                          *
// *                                                                            *
// ******************************************************************************
void SIB_write_reg(SIB_device_t device,BYTE offset,BYTE value) {

    SET_BIT(SIBCTRL, CSAE);             // set SIBCTRL.CSAE
    while (IS_BIT_SET(SIBCTRL, CSRD));  // verify that SIBCTRL.CSRD and SIBCTRL.CSWR are cleared
    while (IS_BIT_SET(SIBCTRL, CSWR));
    CRSMAE = (WORD)device;              // select the module to be accessed (CRSMAE register)
    SET_FIELD(IHIOA, OFFSET, offset);   // specify the offset of the register in the module (IHIOA register)
    IHD = value;                        // write the data to IHD register (this starts the write operation to the device)
    while (IS_BIT_SET(SIBCTRL, CSWR));  // read SIBCTRL.CSWR until it 0 (completion of write)
}


