.TITLE	    WCP_MSG
.FACILITY   WCP,1076/PREFIX=WCP__
.IDENT	    "V1.1"
!+
! DESCRIPTION:
!
!   This module contains message codes used by the WCP utility.
!
! AUTHOR:
!
!   M. Madison
!   COPYRIGHT © 1993, 1994  MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
!
! UPDATE HISTORY:
!
!   16-NOV-1989	V1.0	Madison	    Initial coding.
!   07-JAN-1991	V1.1	Madison	    Add NOPRIV.
!-
.SEVERITY   	ERROR
NOPRIV	    	<no privilege for attempted operation (!AS required)>/FAO=1

.SEVERITY   	WARNING
RECNOTFOUND 	<no record found to match "!AS">/FAO=1
IDERR	    	<error translating identifer "!AS">/FAO=1
UICERR	    	<error translating UIC "!AS">/FAO=1
CANTIND	    	<failed to redirect input stream>
NORDCFG	    	<could not read configuration file !AS>/FAO=1
NOWRTCFG    	<could not write configuration file !AS>/FAO=1
NOOPNOUT    	<could not open output file !AS>/FAO=1
NOCONTACT   	<unable to contact WATCHER process>

.SEVERITY   	INFORMATION
READCFG	    	<read configuration from file !AS>/FAO=1
WROTECFG    	<wrote configuration to file !AS>/FAO=1
VERSION	    	<WATCHER Control Program, version !AS>/FAO=1

.END
