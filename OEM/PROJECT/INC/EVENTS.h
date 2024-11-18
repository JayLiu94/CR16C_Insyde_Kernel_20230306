/*------------------------------------------------------------------------------
 * Copyright (c) 2004-2010 by Nuvoton Electronics Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     EVENTS.H - OEM Events Handling
 *
 * Project:
 *     Driver Set for Nuvoton Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/
#ifndef EVENTS_H
#define EVENTS_H

/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/


void EventsInit(void);
void Handle_Events(void);
void ProcEventActive(BYTE data, FLAG state);
void Send_OEM_Key(BYTE key, BYTE Event);


#endif /* ifndef BUTTONS_H */
