@Echo off
REM *-----------------------------------------------------------------------------
REM * MODULE BUID.BAT
REM *
REM * Copyright(c) 1998-2010, Insyde Software Corporation. All Rights Reserved.
REM *
REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
REM * transmit, broadcast, present, recite, release, license or otherwise exploit
REM * any part of this publication in any form, by any means, without the prior
REM * written permission of Insyde Software Corporation.
REM *
REM *-----------------------------------------------------------------------------



IF EXIST C:\PROGRA~2\CR164.1 (
	SET CRDIR=C:\PROGRA~2\CR164.1
) ELSE (
	SET CRDIR=C:\PROGRA~1\CR164.1
)

SET SIZ=0x000000
SET SIZ1=0x000000
SET SIZ2=0x000000
SET SRT=0x000000

if     "%1" == "?"  goto ShowHelp
if /i  "%1" == "clean" goto No1
if NOT "%1" == "/?" goto NoHelp

:ShowHelp

:instruct
Echo BUILD [CLEANINC | CLEAN] [ALL] [/?]
Echo    CLEANINC : To clean up the INC directory.
Echo       CLEAN : To clean up the OBJ directories.
Echo         ALL : Build anything that is required.
Echo          /? : Show this help.
Echo.

goto exit

:No1
Rem The E macro is used to workaround a bug.
Rem The last macro passed to nmaker did not work correctly.
rem copy bind\*.* obj\.
..\..\TOOLS\nmake /B /R TMP=%TEMP% NSC=%CRDIR% E=1 /F oembld.mak %1 %2 %3 %4 %5 %6 %7 %8 %9

goto exit
:NoHelp

Rem The E macro is used to workaround a bug.
Rem The last macro passed to nmaker did not work correctly.
rem copy bind\*.* obj\.
..\..\TOOLS\nmake /B /R TMP=%TEMP% NSC=%CRDIR% E=1 /F oembld.mak %1 %2 %3 %4 %5 %6 %7 %8 %9
:exit
