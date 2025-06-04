$!
$! @(#)vmswrap.com	6.2 98/08/25
$!
$! Web browser handling wrapper command procedure for OpenVMS.
$! Currently supports the Netscape and Mosaic browsers.  See
$! the INSTALL.VMS file for details.
$! OpenVMS versions: 6.0 and later, tested with 7.1 on VAX(tm) machines
$!
$!    xmcd  - Motif(tm) CD Audio Player
$!    cda   - Command-line CD Audio Player
$!    libdi - CD Audio Player Device Interface Library
$!
$!    Copyright (C) 1993-1998  Ti Kan
$!    E-mail: ti@amb.org
$!    Contributing author: Michael Monscheuer
$!    E-mail: Monscheuer@T-Online.de
$!
$!    This program is free software; you can redistribute it and/or modify
$!    it under the terms of the GNU General Public License as published by
$!    the Free Software Foundation; either version 2 of the License, or
$!    (at your option) any later version.
$!
$!    This program is distributed in the hope that it will be useful,
$!    but WITHOUT ANY WARRANTY; without even the implied warranty of
$!    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
$!    GNU General Public License for more details.
$!
$!    You should have received a copy of the GNU General Public License
$!    along with this program; if not, write to the Free Software
$!    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
$!
$!
$! No way without the "Change Mode Kernel" privilege, CMKRNL is needed for SDA 
$! to trace a process' channels (in this case display and mailbox devices). If 
$! user doesn't have CMKRNL set, check whether it can be set. If so, set it or
$! if not, exit reporting an error to xmcd.
$!
$	IF .NOT. F$PRIVILEGE("CMKRNL")
$	 THEN
$	  PRIVS = F$GETJPI("","AUTHPRIV")
$	  IF F$LOCATE("CMKRNL",PRIVS) .EQ. F$LENGTH(PRIVS) 
$	   THEN
$	    IF F$LOCATE("SETPRV",PRIVS) .EQ. F$LENGTH(PRIVS) 
$	     THEN
$	      EXIT
$	     ELSE
$	      SET PROCESS/PRIVILEGE=SETPRV
$	      SET PROCESS/PRIVILEGE=CMKRNL
$	    ENDIF
$	   ELSE
$	    SET PROCESS/PRIVILEGE=CMKRNL
$	  ENDIF
$	ENDIF
$!
$! Define a default browser if not already done (in user's LOGIN.COM).
$! Mosaic was chosen as the default browser here because it's
$! remarkably faster than Netscape with executing subsequently
$! issued remote control commands.
$!
$	IF F$TRNLNM("XMCD_BROWSER") .EQS. "" 
$	 THEN 
$	  XMCD_BROWSER = "MOSAIC"
$	 ELSE
$	  XMCD_BROWSER = F$TRNLNM("XMCD_BROWSER") 
$	ENDIF
$!
$! Branch if not running in detached mode
$!
$	IF F$MODE() .NES. "OTHER"
$	 THEN
$!
$! Branch if trying to control browser remotely and initialize flag
$!
$	  IF P2 .NES. ""
$	   THEN
$	    BROWSER_REMOTE = 0
$!
$! Check whether Mosaic perhaps is already running in remote node ...
$!
$	    IF F$TRNLNM("MOSAIC_MAILBOX") .NES. ""
$	     THEN
$	      IF F$GETDVI("''F$TRNLNM("MOSAIC_MAILBOX")'","EXISTS")
$	       THEN
$	        BROWSER_REMOTE = 1
$		MBA = F$TRNLNM("MOSAIC_MAILBOX")
$!
$! ... and if so branch for a quick execution (if MOSAIC still is XMCD_BROWSER, 
$! if not, do not branch and reset flag), the rest was already checked.
$!
$		IF XMCD_BROWSER .EQS. "MOSAIC" THEN GOTO GOT_STATUS
$	        BROWSER_REMOTE = 0
$	      ENDIF
$	    ENDIF
$!
$! Some definitions first
$!
$	    VVER = F$EDIT(F$GETSYI("VERSION"),"COLLAPSE")
$	    I_AM = F$EDIT(F$GETJPI("","UIC"),"COLLAPSE")
$	    NODENAME = F$GETSYI("NODENAME")
$	    MESENV = F$ENVIRONMENT("MESSAGE")
$!
$! Just in case somebody tries to execute this procedure from an unsuitable
$! terminal...
$!
$	    DEFINE/NOLOG SYS$OUTPUT NLA0:
$	    SHOW DISPLAY/SYMBOL
$	    DSPSTAT = $SEVERITY 
$	    DEASSIGN SYS$OUTPUT
$	    IF .NOT. DSPSTAT THEN EXIT
$	    DSPLNODE = DECW$DISPLAY_NODE
$!
$! Now collect information on running processes (necessary to get hold
$! of display and mailbox devices "owned" by user running this procedure)
$!
$	    OPEN/WRITE 0 PID_LIS.TMP
$	    CTX = ""
$ GET_PROCS:
$	    PID = F$PID(CTX)
$	    IF PID .EQS. "" THEN GOTO END_PROCS
$	    OWNER = F$EDIT(F$GETJPI(PID,"UIC"),"COLLAPSE")
$	    IF OWNER .NES. "''I_AM'" THEN GOTO GET_PROCS
$	    IF F$GETJPI(PID,"NODENAME") .NES. "''NODENAME'" THEN GOTO GET_PROCS
$	    IMAGE = F$GETJPI(PID,"IMAGNAME")
$	    IMAGE = F$PARSE(IMAGE,,,"NAME")+F$PARSE(IMAGE,,,"TYPE")
$	    IF IMAGE .NES. "''XMCD_BROWSER'.EXE" THEN GOTO GET_PROCS
$!
$! Found browser running on local node "owned" by user running this procedure
$!
$	    WRITE 0 PID
$	    GOTO GET_PROCS
$ END_PROCS:
$	    CLOSE 0
$	    IF F$FILE_ATTRIBUTES("PID_LIS.TMP","EOF") .EQ. 0
$	     THEN
$!
$! No browser found running on this node
$!
$	      DELETE/NOCONFIRM PID_LIS.TMP;*
$	      GOTO GOT_STATUS
$	    ENDIF
$!
$! Now collect channel info on all processes found running a browser 
$!
$	    OPEN/WRITE 1 GET_MBX.COM
$	    WRITE 1 "$ DEFINE SYS$OUTPUT GET_MBX.DAT"
$	    WRITE 1 "$ ANALYZE/SYSTEM"
$	    OPEN/READ 0 PID_LIS.TMP
$ GET_IMAGE:
$	    READ/ERROR=END_IMAGE 0 PID
$	    WRITE 1 "SET OUTPUT ''PID'.SDA_OUT"
$	    WRITE 1 "SHOW PROCESS/CHANNEL/ID=''PID'"
$	    GOTO GET_IMAGE
$ END_IMAGE:
$	    WRITE 1 "EXIT"
$	    WRITE 1 "$ DEASSIGN SYS$OUTPUT"
$	    CLOSE 0
$	    CLOSE 1
$	    @GET_MBX
$ GET_DEVICE:
$	    SEARCH_FILE = F$SEARCH("*.SDA_OUT;*")
$	    IF XMCD_BROWSER .EQS. "MOSAIC"
$!
$! Check SDA process channel info for MBA (mosaic mailbox) devices
$! (only in case Mosaic is the browser)
$!
$	     THEN
$	      IF SEARCH_FILE .EQS. "" THEN GOTO END_DEVICE
$	      IF VVER .GES. "V6.2"
$	       THEN
$	        SEARCH/OUTPUT=FND.TMP/NOWARNINGS 'SEARCH_FILE' MBA
$	        SEARCH_SEVER = $SEVERITY
$	       ELSE
$	        SET MESSAGE/NOTEXT/NOFACILITY/NOIDENTIFICATION/NOSEVERITY
$	        SEARCH/OUTPUT=FND.TMP 'SEARCH_FILE' MBA
$	        SEARCH_SEVER = $SEVERITY
$	        SET MESSAGE 'MESENV'
$	      ENDIF
$	      IF SEARCH_SEVER .NE. 1
$	       THEN
$	        DELETE/NOCONFIRM 'SEARCH_FILE',FND.TMP;*
$!
$! Nothing appropriate found
$!
$	        GOTO GET_DEVICE
$	      ENDIF
$	    ENDIF
$!
$! Check SDA process channel info for MBA and/or WSA (display) devices
$!
$	    IF VVER .GES. "V6.2"
$	     THEN
$	      SEARCH/OUTPUT=FND.TMP/NOWARN 'SEARCH_FILE' MBA,WSA
$	     ELSE
$	      SET MESSAGE/NOTEXT/NOFACILITY/NOIDENTIFICATION/NOSEVERITY
$	      SEARCH/OUTPUT=FND.TMP 'SEARCH_FILE' MBA,WSA
$	      SET MESSAGE 'MESENV'
$	    ENDIF
$!
$! Now exactly determine the MBA (if any) and WSA device names found
$!
$	    OPEN/READ 2 FND.TMP
$ SDA_READ:
$	    READ/ERROR=END_SDA 2 STR
$	    IF F$LOCATE("WSA",STR) .NE. F$LENGTH(STR) THEN WSA = F$ELEMENT(3," ",F$EDIT(STR,"COMPRESS"))
$	    IF F$LOCATE("MBA",STR) .NE. F$LENGTH(STR) THEN MBA = F$ELEMENT(4," ",F$EDIT(STR,"COMPRESS"))
$	    GOTO SDA_READ
$ END_SDA:
$	    CLOSE 2
$	    DELETE/NOCONFIRM 'SEARCH_FILE',FND.TMP;*
$!
$! Check whether the display device found matches the current display ...
$!
$	    DEFINE/NOLOG SYS$OUTPUT NLA0:
$	    SHOW DISPLAY/SYMBOL 'WSA'
$	    DEASSIGN SYS$OUTPUT
$!
$! ... if it doesn't, check next one found
$!
$	    IF DSPLNODE .NES. DECW$DISPLAY_NODE THEN GOTO GET_DEVICE
$!
$! Found a display owned by current user where a browser is running on
$! in remote mode
$!
$	    BROWSER_REMOTE = 1
$ END_DEVICE:
$	    DELETE/NOCONFIRM PID_LIS.TMP;*,GET_MBX.*;*
$ GOT_STATUS:
$	    IF BROWSER_REMOTE 
$	     THEN 
$!
$! Now setup browser specific command syntax and issue remote control command
$!
$	      IF XMCD_BROWSER .EQS. "MOSAIC"
$	       THEN
$	        URL = F$ELEMENT(1,"(",P2) - ")"
$	        OPEN/WRITE BROWSER 'MBA'
$	        WRITE BROWSER "goto|''URL'"
$	        CLOSE BROWSER
$	        DEFINE/NOLOG/JOB MOSAIC_MAILBOX "''MBA'"
$	      ENDIF
$	      IF XMCD_BROWSER .EQS. "NETSCAPE"
$	       THEN
$		MCR NETSCAPE 'P1' 'P2'
$	      ENDIF
$!
$! Report success to xmcd
$!
$	      EXIT "%X10000000"
$	     ELSE
$!
$! Report failure to xmcd (no browser found controlabel remotely,
$! implies BrowserDirect command in common.cfg)
$!
$	      EXIT
$	    ENDIF
$	  ENDIF
$!
$! Prepare to fire up browser (BrowserDirect command)
$! Determine display environment and save it as well as the requested URL
$! to a file
$! 
$	  DEFINE SYS$OUTPUT NLA0:
$	  SHOW DISPLAY/SYMBOL
$	  DEASSIGN SYS$OUTPUT
$	  OPEN/WRITE 0 SYS$LOGIN:XMCD_BROWSER.PAR
$	  WRITE 0 "''P1'"
$	  WRITE 0 "''DECW$DISPLAY_NODE'"
$	  WRITE 0 "''DECW$DISPLAY_TRANSPORT'"
$	  CLOSE 0
$!
$! Now fire up this procedure as a detached process (so that xmcd does
$! not need to wait for the browser to terminate)
$!
$	  SET MESSAGE/NOTEXT/NOFACILITY/NOIDENTIFICATION/NOSEVERITY
$	  RUN/DETACHED SYS$SYSTEM:LOGINOUT-
             /INPUT='F$ENVI("PROCEDURE")'/AUTHORIZE-
             /OUTPUT=NLA0:/ERROR=NLA0:/PRIO=4
$         IF $SEVERITY .EQ. 1 THEN EXIT %X10000000
$	  EXIT
$	ENDIF
$!
$! Running in detached mode now
$! First read the info necessary for firing up the browser (see the
$! last but one section above)
$!
$	SET DEFAULT SYS$LOGIN
$	OPEN/READ 0 XMCD_BROWSER.PAR
$	READ 0 URL
$	READ 0 NODE
$	READ 0 TRANS
$	CLOSE 0
$	DELETE/NOCONFIRM  XMCD_BROWSER.PAR;*
$!
$! Now create the display, determine the browser and setup the command
$! it is expected to execute ...
$!
$	SET DISPLAY/CREATE/NODE='NODE'/TRANSPORT='TRANS'/EXECUTIVE
$	BROWSER :== $SYS$SYSTEM:'XMCD_BROWSER'
$	IF XMCD_BROWSER .EQS. "MOSAIC" THEN COMMAND = "/REMOTE/MAILBOX=XMCD ''URL'"
$	IF XMCD_BROWSER .EQS. "NETSCAPE" THEN COMMAND = "''URL'"
$!
$! ... and finally fire up the browser
$!
$	BROWSER 'COMMAND'
$	

