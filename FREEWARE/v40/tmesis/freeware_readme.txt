SSINT, Software, System Service Intercept src for Alpha VMS
.-----------------------------------------------------------------------------.
| Copyright © 1992, 1994, 1996 by Brian Schenkenberger.  ALL RIGHTS RESERVED. |
|                                                                             |
| This software is provided "AS IS" and is supplied for informational purpose |
| only.  No warranty is expressed or implied and no liability can be accepted |
| for any actions or circumstances incurred from the use of this software or  |
| from the information contained herein.  The author makes no claim as to the |
| suitablility or fitness of the software or information contain herein for a |
| particular purpose.                                                         |
|                                                                             |
| Permission is hereby granted *ONLY* for the "not-for-profit" redistribution |
| of this software provided that ALL SOURCE and/or OBJECT CODE remains intact |
| and ALL COPYRIGHT NOTICES remain intact from its original distribution.     |
|                                                                             |
|(!) NO TITLE TO AND/OR OWNERSHIP OF THIS SOFTWARE IS HEREBY TRANSFERRED. (!) |
`-----------------------------------------------------------------------------'

Package name:	SSINT (System Service INTercept)
Version ident:	V1.3%6.233	20-August-1996
Description:	Demonstration of interception of system service routines
		on OpenVMS Alpha via a loadable executive image(execlet).
Author's name:  Brian Schenkenberger, VAXman

Package Overview:
----------------
This package is comprised of a single loadable executive image(execlet)
which demonstrates a method for intercepting a system service's service
routine on OpenVMS Alpha.  For demonstration purposes, this incarnation
of SSINT intercepts two system services SYS$CREPRC and SYS$DELPRC.  For
each invocation of either of the intercepted system services, a message
is output on the system console device to display the PID of the process
invoking the service and the status of the call.

This execlet is unique in that, it does not declare its initialization
routines in the normal fashion using the INITIALIZATION_ROUTINE macro.
Normally, an initialization routine would be declared using this macro
which, in turn, builds the table of vectors of initialization routines.
This table is then used by the INI$DOINIT routine in module SYS$DOINIT
to call the execlet's initialization routines.  Unfortunately, this is
not an appropriate initialization senario in cases where an execlet is
required to perform some function prior to default functions performed 
as a consequence of the INI$DOINIT routine.
 
In this particular execlet, certain initialization functions need to be
performed before the INI$DOINIT routine invokes the INI$SYSTEM_SERVICE
procedure which, in turn, invokes the procedure EXE$CONNECT_SERVICES to 
define the replacement system service function.

To accomplish the requirements discussed above, a local routine called
SSINT$DOINIT, is established for this execlets transfer address; thus,
causing the execlet loader to invoke IT for the initialization routine
instead of the normal INI$DOINIT routine.

After this initialization code is invoked, the INI$DOINIT routine must
be invoked.  Unfortunately, this routine is not defined globally to be
called by another routine.  Therefore, by playing a little "game" with
the ordering of the initialization linkage, a symbol can be defined in
the EXEC$INIT_LINKAGE psect to establish the location of the Procedure
DeSCriptor (PDSC) of the INI$DOINIT routine.  This is accomplished via
the module SSDOINIHACK.MAR.

The initialization routine in this execlet is used to obtain the PDSC
address of the original service routine for the system service that is
being intercepted.  A change mode system service which changed mode is
defined by a bound procedure value procedure descriptor.  The address 
found at PDSC$Q_ENTRY in the system service's procedure descriptor is
the address of the system service transfer vector code.  For a change 
mode system service, this vector will contain the following code thread:

SYS$<SS-name>_C+00: LDL  R1,(SP)		; A03E0000
SYS$<SS-name>_C+04: BIS  SP,R31,R28		; 47DF041C
SYS$<SS-name>_C+08: LDA  R0,#X<chm-code>(R31)	; 201Fxxxx
SYS$<SS-name>_C+0C: CHMx			; 0000008x (x: 2=E/3=K) 

* Note: The format of the system service transfer vector under OpenVMS
	Alpha version prior to V6.1 differ from the above and therefore,
	this execlet will not function without making necessary changes.

The initialization code looks through the vector to determine the mode
of the system service and the assigned change mode code of the system
service.  After obtaining these values, the address of the change mode
dispatch vector is calculated and the service routine's PDSC address
is stored for transfer to later in the intercept (replacement) system 
service routine.  The dispatch table entry vector format is depicted 
below.

+-----+-----+-----+-----+
| DISP_A_SERVICE_ROUTINE|:CMOD$AR_<mode>_DISPATCH_VECTOR+10(16)*chm-code
+-----+-----+-----+-----+ 
|   DISP_A_ENTRY_POINT  |
+-----+-----+-----+-----+ 
|   <reserved>    |FLAGS|
+-----+-----+-----+-----+ 
|      <reserved>       |
+-----+-----+-----+-----+ 


The module SSINTLOADER.MAR can be used to dynamically load the execlet. 
This execlet could also be loaded during system initialization.  To do
this, define the execlet using the SYSMAN command:  SYS_LOADABLE ADD.  
An execlet must reside in SYS$LOADABLE_IMAGES to load it during system
bootstrapping.  After adding the execlet using SYSMAN, a DCL procedure
called VMS$SYSTEM_IMAGES.COM located in SYS$UPDATE must be executed.



Modification Information:	V1.3%6.233	20-August-1996
------------------------
This version of SSINT contains new code to support the interception of
64 bit system services and system services enhanced to permit passing
of 64 bit addresses.  This version defines the macro: $SETUP_CALLG_64
for creating a 64 bit argument list which can, subsequently, be passed
to the built-in EVAX_CALLG_64 to invoke the original service routine.
This 64 bit mechanism, available as of OpenVMS Alpha V7.0, may be used 
to intercept any change mode system service even though the service is
not 64 bit enhanced.

This version also changes the way in which the SSINI$DOINIT routine is
invoked as the primary initialization routine.  This version now elim-
inates the ugly "%LINK-W-MULTFR, multiply defined transfer address..." 
when linking SSINTERCEPT.EXE.

.END.
