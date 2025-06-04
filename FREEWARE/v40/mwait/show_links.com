$!
$!***********************************************************************
$!
$! SHOW_LINKS.COM
$!
$!
$!	This procedure simulates the NCP command SHOW KNOWN LINKS
$!	for DECnet/OSI
$!
$!
$! VERSION:	01.00
$!
$! AUTHOR:	Kari Salminen
$!
$! DATE:	18-APR-1996
$!
$!
$! MODIFIED BY:
$!
$!	V1.1    19-APR-1996     Kari Salminen
$!				Add check if using the same NSP/OSI port.
$!                              Add debug switch, delete temp files if not dbg.
$!
$!	V1.2    29-APR-1996     Kari Salminen
$!				Add save/search for NSP/OSI ports out of order.
$!
$!	V1.3     3-MAY-1996     Kari Salminen
$!				Handle missing and "<Default value>" cases.
$!				Change the .TMP file names.
$!
$!
$!
$!
$!***********************************************************************
$!
$!	The debug option D will not delete the .TMP files
$!
$!	The debug option X will use the last .TMP files again
$!	The debug option Y will use the last .TMP files again and skip GETJPI
$!
$!	If P1 = X/Y , then if P2 .eqs. "" , use current PID for .TMP file name
$!			      P2 .nes. "" , use P2 as PID for .TMP file name
$!
$!				    " PID  "
$!				OSI_3B415B62.TMP;1
$!
$!
$! 	For a good NSP link from node A to B you must see similar to the following.
$! 	Note:  the "REM and LOC LINK" numbers must match:
$!
$!   LINK
$! REM     LOC    DIRECTION   LOCAL USER          PID       PROCESS NAME     REMOTE USER                  NODE    PORT
$!--------------  ---------   ---------------     --------  ---------------  ---------------------------  ------  ---------------
$! - On node A:
$! 60      132    OUTGOING    [0,0]SALMINEN       20204E8C  DECW$TE_4E8C     X$X0                         B       NSP$PORT_00002323
$! - On node B:
$! 132     60     INCOMING    X$X0                41E0005D  DECW$SERVER_0    [0,0]SALMINEN                A       NSP$PORT_0000203C
$!
$!
$!
$ DEBUG = 0 		! Default no debug
$!
$  if p1 .eqs. "D" then DEBUG = 1
$  if p1 .eqs. "d" then DEBUG = 1
$!
$!
$!
$ wo = "WRITE SYS$OUTPUT"
$!
$ wo (" ")
$ wo ("              *****************************")
$ wo ("              *   SHOW_LINKS.COM  V1.3    *")
$ wo ("              *   Show DECnet/OSI Links   *")
$ wo ("              *****************************")
$ wo (" ")
$ wo (" ")
$ wo ("   This procedure simulates the PHASE IV NCP command")
$ wo ("   SHOW KNOWN LINKS for DECnet/OSI")
$ wo (" ")
$ wo ("   Collecting data...")
$ wo (" ")
$ wo (" ")
$!
$ total =0
$!
$ nsp_counter  = 0		! NSP ring buffer
$ nsp_saved    = 0		! # of saved NSP ports
$ osi_counter  = 0		! OSI ring buffer
$ osi_saved    = 0		! # of saved OSI ports
$ init_counter = 0
$!
$ init_loop:
$!
$ NSP_SAVE'init_counter'_SCL_PORT = ""
$ NSP_SAVE'init_counter'_TMP_PORT = ""
$ NSP_SAVE'init_counter'_NODE	  = ""
$ NSP_SAVE'init_counter'_LOC_LINK = ""
$ NSP_SAVE'init_counter'_REM_LINK = ""
$!
$ OSI_SAVE'init_counter'_SCL_PORT = ""
$ OSI_SAVE'init_counter'_TMP_PORT = ""
$ OSI_SAVE'init_counter'_NODE	  = ""
$ OSI_SAVE'init_counter'_LOC_LINK = ""
$ OSI_SAVE'init_counter'_REM_LINK = ""
$!
$ init_counter = init_counter + 1
$ if init_counter .lt. 17 then goto init_loop
$!
$!
$!*********** Temporary filename and version **********************************
$!
$!	Insert PID into .TMP file name, so the user can run simultaneously
$!	several copies of this procedure from the same directory on a cluster.
$!
$ MY_PID = F$GETJPI("","PID")		! Get PID
$ if P2 .nes. "" then MY_PID = P2	! Debug mode, use P2
$!
$ file   = "SCL_''MY_PID'.TMP;"
$ file_2 = "NSP_''MY_PID'.TMP;"
$ file_3 = "OSI_''MY_PID'.TMP;"
$!
$ if p1 .eqs. "x" then goto skip	! test
$ if p1 .eqs. "X" then goto skip	! test
$ if p1 .eqs. "y" then goto skip	! test
$ if p1 .eqs. "Y" then goto skip	! test
$!
$!	We must use version numbers for the temp files, so the user 
$!	may run several times this procedure from the same directory.
$!
$ version = 1
$ string = F$SEARCH ("''file'")
$ if string .eqs. "" then goto OK	! File does not exist
$!
$!****** File exists already, create new version ************
$!
$ tmpstr = F$EXTRACT(F$LOCATE("]",string) + 1 ,20,string)
$!
$ version = f$extract(17, 4, tmpstr)
$ version = version + 1
$!
$ OK:
$ file = "''file'''version'"
$!
$!***************************************************************************
$!
$ version = 1
$ string = F$SEARCH ("''file_2'")
$ if string .eqs. "" then goto OK_2		! File does not exist
$!
$!****** File exists already, create new version ************
$!
$ tmpstr = F$EXTRACT(F$LOCATE("]",string) + 1 ,20,string)
$!
$ version = f$extract(17, 4, tmpstr)
$ version = version + 1
$!
$ OK_2:
$ file_2 = "''file_2'''version'"
$!
$!****************************
$!
$ version = 1
$ string = F$SEARCH ("''file_3'")
$ if string .eqs. "" then goto OK_3       ! File does not exist
$!
$!****** File exists already, create new version ************
$!
$ tmpstr = F$EXTRACT(F$LOCATE("]",string) + 1 ,20,string)
$!
$ version = f$extract(17, 4, tmpstr)
$ version = version + 1
$!
$ OK_3:
$ file_3 = "''file_3'''version'"
$!
$!***************************************************************************
$!***************************************************************************
$!
$!	Get all link information.
$!
$!	NOTE:	Doing this from DCL is rather slow and a link may go away
$!		or come up between the NCL calls.
$!
$!		Unfortunately the SHOW NSP and OSI PORT do not always display
$!		the ports in the same order as SHOW SESSION CONTROL PORT.
$!
$!		This is checked later to prevent the information getting 
$!		out of order after a missing/new link.
$!
$!
$ DEF/USER SYS$OUTPUT 'file				! SCL.TMP
$ MC NCL SHOW SESSION CONTROL PORT  * ALL ATTRIBUTES
$!
$ DEF/USER SYS$OUTPUT 'file_2				! NSP.TMP
$ MC NCL SHOW NSP PORT * ALL ATTRIBUTES
$!
$ DEF/USER SYS$OUTPUT 'file_3				! OSI.TMP
$ MC NCL SHOW OSI TRANSPORT PORT * ALL ATTRIBUTES
$!
$ skip:
$!
$!****************************************************************************
$!***************************************************************************
$!
$!	Get screen width
$!
$ set_term = 0
$ screen_width = F$GETDVI("SYS$OUTPUT","DEVBUFSIZ")
$!
$!***	Screen width must be 132
$!
$ if screen_width .lt. 81 then set_term = 1
$ if screen_width .lt. 81 then $set term/width=132
$!
$!****	Output file for links ************************************************
$!
$ link_file = "''F$GETSYI("nodename")_LINKS.TMP"
$ open/write/error=error outfile 'link_file
$!
$ wo (" ")
$ write/error=error outfile  " "
$ wo ("    LINK")
$ write/error=error outfile  "    LINK"
$ wo (" REM     LOC    DIRECTION   LOCAL USER          PID       PROCESS NAME     REMOTE USER                  NODE    PORT")
$ write/error=error outfile  " REM     LOC    DIRECTION   LOCAL USER          PID       PROCESS NAME     REMOTE USER                  NODE    PORT"
$ wo ("--------------  ---------   ---------------     --------  ---------------  ---------------------------  ------  ---------------")
$ write/error=error outfile  "--------------  ---------   ---------------     --------  ---------------  ---------------------------  ------  ---------------"
$!
$ ON CONTROL_Y THEN GOTO clean_up
$!
$ open/read tempfile 'file
$ open/read nspfile 'file_2
$ open/read osifile 'file_3
$!
$!****************************************************************************
$!*************** Scan for items *********************************************
$!****************************************************************************
$!
$ BIG_LOOP:
$!
$!
$!***	Search "Node 0 Session Control Port SCL$PORTxxxx " *******************
$!
$ next_line_20:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_20
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_20
$ if tmpstr .eqs. "" then goto next_line_20
$!
$ !xxxxx show symbol string
$!
$ SCL_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30 ,string)
$!
$ !xxxxx show symbol SCL_PORT
$!
$!
$!***	Search "Local End User Address  = UIC = [0,0]SYSTEM" *****************
$!
$ next_line_1:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_1
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 5, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "LOCAL" then goto next_line_1
$ if tmpstr .eqs. "" then goto next_line_1
$!
$ !xxxxx show symbol string
$!
$ tmpstr = F$EXTRACT(F$LOCATE("=", string) + 1 , 30 ,string)
$!
$ !xxxxx show symbol tmpstr
$!
$ LUSER = F$EXTRACT(F$LOCATE("=", tmpstr) + 1 , 30 ,tmpstr)
$!
$ !xxxxx show symbol LUSER
$!
$!***	Search for "Transport Port" ******************************************
$!
$!
$ next_line_2:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_2
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 9, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "TRANSPORT" then goto next_line_2
$ if tmpstr .eqs. "" then goto next_line_2
$!
$ !xxxxx show symbol string
$!
$!    Transport Port                    = NSP Port NSP$PORT_00002004
$!    Transport Port                    = OSI Transport Port OSI$PORT_0_0040
$!
$ temp_p = F$EXTRACT(F$LOCATE("=", string) + 2, 3 ,string)
$!
$ !xxxxx show symbol temp
$!
$ if temp_p .eqs. "NSP" then NSP_PORT = F$EXTRACT(F$LOCATE("NSP$PORT", string), 30 ,string)
$ if temp_p .eqs. "OSI" then NSP_PORT = F$EXTRACT(F$LOCATE("OSI$PORT", string), 30 ,string)
$!
$!	We can have "<Default value>" too.
$!
$ if temp_p .eqs. "<DE" then NSP_PORT = "???"
$!
$ !xxxxx show symbol NSP_PORT
$ !xxxxx show symbol temp_p
$!
$!***	Search for "Direction" ***********************************************
$!
$ next_line_3:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_3
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 9, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "DIRECTION" then goto next_line_3
$ if tmpstr .eqs. "" then goto next_line_3
$!
$ !xxxxx show symbol string
$!
$ DIRECTION = F$EXTRACT(F$LOCATE("=", string) + 2 , 30 ,string)
$!
$ !xxxxx show symbol DIRECTION
$!
$!
$!***	Search for "Remote End User" ******************************************
$!
$ next_line_4:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_4
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "REMOTE" then goto next_line_4
$ if tmpstr .eqs. "" then goto next_line_4
$!
$ !xxxxx show symbol string
$!
$ tmpstr = F$EXTRACT(F$LOCATE("=", string) + 1 , 40 ,string)
$!
$ !xxxxx show symbol tmpstr
$!
$ RUSER = F$EXTRACT(F$LOCATE("=", tmpstr) + 1 , 30 ,tmpstr)
$!
$ !xxxxx show symbol RUSER
$!
$!***	Search for "Node Name Sent" ******************************************
$!
$ next_line_9:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_9
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 9, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE NAME" then goto next_line_9
$ if tmpstr .eqs. "" then goto next_line_9
$!
$ !xxxxx show symbol string
$!
$ NODE_X = F$EXTRACT(F$LOCATE("LOCAL:", string) + 7 , 6 ,string)
$!
$ !xxxxx show symbol NODE_X
$!
$!
$!***	Search for "PID" ******************************************************
$!
$ next_line_5:
$!
$ read/error=error/END_OF_FILE=end_file tempfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_5
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 7, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "PROCESS" then goto next_line_5
$ if tmpstr .eqs. "" then goto next_line_5
$!
$ !xxxxx show symbol string
$!
$ PID = F$EXTRACT(F$LOCATE("=", string) + 3 , 8 ,string)
$!
$ !xxxxx show symbol PID
$!
$ PROC_NAME = "???"				! Default if gone
$!
$ if PID .eqs. "DEFAULT " then goto skip3	! It can be <Default value> !
$!
$ if p1 .eqs. "y" then goto skip3	! test
$ if p1 .eqs. "Y" then goto skip3	! test
$!
$ PROC_NAME = F$GETJPI("''PID'","PRCNAM")
$!
$ skip3:
$!
$!***************************************************************************
$!
$ if temp_p .eqs. "OSI" then goto OSI_PORT
$!
$!****************************************************************************
$!****************************************************************************
$!
$!	NSP port
$!
$ if nsp_saved .eq. 0 then goto next_line_06	! No saved ports
$!
$!***** Look if we have the values in saved ones *****************************
$!
$ counter = 0
$!
$ nsp_loop_0:
$!
$ if NSP_PORT .eqs. NSP_SAVE'counter'_TMP_PORT then goto found_nsp_saved_0
$ if SCL_PORT .eqs. NSP_SAVE'counter'_SCL_PORT then goto found_nsp_saved_x_0
$!
$ counter = counter + 1
$ if counter .lt. 16 then goto nsp_loop_0       ! Next
$ goto next_line_06				! Was last
$!
$!********** Found from saved ports
$!
$ found_nsp_saved_x_0:
$!
$ NSP_PORT = NSP_SAVE'counter'_TMP_PORT
$!
$ found_nsp_saved_0:
$!
$! wo ("====> JUHUU 0 we can use the saved values ''counter' left ''nsp_saved'")
$!
$ nsp_saved = nsp_saved - 1
$!
$ NODE	   = NSP_SAVE'counter'_NODE
$ LOC_LINK = NSP_SAVE'counter'_LOC_LINK
$ REM_LINK = NSP_SAVE'counter'_REM_LINK
$!
$ goto conti
$!
$!***************************************************************************
$!***	Search for "NSP$PORT" to verify we have the right port **************
$!***************************************************************************
$!
$ next_line_06:
$!
$ read/error=error/END_OF_FILE=end_file nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_06
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_06
$ if tmpstr .eqs. "" then goto next_line_06
$!
$ TMP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol TMP_PORT
$!
$!***	Search for "Client Name" *********************************************
$!
$ next_line_21:
$!
$ read/error=error/END_OF_FILE=end_file nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_21
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "CLIENT" then goto next_line_21
$ if tmpstr .eqs. "" then goto next_line_21
$!
$ tmpstr = F$EXTRACT(F$LOCATE("=", string) + 1 , 40 ,string)
$!
$ !xxxxx show symbol tmpstr
$!
$ SCL_NSP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol SCL_NSP_PORT
$!
$!
$ if NSP_PORT .eqs. TMP_PORT then goto found_nsp
$ if SCL_PORT .eqs. SCL_NSP_PORT then goto found_nsp_x
$!
$!***************************************************************************
$!**** NSP ports are out of order *******************************************
$!***************************************************************************
$!
$!**** Save the values for this port into NSP ring buffer *******************
$!
$ gosub READ_NSP
$!
$ !xxxxx show symbol TMP_PORT
$ !xxxxx show symbol LOC_LINK
$ !xxxxx show symbol REM_LINK
$!
$ nsp_saved = nsp_saved + 1
$!
$! wo ("********* Save NSP values ''nsp_counter' now ''nsp_saved'")
$!
$ NSP_SAVE'nsp_counter'_SCL_PORT = SCL_NSP_PORT 
$ NSP_SAVE'nsp_counter'_TMP_PORT = TMP_PORT
$ NSP_SAVE'nsp_counter'_NODE	 = NODE
$ NSP_SAVE'nsp_counter'_LOC_LINK = LOC_LINK
$ NSP_SAVE'nsp_counter'_REM_LINK = REM_LINK
$!
$ nsp_counter = nsp_counter + 1
$ if nsp_counter .gt. 15 then nsp_counter = 0
$!
$!
$!	We might have the case "Transport Port = <Default value>",
$!	read one NSP port and if no match then read one OSI port.
$!
$ if temp_p .eqs. "<DE" then goto OSI_PORT
$!
$!***************************************************************************
$!*** NSP Port not found, Scan nspfile ***************************************
$!***************************************************************************
$!
$ scan_nsp:
$!
$!***	Scan first the next 15 entries for the NSP$PORT **********************
$!
$ counter = 0
$!
$ !xxxxx show symbol NSP_SAVE'counter'_TMP_PORT
$!
$ next_line_XXX6:
$!
$ read/error=error/END_OF_FILE=NSP_NOT_FOUND_X nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_XXX6
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_XXX6
$ if tmpstr .eqs. "" then goto next_line_XXX6
$!
$ counter = counter + 1
$ if counter .eq. 16 then goto NSP_NOT_FOUND_X
$!
$ TMP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol TMP_PORT
$!
$!***	Search for "Client Name" *********************************************
$!
$ next_line_X21:
$!
$ read/error=error/END_OF_FILE=end_file nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_X21
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "CLIENT" then goto next_line_X21
$ if tmpstr .eqs. "" then goto next_line_X21
$!
$ tmpstr = F$EXTRACT(F$LOCATE("=", string) + 1 , 40 ,string)
$!
$ !xxxxx show symbol tmpstr
$!
$ SCL_NSP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol SCL_NSP_PORT
$!
$! if NSP_PORT .eqs. TMP_PORT then $ wo (" ====> JIPPII NSP counter ''counter'")
$ if NSP_PORT .eqs. TMP_PORT then goto found_nsp
$ if SCL_PORT .eqs. SCL_NSP_PORT then goto found_nsp_x
$!
$!
$!****** Save values for this port into NSP ring buffer ***********************
$!
$ gosub READ_NSP
$!
$ nsp_saved = nsp_saved + 1
$!
$! wo ("********* Save NSP values ''nsp_counter' now ''nsp_saved'")
$!
$ NSP_SAVE'nsp_counter'_SCL_PORT = SCL_NSP_PORT 
$ NSP_SAVE'nsp_counter'_TMP_PORT = TMP_PORT
$ NSP_SAVE'nsp_counter'_NODE	  = NODE
$ NSP_SAVE'nsp_counter'_LOC_LINK = LOC_LINK
$ NSP_SAVE'nsp_counter'_REM_LINK = REM_LINK
$!
$ !xxxxx show symbol NSP_SAVE'nsp_counter'_TMP_PORT
$!
$ nsp_counter = nsp_counter + 1
$ if nsp_counter .gt. 15 then nsp_counter = 0
$!
$ goto next_line_XXX6
$!
$!***************************************************************************
$!
$ NSP_NOT_FOUND_X:
$!
$!***** Scan from beginning of the file *************************************
$!
$!	Close and reopen nspfile
$!
$ wo ("======> NSP ports badly out of order, need to scan the file...")
$!
$ close nspfile
$ open/read nspfile 'file_2
$!
$!***	Scan for the NSP$PORT ************************************************
$!
$ next_line_X6:
$!
$ read/error=error/END_OF_FILE=NSP_NOT_FOUND nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_X6
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_X6
$ if tmpstr .eqs. "" then goto next_line_X6
$!
$ TMP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol TMP_PORT
$!
$ if NSP_PORT .eqs. TMP_PORT then goto found_nsp
$!
$ goto next_line_X6
$!
$!***************************************************************************
$!
$ NSP_NOT_FOUND:
$!
$!***	Reposition to the begin of the file *********************************
$!
$ close nspfile
$ open/read nspfile 'file_2
$!
$!***** The following can be found only from NSP ****************************
$!
$ NO_PORT_INFO:
$!
$ if nsp_saved .eq. 0 then goto skip_nsp
$!
$!***** Look if we have the values in saved ones ****************************
$!
$ counter = 0
$!
$ nsp_loop_2:
$!
$ if NSP_PORT .eqs. NSP_SAVE'counter'_TMP_PORT then goto found_nsp_saved_2
$ if NSP_PORT .eqs. NSP_SAVE'counter'_SCL_PORT then goto found_nsp_saved_x_2
$!
$ counter = counter + 1
$ if counter .lt. 16 then goto nsp_loop_2
$ goto skip_nsp
$!
$!****************************************************************************
$!
$ found_nsp_saved_x_2:
$!
$ NSP_PORT = NSP_SAVE'counter'_TMP_PORT
$!
$ found_nsp_saved_2:
$!
$ wo ("====> JUHUU 0 we can use the saved values ''counter' left ''nsp_saved'")
$!
$ nsp_saved = nsp_saved - 1
$!
$ NODE	   = NSP_SAVE'counter'_NODE
$ LOC_LINK = NSP_SAVE'counter'_LOC_LINK
$ REM_LINK = NSP_SAVE'counter'_REM_LINK
$!
$ goto conti
$!
$!***************************************************************************
$!
$ skip_nsp:
$!
$ if osi_saved .eq. 0 then goto skip_osi
$!
$!***** Look if we have the values in saved ones ****************************
$!
$ counter = 0
$!
$ osi_loop_2:
$!
$ if NSP_PORT .eqs. OSI_SAVE'counter'_TMP_PORT then goto found_osi_saved_2
$ if NSP_PORT .eqs. OSI_SAVE'counter'_SCL_PORT then goto found_osi_saved_x_2
$!
$ counter = counter + 1
$ if counter .lt. 16 then goto osi_loop_2
$ goto skip_osi
$!
$ found_osi_saved_x_2:
$!
$ NSP_PORT = OSI_SAVE'counter'_TMP_PORT
$!
$ found_osi_saved_2:
$!
$ wo ("====> JIPII 0 we can use the saved values ''counter' left ''osi_saved'")
$!
$ osi_saved = osi_saved - 1
$!
$ NODE	   = OSI_SAVE'counter'_NODE
$ LOC_LINK = OSI_SAVE'counter'_LOC_LINK
$ REM_LINK = OSI_SAVE'counter'_REM_LINK
$!
$ goto conti
$!
$!***************************************************************************
$!
$ skip_osi:
$!
$ NODE     = "???"
$ if f$length(NODE_X) .ne. 0 then NODE = NODE_X
$!
$ LOC_LINK = " ???"
$ REM_LINK = " ???"
$!
$ goto conti
$!
$!***************************************************************************
$!
$ found_nsp_x:
$!
$ NSP_PORT = TMP_PORT
$!
$ found_nsp:
$!
$ gosub READ_NSP
$!
$ goto conti
$!
$!****************************************************************************
$!******************* OSI port ***********************************************
$!****************************************************************************
$!
$ OSI_PORT:
$!
$ if osi_saved .eq. 0 then goto next_line_016
$!
$!***** Look if we have the values in saved ones *****************************
$!
$ counter = 0
$!
$ osi_loop_0:
$!
$ if NSP_PORT .eqs. OSI_SAVE'counter'_TMP_PORT then goto found_osi_saved_0
$ if SCL_PORT .eqs. OSI_SAVE'counter'_SCL_PORT then goto found_osi_saved_x_0
$!
$ counter = counter + 1
$ if counter .lt. 16 then goto osi_loop_0
$ goto next_line_016
$!
$!***************************************************************************
$!
$ found_osi_saved_x_0:
$!
$ NSP_PORT = OSI_SAVE'counter'_TMP_PORT
$!
$ found_osi_saved_0:
$!
$! wo ("====> JIPII 0 we can use the saved values ''counter' left ''osi_saved'")
$!
$ osi_saved = osi_saved - 1
$!
$ NODE	   = OSI_SAVE'counter'_NODE
$ LOC_LINK = OSI_SAVE'counter'_LOC_LINK
$ REM_LINK = OSI_SAVE'counter'_REM_LINK
$!
$ goto conti
$!
$!***	Search for "OSI$PORT" to verify we have the right port **************
$!
$ next_line_016:
$!
$ read/error=error/END_OF_FILE=end_file osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_016
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_016
$ if tmpstr .eqs. "" then goto next_line_016
$!
$ TMP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 10 , 30, string)
$!
$ !xxxxx show symbol TMP_PORT
$!
$!***	Search for "Client Name" *********************************************
$!
$ next_line_021:
$!
$ read/error=error/END_OF_FILE=end_file osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_021
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "CLIENT" then goto next_line_021
$ if tmpstr .eqs. "" then goto next_line_021
$!
$ tmpstr = F$EXTRACT(F$LOCATE("=", string) + 1 , 40 ,string)
$!
$ !xxxxx show symbol tmpstr
$!
$ SCL_OSI_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol SCL_OSI_PORT
$!
$ if NSP_PORT .eqs. TMP_PORT then goto found_osi
$ if SCL_PORT .eqs. SCL_OSI_PORT then goto found_osi_x
$!
$!***************************************************************************
$!**** OSI ports are out of order *******************************************
$!***************************************************************************
$!
$!**** Save the values for this port into OSI ring buffer *******************
$!
$ gosub READ_OSI
$!
$ !xxxxx show symbol TMP_PORT
$ !xxxxx show symbol LOC_LINK
$ !xxxxx show symbol REM_LINK
$!
$ osi_saved = osi_saved + 1
$!
$! wo ("********* Save OSI values ''osi_counter' now ''osi_saved'")
$!
$ OSI_SAVE'osi_counter'_SCL_PORT = SCL_OSI_PORT
$ OSI_SAVE'osi_counter'_TMP_PORT = TMP_PORT
$ OSI_SAVE'osi_counter'_NODE	 = NODE
$ OSI_SAVE'osi_counter'_LOC_LINK = LOC_LINK
$ OSI_SAVE'osi_counter'_REM_LINK = REM_LINK
$!
$ osi_counter = osi_counter + 1
$ if osi_counter .gt. 15 then osi_counter = 0
$!
$!
$!      We might have the case "Transport Port = <Default value>",
$!      read one NSP port and if no match then read one OSI port.
$!	If both fail, then skip.
$!
$ if temp_p .eqs. "<DE" then goto NO_PORT_INFO
$!
$!***************************************************************************
$!*** OSI Port not found, Scan osifile **************************************
$!***************************************************************************
$!
$ scan_osi:
$!
$!***   Scan first the next 15 entries for the OSI$PORT *********************
$!
$ counter = 0
$!
$ next_line_YYY016:
$!
$ read/error=error/END_OF_FILE=OSI_NOT_FOUND_X osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_YYY016
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_YYY016
$ if tmpstr .eqs. "" then goto next_line_YYY016
$!
$ TMP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 10 , 30, string)
$!
$ !xxxxx show symbol TMP_PORT
$!
$!***	Search for "Client Name" ********************************************
$!
$ next_line_X021:
$!
$ read/error=error/END_OF_FILE=end_file osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_X021
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "CLIENT" then goto next_line_X021
$ if tmpstr .eqs. "" then goto next_line_X021
$!
$ tmpstr = F$EXTRACT(F$LOCATE("=", string) + 1 , 40 ,string)
$!
$ !xxxxx show symbol tmpstr
$!
$ SCL_OSI_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 5 , 30, string)
$!
$ !xxxxx show symbol SCL_OSI_PORT
$!
$! if NSP_PORT .eqs. TMP_PORT then $ wo (" ====> JIPPII OSI counter ''counter'")
$ if NSP_PORT .eqs. TMP_PORT then goto found_osi
$ if SCL_PORT .eqs. SCL_OSI_PORT then goto found_osi_x
$!
$ counter = counter + 1
$ if counter .eq. 15 then goto OSI_NOT_FOUND_X
$!
$!****** Save values for this port into OSI ring buffer *********************
$!
$ gosub READ_OSI
$!
$ osi_saved = osi_saved + 1
$!
$! wo ("********* Save OSI values ''osi_counter' now ''osi_saved'")
$!
$ OSI_SAVE'osi_counter'_TMP_PORT = TMP_PORT
$ OSI_SAVE'osi_counter'_NODE	 = NODE
$ OSI_SAVE'osi_counter'_LOC_LINK = LOC_LINK
$ OSI_SAVE'osi_counter'_REM_LINK = REM_LINK
$!
$ !xxxxx show symbol OSI_SAVE'osi_counter'_TMP_PORT
$!
$ osi_counter = osi_counter + 1
$ if osi_counter .gt. 15 then osi_counter = 0
$!
$ goto next_line_YYY016
$!
$!***************************************************************************
$!
$ OSI_NOT_FOUND_X:
$!
$!***** Scan from beginning of the file *************************************
$!
$!	Close and reopen osifile
$!
$ wo ("======> OSI ports badly out of order, need to scan the file...")
$!
$ close osifile
$ open/read osifile 'file_3
$!
$!***	Scan for the OSI$PORT ***********************************************
$!
$ next_line_Y16:
$!
$ read/error=error/END_OF_FILE=OSI_NOT_FOUND osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_Y16
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 4, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "NODE" then goto next_line_Y16
$ if tmpstr .eqs. "" then goto next_line_Y16
$!
$ TMP_PORT = F$EXTRACT(F$LOCATE("PORT", string) + 10 , 30, string)
$!
$ !xxxxx show symbol TMP_PORT
$!
$ if NSP_PORT .eqs. TMP_PORT then goto found_osi
$!
$ goto next_line_Y16
$!
$!***************************************************************************
$!
$ OSI_NOT_FOUND:
$!
$!*** 	Reposition to the begin of the file *********************************
$!
$ close osifile
$ open/read osifile 'file_3
$!
$!***** The following can be found only from OSI ****************************
$!
$ NODE     = "???"
$ LOC_LINK = " ???"
$ REM_LINK = " ???"
$!
$ goto conti
$!
$!***************************************************************************
$!
$ found_osi_x:
$!
$ NSP_PORT = TMP_PORT
$!
$ found_osi:
$!
$ gosub READ_OSI
$!
$!****************************************************************************
$!********** We have now all the information for this port *******************
$!****************************************************************************
$!
$ conti:
$!
$!	 tmp = F$LOCATE("=",string)
$!	 tmpstr = F$EXTRACT(0,F$LOCATE("Local",string),string)
$!
$!   REM  LOC   DIRECTION  REMOTE USER      PID       PROCESS NAME     NODE   LOCAL USER   PORT
$!
$!
$ if f$length(NODE) .eq. 0 then NODE = NODE_X	! (LOCAL:.mynode) missing
$ if f$length(NODE) .eq. 0 then NODE = "???"	! Neither in "Node Name Sent"
$!
$!
$!               REM LOC   DIR   LUSER   PID  PNAM  RU  NOD LINK
$ text = f$fao("!7AS !7AS !10AS !20AS !10AS!15AS !27AS   !6AS  !20AS", -
	REM_LINK, LOC_LINK, DIRECTION, LUSER, PID, PROC_NAME, RUSER, NODE, NSP_PORT)
$!
$!	12345678901234567890123456
$!	[145003,20040]KUDA_STARTUP
$!
$ wo text
$ write/error=error outfile  text
$!
$ total = total + 1
$!
$ goto BIG_LOOP
$!
$!****************************************************************************
$!********** Oops, something went wrong **************************************
$!****************************************************************************
$!
$ error:
$ end_file:
$!
$ wo ("--------------  ---------   ---------------     --------  ---------------  ---------------------------  ------  ---------------")
$ write/error=error outfile  "--------------  ---------   ---------------     --------  ---------------  ---------------------------  ------  ---------------"
$ wo (" REM     LOC    DIRECTION   LOCAL USER          PID       PROCESS NAME     REMOTE USER                  NODE    PORT")
$ write/error=error outfile  " REM     LOC    DIRECTION   LOCAL USER          PID       PROCESS NAME     REMOTE USER                  NODE    PORT"
$ wo (" ")
$ write/error=error outfile  " "
$ wo (" Total ''total' Links")
$ write/error=error outfile  " Total ''total' Links"
$ wo (" ")
$ write/error=error outfile  " "
$!
$!
$ wo (" Screen output is stored in the file ''link_file'")
$ wo (" ")
$!
$!***************************************************************************
$!
$ clean_up:
$!
$ ON CONTROL_Y THEN GOTO exit
$!
$ close tempfile
$ close nspfile 
$ close osifile 
$ close outfile
$!
$ if p1 .eqs. "x" then goto skip1	! test
$ if p1 .eqs. "X" then goto skip1	! test
$ if p1 .eqs. "y" then goto skip1	! test
$ if p1 .eqs. "Y" then goto skip1	! test
$!
$!	Delete temporary files
$!
$  if DEBUG .eq. 0
$  then
$ DELETE/NOLOG 'file'
$ DELETE/NOLOG 'file_2'
$ DELETE/NOLOG 'file_3'
$  else
$ wo ("	For debug purposes:")
$ wo ("		SCL port information is stored in the file ''file'")
$ wo ("		NSP port information is stored in the file ''file_2'")
$ wo ("		OSI port information is stored in the file ''file_3'")
$ wo ("	")
$  endif
$!
$ skip1:
$!
$ if set_term .eq. 0 then goto exit
$!
$ tmp = "Y"
$!
$ INQUIRE TMP "Do you want the terminal width set back to ''screen_width' [y/n, Def=y]? "
$!
$ if tmp .eqs. "N" then goto exit
$ if tmp .eqs. "n" then goto exit
$!
$ set term/width='screen_width'
$!
$exit:
$!
$ exit
$!
$!
$!****************************************************************************
$!********* Subroutine read from NSP file ************************************
$!****************************************************************************
$!
$ READ_NSP:
$!
$!***	Search for "Remote NSAP" *********************************************
$!
$ next_line_6:
$!
$ read/error=error/END_OF_FILE=end_file nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_6
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "REMOTE" then goto next_line_6
$ if tmpstr .eqs. "" then goto next_line_6
$!
$!
$ tmpstr = F$EXTRACT(F$LOCATE("LOCAL:", string) + 7 , 6 ,string)
$ tmp    = F$LOCATE(")",tmpstr)
$ NODE   = F$EXTRACT(0,tmp,tmpstr)
$!
$!
$ !xxxxx show symbol NODE
$!
$!
$!***	Search for "Local Reference" *****************************************
$!
$ next_line_7:
$!
$ read/error=error/END_OF_FILE=end_file nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_7
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 5, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "LOCAL" then goto next_line_7
$ if tmpstr .eqs. "" then goto next_line_7
$!
$ LOC_LINK = F$EXTRACT(F$LOCATE("=", string) + 1 , 6 ,string)
$!
$ !xxxxx show symbol LOC_LINK
$!
$!
$!***	Search for "Remote Reference" ****************************************
$!
$ next_line_8:
$!
$ read/error=error/END_OF_FILE=end_file nspfile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_8
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 6, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "REMOTE" then goto next_line_8
$ if tmpstr .eqs. "" then goto next_line_8
$!
$ REM_LINK = F$EXTRACT(F$LOCATE("=", string) + 1 , 6 ,string)
$!
$ !xxxxx show symbol REM_LINK
$!
$ return
$!
$!****************************************************************************
$!********* Subroutine read from OSI file ************************************
$!****************************************************************************
$!
$ READ_OSI:
$!
$!***	Search for "Remote NSAP" *********************************************
$!
$ next_line_16:
$!
$ read/error=error/END_OF_FILE=end_file osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_16
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 11, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "REMOTE NSAP" then goto next_line_16
$ if tmpstr .eqs. "" then goto next_line_16
$!
$ tmpstr = F$EXTRACT(F$LOCATE("LOCAL:", string) + 7 , 6 ,string)
$ tmp    = F$LOCATE(")",tmpstr)
$ NODE   = F$EXTRACT(0,tmp,tmpstr)
$!
$ !xxxxx show symbol NODE
$!
$!
$!***	Search for "Local Reference" *****************************************
$!
$ next_line_17:
$!
$ read/error=error/END_OF_FILE=end_file osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_17
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 15, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "LOCAL REFERENCE" then goto next_line_17
$ if tmpstr .eqs. "" then goto next_line_17
$!
$ LOC_LINK = F$EXTRACT(F$LOCATE("=", string) + 1 , 6 ,string)
$!
$ !xxxxx show symbol LOC_LINK
$!
$!
$!***	Search for "Remote Reference" ****************************************
$!
$ next_line_18:
$!
$ read/error=error/END_OF_FILE=end_file osifile string
$!
$ string = f$edit(string,"upcase,compress")
$ if string .eqs. "" then goto next_line_18
$!
$ tmpstr = f$edit(string,"TRIM")
$!
$ tmpstr = F$EXTRACT(0, 16, tmpstr)
$!
$ !xxxxx show symbol tmpstr
$!
$ if tmpstr .nes. "REMOTE REFERENCE" then goto next_line_18
$ if tmpstr .eqs. "" then goto next_line_18
$!
$ REM_LINK = F$EXTRACT(F$LOCATE("=", string) + 1 , 6 ,string)
$!
$ !xxxxx show symbol REM_LINK
$!
$ return
$!
$!****************************************************************************
$!
