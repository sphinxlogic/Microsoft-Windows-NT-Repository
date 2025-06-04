
$ Local_verify = F$VERIFY(0)
$!+
$! VAX SCAN kit building command file 
$! 
$! This procedure will assemble all necessary files in order 
$! to build a SCAN kit. 
$!-
$
$!+
$! Set up constants 
$!-
$Version_num = "1" 
$Update_level_num = "0"
$CLR :== "write sys$output ""[H[J"
$!+
$!  Set up for kit build
$!-
$CLR
$WRITE SYS$OUTPUT "VAX SCAN Kit Build procedure"
$!USE SYS
$Local_default = F$ENVIRONMENT ("DEFAULT")
$ON SEVERE_ERROR THEN CONTINUE
$SET DEF SCAN$$DISK:[SCAN.KIT.PREP_FILES] 
$
$
$!+
$! Determine if Fieldtest or SDC kit 
$!-
$Get_kit_type: 
$ WRITE SYS$OUTPUT ""
$ INQUIRE Kit_type "Kit type (Fieldtest = T, SDC = V)" 
$ Kit_type = F$EDIT (Kit_type,"UPCASE,COLLAPSE") 
$ IF (Kit_type .EQS. "T") .OR. (Kit_type .EQS. "V") THEN GOTO Cont1 
$ WRITE SYS$ERROR "Please specifiy T or V" 
$ GOTO Get_kit_type 
$Cont1: 
$
$!+
$! Determine the point release number 
$!- 
$Get_point_release_num: 
$ WRITE SYS$OUTPUT ""
$ INQUIRE Point_release_num "Point Release number (0-9)" 
$ Point_release_num = F$EDIT (Point_release_num,"UPCASE,COLLAPSE") 
$ IF (Point_release_num .GES. "0") .AND. -
     (Point_release_num .LES. "9") .AND. - 
     (F$LENGTH(Point_release_num) .EQ. 1) THEN GOTO Cont2 
$ WRITE SYS$ERROR "Please specify a number from 0 to 9" 
$ GOTO Get_point_release_num 
$cont2: 
$
$!+
$! If fieldtest determine update level
$!-
$ IF kit_type .NES. "T" THEN GOTO Cont2a
$Get_update_level_num:
$ WRITE SYS$OUTPUT ""
$ INQUIRE Update_level_num "Update level number (0-9)" 
$ Update_level_num = F$EDIT (Update_level_num,"UPCASE,COLLAPSE") 
$ IF (Update_level_num .GES. "0") .AND. -
     (Update_level_num .LES. "9") .AND. - 
     (F$LENGTH(Update_level_num) .EQ. 1) THEN GOTO Cont2a 
$ WRITE SYS$ERROR "Please specify a number from 0 to 9" 
$ GOTO Get_Update_level_num
$Cont2a:
$
$
$!+ 
$! Determine CMS stream
$!-
$cms_stream:
$ WRITE SYS$OUTPUT ""
$ INQUIRE cms_stream "Specify CMS stream ( D=DEVELOPMENT, M=MAINTENANCE)"
$ cms_stream = F$EDIT (cms_stream,"UPCASE,COLLAPSE") 
$ IF (cms_stream .NES. "M") .AND. (cms_stream .NES. "D") THEN GOTO cms_stream
$ IF cms_stream .EQS. "M" THEN cms_stream = "/GENERATION=MAINTENANCE"
$ IF cms_stream .EQS. "D" THEN cms_stream = ""
$
$!+ 
$! Determine directory name for kit
$!-
$ CLR
$ Kit_name = Kit_type + Version_num + point_release_num 
$ IF Update_level_num .NES. "0" THEN Kit_name = Kit_name + "-" + Update_level_num
$ WRITE SYS$OUTPUT "Creating Kit for: "+Kit_name
$ Kit_dir = "SCAN$$DISK:[SCAN.KIT."+Kit_name+"]" 
$ Small_kit_dir = "SCAN$$DISK:[SCAN.KIT."+Kit_name+"_SMALL]" 
$!
$Cont2b:
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Gathering kit files in ''Kit_dir'.  "
$ WRITE SYS$OUTPUT "All files now in ''Kit_dir' will be deleted.  "
$ INQUIRE OK "Is this OK (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Cont2b
$ IF OK .EQS. "Y" THEN GOTO Cont2c
$ INQUIRE Kit_dir "Alternate directory to use"
$ GOTO Cont2b
$
$!+
$! Determine if this is a small media kit
$!-
$Cont2c:
$ WRITE SYS$OUTPUT ""
$ INQUIRE OK "Build a small media kit (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Cont2c
$ IF OK .EQS. "Y" THEN GOTO small_kit_build
$
$!+
$!  Build or update the kit area
$!-
$Build_or_create:
$ WRITE SYS$OUTPUT ""
$ INQUIRE OK "Should the kit directory [SCAN.KIT.PREP_FILES] be updated (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Build_or_create
$ IF OK .NES. "Y" THEN GOTO Build_physical_kit
$
$!+
$! Clean-up prep-files directory
$!-
$ DEFINE/USER SYS$OUTPUT NL:
$ DEFINE/USER SYS$ERROR NL:
$ DELETE SCAN$$DISK:[SCAN.KIT.PREP_FILES]*.*;*
$
$Get_update_type:
$ WRITE SYS$OUTPUT ""
$ INQUIRE Only_release_notes "Should only the release notes and help be updated (Y/N)"
$ Only_release_notes = F$EDIT (Only_release_notes,"UPCASE,COLLAPSE")
$ IF (Only_release_notes .NES. "Y") .AND. (Only_release_notes .NES. "N") THEN GOTO Get_update_type
$!+
$!  Create or update kit directory
$!-
$ IF F$SEARCH ("SCAN$$DISK:[SCAN.KIT]"+Kit_name+".DIR") .NES. "" THEN GOTO Cont4
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Creating Kit directory: "+Kit_dir 
$ CREATE/DIR 'Kit_dir'
$Cont4: 
$
$ IF F$SEARCH ("SCAN$$DISK:[SCAN.KIT."+Kit_name+"]*.*") .EQS. "" THEN GOTO Cont4a
$ INQUIRE OK "Any changes in KIT$CMS library sources (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO cont4
$ IF OK .EQS. "N" THEN GOTO Cont3
$Cont4a:
$!+
$! Clean-up kit directory
$!-
$ DEFINE/USER SYS$OUTPUT NL:
$ DEFINE/USER SYS$ERROR NL:
$ DELETE 'kit_dir'*.*;*
$
$!+
$!  Create kit skeleton from KIT$CMS library
$!-
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Extracting from KIT$CMS library..."
$ WRITE SYS$OUTPUT ""
$ CMS
	SET LIBRARY	KIT$CMS
	FETCH/NOLOG	KITINSTAL.COM	""
	FETCH/NOLOG	EXAMPLES.DAT	""
	FETCH/NOLOG	LSEUPDATE_ENV.TPU	""
	FETCH/NOLOG	KITLSE.TPU      ""
	FETCH/NOLOG	SCANIVP.IVP	""
	FETCH/NOLOG	VUE$COMPILE.COM	""
	FETCH/NOLOG	SCANFILES.DAT	""
	FETCH/NOLOG	CAPS.*	""
	FETCH/NOLOG	CLEAR.COM	""
	FETCH/NOLOG	COMPRESS.*	""
	FETCH/NOLOG	COMMENT.*	""
	FETCH/NOLOG	CONTINUE.COM	""
	FETCH/NOLOG	CRLF.*	""
	FETCH/NOLOG	CRYPT.*	""
	FETCH/NOLOG	DCL.*	""
	FETCH/NOLOG	DISPLAY.COM	""
	FETCH/NOLOG	RECORD.*	""
	FETCH/NOLOG	RECOVERY.SCN	""
	FETCH/NOLOG	REPLACE.*	""
	FETCH/NOLOG	TIME.*	""
	FETCH/NOLOG	TRACEBACK.*	""
	FETCH/NOLOG	TREES.*	""
	FETCH/NOLOG	HANOI.*	""
	FETCH/NOLOG	KEYS.INC	""
	FETCH/NOLOG	LOGFILE.RFO	""
	FETCH/NOLOG	MACRO.*	""
	FETCH/NOLOG	MMS*.*	""
	FETCH/NOLOG	*.BLI	""
	FETCH/NOLOG	*.REQ	""
	FETCH/NOLOG	PROGRAM.*	""
	EXIT
$
$ copy/nolog  KITINSTAL.COM, -
	EXAMPLES.DAT, -
	LSEUPDATE_ENV.TPU, -
	SCANIVP.IVP, -
	SCANFILES.DAT, -
	CAPS.*, -
	CLEAR.COM, -
	COMPRESS.*, -
	COMMENT.*, -
	CONTINUE.COM, -
	CRLF.*, -
	CRYPT.*, -
	DCL.*, -
	DISPLAY.COM, -
	RECORD.*, -
	RECOVERY.SCN, -
	REPLACE.*, -
	TIME.*, -
	TRACEBACK.*, -
	TREES.*, -
	HANOI.*, -
	KEYS.INC, -
	LOGFILE.RFO, -
	MACRO.*, -
	MMS*.*, -
	*.BLI, -
	*.REQ, -
	PROGRAM.*, -
	VUE$COMPILE.COM	-
	'kit_dir'
$Cont3:
$
$!+
$!  Create a help library for SCAN kit
$!
$!	SCANHLP.DAT ----> SCANHLP.RNH
$!	SCANHLP.RNH ----> SCANHLP.HLP
$!	SCANHLP.HLP ----> SCANHLP.HLB
$!
$!  This piece is from the previous kithelp.com 
$!-
$ CLR
$ CMS SET LIB/NOLOG KIT$CMS
$ CMS FETCH/NOLOG	RELEASE_V'Version_num''Point_release_num'.RNO ""
$ CMS SET LIB SCAN$$DISK:[SCAN.COMPILER.SRC.CMS]
$ CMS FETCH/NOLOG SCANHLP.DAT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT " Creating the help library and release notes... "
$ WRITE SYS$OUTPUT ""
$ DEFINE/NOLOG SCAN$DAT SCANHLP.DAT
$ DEFINE/NOLOG SCAN$RNH SCANHLP.RNH
$ DEFINE/NOLOG SCAN$HLP SCANHLP.HLP
$ RUN SCANCOM$:CONVRT
$ DSR/OUTPUT SCAN$RNH
$ RUN SCANCOM$:REMJUNK
$ PURGE SCAN$RNH,SCAN$HLP
$!+
$!  Prepare the release notes
$!-
$ DSR RELEASE_V'Version_num''Point_release_num'.RNO/OUTPUT=SCAN0'Version_num''Point_release_num'.RELEASE_NOTES
$ RUN SCANCOM$:REMJUNK
$ PURGE SCAN$HLP
$!
$!  Insert the help files to the help library
$!
$ LIB/CREATE/HELP SCANHLP.HLB 
$ LIB/INSERT/HELP SCANHLP.HLB SCANHLP.HLP
$!LIB/DATA=REDUCE/HELP SCANHLP.HLB
$ PURGE SCANHLP.HLB
$
$!+
$!  Create help library for DWCI
$!-
$ CMS SET LIB SCAN$$DISK:[SCAN.COMPILER.SRC.CMS]
$ CMS FETCH/NOLOG SCAN$DWCI_GENERIC_HELP.RNH ""
$ CMS FETCH/NOLOG SCAN$DWCI_HELP.RNH ""
$ DSRPLUS SCAN$DWCI_GENERIC_HELP.RNH
$ DSRPLUS SCAN$DWCI_HELP.RNH
$ LIBRARY/CREATE=(KEYSIZE:30)/HELP SCAN$DWCI.HLB SCAN$DWCI_HELP.HLP, -
  SCAN$DWCI_GENERIC_HELP.HLP
$!+
$!  Copy the new Help libraries to the kit directory
$!-
$ copy/log SCAN0'Version_num''Point_release_num'.RELEASE_NOTES 'Kit_dir'
$ copy/log SCANHLP.HLB 'Kit_dir'
$ copy/log SCAN$DWCI.HLB 'Kit_dir'
$!+
$!  Invoke editor to modify release notes name in SCANFILES.DAT
$!-
$ CLR
$ WRITE SYS$OUTPUT "You must now manually correct the SCANFILES.DAT file to "
$ WRITE SYS$OUTPUT "reference the file: SCAN0"+Version_num+Point_release_num+".RELEASE_NOTES"
$ WRITE SYS$OUTPUT "You will put in LSEDIT with file to be modified."
$ WRITE SYS$OUTPUT ""
$ INQUIRE/NOPUNC OK "Press <RETURN> to enter LSEDIT..."
$ DEFINE/USER SYS$INPUT SYS$OUTPUT
$ LSEDIT SCANFILES.DAT
$
$ IF Only_release_notes .EQS. "Y" THEN GOTO Build_physical_kit
$
$!+
$!  Create SCAN.ENV from SCAN.LSE
$!-
$ CLR
$! CMS SET LIB/NOLOG SCAN$$DISK:[SCAN.COMPILER.SRC.CMS]
$! CMS FETCH/NOLOG SCAN.LSE 'cms_stream'
$! WRITE SYS$OUTPUT ""
$! WRITE SYS$OUTPUT "Creating SCAN.ENV file..."
$! WRITE SYS$OUTPUT ""
$! LSEDIT/NOSECTION -
$!	/NODISPLAY -
$!	/NOENVIRONMENT -
$!	/NOSYSTEM -
$!	/COMMAND=KITLSE.TPU -
$!	/INIT=SCAN.LSE
$! copy/log SCAN.ENV 'kit_dir'
$ Def_env_dir = "TOOL$:"
$ write sys$output "We need to build SCAN.ENV using LSE V2.2.  Since the"
$ write sys$output "cluster is running V2.3, we need to build this on another"
$ write sys$output "system, then copy the file here."
$get_env_dir:
$ inquire Env_dir "Where is SCAN.ENV? <''Def_env_dir'>"
$ IF Env_dir .NES. "" THEN GOTO Cont_env
$ Env_dir = Def_env_dir
$Cont_env:
$ IF F$SEARCH (Env_dir+"SCAN.ENV") .NES. "" THEN GOTO Copy_env
$ WRITE SYS$ERROR "The file "+Env_dir+"SCAN.ENV not found"
$ Goto get_env_dir
$Copy_env:
$ copy/log 'Env_dir'SCAN.ENV 'Kit_dir'
$!
$!+
$!  Copy the .EXE file to  the KIT directory
$!-
$ write sys$output ""
$Get_exe_dir:
$ Def_exe_dir = "PROBJ$:"
$ IF cms_stream .EQS. "/GENERATION=MAINTENANCE" THEN Def_exe_dir = "MPROBJ$:"
$ INQUIRE Exe_dir -
"Where are SCANEXE.EXE, SCANMSG.EXE, SCAN$DWCI.EXE, and SCAN$DWCI.UID <''Def_exe_dir'>"
$ IF Exe_dir .NES. "" THEN GOTO Cont5
$ Exe_dir = Def_exe_dir
$Cont5:
$ IF F$SEARCH (Exe_dir+"SCANEXE.EXE") .NES. "" THEN GOTO Cont6
$ WRITE SYS$ERROR "The image "+Exe_dir+"SCANEXE.EXE not found"
$ GOTO Get_exe_dir
$Cont6:
$ IF F$SEARCH (Exe_dir+"SCANMSG.EXE") .NES. "" THEN GOTO Cont7
$ WRITE SYS$ERROR "The image "+Exe_dir+"SCANMSG.EXE not found"
$ GOTO Get_exe_dir
$Cont7:
$ IF F$SEARCH (Exe_dir+"SCAN$DWCI.EXE") .NES. "" THEN GOTO Cont8
$ WRITE SYS$ERROR "The image "+Exe_dir+"SCAN$DWCI.EXE not found"
$ GOTO Get_exe_dir
$Cont8:
$ IF F$SEARCH (Exe_dir+"SCAN$DWCI.UID") .NES. "" THEN GOTO Cont8a
$ WRITE SYS$ERROR "The file "+Exe_dir+"SCAN$DWCI.UID not found"
$ GOTO Get_exe_dir
$Cont8a:
$ copy/log 'Exe_dir'SCANEXE.EXE 'Kit_dir'
$ copy/log 'Exe_dir'SCANMSG.EXE 'Kit_dir'
$ copy/log 'Exe_dir'SCAN$DWCI.EXE 'Kit_dir'
$ copy/log 'Exe_dir'SCAN$DWCI.UID 'Kit_dir'
$Verify_SCAN_ident:
$ ANA/IMAGE/INTER/HEADER 'kit_dir'SCANEXE




.
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Please verify the the Image Identification field of"
$ WRITE SYS$OUTPUT "SCANEXE.EXE is correct as listed above."
$ WRITE SYS$OUTPUT ""
$ INQUIRE OK "Is the Image Identification correct (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Verify_SCAN_ident
$ IF OK .EQS. "Y" THEN GOTO Cont9
$ CLR
$ WRITE SYS$SYSTEM "*ERROR* -- Kit build cannot continue with incorrect ident"
$ WRITE SYS$SYSTEM "for SCAN Image.  Image must be relinked with the proper"
$ WRITE SYS$SYSTEM "ident.  Invoke KITBUILD when ident has been corrected"
$ GOTO Exit_kitbuild
$Cont9:
$
$ !+
$ ! Analyze the scanmsg image
$ !-
$Verify_SCANmsg_ident:
$ CLR
$ ANA/IMAGE/INTER/HEADER 'Kit_dir'SCANMSG


exit
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT  ""
$ WRITE SYS$OUTPUT  ""
$ WRITE SYS$OUTPUT "Please verify that the Image Identification field of"
$ WRITE SYS$OUTPUT "SCANMSG.EXE is correct as listed above."
$ WRITE SYS$OUTPUT  ""
$ INQUIRE OK "Is the Image Identification correct (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Verify_SCANmsg_ident
$ IF OK .NES. "Y" THEN GOTO Exit_Kitbuild
$
$
$
$!+
$!  Change default to kit directory for the rest of the build
$!-
$SET DEF 'Kit_dir'
$
$!+
$!  Invoke editor to clean-up kintinstal.com
$!-
$ CLR 
$ WRITE SYS$OUTPUT "You must now manually correct KITINSTAL.COM to announce"
$ WRITE SYS$OUTPUT "that this is VAX SCAN "+Kit_type+Version_num+"."+Point_release_num
$ WRITE SYS$OUTPUT "You will put in LSEDIT with file to be modified."
$ WRITE SYS$OUTPUT ""
$ INQUIRE/NOPUNC OK "Press <RETURN> to enter LSEDIT..."
$ DEFINE/USER SYS$INPUT SYS$OUTPUT
$ LSEDIT KITINSTAL.COM
$ 
$
$!+
$!  Invoke editor to modify SCANCLD.CLD
$!-
$ CLR 
$ CMS SET LIBR/NOLOG SCAN$$DISK:[SCAN.COMPILER.SRC.CMS]
$ CMS FETCH/NOLOG SCANCLD.CLD 'cms_stream'
$ IF .NOT. $STATUS THEN  -
	CMS FETCH/LOG SCANCLD.CLD /GEN=MAINTENANCE_BASE
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "You must now manually remove the comments from SCANCLD.CLD"
$ WRITE SYS$OUTPUT "You will put in LSEDIT with file to be modified."
$ WRITE SYS$OUTPUT ""
$ INQUIRE/NOPUNC OK "Press <RETURN> to enter LSEDIT..."
$ DEFINE/USER SYS$INPUT SYS$OUTPUT
$ LSEDIT SCANCLD.CLD
$
$!+
$! Purge the kit area 
$!-
$ CLR
$ DEFINE/USER SYS$OUTPUT NL:
$ DEFINE/USER SYS$ERROR NL:
$ DELETE SCAN$:[SCAN.KIT.PREP_FILES]*.*;*
$ WRITE SYS$OUTPUT "VAX SCAN Master Kit completed"
$ PURGE
$ DEASSIGN CRLF_STRIPPER_FILE
$
$!+
$!  Ask if Phyiscal kit is to be built
$!-
$Build_physical_kit:
$ CLR
$ WRITE SYS$OUTPUT ""
$ INQUIRE OK "Do you want to build a real kit now (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Build_physical_kit
$ IF OK .NES. "Y" THEN GOTO Exit_kitbuild
$
$!+
$!  Physical Kit build phase 
$!-
$ IF F$SEARCH (Kit_dir+"*.*") .NES. "" THEN GOTO Cont10
$ WRITE SYS$ERROR ""
$ WRITE SYS$ERROR "There are no files in "+Kit_dir+" for "+Kit_name
$ GOTO Build_or_create
$Cont10:
$ PURGE 'kit_dir'
$ WRITE SYS$OUTPUT ""
$ INQUIRE OK "Will you be building the kit on CLT (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Cont10
$ IF OK .NES. "Y" THEN GOTO get_remote_node
$
$!+
$!  Build on CLT, simply invoke build command file
$!-
$ CLR
$ SET DEF 'kit_dir'
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Your default directory is :"+kit_dir
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Set privileges and invoke KITBUILD.COM..."
$ Local_verify = F$VERIFY(Local_verify)
$ EXIT
$
$
$!+
$!  Build on node other that CLT.  Find out where and
$!  copy files to new node along with KITBUILD.COM 
$!-
$Get_remote_node:
$ CLR
$ INQUIRE Remote_node "On which node do you wish to build "
$ Remote_node = F$EDIT (Remote_node,"UPCASE,COLLAPSE")
$ INQUIRE Remote_account "Which account should be used on "'Remote_node'" <SCAN>"
$ IF Remote_account .EQS. "" THEN Remote_account = "SCAN"
$ INQUIRE Remote_password "What is the password"
$ CLR
$
$!+
$!  Create kit build directory on remote node if necessary
$!  and copy files over.
$!-
$ Remote_dir = Remote_node+""""+remote_account+" "+Remote_password+""""+"::[.KIT]
$ Remote_top_dir = Remote_node+""""+remote_account+" "+Remote_password+""""+"::"
$ IF F$SEARCH (Remote_top_dir+"KIT.DIR") .NES. "" THEN GOTO Cont12
$ CLR
$ WRITE SYS$OUTPUT "Cannot create a directory remotely.  Please create: "+Remote_node+"::["+Remote_account+".KIT]"
$ WRITE SYS$OUTPUT "When directory is created, reinvoke this procedure"
$ GOTO Exit_kitBuild
$Cont12:
$ IF F$SEARCH (Remote_dir+"*.*") .EQS. "" THEN GOTO Copy_files
$ WRITE SYS$OUTPUT "Files already exists in "+Remote_node+"::["+Remote_account+".KIT]"
$ INQUIRE OK "Delete existing files (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF OK .NES. "Y" THEN GOTO Get_remote_node
$
$!+
$! Copy over files
$!-
$Copy_files:
$ CLR
$ WRITE SYS$OUTPUT "Copying kit to "+Remote_node+"..."
$ DELETE/noconfirm 'Remote_dir'*.*;*
$ copy/log 'Kit_dir'*.* 'Remote_dir'
$ copy/log KITBUILD.COM 'Remote_dir'
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Build must continue on "+Remote_node+".  After login continue build"
$ WRITE SYS$OUTPUT "by doing a SET DEF [.KIT] and invoking "
$ WRITE SYS$OUTPUT "SCANCOM$:KITBUILD.COM."
$ WRITE SYS$OUTPUT ""
$ GOTO Exit_kitbuild
$
$
$!+
$!
$!-
$Small_kit_build:
$ CLR
$ WRITE SYS$OUTPUT ""
$ INQUIRE OK "Is the master kit in "'kit_dir'" complete (Y/N)"
$ OK = F$EDIT (OK,"UPCASE,COLLAPSE")
$ IF (OK .NES. "Y") .AND. (OK .NES. "N") THEN GOTO Small_kit_build
$ IF (OK .EQS. "Y") THEN GOTO Cont13
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Master kit must be completed before small media kit."
$ WRITE SYS$OUTPUT "Please re-invoke this procedure to build a master kit."
$ GOTO Exit_kitbuild
$Cont13:
$
$
$!+
$!  Create or update kit directory
$!-
$ IF F$SEARCH ("SCAN$$DISK:[SCAN.KIT]"+Kit_name+"_SMALL.DIR") .NES. "" THEN GOTO Cont14
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Creating Kit directory: "+Small_kit_dir
$ CREATE/DIR 'Small_kit_dir'
$Cont14:
$
$!+
$! Delete all file in the directory
$!-
$ DEFINE/USER SYS$OUTPUT NL:
$ DEFINE/USER SYS$ERROR NL:
$ DELETE 'Small_kit_dir'*.*;*
$
$!+
$! Create small media kit from master kit.
$!-
$ SET DEF 'Small_kit_dir'
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Copying files from master kit..."
$ copy/log 'Kit_dir'*.*;*  'Small_kit_dir'
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Fetching files specific to small media kit..."
$ CMS SET LIB/NOLOG KIT$CMS
$ CMS FETCH/NOLOG SCANFILES.DAT ""
$ CMS SET LIB/NOLOG SRC$
$ 
$!+
$! Create image library
$!-
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Creating and compressing SCAN_IMAGES.TLB..."
$ LIBR/CREATE/TEXT SCAN_IMAGES.TLB
$ LIBR SCAN_IMAGES.TLB SCANEXE.EXE,SCANMSG.EXE
$ LIBR/COMPRESS/DATA=REDUCE/TEXT SCAN_IMAGES.TLB
$ DELETE SCANEXE.EXE;*,SCANMSG.EXE;*
$
$!+
$! Purge the kit area 
$!-
$ CLR
$ DEFINE/USER SYS$OUTPUT NL:
$ DEFINE/USER SYS$ERROR NL:
$ DELETE SCAN$:[SCAN.KIT.PREP_FILES]*.*;*
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "VAX SCAN small media Kit completed"
$ PURGE
$!+
$!  Build on CLT, simply invoke build command file
$!-
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT ""
$ SET DEF 'Small_kit_dir'
$ WRITE SYS$OUTPUT "Your default directory is :"+Small_kit_dir
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Set privileges and invoke KITBUILD.COM "
$ WRITE SYS$OUTPUT "to build physical kit."
$ Local_verify = F$VERIFY(Local_verify)
$ EXIT
$
$
$Exit_kitbuild:
$!+
$!  Return to original directory and verify state
$!-
$
$ SET DEF 'Local_default'
$ Local_verify = F$VERIFY(Local_verify)
