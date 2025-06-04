$!
$! @(#)instweb.com	6.5 98/08/25
$!
$! Xmcd wwwWarp installation command procedure for OpenVMS.
$! OpenVMS versions: 6.0 and later, tested with 7.1 on VAX(tm) machines
$!
$!	xmcd  - Motif(tm) CD Audio Player
$!	libdi - CD Audio Player Device Interface Library
$!	cddb  - CD Database Management Library
$!
$!   Copyright (C) 1993-1998  Ti Kan
$!   E-mail: ti@amb.org
$!   Contributing author: Michael Monscheuer
$!   Email: Monscheuer@T-Online.de
$!
$!   This program is free software; you can redistribute it and/or modify
$!   it under the terms of the GNU General Public License as published by
$!   the Free Software Foundation; either version 2 of the License, or
$!   (at your option) any later version.
$!
$!   This program is distributed in the hope that it will be useful,
$!   but WITHOUT ANY WARRANTY; without even the implied warranty of
$!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
$!   GNU General Public License for more details.
$!
$!   You should have received a copy of the GNU General Public License
$!   along with this program; if not, write to the Free Software
$!   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
$!
$!
$! Define some basics first and check whether we are in the right directory.
$! If so, check whether COMMON.CFG appears to be xmcd's. If it doesn't, exit.
$!
$	USRNAM = F$EDIT(F$GETJPI("","USERNAME"),"COLLAPSE")
$	DEFENV = F$ENVIRONMENT("DEFAULT")
$	VVER = F$EDIT(F$GETSYI("VERSION"),"COLLAPSE")
$	IF F$VERIFY()
$	 THEN
$	  ESC = "<ESC>"
$	 ELSE
$	  ESC[0,8] = 27
$	ENDIF
$	IF F$SEARCH("COMMON.CFG") .EQS. ""
$	 THEN
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  Xmcd's file COMMON.CFG is not in the directory you are running this procedure" 
$	  WRITE SYS$OUTPUT "  from. SET DEFAULT to the directory COMMON.CFG is resident in, copy or"
$	  WRITE SYS$OUTPUT "  rename this procedure to this directory if it's not there and start this"
$	  WRITE SYS$OUTPUT "  procedure again."
$	  WRITE SYS$OUTPUT ""
$	  EXIT
$	 ELSE
$	  VSCOM = ""
$	  IF VVER .GES. "V6.2" 
$	   THEN 
$	    VSCOM = "/NOWARNING" 
$	   ELSE
$	    MESENV = F$ENVIRONMENT("MESSAGE")
$	    SET MESSAGE/NOTEXT/NOSEVERITY/NOFACILITY/NOIDENTIFICATION 
$	  ENDIF
$	  SEARCH/NOOUT'VSCOM' COMMON.CFG "XMCD"
$	  COMCOR = $SEVERITY
$	  IF VVER .LTS. "V6.2" THEN SET MESSAGE 'MESENV'
$	  IF COMCOR .NE. 1 
$	   THEN
$	    WRITE SYS$OUTPUT ""
$	    WRITE SYS$OUTPUT "  The file COMMON.CFG found in this directory does not appear to be"
$	    WRITE SYS$OUTPUT "  xmcd's COMMON.CFG"
$	    WRITE SYS$OUTPUT "  Check whether the directory you started this procedure from really is xmcd's"
$	    WRITE SYS$OUTPUT "  home directory. If it is, you need to get a new copy of xmcd's common.cfg"
$	    WRITE SYS$OUTPUT ""
$	  EXIT
$	  ENDIF
$	ENDIF
$!
$! Create a list for things which may need attention in case the procedure
$! does not complete successfully. Then check if the user's privileges are
$! sufficient for browser control from inside xmcd. If they aren't, notify 
$! user and continue as this is not of vital evidence for setting up www
$! access and browser control.
$!
$	OPEN/WRITE LIST SYS$LOGIN:TO_BE_DONE.LIST 
$
$	IF .NOT. F$PRIVILEGE("CMKRNL")
$	 THEN
$	  PRIV = F$GETJPI("","AUTHPRIV")
$	  IF F$LOCATE("CMKRNL",PRIV) .EQ. F$LENGTH(PRIV) 
$	   THEN
$	    IF F$LOCATE("SETPRV",PRIV) .EQ. F$LENGTH(PRIV)
$	     THEN
$	      WRITE LIST ""
$	      WRITE LIST "  You don't seem to have the CMKRNL (Change Mode Kernel) privilege which"
$	      WRITE LIST "  is needed during the process of controlling a browser remotely from"
$	      WRITE LIST "  inside xmcd."
$	      WRITE LIST "  Add the CMKRNL privilege to the list of your account's authorized"
$	      WRITE LIST "  priviliges using the following commands:"
$	      WRITE LIST ""
$	      WRITE LIST "  $ SET DEF SYS$SYSTEM:"
$	      WRITE LIST "  $ MCR AUTHORIZE"
$	      WRITE LIST "  MOD ''USRNAM'/PRIV=CMKRNL"
$	      WRITE LIST "  EXIT"
$	      WRITE LIST ""
$	      WRITE LIST "  If you aren't allowed to run AUTHORIZE, see your system manager"
$	      WRITE LIST "  and ask him to add the CMKRNL privilege to your account's authorized"
$	      WRITE LIST "  privileges." 
$	      WRITE LIST ""
$	      WRITE LIST "  Lacking CMKRNL does not affect the insatllation procedure but it will"
$	      WRITE LIST "  affect XMCD at run-time, i.e. XMCD will fail in trying to start or"
$	      WRITE LIST "  control a browser."
$	      WRITE LIST ""
$	      CLOSE LIST
$	      TYPE SYS$LOGIN:TO_BE_DONE.LIST
$	      INQUIRE/NOPUNCTATION DUMMY "''ESC'[28C''ESC'[7mPress Return to continue''ESC'[0m"
$	      OPEN/APPEND LIST SYS$LOGIN:TO_BE_DONE.LIST 
$	      WRITE SYS$OUTPUT ""
$	    ENDIF
$	  ENDIF
$	ENDIF
$!
$! Now check for browsers available on this system
$!	
$	NETSCAPE_FOUND = 0
$	MOSAIC_FOUND   = 0
$	HAVE_NETSCAPE  = 0
$	HAVE_MOSAIC    = 0
$	DEF_BROW = 1
$	IF F$SEARCH("SYS$SYSTEM:MOSAIC.EXE") .NES. "" THEN MOSAIC_FOUND = 1
$	IF MOSAIC_FOUND .EQ. 1 THEN HAVE_MOSAIC = 1
$	IF F$SEARCH("SYS$SYSTEM:NETSCAPE.EXE") .NES. "" THEN NETSCAPE_FOUND = 1
$	IF NETSCAPE_FOUND .EQ. 1 THEN HAVE_NETSCAPE = 1
$	IF MOSAIC_FOUND .AND. NETSCAPE_FOUND THEN GOTO DETERMINE_BROWSERS
$	WRITE SYS$OUTPUT ""
$!
$! No(t all) browser(s) found where expected, ask user for browsers
$!
$	IF .NOT. MOSAIC_FOUND THEN INQUIRE HAVE_MOSAIC "  Do you have MOSAIC installed on your system ? [Y/(N)]" 
$	IF .NOT. NETSCAPE_FOUND THEN INQUIRE HAVE_NETSCAPE "  Do you have NETSCAPE installed on your system ? [Y/(N)]" 
$	HAVE_MOSAIC = F$EDIT(HAVE_MOSAIC,"UPCASE,COLLAPSE")
$	HAVE_NETSCAPE = F$EDIT(HAVE_NETSCAPE,"UPCASE,COLLAPSE")
$	IF HAVE_MOSAIC .EQS. "" .OR. HAVE_MOSAIC .EQS. "N" THEN HAVE_MOSAIC = 0
$	IF HAVE_NETSCAPE .EQS. "" .OR. HAVE_NETSCAPE .EQS. "N" THEN HAVE_NETSCAPE = 0
$	IF HAVE_MOSAIC THEN HAVE_MOSAIC = 1
$	IF HAVE_NETSCAPE THEN HAVE_NETSCAPE = 1
$	SUM = HAVE_MOSAIC + HAVE_NETSCAPE
$	IF SUM .EQ. 0
$	 THEN
$!
$! Really no browser available!!!
$!
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  You indicated that you have neither MOSAIC nor NETSCAPE installed on"
$	  WRITE SYS$OUTPUT "  your system. Please get a copy of at least one browser first and start"
$	  WRITE SYS$OUTPUT "  this procedure again then."
$	  WRITE SYS$OUTPUT ""
$	  WRITE LIST ""
$	  WRITE LIST "  You indicated that you have neither MOSAIC nor NETSCAPE installed on"
$	  WRITE LIST "  your system. Please get a copy of at least one browser first and start"
$	  WRITE LIST "  this procedure again then."
$	  WRITE LIST ""
$	  GOTO DONE
$	ENDIF
$	WRITE SYS$OUTPUT ""
$!
$! Check user's privs for write access to SYS$SYSTEM
$!
$	IF F$LOCATE("SYSPRV",PRIV) .NE. F$LENGTH(PRIV) 
$	 THEN 
$	  SET PROC/PRIV=SYSPRV
$	  GOTO COPY
$	ENDIF
$	IF F$LOCATE("BYPASS",PRIV)  .NE. F$LENGTH(PRIV)
$	 THEN 
$	  SET PROC/PRIV=BYPASS
$	  GOTO COPY
$	ENDIF
$	IF F$LOCATE("SETPRV",PRIV)  .NE. F$LENGTH(PRIV)
$	 THEN 
$	  SET PROC/PRIV=SETPRV
$	  SET PROC/PRIV=SYSPRV
$	  GOTO COPY
$	ENDIF
$	IF F$GETJPI("","GRP") .EQ. 1 THEN GOTO COPY
$!	 
$! User isn't allowed to write to SYS$SYSTEM
$!	 
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "  Appearently you don't have the right to place anything into SYS$SYSTEM."
$	WRITE SYS$OUTPUT "  As the browser executables you specified above are not resident in "
$	WRITE SYS$OUTPUT "  SYS$SYSTEM, please see your system manager and ask him to place a"
$	WRITE SYS$OUTPUT "  copy of the browsers in SYS$SYSTEM. The file names need to be"
$	WRITE SYS$OUTPUT "  MOSAIC.EXE and/or NETSCAPE.EXE respectively."
$	WRITE SYS$OUTPUT ""
$	WRITE LIST ""
$	WRITE LIST "  Appearently you don't have the right to place anything into SYS$SYSTEM."
$	WRITE LIST "  As the browser executables you specified above are not resident in "
$	WRITE LIST "  SYS$SYSTEM, please see your system manager and ask him to place a"
$	WRITE LIST "  copy of the browsers in SYS$SYSTEM. The file names need to be"
$	WRITE LIST "  MOSAIC.EXE and/or NETSCAPE.EXE respectively."
$	WRITE LIST ""
$	INQUIRE/NOPUNCTATION DUMMY "''ESC'[28C''ESC'[7mPress Return to continue''ESC'[0m"
$	WRITE SYS$OUTPUT ""
$	GOTO NOCOPY
$ COPY:
$!
$! Now try to copy the stuff indicated by user over to SYS$SYSTEM... 
$!
$	IF HAVE_MOSAIC .AND. MOSAIC_FOUND .EQ. 0
$	 THEN
$	  WRITE SYS$OUTPUT "  Please enter the current location (DISK:[DIR...]filename.ext) of your MOSAIC"
$	  WRITE SYS$OUTPUT "  executable now."
$	  WRITE SYS$OUTPUT "  "
$	  INQUIRE MOSLOC   "  The MOSAIC executable is"
$	  WRITE SYS$OUTPUT "  "
$	  IF MOSLOC .EQS. "" .OR. F$SEARCH("''MOSLOC'") .EQS. "" 
$	   THEN
$	    WRITE SYS$OUTPUT "  "
$	    WRITE SYS$OUTPUT "  ''MOSLOC' is NOT"
$	    WRITE SYS$OUTPUT "  the correct location or filename. Please copy your MOSAIC executable to"
$	    WRITE SYS$OUTPUT "  SYS$COMMON:[SYSEXE]MOSAIC.EXE manually after completion of this procedure"
$	    WRITE SYS$OUTPUT "  "
$	    WRITE LIST "  "
$	    WRITE LIST "  ''MOSLOC' is NOT"
$	    WRITE LIST "  the correct location or filename. Please copy your MOSAIC executable to"
$	    WRITE LIST "  SYS$COMMON:[SYSEXE]MOSAIC.EXE manually so that xmcd can make use of it."
$	    WRITE LIST "  "
$	    INQUIRE/NOPUNCTATION DUMMY "''ESC'[28C''ESC'[7mPress Return to continue''ESC'[0m"
$	   ELSE
$	    WRITE SYS$OUTPUT "  "
$	    WRITE SYS$OUTPUT "  Copying file to SYS$COMMON:[SYSEXE]MOSAIC.EXE"
$	    WRITE SYS$OUTPUT "  "
$	    COPY 'MOSLOC' SYS$COMMON:[SYSEXE]MOSAIC.EXE/PROT=(S:RWED,O:RWED,G:RWE,W:RE)
$	  ENDIF
$	ENDIF
$	IF HAVE_NETSCAPE .AND. NETSCAPE_FOUND .EQ. 0
$	 THEN
$	  WRITE SYS$OUTPUT "  Please enter the current location (DISK:[DIR...]filename.ext) of your NETSCAPE"
$	  WRITE SYS$OUTPUT "  executable now."
$	  WRITE SYS$OUTPUT "  "
$	  INQUIRE NSCLOC   "  The NETSCAPE executable is"
$	  WRITE SYS$OUTPUT "  "
$	  IF NSCLOC .EQS. "" .OR. F$SEARCH("''NSCLOC'") .EQS. "" 
$	   THEN
$	    WRITE SYS$OUTPUT "  "
$	    WRITE SYS$OUTPUT "  ''NSCLOC' is NOT"
$	    WRITE SYS$OUTPUT "  the correct location or filename. Please copy your NETSCAPE executable to"
$	    WRITE SYS$OUTPUT "  SYS$COMMON:[SYSEXE]NETSCAPE.EXE manually after completion of this procedure."
$	    WRITE SYS$OUTPUT "  "
$	    WRITE LIST "  "
$	    WRITE LIST "  ''NSCLOC' is NOT"
$	    WRITE LIST "  the correct location or filename. Please copy your NETSCAPE executable to"
$	    WRITE LIST "  SYS$COMMON:[SYSEXE]NETSCAPE.EXE manually so that xmcd can make use of it."
$	    WRITE LIST "  "
$	    INQUIRE/NOPUNCTATION DUMMY "''ESC'[28C''ESC'[7mPress Return to continue''ESC'[0m"
$	   ELSE
$	    WRITE SYS$OUTPUT "  "
$	    WRITE SYS$OUTPUT "  Copying file to SYS$COMMON:[SYSEXE]MOSAIC.EXE"
$	    WRITE SYS$OUTPUT "  "
$	    COPY 'NSCLOC' SYS$COMMON:[SYSEXE]NETSCAPE.EXE/PROT=(S:RWED,O:RWED,G:RWE,W:RE)
$	  ENDIF
$	ENDIF
$	IF .NOT. HAVE_NETSCAPE
$	 THEN
$	  BROWSERS = 1
$	  DEF_BROW = 1 
$	  GOTO NOCOPY
$	ENDIF
$	IF .NOT. HAVE_MOSAIC
$	 THEN
$	  BROWSERS = 2
$	  DEF_BROW = 2 
$	  GOTO NOCOPY
$	ENDIF
$ DETERMINE_BROWSERS:
$!
$! Now determine all browsers to be used as well as the user's favourite one. 
$!	
$	WRITE SYS$OUTPUT "''ESC'[2J''ESC'[0;0H"
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "  Please determine the www browser(s) you intend to use with xmcd now."
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Mosaic only          1"
$	WRITE SYS$OUTPUT "    Netscape only        2"
$	WRITE SYS$OUTPUT "    Mosaic and Netscape  3"
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Choice [1,2,(3)]
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "  Please determine the www browser you want to be your default browser for"
$	WRITE SYS$OUTPUT "  use with xmcd:"
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Mosaic     1"
$	WRITE SYS$OUTPUT "    Netscape   2"
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Choice [(1),2]
$ BROWSERS:
$	INQUIRE BROWSERS "''ESC'[9;0H    Choice [1,2,(3)]
$	IF BROWSERS .EQS. "" THEN BROWSERS = 3
$	IF F$INTEGER(BROWSERS) .LT. 1 .OR. F$INTEGER(BROWSERS) .GT. 3 THEN GOTO BROWSERS
$	
$ DEF_BROW:
$	INQUIRE DEF_BROW "''ESC'[18;0H    Choice [(1),2]
$	IF DEF_BROW .EQS. "" THEN DEF_BROW = 1
$	IF F$INTEGER(DEF_BROW) .LT. 1 .OR. F$INTEGER(DEF_BROW) .GT. 2 THEN GOTO DEF_BROW
$	
$ NOCOPY:
$	
$	IF DEF_BROW .EQ. 1 THEN DEF_BROW = "MOSAIC"
$	IF DEF_BROW .EQ. 2 THEN DEF_BROW = "NETSCAPE"
$!	
$! Determine which modifications have to be applied to user's LOGIN.COM ...
$!	
$	SET DEF SYS$LOGIN
$	VSCOM = ""
$	IF VVER .GES. "V6.2" 
$	 THEN 
$	  VSCOM = "/NOWARNING" 
$	 ELSE
$	  MESENV = F$ENVIRONMENT("MESSAGE")
$	  SET MESSAGE/NOTEXT/NOSEVERITY/NOFACILITY/NOIDENTIFICATION 
$	ENDIF
$	IF F$SEARCH("LOGIN.COM") .EQS. ""
$	 THEN
$	  OPEN/WRITE LOFI LOGIN.COM
$	  CLOSE LOFI
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  Obviously you don't have SYS$LOGIN:LOGIN.COM executed as the default"
$	  WRITE SYS$OUTPUT "  procedure when loging into the system."
$	  WRITE SYS$OUTPUT "  Please include LOGIN.COM being now created into your default login"
$	  WRITE SYS$OUTPUT "  procedure after completion of this procedure."
$	  WRITE SYS$OUTPUT ""
$	  WRITE LIST ""
$	  WRITE LIST "  Obviously you don't have SYS$LOGIN:LOGIN.COM executed as the default"
$	  WRITE LIST "  procedure when loging into the system."
$	  WRITE LIST "  Please include LOGIN.COM created by the setup procedure into your "
$	  WRITE LIST "  default login procedure so that the comnmands can take effect."
$	  WRITE LIST ""
$	ENDIF
$	SEARCH/NOOUT'VSCOM' LOGIN.COM "IF F$MODE() .NES. """OTHER""" THEN DEFINE/GROUP XMCD_BROWSER ''DEF_BROW'"
$	BRO_SEVER = $SEVERITY
$	SEARCH/NOOUT'VSCOM'/MATCH=AND LOGIN.COM "VMSWRAP","@",":="
$	WWW_SEVER = $SEVERITY
$	IF VVER .LTS. "V6.2" THEN SET MESSAGE 'MESENV'
$	IF BRO_SEVER .EQ. 1 .AND. WWW_SEVER .EQ. 1 THEN GOTO MOD_COMMON
$	
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "  Next you will automatically enter your LOGIN.COM."
$	WRITE SYS$OUTPUT "  Then move your cursor to the line where you want the set up commands to be."
$	WRITE SYS$OUTPUT "  Simply press the keys ""PF1"" and ""I"" to insert the line(s) needed."
$	WRITE SYS$OUTPUT "  To exit press ""PF1"" and ""E"" then."
$	WRITE SYS$OUTPUT ""
$	INQUIRE/NOPUNCTATION DUMMY "''ESC'[28C''ESC'[7mPress Return to continue''ESC'[0m"
$!
$! ... prepare the commands needed...
$!
$	OPEN/WRITE 115 LGI.TXT
$	IF BRO_SEVER .NE. 1 
$	 THEN 
$	  WRITE 115 "$	IF F$MODE() .NES. ""OTHER"""
$	  WRITE 115 "$	 THEN"
$	  WRITE 115 "$	  IF F$PRIVILEGE(""GRPPRV"")" 
$	  WRITE 115 "$	   THEN"
$	  WRITE 115 "$	    DEFINE/GROUP XMCD_BROWSER ''DEF_BROW'"
$	  WRITE 115 "$	   ELSE"
$	  WRITE 115 "$	    PRIVSA = F$GETJPI("""",""AUTHPRIV"")"
$	  WRITE 115 "$	    IF F$LOCATE(""GRPPRV"",PRIVSA) .NE. F$LENGTH(PRIVSA)"
$	  WRITE 115 "$	     THEN"
$	  WRITE 115 "$	      SET PROCESS/PRIVILEGE=GRPPRV"
$	  WRITE 115 "$	      DEFINE/GROUP XMCD_BROWSER ''DEF_BROW'"
$	  WRITE 115 "$	      SET PROCESS/PRIVILEGE=NOGRPPRV"
$	  WRITE 115 "$	     ELSE"
$	  WRITE 115 "$	      DEFINE/JOB XMCD_BROWSER ''DEF_BROW'"
$	  WRITE 115 "$	      WRITE SYS$OUTPUT """""
$	  WRITE 115 "$	      WRITE SYS$OUTPUT ""  You don't have the GRPPRV privilege."""
$	  WRITE 115 "$	      WRITE SYS$OUTPUT ""  Changing your default xmcd browser from a text window is not possible."""
$	  WRITE 115 "$	      WRITE SYS$OUTPUT """""
$	  WRITE 115 "$	    ENDIF"
$	  WRITE 115 "$	  ENDIF"
$	  WRITE 115 "$	ENDIF"
$	ENDIF
$	IF WWW_SEVER .NE. 1 THEN WRITE 115 "$	VMSWRAP :== @''DEFENV'VMSWRAP.COM
$	CLOSE 115
$	OPEN/WRITE 116 E.E
$	WRITE 116 "SET TEXT END ""[EOF]"""
$	WRITE 116 "SET MODE CHANGE"
$	WRITE 116 "DEFINE KEY GOLD E AS 'EXT EXIT.'"
$	WRITE 116 "DEFINE KEY GOLD I AS ""EXT INCLUDE LGI.TXT."""
$	CLOSE 116
$!
$! ... and apply the changes.
$!
$	DEFINE/USER/NOLOG SYS$INPUT SYS$COMMAND
$	EDITTT/EDT/COMMAND=E.E LOGIN.COM
$	DELETE/NOCONFIRM LGI.TXT;*,E.E;*
$	
$ MOD_COMMON:
$!	
$! Set up BrowserRemote and BrowserDirect command in COMMON.CFG
$! First check if user has write access.
$!	
$	SET DEF 'DEFENV'
$	WRITE SYS$OUTPUT ""
$	COMCFG_PRO = F$FILE_ATTRIBUTES("COMMON.CFG","PRO")
$	WORLD = F$ELEMENT(3,",",COMCFG_PRO) - "WORLD="
$	GROUP = F$ELEMENT(2,",",COMCFG_PRO) - "GROUP="
$	IF F$LOCATE("W",WORLD) .NE. F$LENGTH(WORLD) THEN GOTO UPDATE_CFG
$	COMCFG_OWN = F$FILE_ATTRIBUTES("COMMON.CFG","UIC")
$	COMCFG_GRP = F$FILE_ATTRIBUTES("COMMON.CFG","GRP")
$	PROCES_OWN = F$GETJPI("","UIC")
$	PROCES_GRP = F$GETJPI("","GRP")
$	IF PROCES_GRP .EQ. COMCFG_GRP 
$	 THEN
$	  IF F$LOCATE("W",GROUP) .NE. F$LENGTH(GROUP) THEN GOTO UPDATE_CFG
$	ENDIF
$	IF COMCFG_OWN .EQS. PROCES_OWN THEN GOTO MODFILE
$	IF F$PRIVILEGE("BYPASS") THEN GOTO MODFILE 
$	IF F$LOCATE("BYPASS",PRIV) .NE. F$LENGTH(PRIV)
$	 THEN
$	  SET PROC/PRIV=BYPASS
$	  GOTO MODFILE
$	ENDIF
$	IF F$LOCATE("SETPRV",PRIV) .NE. F$LENGTH(PRIV)
$	 THEN
$	  SET PROC/PRIV=SETPRV
$	  SET PROC/PRIV=BYPASS
$	  GOTO MODFILE
$	ENDIF
$!
$! Nope, user hasn't write access to COMMON.CFG
$!
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "  You don't seem to have sufficient privileges to update COMMON.CFG."
$	WRITE SYS$OUTPUT "  Please see your system manager and ask him for the right to modify this file."
$	WRITE SYS$OUTPUT ""
$	WRITE LIST ""
$	WRITE LIST "  You don't seem to have sufficient privileges to update COMMON.CFG."
$	WRITE LIST "  Please see your system manager and ask him for the right to modify this file."
$	WRITE LIST ""
$	GOTO DONE
$ MODFILE:
$	SET FILE COMMON.CFG/PROTECTION=(SYSTEM:RWED,OWNER:RWED,GROUP:RWE,WORLD:RE)
$	
$ UPDATE_CFG:
$!
$! Determine how to apply changes...
$!
$	VSCOM = ""
$	IF VVER .GES. "V6.2" 
$	 THEN 
$	  VSCOM = "/NOWARNING" 
$	 ELSE
$	  MESENV = F$ENVIRONMENT("MESSAGE")
$	  SET MESSAGE/NOTEXT/NOSEVERITY/NOFACILITY/NOIDENTIFICATION 
$	ENDIF
$	SEARCH/NOOUTPUT'VSCOM' COMMON.CFG "BROWSERREMOTE:"
$	BROWREMO = $SEVERITY
$	SEARCH/NOOUTPUT'VSCOM' COMMON.CFG "BROWSERDIRECT:"
$	BROWDIRE = $SEVERITY
$	IF VVER .LTS. "V6.2" THEN SET MESSAGE 'MESENV'
$	OPEN/WRITE 100 BRC.TMP
$	WRITE 100 "browserRemote:	VMSWRAP ""-remote"" ""openURL(%U)"""
$	CLOSE 100
$	OPEN/WRITE 101 BDC.TMP
$	WRITE 101 "browserDirect:	VMSWRAP """%U""""
$	CLOSE 101
$	OPEN/WRITE 102 BDC.EDT
$	IF BROWDIRE .EQ. 1
$	 THEN
$	  WRITE 102 "FIND 'BROWSERDIRECT:'"
$	  WRITE 102 "DELETE"
$	ENDIF
$	WRITE 102 "INCLUDE BDC.TMP"
$	WRITE 102 "EXIT"
$	CLOSE 102
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "  Updating BrowserDirect command in COMMON.CFG ..."
$	DEFINE/NOLOG SYS$INPUT SYS$COMMAND
$	DEFINE/NOLOG SYS$OUTPUT NLA0:
$	EDITTT/EDT/COMMAND=BDC.EDT COMMON.CFG
$	DEASSIGN SYS$OUTPUT 
$	DELETE/NOCONFIRM BDC.*;0
$	OPEN/WRITE 103 BRC.EDT
$	IF BROWREMO .EQ. 1
$	 THEN
$	  WRITE 103 "FIND 'BROWSERREMOTE:'"
$	  WRITE 103 "DELETE"
$	ENDIF
$	WRITE 103 "INCLUDE BRC.TMP"
$	WRITE 103 "EXIT"
$	CLOSE 103
$	WRITE SYS$OUTPUT "  Updating BrowserRemote command in COMMON.CFG ..."
$	DEFINE/NOLOG SYS$INPUT SYS$COMMAND
$	DEFINE/NOLOG SYS$OUTPUT NLA0:
$	EDITTT/EDT/COMMAND=BRC.EDT COMMON.CFG
$	DEASSIGN SYS$OUTPUT 
$	DELETE/NOCONFIRM BRC.*;0
$	PURGE/NOLOG COMMON.CFG
$	WRITE SYS$OUTPUT ""
$ DONE:
$	CLOSE LIST
$	IF F$FILE_ATTRIBUTES("SYS$LOGIN:TO_BE_DONE.LIST","EOF") .EQ. 0
$	 THEN
$! 
$! No errors occured
$! 
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  The installation procedure for setting up www access from inside xmcd via"
$	  WRITE SYS$OUTPUT "  a browser has completed successfully."
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  Restart your session so that the changes can take effect or execute your"
$	  WRITE SYS$OUTPUT "  LOGIN.COM manually and run xmcd from the command prompt then."
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  ''DEF_BROW' should come up upon clicking on xmcd's www button"
$	  WRITE SYS$OUTPUT "  and chosing a site then."
$	  WRITE SYS$OUTPUT ""
$	  DELETE/NOCONFIRM SYS$LOGIN:TO_BE_DONE.LIST;*
$	 ELSE
$!
$! At least one error occured
$!
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT "  The above items needing attention also can be reviewed in"
$	  WRITE SYS$OUTPUT "  SYS$LOGIN:TO_BE_DONE.LIST"
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT ""
$	ENDIF
