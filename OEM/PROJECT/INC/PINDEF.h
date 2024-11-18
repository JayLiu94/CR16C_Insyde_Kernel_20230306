/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     pindef.h - OEM pins definition
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/
#ifndef PINDEF_H
#define PINDEF_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#define ACTIVATE    1
#define INACTIVE    0

/*------------------------------------------------------------------------------
 * Macro:
 *    PORT_PIN
 *
 * Description:
 *    This macro created a 8 bit Port pin combination as follows:
 *    Bits 7 through 4 are used for the port code.
 *    Bit  3 is cleared.
 *    Bits 2 through 0 are used for the bit number.
 *
 * Parameters:
 *    port     - Gpio port number (example for gpio 32 the
 *               port is 3 and the pin is 2)
 *    pin      - the gpio pin number.
 * Return value:
 *    the Port Pin combination byte
 * ------------------------------------------------------------------------------*/
#define PORT_PIN(port, pin) ((((port) & 0x0F) << 4) + ((pin) & 0x07))

#define HIGH   0x01
#define LOW    0x00
/*------------------------------------------------------------------------------
 * Macro:
 *    READ_GPIO
 *
 * Description:
 *    This macro read the gpio pin define by port and pin
 *
 * Parameters:
 *    port     - Gpio port number (example for gpio 32 the
 *               port is 3 and the pin is 2)
 *    pin      - the gpio pin number.
 * Return value:
 *    0 : gpio is low
 *    1 : gpio is high
 * ------------------------------------------------------------------------------*/
#define READ_GPIO(port, pin)          ((Extended_Read_Port((port) & 0x0F, 0) >> ((pin) & 7)) & 1)

/*------------------------------------------------------------------------------
 * Macro:
 *    WRITE_GPIO
 *
 * Description:
 *    This macro set the gpio define by port, pin to the specified data
 *
 * Parameters:
 *    port     - Gpio port number (example for gpio 32 the
 *               port is 3 and the pin is 2)
 *    pin      - the gpio pin number.
 *    data     - 0 : set gpio to low
 *               1 : set gpio to high
 * Return value:
 *    None.
 * ------------------------------------------------------------------------------*/
#define WRITE_GPIO(port, pin, data)   Extended_Write_Port((((port) & 0x0F) << 4)| (((data) & 1) << 3)| ((pin) & 7), 0)

/*------------------------------------------------------------------------------
 * Macro:
 *    SET_PIN
 *
 * Description:
 *    This macro set the gpio define by port_pin according to the data
 *
 * Parameters:
 *    port_pin - Defines the gpio using the PORT_PIN macro
 *    data     - 0 : set gpio to low
 *               1 : set gpio to high
 * Return value:
 *    None.
 * ------------------------------------------------------------------------------*/
#define SET_PIN(port_pin, data)       Extended_Write_Port((port_pin) | ((data) << 3) , 0)

/*------------------------------------------------------------------------------
 * Macro:
 *    READ_INP_PIN
 *
 * Description:
 *    This macro read the gpio pin input register define by port_pin
 *    Use this maco when the pin is configured as input.
 *
 * Parameters:
 *    port_pin - Defines the gpio using the PORT_PIN macro
 * Return value:
 *    0 : gpio is low
 *    1 : gpio is high
 * ------------------------------------------------------------------------------*/
#define READ_INP_PIN(port_pin)           ((Extended_Read_Port(((port_pin) >> 4) & 0x0F, 0) >> ((port_pin) & 0x07)) & 0x01)

/*------------------------------------------------------------------------------
 * Macro:
 *    READ_OUTP_PIN
 *
 * Description:
 *    This macro read the gpio pin output register define by port_pin
 *    Use this maco when the pin is configured as output.
 *
 * Parameters:
 *    port_pin - Defines the gpio using the PORT_PIN macro
 * Return value:
 *    0 : gpio is low
 *    1 : gpio is high
 * ------------------------------------------------------------------------------*/
#define READ_OUTP_PIN(port_pin)           ((Extended_Read_Port(((port_pin) >> 4) & 0x0F, 1) >> ((port_pin) & 0x07)) & 0x01)

/*
 * Define input and output pins for your platform
 */
#define TEMP_ALERT      1  // need to define the appropriate pin - (PORT_PIN(a, b))
#define EC_PWRSW        2  // need to define the appropriate pin - (PORT_PIN(a, b))
#define AC_IN           3  // need to define the appropriate pin - (PORT_PIN(a, b))
#define LIDSW           4  // need to define the appropriate pin - (PORT_PIN(a, b))
#define PM_SLP_S4       5  // need to define the appropriate pin - (PORT_PIN(a, b))
#define PM_SLP_S3       6  // need to define the appropriate pin - (PORT_PIN(a, b))
#define WIRELESS_EN     7  // need to define the appropriate pin - (PORT_PIN(a, b))
#define SW_AUD_VD       8  // need to define the appropriate pin - (PORT_PIN(a, b))
#define SW_AUD_MUTE     9  // need to define the appropriate pin - (PORT_PIN(a, b))
#define SW_AUD_VU       10 // need to define the appropriate pin - (PORT_PIN(a, b))
#define SW_AUD_STOP     11 // need to define the appropriate pin - (PORT_PIN(a, b))
#define SW_AUD_PAUSE    12 // need to define the appropriate pin - (PORT_PIN(a, b))
#define BAT_IN          13 // need to define the appropriate pin - (PORT_PIN(a, b))
#define INTERNET_KEY    14 // need to define the appropriate pin - (PORT_PIN(a, b))
#define RF_KILL         15 // need to define the appropriate pin - (PORT_PIN(a, b))

#define IRQ_EC_HID          PORT_PIN(7,6)
#define Set_GPIO76_High()   P7DOUT |=  0x40
#define Set_GPIO76_Low()    P7DOUT &= ~0x40

#define GPIO76_High         ((P7DIN & MASK_bit6) ? 1 : 0)

#define Set_IRQ_EC_HID_Free()   Set_GPIO76_High()
#define Set_IRQ_EC_HID_Active() Set_GPIO76_Low()
#define IRQ_EC_HID_Free         GPIO76_High


#define IRQ_EC_NTC          PORT_PIN(8,4)
#define Set_GPIO84_High()   P8DOUT |=  0x10
#define Set_GPIO84_Low()    P8DOUT &= ~0x10
#define GPIO84_High         ((P8DIN & MASK_bit4) ? 1 : 0)

#define Set_IRQ_EC_NTC_Free()   Set_GPIO84_High()
#define Set_IRQ_EC_NTC_Active() Set_GPIO84_Low()
#define IRQ_EC_NTC_Free         GPIO84_High


#endif  /* ifndef PINDEF_H */
