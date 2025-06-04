$!++
$!			  COPYRIGHT (c) 1989  BY
$!	      DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
$!
$! THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND  COPIED
$! ONLY  IN  ACCORDANCE  WITH  THE  TERMS  OF  SUCH  LICENSE AND WITH THE
$! INCLUSION OF THE ABOVE COPYRIGHT NOTICE.  THIS SOFTWARE OR  ANY  OTHER
$! COPIES  THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
$! OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE  IS  HEREBY
$! TRANSFERRED.
$!
$! THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE  WITHOUT  NOTICE
$! AND  SHOULD  NOT  BE  CONSTRUED  AS  A COMMITMENT BY DIGITAL EQUIPMENT
$! CORPORATION.
$!
$! DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE OR  RELIABILITY  OF  ITS
$! SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
$!
$!**
$!
$!++
$! FACILITY:  Terminal Server Manager- TSM
$!
$! ABSTRACT:  This module contains the dcl command procedure to create a
$!            command file for use with TSM which sets the server's
$!            permanent characteristics.  When this procedure is run a
$!            command file is created which contains DEFINE commands to
$!            reflect the DECserver 100's permanent characteristics.
$!
$! ENVIRONMENT:  VAX/VMS Version 4.4 or later.
$!
$! AUTHOR:
$!
$! REVISION HISTORY:
$!
$! 	      7 June 1988 Laurie Fairbanks
$!
$! 	      Changed module name to include version number.
$!
$!            14 January, 1987  Lisa Sawyer
$!
$!            Changed information reporting format a bit.  Removed duplicate
$!            calls to f$time and simply record the time once, and report it
$!            at the top of the setup command file.
$!
$!            12 January, 1987  Lisa Sawyer
$!
$!            Revised existing command procedure as necessary for use with
$!            the current version of TSM.  (TSM V1.2I)
$!
$!**
$!*****************************************************************************
$! 17-Jan-1997	J. Malmberg	Adapted for Lantronix units.
$!			*	Fixed and activated saving of services.
$! 14-Feb-1997	J. Malmberg	Set up unique temp file names.
$!*****************************************************************************
$! DCL COMMAND PROCEDURE tsm$ds1_v20_get_char ( p1, p2 ) =
$!++
$! FUNCTIONAL DESCRIPTION:
$!
$! This DCL procedure interrogates a DECserver 100 via TSM, and generates a
$! TSM command file which may be used at a future time to reproduce the
$! current settings. The permanent settings are used.
$!
$!
$! INPUTS:      p1 = Name of server to use
$!              p2 = Verify Switch - Default equals noverify
$!                   (i.e. No echoing to sys$output) This is
$!                   an optional parameter.
$!
$! OUTPUTS:     NONE
$!
$! IMPLICIT INPUTS:   None
$!
$! IMPLICIT OUTPUTS:
$!
$! COMPLETION CODES:
$!
$! SIDE EFFECTS:
$!    Creates a command file for reproduction of the current
$!    permanent settings for the designated server.
$!
$!    The command file created, for use with TSM, is named
$!    "server_name"_setup.com.  This file is created in the
$!    current default directory.
$!
$!    Below is a summary of the restrictions imposed with regard
$!    to using this command procedure.
$!
$!    RESTRICTIONS
$!
$!     USAGE:
$!       The setup command file created, simply contains a number of
$!       DEFINE server, DEFINE port and DEFINE service commands, it
$!       does not contain the USE SERVER or SET PRIV commands.  It is
$!       expected that the user will enter TSM, issue the USE SERVER command,
$!       and enter privileged mode before executing the setup command file.
$!
$!     FILE NAMING:
$!       The user is not free to name the command file created.  The command
$!       file incoporates the server's name and the file name is created as
$!       follows:
$!
$!                 "server_name"_setup.com
$!
$!       This file is created in the current default directory.
$!
$!     TEMPORARY FILES:
$!       Two temporary files are created and subsequently deleted during
$!       execution of this DCL procedure.  The files are created in the
$!       current default directory.  If creation fails due to the
$!       coincidence of a file existing with this name, an error message is
$!       displayed.   The files are:
$!
$!                   tsm$ds1_temp_outfile.tmp;1
$!                   tsm$ds1_temp_cmd_file.tmp;1
$!
$!     SERVICES -
$!       NOT SUPPORTED - (i.e. no service characteristics are defined by
$!                        the setup command file )
$!
$!
$!     PASSWORDS -
$!       If passwords are enabled for selected ports - they will be enabled
$!       by the setup command procedure created by this DCL procedure.  The
$!       actual password, however, will not be reset.
$!
$!**
$!*****************************************************************************
$!
$	on error then goto err
$	on control_y then goto ctrly
$	CStr1 = "Define !AS !20AS !AS"
$	IdStr = "Define !AS !20AS ""!AS"""
$	Server = P1
$	Ver = "N"
$	If P2 .nes. "" Then Ver = "Y"
$       openflg = "N"
$!
$! Prompt the user for a server name if a name was not passed into this
$! procedure.
$!
$Server_name:
$	If Server .eqs. "" Then inq server "Server name"
$       If Server .eqs. "" Then goto server_name
$	Port = "ALL char"
$!
$! Check for the existence of a file with same name as temporary files that
$! will be created here.  If it exists display error message and exit this
$! procedure.
$!
$	pid = f$getjpi("","PID")
$	cmd_file = "tsm$ds1_temp_cmd_file_''pid'.tmp"
$       if f$search(cmd_file) then delete 'cmd_file';*
$!       if file .nes. "" then goto file_exists
$	tmp_file = "tsm$ds1_temp_outfile_''pid'.tmp"
$       if f$search(tmp_file) then delete 'tmp_file';*
$!       if file .nes. "" then goto file_exists
$
$! Place commands in temporary command file for future interrogation of a
$! decserver 100.
$
$	open/write com 'cmd_file'
$	write com "use server ''server'"
$	write com "show server"
$	write com "List service" !Uncommented to see what it would do
$	write com "List port ''port'"
$	write com "List prot enable"
$	write com "List service * char"
$       write com "exit "
$	close com
$!
$! Redirect the TSM output data to a temporary file.  This temporary file
$! will contain all the data regarding the server's permanent characteristics.
$!
$	tsm/output='tmp_file' @'cmd_file'
$	outfile = server + "_Setup" + ".com"
$!
$! Read the characteristic data from the temporary file and create DEFINE
$! command lines in the "server_name"_setup.com file as appropriate.
$!
$	open /read in 'tmp_file'
$	open /write out 'outfile'
$       openflg = "Y"
$	pflg = "Y"
$ 	char = "N"
$	serv = "N"
$!
$! List of Lantronix Protocols
$!----------------------------
$lprotocols = ",IP,Lat,Mop,AppleTalk,NetWare,LanManager,"
$lprotocols_len = f$length(lprotocols)
$dec_flag = "Y"
$eps_flag = "N"
$!
$srvc = "N"
$port_num = ""
$!
$! Read the next line in tsm$ds1_temp_outfile.tmp temporary file.
$!
$read_next:
$	read /end=done in line
$!
$! Parse description line to check for keyword identifying what action
$! should be taken with regard to adding a DEFINE command to the server
$! setup command file being created.
$!
$parse:
$	len = f$length(line)
$	If len .le. 2 Then goto read_next
$	clue = f$extract(0, 5, line)
$	clue2 = f$extract(0,7,line)
$	if clue2 .eqs. "Local -" then goto read_next
$	if clue .eqs. "DECse" then goto server_info
$	if clue .eqs. "   EP" then goto lserver_info
$!	if clue .eqs. "TCP/I" then goto tcp_info
$	if clue .eqs. "Port " then goto Port_line
$!
$! Lantronix Patches
$!----------------------
$	if clue .eqs. "LAT  " then line = line - clue
$	if clue .eqs. "TSM> "
$	then
$	    if port_num .eqs. "proto" then goto read_next
$	    clue2 = f$extract(0,7,line)
$	    if clue2 .eqs. "TSM> Li"
$	    then
$		char = "N"
$		goto read_next
$	    endif
$	endif
$	if ( clue .eqs. "Enabl" ) .and. ( .not. srvc ) then goto Char_line
$	If f$extract(0, 8, line) .eqs. "Service:" then goto Service_St
$       if f$extract(0, 9, line) .eqs. "TSM> exit"  then goto done
$!
$! Lantronix Service information
$!------------------------------
$      	If srvc then goto Parse_Srvc
$	If serv then goto parse_line
$	If char then goto char_parse
$	if pflg then goto read_next
$!
$parse_line:
$	line = F$Edit( line, "trim,compress" )
$	param = f$element( 0, ":", line) - ":"
$!
$!	Lantronix report syntax differences
$!-----------------------------------------
$	if (param .eqs. "Characteristics") .and. (.not. srvc)
$	then
$	    char = "Y"
$	    serv = "N"
$	    line = f$edit(f$element(1,":",line),"trim,compress")
$	    len = f$length(line)
$	    goto char_parse
$	endif
$	if param .eqs. "TCP/IP Address"
$	then
$	    param = "IPADDRESS"
$	endif
$	if param .eqs. "TCP/IP Gateway"
$	then
$	    param = "GATEWAY"
$	endif
$	if param .eqs. "Backup Gateway"
$	then
4	    param = "Secondary Gateway"
$	endif
$	If param .eqs. "Address" Then goto addr_fix
$	if param .eqs. "Hardware Addr" then goto addr_fix
$	If param .eqs. ":" Then goto read_next
$	If param .eqs. "" Then goto read_next
$	tvalue = f$element( 1, ":", line )
$	tvalue = F$Edit( tvalue, "trim,compress" )
$	If tvalue .eqs. "" Then goto read_next
$	value = f$element( 0, " ", tvalue )
$	value2 = f$element( 2, ":", line )
$       If value2 .eqs. ":" then value = tvalue
$	If F$Extract( 0,5,param ) .eqs. "Ident"
$	Then
$	    Value = TValue
$	    if eps_flag
$	    then
$		value = value - "Daytime"
$		value = f$edit(value,"TRIM")
$	    endif
$	endif
$	param = F$edit( param, "trim" )
$	value = F$edit( value, "trim" )
$	param2 = ""
$	param3 = ""
$	param4 = ""
$	value3 = ""
$	value4 = ""
$	If tvalue .eqs. ":" then goto Noparm2
$	param2 = tvalue - value
$	param2 = F$edit( param2, "trim" )
$	value2 = F$edit( value2, "trim" )
$!
$noparm2:
$!
$!
$! Lantronix report different syntax then define
$!----------------------------------------------
$	if eps_flag
$	then
$!
$!	    Lantronix documentation claims that queue limit can be set to 100
$!	    but the server will not except values greater than 32
$!-----------------------------------------------------------------------------
$	    if param .eqs. "Queue Limit" .and. port_num .eqs. "Server"
$	    then
$		if value .gts. "32" then value = "NONE"
$	    endif
$	    if param .eqs. "Printer Type"
$	    then
$		if value .nes. ""
$		then
$		    oStr = f$fao(CStr1,"Printer", value,"")
$		    OStr = F$Edit( OStr, "trim" )
$		    write out OStr
$		    If Ver Then write sys$output OStr
$		endif
$		value = ""
$	    endif
$	    if param .eqs. "Enabled Groups" then value = ""
$	    if param .eqs. "Ident String" then param = "Ident"
$	    if param .eqs. "Console Port" then value = ""
$	    if param2 .eqs. "Console Port" then value2 = ""
$	    if param2 .eqs. "Daytime" then param2 = ""
$	    if param2 .eqs. "Input Speed" then param2 = "Speed"
$	    if param2 .eqs. "Output Speed" then param2 = "Speed"
$	    if param .eqs. "Network Buffering" then param = "Buffering"
$	    if param2 .eqs. "Network Buffering" then param2 = "Buffering"
$	    if param .eqs. "Inactive Timer (min)" then param = "Inact Timer"
$	    if param2 .eqs. "Inactive Timer (min)" then param = "Inact Timer "
$	    if param .eqs. "Printer Status" then value = ""
$	    if param2 .eqs. "Printer Status" then value2 = ""
$	    if param .eqs. "Groups" then value = value + " Enable"
$	    if param .eqs. "Authorized Groups" then value = value + " Enable"
$	    if param .eqs. "(Current) Groups" then goto read_next
$	    if param .eqs. "Terminal Type" Then goto read_next
$	    if param2 .eqs. "Terminal Type" Then goto read_next
$	    if param .eqs. "Preferred Services" then goto read_next
$	    if param .eqs. "(Telnet)" then goto read_next
$	    if param .eqs. "Flow Ctrl" then param = "Flow Control"
$	    if value .eqs. "Xon/Xoff" then value = "XON"
$	    if value .eqs. "Cts/Rts" then value = "Cts"
$	    if value .eqs. "Dsr/Dtr" then value = "DSR"
$	    if value .eqs. "N/A" then value = ""
$	    if value2 .eqs. "N/A" then value2 = ""
$	    if value .eqs. "(undefined)" then value = ""
$	    if value2 .eqs. "(undefined)" then value2 = ""
$	    if param2 .eqs. "Modem Control" .and. value2 .eqs. "None"
$	    then
$		value2 = "Disable"
$	    endif
$	    if param .eqs. "Backward" then value = ""
$	    if param2 .eqs. "Port Name" then param2 = "Name"
$	    if param2 .eqs. "Local Switch" then value2 = ""
$	    if param .eqs. "Break Ctrl" then value = ""
$	    if param2 .eqs. "Node/Host Limits" then param2 = "Node/Host Limit"
$	    if f$locate(param,lprotocols) .lt lprotocols_len
$	    then
$		port_num = "proto"
$		if param .eqs. "Mop" then value = ""
$	    endif
$	endif
$!
$! Lantronix reports multiple parameters separated by "/"
$!-------------------------------------------------------
$	if f$locate("/",param) .lt. f$length(param)
$	then
$	    param4 = param3
$	    value4 = value3
$	    param3 = param2
$	    value3 = value2
$	    param2 = f$edit(f$element(1,"/",param),"trim,compress")
$	    param = f$edit(f$element(0,"/",param),"trim,compress")
$	    value2 = f$edit(f$element(1,"/",value),"trim,compress")
$	    if value2 .eqs. "/" then value2 = ""
$	    value = f$edit(f$element(0,"/",value),"trim,compress")
$	    if param2 .eqs. "Nodenum" then value2 = ""
$	endif
$!
$!	Fix for bug in DS200
$!-----------------------------
$	if param .eqs. "Flow Control" .and. value .eqs. "None"
$	then
$	    value = "disabled"
$	endif
$!
$	if eps_flag
$	then
$	    if param .eqs. "Stop Bits" then param = "Stop"
$	    if param2 .eqs. "Stop Bits" then param2 = "Stop"
$	endif
$!
$	CStr = CStr1
$	If F$Extract( 0,5,param ) .eqs. "Ident" Then CStr = IdStr
$	if param .eqs. "Prompt"
$	then
$		CStr = IdStr
$		value = value + " "
$	endif
$!
$!	Lantronix Service SOJ Parameter
$!-------------------------------------
$	if param .eqs. "SOJ"
$	then
$	    CStr = IdStr
$	    if value .eqs. "<none>"
$	    then
$		value = ""
$	    else
$		value = """" + value + """"
$	    endif
$	endif
$	if value .nes. ""
$	then
$	    param = param - "(msec)" - "(sec)" - "(min)"
$	    oStr = f$fao(CStr, port_num, param, value)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$!
$	If param2 .eqs. "" Then goto read_next
$       If value2 .eqs. ":" Then goto read_next
$	if f$locate("/",param2) .lt. f$length(param2)
$	then
$	   param4 = param3
$	   value4 = value3
$	   param3 = f$edit(f$element(1,"/",param2),"trim,compress")
$	   param2 = f$edit(f$element(0,"/",param2),"trim,compress")
$	   value3 = f$edit(f$element(1,"/",value2),"trim,compress")
$	   if value3 .eqs. "/" then value3 = ""
$	   value2 = f$edit(f$element(0,"/",value2),"trim,compress")
$	endif
$       If F$length( value2 ) .le. 0 THEN goto param3
$!
$	CStr = CStr1
$	if param2 .eqs. "Prompt"
$	then
$		CStr = IdStr
$		value2 = value2 + " "
$	endif
$	if param2 .eqs. "EOJ"
$	then
$	    CStr = IdStr
$	    if value2 .eqs. "<none>"
$	    then
$		value2 = ""
$	    else
$		value2 = """" + value2 + """"
$	    endif
$	endif
$	param2 = param2 - "(msec)" - "(sec)" - "(min)"
$	if value2 .nes. ""
$	then
$	    oStr = f$fao(CStr, port_num, param2, value2)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$!
$param3:
$	If param3 .eqs. "" Then goto read_next
$       If value3 .eqs. ":" Then goto read_next
$	if f$locate("/",param3) .lt. f$length(param3)
$	then
$	   param4 = f$edit(f$element(1,"/",param3),"trim,compress")
$	   param3 = f$edit(f$element(0,"/",param3),"trim,compress")
$	   value4 = f$edit(f$element(1,"/",value3),"trim,compress")
$	   if value4 .eqs. "/" then value4 = ""
$	   value3 = f$edit(f$element(0,"/",value3),"trim,compress")
$	endif
$	if value3 .nes. ""
$	then
$	    CStr = CStr1
$	    if param3 .eqs. "Prompt"
$	    then
$		CStr = IdStr
$		value3 = value3 + " "
$	    endif
$	    param3 = param3 - "(msec)" - "(sec)" - "(min)"
$	    oStr = f$fao(CStr, port_num, param3, value3)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$	If param4 .eqs. "" Then goto read_next
$       If value4 .eqs. ":" Then goto read_next
$	if value4 .nes. ""
$	then
$	    CStr = CStr1
$	    if param4 .eqs. "Prompt"
$	    then
$		CStr = IdStr
$		value4 = value4 + " "
$	    endif
$	    param4 = param4 - "(msec)" - "(sec)" - "(min)"
$	    oStr = f$fao(CStr, port_num, param4, value4)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$	goto read_next
$!
$port_line:
$!
$! The line containing port number and name
$!
$	srvc = "N"
$	serv = "N"
$	pflg = "N"
$	char = "N"
$	port_num = "Port " + f$extract( 5, 2, f$element (0, ":", line))
$	write out "!"
$	write out "! ''port_num' Setup"
$	write sys$output "! ''port_num' setup "
$	write out "!"
$	if eps_flag then goto read_next
$	name = f$element( 1, ":", line ) - "Server"
$	name = F$Edit( name, "trim" )
$	If F$Length( Name ) .le. 0 Then goto read_next
$	CStr = IdStr
$	OStr = f$fao(CStr, port_num, "Username", name)
$	OStr = F$Edit( OStr, "trim" )
$	write out OStr
$	If Ver Then write sys$output OStr
$	goto read_next
$!
$! Parse through definition for service.
$!
$Service_St:
$	Srvc = "Y"
$	serv = "N"
$	char = "N"
$	Srvc_Enab = "N"
$	if eps_flag
$	then
$	    Srvc_Name = F$edit(F$Element(1,":",line),"TRIM,COMPRESS")
$	    Srvc_Name = F$edit(f$element(0," ",Srvc_Name),"TRIM")
$	    I = F$locate("Ident:",line)
$	    line = f$extract(I,len,line)
$	    clue = "Ident"
$	else
$	    Srvc_Name = F$Element( 1, " ", line )
$	endif
$	write out "!"
$	port_num = "SERVICE " + Srvc_Name
$     	Srvc_Port = ""
$	Srvc_Id = ""
$       Srvc_variab = ""
$       Write Sys$output " Processing Service ''Srvc_Name'"
$	if eps_flag then goto parse_srvc
$	Goto read_next
$!
$! Check for service Identification string.
$!
$Parse_Srvc:
$	IF eps_flag .and. Srvc_Enab then goto char_parse
$	If Srvc_Enab Then Goto Srvc_Char
$	If clue .nes. "Ident" Then Goto Srvc_P
$	Srvc_Id = F$Extract( F$Locate( ":", line )+1, F$Length( line ), Line )
$	Srvc_Id = F$Edit( Srvc_Id, "trim" )
$	CStr = IdStr
$	if Srvc_id .nes. ""
$	then
$	    OStr = f$fao(CStr, Port_Num, clue, Srvc_Id)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$	Goto read_next
$!
$! Check which port(s) service is available on, if any at all.
$!
$Srvc_P:
$	if eps_flag
$	then
$	    line = f$edit(line,"TRIM,COMPRESS")
$	    len = f$length(line)
$	    clue = f$extract(0,5,line)
$	endif
$	If clue .nes. "Ports" Then Goto Srvc_R
$	Srvc_Port = F$Extract( F$Locate( ":", line )+1, F$Length( line ), line )
$	Srvc_Port = F$Edit( Srvc_Port, "trim" )
$	Cstr = Cstr1
$	if Srvc_id .nes. ""
$	then
$	    OStr = f$fao(CStr, Port_Num, clue, Srvc_Port)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$	Goto read_next
$!
$Srvc_R:
$	If clue .nes. "Ratin" Then Goto Srvc_E
$	Srvc_Rtng = F$Extract( F$Locate( ":", line )+1, F$Length( line ), line )
$	Srvc_Rtng = F$Element(0," ",F$Edit( Srvc_Rtng, "trim" ))
$	Cstr = Cstr1
$	if Srvc_id .nes. "" .and. Srvc_Rtng .nes. "N/A"
$	then
$	    OStr = f$fao(CStr, Port_Num, "Rating", Srvc_Rtng)
$	    OStr = F$Edit( OStr, "trim" )
$	    write out OStr
$	    If Ver Then write sys$output OStr
$	endif
$	if eps_flag
$	then
$	    i = f$locate("Ports:",line)
$	    line = f$extract(I,len,line)
$	    clue = "Ports"
$	    Goto Srvc_P
$	endif
$	Goto read_next
$!
$! Check for keyword 'enable' indicating list of enable characteristics
$! follows.
$!
$Srvc_E:
$	if eps_flag
$	then
$	    if clue .eqs. "Chara"
$	    then
$		line = line - "Characteristics:"
$		serv = "N"
$		char = "Y"
$		Srvc_Enab = "Y"
$		goto char_parse
$	    endif
$	endif
$	If clue .eqs. "Enabl" Then Srvc_Enab  = "Y"
$	Goto read_next
$!
$! Check to see which service characteristics should be enabled and which
$! should be disabled.  The only characteristics for which to check are
$! connections, queueing, and Password.  NOTE:  If password is enabled it
$! is not read here and thus in the setup command file will be reset to
$! "SYSTEM".
$!
$Srvc_Char:
$Srvc_Password:
$	K = F$Locate( "Password", Line )
$	If K .lt. F$Length( Line ) Then Srvc_variab = "Password ""SYSTEM"""
$	OStr = "Define Service " + Srvc_Name
$       If Srvc_Port .nes. "" THEN Ostr = Ostr + " Port " + Srvc_Port
$	Ostr = OStr + " Ident """ + Srvc_id + """ " + srvc_variab
$	Write Out "!"
$!	Write Out Ostr
$	If Ver Then Write Sys$Output Ostr
$	Srvc = "N"
$Srvc_connect:
$	K = F$Locate( "Connections", Line )
$	If K .lt. F$Length( Line ) Then Srvc_variab = " Connections enabled"
$       If K .eqs. F$length( Line ) Then Srvc_variab = " Connections disabled"
$	OStr = "Define Service " + Srvc_Name + Srvc_variab
$	Write Out Ostr
$	If Ver Then Write Sys$Output Ostr
$Srvc_queue:
$	K = F$Locate( "Queueing", Line )
$	If K .le. F$Length( Line ) Then Srvc_variab = " Queue enabled"
$!??       If K .eqs. F$length( Line ) Then Srvc_variab = " Queue disabled"
$	OStr = "Define Service " + Srvc_Name +  Srvc_variab
$	Write Out Ostr
$	Write Out "!"
$	If Ver Then Write Sys$Output Ostr
$	Goto read_next
$!
$! All characteristics for the server are disabled here.  Those which
$! are enabled currently for the designated server will result in addition
$! of other DEFINE commands which will re-enable these characteristics.
$! Characteristics for all the ports are handled in a similiar manner.
$! (i.e. All characteristics are disabled and those which should be are
$! subsequently re-enabled.)
$!
$server_info:
$	serv = "Y"
$	port_num = "Server"
$	write out "!"
$       Write Sys$output " Creating command file ''outfile'"
$	Write Sys$output " Processing ''port_num' ''server'"
$	write out "! ''port_num' ''server'"
$	write out "!"
$	write out "! Setup saved on ''f$time()'"
$	write out "!"
$	write out "! generalized characteristics:"
$       write out "!"
$	Write out "Define Server   Broadcast           Disabled"
$	Write out "Define Server   Dump                Disabled"
$	Write out "Define Server   Heartbeat           Disabled"
$	Write out "Define Server   Lock                Disabled"
$	write out "!"
$	write out "Define Port All AUTOBAUD            Disabled"
$	write out "Define Port All AUTOCONNECT         Disabled"
$	write out "Define Port All AUTOPROMPT          Disabled"
$	write out "Define Port All BROADCAST           Disabled"
$	write out "Define Port All INPUT FLOW CONTROL  Disabled"
$	write out "Define Port All OUTPUT FLOW CONTROL Disabled"
$	write out "Define Port All INACTIVITY LOGOUT   Disabled"
$	write out "Define Port All INTERRUPTS          Disabled"
$	write out "Define Port All LOSS NOTIFICATION   Disabled"
$	write out "Define Port All MESSAGE CODES       Disabled"
$	write out "Define Port All PASSWORD            Disabled"
$	write out "Define Port All SECURITY            Disabled"
$	write out "Define Port All VERIFICATION        Disabled"
$	write out "!"
$	write out "! specific characteristics follow"
$	write out "!"
$	goto read_next
$!
$!
$lserver_info:
$	port_num = "Server"
$	dec_flag = "N"
$	if eps_flag then goto read_next
$	eps_flag = "Y"
$	serv = "Y"
$	write out "Define server name ''server'"
$	!CONFIG_CM
$	write out "Define server announcements disabled"
$	write out "Define protocol ip disabled"
$	write out "Define protocol appletalk disabled"
$	write out "Define protocol lanmanager disabled"
$	write out "Define protocol netware disabled"
$	write out "Define protocol lat enabled"
$	write out "Define server groups all enabled"
$	write out "Define port all authorized groups all"
$	!EPSCONFIG
$	write out "Define port all access remote"
$!	write out "Define protocol lat license ??????
$	!EPS_ARSR_CRANE
$!	write out "Define port all character 7
$!	write out "Define port all stop 1
$!	write out "Define port all parity Even
$!	write out "Define port all flow none
$!	write out "Define port all speed 1200
$	!HPCONFIG
$!	write out "Define port 2 bitronics enabled
$!	Lanman
$!	write out "Define proto lanmanager enabled
$	goto read_next
$!
$!
$addr_fix:
$	I = F$Locate( "Name", Line )
$	line = F$Extract( I, len, line )
$	goto parse
$!
$char_line:
$	char = "Y"
$	serv = "N"
$	srvc = "N"
$	Goto read_next
$char_parse:
$	line = F$Edit( line, "trim,compress" )
$	value = "Enable"
$       If clue .eqs. "TSM> " Then goto char_done
$       If clue .eqs. "Port " Then goto char_done
$	if eps_flag
$	then
$	    if f$element(0,":",line) .eqs. "Incoming Logins"
$	    then
$		param = "Incoming"
$		temp = f$edit(f$element(1,":",line),"TRIM,COMPRESS")
$		temp1 = f$element(0," ",temp)
$		temp2 = f$element(1," ",temp)
$		temp3 = f$element(2," ",temp)
$		tlat = "FALSE"
$		ttel = "FALSE"
$		value = "NONE"
$		if temp1 .eqs. "LAT" .or. temp2 .eqs. "LAT"
$		then
$		    tlat = "TRUE"
$		    value = "LAT"
$		endif
$		if temp1 .eqs. "Telnet" .or. temp2 .eqs. "Telnet"
$		then
$		    ttel = "TRUE"
$		    if tlat
$		    then
$			value = "BOTH"
$		    else
$			value = "Telnet"
$		    endif
$		endif
$		CStr = CStr1
$		oStr = f$fao(CStr, port_num, param, value)
$		OStr = F$Edit( OStr, "trim" )
$		write out OStr
$		If Ver Then write sys$output OStr
$		if temp2 .eqs. "(No" .or. temp3 .eqs. "(No"
$		then
$		    value = "NOPASSWORD"
$		else
$		    value = "PASSWORD"
$		endif
$		oStr = f$fao(CStr, port_num, param, value)
$		OStr = F$Edit( OStr, "trim" )
$		write out OStr
$		If Ver Then write sys$output OStr
$		goto char_done
$	    endif
$	    if f$extract(0,4,line) .eqs. "SOJ:"
$	    then
$		char = "N"
$		srvc = "N"
$		goto parse_line
$	    endif
$	endif
$	I = 0
$!
$! Dec characteristics separated by comma, Lantronix by a space.
$!---------------------------------------------------------------
$	tag = ","
$	if eps_flag
$	then
$	    tag = " "
$	    line = f$edit(line,"trim,compress")
$	    len = f$length(line)
$	endif
$
$char_loop:
$	param = F$Element( I, tag, line )
$	if eps_flag .and. (param .eqs. tag) then goto read_next
$	if param .eqs. tag Then goto char_done
$	if param .eqs. "" Then goto read_next
$	param = f$edit( param, "trim" )
$	if eps_flag
$	then
$	    if param .eqs. "Queueing" then goto char_next
$	    if param .eqs. "Connections" then goto char_next
$	    if param .eqs. "Verify" then goto char_next
$	    if param .eqs. "Broadcast" .and. port_num .eqs. "Server"
$	    then
$		goto char_next
$	    endif
$	endif
$	CStr = CStr1
$	oStr = f$fao(CStr, port_num, param, value)
$	OStr = F$Edit( OStr, "trim" )
$	write out OStr
$	If Ver Then write sys$output OStr
$char_next:
$	I = I + 1
$	goto char_loop
$char_done:
$	char = "N"
$	goto read_next
$!
$! Execution is complete.
$!
$done:
$	set noon
$	If openflg THEN close in
$	If openflg THEN close out
$       file = f$search(cmd_file)
$       if file .nes. "" then delete 'cmd_file';*
$       file = f$search(tmp_file)
$       if file .nes. "" then delete 'tmp_file';*
$	exit
$err:
$     	write sys$output "Error exit..."
$	goto done
$ctrly:
$	write sys$output "<CNTRL> Y exit..."
$	goto done
$!
$! One of the temporary files for which an attempt was made to create,
$! already exists.
$!
$file_exists:
$       write sys$output "Unable to create ''file' - File Exists"
$       exit
$nopriv:
$	write sys$output "You require OPER and SYSPRV to run this command file"
$	exit
