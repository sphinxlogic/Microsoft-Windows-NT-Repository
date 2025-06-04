
			*** COPYRIGHT NOTICE ***

    Copyright (C) 1995  James Shen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.



  **********************************************************************
                       		TO RUN SHS
  **********************************************************************

     SHS Requires GROUP privilege to access the process(es) not owned 
     by you but within the same group.  SHS requires WORLD privilege to 
     access any process(es) outside your own group.  It runs on both 
     VAX and Alpha.


     To create SHS image:
	
	o If your system has a MMS, and a DEC FORTRAN compiler V6.2 or 
          above installed, you can simply type MMS at command line after 
          you unzip the package. (at least on DEC FORTRAN V6.1, it will 
          crash when it tries to compile one of the recursive routines :-(, 
          so I attached the object files for both VAX and Alpha) 

	    $ MMS

	o If your system has only a DEC FORTRAN compiler V6.2 or above
          installed, after you unzip the package, you can enter:
	    
	    $ @BUILD.COM

			   *** NOTE ***

     The image SHS.EXE built by following the above procedure is linked 
     against the system shareable image library.  This is upward 
     compatible _ONLY_.  Which means the image SHS.EXE will run on any 
     other VMS system within the same architecture without any recompiling 
     and relinking as long as the version of the VMS is same or higher 
     than the version on which SHS.EXE was built.  Otherwise, you may 
     receive the message - "image identification mismatch".

     To build the SHS.EXE image, which can be executed on the system  
     having an older version of run time shareable images, you need to 
     link the object files against the default system object library - 
     STARLET.OlB.  Edit the build.com to suit your desire.  If you use 
     MMS, enter:
	
		$ MMS/MACRO=(__NOSHR__=1)
		
			 *** END OF NOTE ***

     You may use SHS by either defining it as a real command (via the 
     SET COMMAND command), or by defining it as a foreign command (via 
     DCLsymbol :== $<wherever>SHS).

     To install SHS system wide, you need to follow this procedure 
     (assuming you have the proper privileges): 

	o Move the SHS.EXE image to proper location, with FILE PROTECTION
	  W:RE.

	o Modify the SHS.CLD file VERB defintion section to point 
	  the image to the location where it resides.
	  For example:
	 	IMAGE SYS$SYSTEM:SHS.EXE

	o Install the NEW command definition table (with assumption
          users' dcltable is SYS$LIBRARY:DCLTABLES.EXE
 
	  $ SET COMMAND /TABLE=SYS$COMMON:[SYSLIB]DCLTABLES -
	  _$ /OUTPUT=SYS$COMMON:[SYSLIB]DCLTABLES DEV:[DIR]SHS.CLD

	  $ INSTALL REPLACE/LOG/SHARE/HEADER_RESIDENT/OPEN -
	  _$ SYS$COMMON:[SYSLIB]DCLTABLES.EXE

	o Install the SHS help file to the system help library.

	  $ LIBRARY/INSERT/HELP SYS$HELP:HELPLIB.HLB DEV:[DIR]SHS

	  OR:
   
	  $ DEFINE/SYSTEM HLP$LIBRARY_# SHS DEV:[DIR]SHS.HLB
	   
	    - Where # needs to be a consecutive number, the first
              one will be HLP$LIBRARY, second will be HLP$LIBRARY_1,
	      third one will be HLP$LIBRARY_2, and so on.  You 
	      need to put the above command in your SYS$MANAGER:
	      SYLOGICALS.COM if you want this help be available
              to all users everytime the system reboots.

     To use SHS process wide, you can follow this procedure. If you 
     want this setting available every time you login to the system, 
     you may put these commands in your LOGIN.COM.

	o After you have built the SHS image as stated above,
          you need to setup your process command table by:
	  
	  $ SET COMMAND DEV:[DIR]SHS.CLD  
	
	  But before you do that, if you want to use default supplied 
	  SHS.CLD file, then you need to move SHS.EXE to your SYS$LOGIN 
	  directory, otherwise, you need to modify the verb definition 
	  section in the SHS.CLD file to point to the location of the 
	  image by changing this:

		IMAGE device:[directory]SHS.EXE

	o Setup the SHS help facility.  You need to put SHS.HLB in
	  your SYS$LOGIN: directory in order for the program to 
          work right.

	o To use the SHS help, you may enter:

	  $ HELP/LIBRARY=SYS$LOGIN:SHS.HLB SHS

	  OR:

	  $ DEFINE HLP$LIBRARY_# SYS$LOGIN:SHS.HLB

	    - Where # needs to be a consecutive number, the first
              one will be HLP$LIBRARY, second will be HLP$LIBRARY_1,
	      third one will be HLP$LIBRARY_2, and so on.  You 
	      need to put the above command in your LOGIN.COM,
	      if you want this help be available everytime you login.

     To use SHS as a foreign command, You need to define a DCL symbol 
     to point to the image.  (With OpenVMS 6.2, you may use DCL$PATH
     to implement automatic foreign command from DCL without defining
     a DCL symbol).

     You may choose to define the DCL symbol system wide by putting
     it in SYS$MANAGER:SYLOGIN.COM, or in process wide by putting
     it in SYS$LOGIN:LOGIN.COM, or even in group wide if you have
     some mechanism to distinguish which group the user belongs to in
     SYS$MANAGER:SYLOGIN.COM and define the logical name HLP$LIBRARY_# 
     in group wide.

James Shen (jshen1@ford.com)
Powertrain Operation
Ford Motor Company
1981 Front Wheel Drive,
Batavia, OH 45103
