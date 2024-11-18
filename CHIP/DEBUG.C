/*-----------------------------------------------------------------------------
 * MODULE DEBUG.C
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
 * Revision History (Started July 2, 1996 by GLP)
 *
 * GLP
 * 09 Apr 98   Added includes for string.h and unistd.h.
 * 19 Aug 99   Updated copyright.
 * ------------------------------------------------------------------------- */

#define DEBUG_C

#include "swtchs.h"

#ifndef MESSAGES
#error MESSAGES switch is not defined.
#endif

#if MESSAGES

#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"

int errno;
char buff[30];

/* ---------------------------------------------------------------
   Name: Message

   To print a message on a debugging terminal.
   The format is a string which can have one parameter
   (as in the printf() function) to be printed as an int.
   --------------------------------------------------------------- */
void Message(char *format, int var)
{
   sprintf(buff, format, var);
   write(1, buff, strlen(buff));
}

#else
/* If this debugging file is not being used, then there is no
   need to take up code or RAM space.  But, errors are generated
   if this file is empty.  The following takes care of two errors.
   1) ANSI C does not allow an empty source file.
   2) If the linker definition file puts the VIO library in a specific
      section and no messages are used, then libvio.a is not used.
      It does not appear as an input file and the linker generates
      an error.
      
   This is an attempt at keeping the debugging code and RAM size
   small while allowing the debugging code to be added easily. */
#include <unistd.h>
int errno;
void Nothing(void);
void Nothing(void)
{
   lseek(1, 1, 1);
}
#endif
