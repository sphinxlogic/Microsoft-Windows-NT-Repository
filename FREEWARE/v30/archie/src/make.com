$! --- MAKE.COM ---						!x='f$verify(0)'
$! Description:
$!  build the Archie client for VAX/VMS
$!
$! Written by Luke Brennan <brennan@coco.cchs.su.oz.au>
$!
$! Modifications:
$!    Date	Programmer	Reason for modification.
$! 20-Jan-92	   ldcb		Initial coding.
$!
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ Archie_EXECUTABLE := "archie.exe"
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! add a new call here with the appropriate new Archie server host(s)
$!
$ arg == 1
$ Call AddHost "archie.ans.net"		"(USA [NY])"
$ Call AddHost "archie.rutgers.edu"	"(USA [NJ])"
$ Call AddHost "archie.sura.net"	"(USA [MD])"
$ Call AddHost "archie.unl.edu"		"(USA [NE])"
$ Call AddHost "archie.mcgill.ca"	"(Canada)"
$ Call AddHost "archie.funet.fi"	"(Finland/Mainland Europe)"
$ Call AddHost "archie.au"		"(Australia)"
$ Call AddHost "archie.doc.ic.ac.uk"	"(Great Britain/Ireland)"
$ MAXHOSTS = arg - 1
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! add a new call here with any new source file(s)
$! (P2 should be "SUPPORT" if intended for supporting an unknown TCPIP)
$!
$ arg == 1
$ Call AddSource "AQUERY"
$ Call AddSource "ARCHIE"
$ Call AddSource "ATALLOC"
$ Call AddSource "DIRSEND"
$ Call AddSource "GET_PAUTH"
$ Call AddSource "GET_VDIR"
$ Call AddSource "PERRMESG"
$ Call AddSource "PROCQUERY"
$ Call AddSource "PTALLOC"
$ Call AddSource "REGEX"
$ Call AddSource "STCOPY"
$ Call AddSource "SUPPORT"
$ Call AddSource "VLALLOC"
$ Call AddSource "VL_COMP"
$ Call AddSource "VMS_SUPPORT" "SUPPORT"
$ MAXSOURCEFILES = arg - 1
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! add a call here with any supported TCP/IP implementations
$!  P1 = name of TCP/IP product, P2 = cc/define to set,
$!  P3 = logical to look for,    P4 = location of link/OPT,
$!  P5 = MINIMUM VERSION of TCP/IP to support
$!
$!  Multinet should be last, as it can 'fake' a UCX if you want it to, so
$!  UCX would come up as the 'real' net even though Multinet is used.
$!
$ arg == 1
$ Call AddTCPIP "UCX"        "UCX"         "UCX$DEVICE" "[.vms]ucx.opt"
$ Call AddTCPIP "WOLLONGONG" "WOLLONGONG"  "TWG$TCP"    "[.vms]woll.opt"
$ Call AddTCPIP "MULTINET"   "MULTINET_30" "MULTINET"   "[.vms]multi.opt" "V3.0"
$ MAXTCPIPTYPES = arg - 1
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ YES = (1.eq.1)
$ NO  = (1.eq.0)
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ GoSub get_command_line_args
$ GoSub check_for_GNU_cc
$ GoSub check_which_TCPIP
$ GoSub ask_nearest_ARCHIE_HOST
$ GoSub check_for_strings_H
$ GoSub set_cc_defines
$ GoSub do_compiles
$ If (LINKAGE_REQUIRED)
$ Then GoSub do_link
$ Else Write Sys$OutPut "ARCHIE is up to date."
$ EndIF
$ Exit
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$get_command_line_args:
$ cmdline = P1 + P2 + P3 + P4 + P5 + P6 + P7 + P8
$ If ((f$locate("DEBUG",cmdline) .ne. f$length(cmdline)) -
 .or. (f$locate("DBG",cmdline)   .ne. f$length(cmdline)))
$ Then debug := "/DeBug"
$ Else debug := "/NOdebug"
$ EndIF
$ If (f$locate("FORCE",cmdline) .ne. f$length(cmdline))
$ Then FORCEBUILD = YES
$ Else FORCEBUILD = NO
$ EndIF
$ If (f$locate("LINK",cmdline) .ne. f$length(cmdline))
$ Then FORCELINK = YES
$ Else FORCELINK = NO
$ EndIF
$ If ((f$locate("?",cmdline) .ne. f$length(cmdline)) -
 .or. (f$locate("H",cmdline) .ne. f$length(cmdline)))
$ Then
$   Write Sys$Output "Usage:"
$   Write Sys$OutPut "     @MAKE [<debug>|<force>|<link>|<help>]
$   EXIT
$ EndIF
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$CHECK_FOR_GNU_CC:
$ If (f$trnlnm("GNU_CC") .nes. "")
$ Then
$   cc := "GCC/Optimize/Include=([])"
$   gnu_cc = YES
$ Else
$   cc := "CC/Optimize=NOinline/Include=([])"
$   gnu_cc = NO
$ EndIF
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! for product P1, cc/define=P2 if logical P3 present on system.
$! Libs/option = P4 if present. ALL get disregarded if less than version P5
$!
$CHECK_WHICH_TCPIP:
$ tcpip_flag :=
$ tcpip_libs :=
$ NO_TCPIP_SUPPORT = YES
$ i = 1
$tcp_loop:
$ If (i .gt. MAXTCPIPTYPES) Then GoTo tcp_check_done
$ If (f$type(tcpip_P2_'i') .eqs. "") Then GoTo tcp_check_done
$ If (f$type(tcpip_P3_'i') .nes. "")
$ Then
$   tcpip_logical = tcpip_P3_'i'
$   If (tcpip_logical .nes. "")
$   Then					! logical to look for
$     If (f$logical(tcpip_logical) .nes. "")
$     Then
$       tcpip_flag = tcpip_P2_'i'
$       tcpip_flag = f$fao(",!AS=1",tcpip_flag)
$       NO_TCPIP_SUPPORT = NO
$       If (f$type(tcpip_P4_'i') .nes. "")
$       Then					! link/OPT file location
$  	  tcpip_linkOPTs = tcpip_P4_'i'
$         If (tcpip_linkOPTs .nes. "")
$         Then
$           If (f$search(tcpip_linkOPTs) .nes. "")
$           Then
$             tcpip_libs = tcpip_P4_'i'
$             tcpip_libs = f$fao("!AS/Option",tcpip_libs)
$           EndIF
$         EndIF
$       EndIF
$       If (f$type(tcpip_P5_'i') .nes. "")
$       Then					! minimum version specified
$         If (tcpip_P5_'i' .nes. "")
$         Then
$           GoSub CheckIfVersionOK
$           If VERSION_TOO_EARLY
$           Then				! too early.. use SUPPORT files
$             tcpip_flag :=
$             tcpip_libs :=
$             NO_TCPIP_SUPPORT = YES
$	      tcp_ver = tcpip_P5_'i'
$             tcp_name = tcpip_P1_'i'
$             Write Sys$OutPut f$fao( -
		   "Your version of !AS is earlier than !AS.",tcp_name,tcp_ver)
	      Write Sys$OutPut "MAKE will use STD support files instead."
$           EndIF
$         EndIF
$       EndIF
$     EndIF
$   EndIF
$ EndIF
$ i = i + 1
$ Goto tcp_loop
$tcp_check_done:
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ASK_NEAREST_ARCHIE_HOST:
$GoSub CLRSCN
$ Write Sys$OutPut f$fao("!AS!/!/", -
	"           Enter the number of the ARCHIE HOST nearest you.")
$!
$ i = 1
$_display_loop:
$ If (i .gt. MAXHOSTS) Then GoTo _display_done
$  Write Sys$OutPut f$fao("!2SL) !25AS  !AS",i,host_P1_'i',host_P2_'i')
$  i = i + 1
$ GoTo _display_loop
$_display_done:
$!
$ Assign/User_Mode/NOlog Sys$Command Sys$InPut
$_select_loop:				! get their selection
$ Read	 Sys$Command selection	-
	/End=_selection_made	-
	/Prompt="Enter number of your selection: "
$ If (selection .gt. MAXHOSTS)
$ Then
$   Write Sys$OutPut f$fao("!AS !2SL", "error: Options only go to", MAXHOSTS)
$   GoTo _select_loop
$ EndIF
$ ascii_string = f$edit(selection,"COLLAPSE,UPCASE")
$ ascii_char = f$extract(0,1,ascii_string)
$ If (.NOT. ("0" .les. ascii_char) .and. (ascii_char .les. "9"))
$ Then
$   Write Sys$OutPut -
	f$fao("error: Enter option NUMBER (up to !2SL)", MAXHOSTS)
$   Goto _select_loop
$ EndIF
$!
$ local_archie = host_P1_'selection'
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! If we're using VAXC then we need to grab STRINGS.H from SYS$LIBRARY.
$CHECK_FOR_STRINGS_H:
$ delete := delete
$ copy   := copy
$ If (f$search("strings.h") .nes. "") Then delete/nolog/noconfirm []strings.h;*
$ If .NOT. (GNU_CC) Then copy/noconfirm sys$library:string.h []strings.h
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! **NOTE** use of global symbol!!!
$! **NOTE** this is the only way I could pass it to the compile subroutine
$! **NOTE** without DCL and/or CC stripping off too many layers of quotes..
$! **NOTE** yeah.. I know.. It's ugly...  you work it out!! :-)
$SET_CC_DEFINES:
$ archie_host = " """"""ARCHIE_HOST=""""""""''local_archie'"""""""" """""" "
$ cflags :== /define=(debug=1,funcs=1,noregex=1'tcpip_flag','archie_host')
$!
$RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$DO_COMPILES:
$ GoSub CLRSCN
$ LINKAGE_REQUIRED == NO
$ If ("''f$type(Archie_EXECUTABLE)'" .nes. "")
$ Then
$   If (Archie_EXECUTABLE .nes. "")
$   Then If (f$search(Archie_EXECUTABLE) .eqs. "") Then LINKAGE_REQUIRED == YES
$   Else If (f$search("Archie.exe") .eqs. "") Then LINKAGE_REQUIRED == YES
$   EndIF
$ Else
$   If (f$search("Archie.exe") .nes. "") Then LINKAGE_REQUIRED == YES
$ EndIF
$ i = 1
$cc_loop:
$ If (i .gt. MAXSOURCEFILES) Then GoTo cc_done
$ source_file = source_P1_'i'
$ If ((f$type(source_P2_'i') .eqs. "") .or. (source_P2_'i' .eqs. ""))
$ Then Call Compile "''cc'" "''source_file'" "''debug'" 'FORCEBUILD'
$ Else
$   If ((NO_TCPIP_SUPPORT) .and. (source_P2_'i' .eqs. "SUPPORT"))
$   Then Call Compile "''cc'" "''source_file'" "''debug'" 'FORCEBUILD'
$   EndIF
$ EndIF
$ i = i + 1
$ GoTo cc_loop
$cc_done:
$ If (FORCELINK) Then LINKAGE_REQUIRED == YES
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$DO_LINK:
$ If (f$type(Archie_EXECUTABLE) .nes. "")
$ Then
$   If (Archie_EXECUTABLE .nes. "")
$   Then executable := /Exec='Archie_EXECUTABLE'
$   Else executable := /Exec=Archie.exe
$   EndIF
$ Else
$   executable := /Exec=Archie.exe
$ EndIF
$ i = 1
$ object_files :=
$object_files_loop:
$ If (i .gt. MAXSOURCEFILES) Then GoTo object_files_done
$ object_file = source_P1_'i'
$ If ((f$type(source_P2_'i') .eqs. "") .or. (source_P2_'i' .eqs. ""))
$ Then object_files := 'object_files'+'object_file'
$ Else
$   If ((NO_TCPIP_SUPPORT) .and. (source_P2_'i' .eqs. "SUPPORT"))
$   Then object_files := 'object_files'+'object_file'
$   EndIF
$ EndIF
$ i = i + 1
$ GoTo object_files_loop
$object_files_done:
$ If (f$extract(0,1,object_files) .eqs. "+")
$ Then object_files = f$extract(1,f$length(object_files),object_files)
$ EndIF
$ If (tcpip_libs .nes. "")
$ Then object_files = object_files + ","
$ EndIF
$!
$ Set Verify
$ Link'debug''executable' 'object_files''tcpip_libs'
$ x='f$verify(0)'
$!
$ Write Sys$OutPut " "
$ Write Sys$OutPut " "
$ Write Sys$OutPut "Done! Define the symbol ARCHIE & fire away."
$!
$ RETURN
$!
$! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$CLRSCN:
$If (f$GetDVI("TT:","TT_ANSICRT"))			! ANSI compatible?
$Then
$  CSI = "x["
$  CSI[0,8] = 27
$  CLS = CSI + "H" + CSI +"2J"
$  Write Sys$OutPut CLS
$Else
$  Write Sys$Output "''f$fao("!/!/!/!/!/!/!/!/!/!/!/")
$  Write Sys$Output "''f$fao("!/!/!/!/!/!/!/!/!/!/!/!/")
$EndIF
$Return
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$AddHOST: SUBROUTINE
$ host_P1_'arg' :== "''P1'"
$ host_P2_'arg' :== "''P2'"
$ arg == arg + 1	! *NOTE* global symbols used...
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$AddSOURCE: SUBROUTINE
$ source_P1_'arg' :== "''P1'"
$ source_P2_'arg' :== "''P2'"
$ arg == arg + 1	! *NOTE* global symbols used...
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$AddTCPIP: SUBROUTINE
$ tcpip_P1_'arg' :== "''P1'"
$ tcpip_P2_'arg' :== "''P2'"
$ tcpip_P3_'arg' :== "''P3'"
$ tcpip_P4_'arg' :== "''P4'"
$ tcpip_P5_'arg' :== "''P5'"
$ arg == arg + 1	! *NOTE* global symbols used...
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! **NOTE** cflags is a GLOBAL symbol due to problems with quoted /Defines
$! **NOTE** not passing down correctly.. (I gave up!)
$Compile: SUBROUTINE
$ YES = (1.eq.1)
$! --- do a Make of only that source which has been modified since its
$!     object code was generated or that is missing its object code.
$ cc       = "''P1'"
$ source   = "''P2'"
$ dbg      = "''P3'"
$ FORCED   = P4
$!
$source = source - ".C" + ".C"
$ t1 = f$search("''source'")				! source exists?
$  If (t1 .eqs. "") Then GoTo _error_source_missing	! YIPE!
$   source = source - ".C"
$    if (FORCED) Then GoTo _compile_the_source		! forced to compile
$     t1 = f$search("''source'.OBJ")			! object exist?
$     If (t1 .eqs. "") Then GoTo _compile_the_source	! object missing
$     t1 = f$file_attributes("''source'.OBJ","RDT")	! when was the OBJECT
$    t1 = f$cvtime(t1)					! produced? (rev date)
$   t2 = f$file_attributes("''source'.C","RDT")		! when was source last
$  t2 = f$cvtime(t2)					! modified?
$ If (t1 .ges. t2) Then GoTo _bypass_compile		! object still current
$_compile_the_source:
$ set verify
$ 'cc -
  'cflags -
  'dbg 'source
$ x='f$verify(0)'
$ LINKAGE_REQUIRED == YES
$  GoTo _cc_done
$_bypass_compile:					! didn't need to
$  GoTo _cc_done					! generate new OBJ file
$_error_source_missing:
$ Write Sys$OutPut "ERROR: unable to locate source file ''source'.C"
$_cc_done:
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$CHECKIFVERSIONOK:
$ required_version = tcpip_P5_'i'
$ tcpip_type = tcpip_P1_'i'
$ If ("MULTINET" .eqs. tcpip_type)
$ Then						! I know how to check MULTINET
$   If (f$search("MULTINET:MULTINET_VERSION.;") .nes. "")
$   Then
$     Open/share=READ fd MULTINET:MULTINET_VERSION.;
$     Read fd buffer
$     Close fd
$     v = buffer - "VERSION"
$     v = f$edit(V,"TRIM,COMPRESS")
$     If (v .ges. required_version)
$     Then VERSION_TOO_EARLY = NO
$     Else VERSION_TOO_EARLY = YES
$     EndIF
$   Else
$     VERSION_TOO_EARLY = YES
$   EndIF
$ Else						! don't know, so assume current
$   VERSION_TOO_EARLY = NO
$ EndIF
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


