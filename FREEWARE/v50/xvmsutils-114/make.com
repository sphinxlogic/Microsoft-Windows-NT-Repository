$!---------------make.com for Xvmsutils---------------------------------------
$! make xvmsutils under OpenVMS
$!
$! In case of problems with the build you might want to contact me at
$! zinser@decus.decus.de(preferred) or eurmpz@eur.sas.com (Work)
$!
$ SAVE_VERIFY='F$VERIFY(0)
$ Make = ""
$!
$! Check for MMK/MMS
$!
$ If F$Search ("Sys$System:MMS.EXE") .nes. "" Then Make = "MMS"
$ If F$Type (MMK) .eqs. "STRING" Then Make = "MMK"
$!
$ if p1 .Eqs. "CLEAN" then goto clean
$ if p1 .Eqs. "CLOBBER" then goto clobber
$ if p1 .Eqs. "INSTALL" then goto install
$!
$!	Compile the X11 Unix and VMS Utils library
$!
$! Establish the Compiling Environment
$!
$ Cpu_Model = F$GetSYI("HW_MODEL")
$!
$ If Cpu_Model .gt. 1024
$ Then 
$	MACRO := MACRO/MIGRATION
$ EndIf
$!
$!  Get the compiler options via the logical name COPTS
$!
$ cflags = f$trnlnm("COPTS")
$!
$!
$! Get the compiler used
$!
$ decc = f$search("SYS$SYSTEM:DECC$COMPILER.EXE") .NES. "" -
         .and. CFLAGS - "/VAXC" .eqs. CFLAGS
$ all_cflags = CFLAGS
$ if decc 
$  then 
$   all_cflags = "/DECC/PREFIX=all " + all_cflags
$   makmac = "/macro=(__DECC__=1"
$ else
$   makmac = "/macro=(__VAXC__=1"
$ endif
$!
$!  Get the assembler options via the logical name MOPTS
$!
$ mac_options = f$trnlnm("MOPTS")
$!
$!  Get the linker options via the logical name LOPTS
$!
$ link_options = f$trnlnm("LOPTS")
$!
$!  Find out which X-Version we're running.  This will fail for older
$!  VMS versions (i.e., v5.5-1).  Therefore, choose DECWindows XUI for
$!  default.
$!
$ On Error Then oldx = 1
$ @sys$update:decw$get_image_version sys$share:decw$xlibshr.exe decw$version
$ if f$extract(4,3,decw$version).eqs."1.2"
$ then
$   oldx = 0
$ else
$   oldx = 1
$   makmac = ''makmac' + ",__oldx__=1"
$ endif
$!
$!  Find out which C compiler we're running.  
$!
$ On Error Then oldc = 1
$ if decc
$  then
$   @sys$update:decw$get_image_version sys$system:decc$compiler.exe decc$version
$   if f$extract(3,3,decc$version).ges."5.2"
$   then
$     oldc = 0
$   else
$     oldc = 1
$     makmac = ''makmac' + ",__oldc__=1"
$   endif
$ else  ! we're talking VAX C, which definitly is old ;-) 
$  oldc = 1 
$  makmac = ''makmac' + ",__oldc__=1"
$ endif
$ if (f$trnlnm("UCX$NETWORK").nes."")
$  then
$   ucx = 1
$   makmac = ''makmac' + ",__ucx__=1"
$ endif  
$!
$! Check out the VMS version we are using
$!
$ if f$extract(1,3,f$getsyi("Version")) .ges. "6.2"
$  then
$   vv62 = 1
$   makmac = ''makmac' + ",__vv62__=1"
$  else
$   vv62 = 0
$ endif
$ if f$extract(1,3,f$getsyi("Version")) .ges. "7.0"
$  then
$   vv7 = 1
$   makmac = ''makmac' + ",__vv7__=1"
$  else
$   vv7 = 0
$ endif
$ makmac = ''makmac' + ")"
$ On Error Then continue
$! CALL MAKE FILE.OBJ	"CC ''all_cflags' FILE.C"	FILE.C
$ write sys$output "Compiling VMS UNIX Emulation Utilites"
$  if (Make .nes. "")
$   then
$    'Make' 'makmac
$  else
$!
$!  Compile the "C" files
$!
$ CALL MAKE CHOOSE_WINDOW.OBJ	"CC ''all_cflags' CHOOSE_WINDOW.C"	CHOOSE_WINDOW.C
$ CALL MAKE CREATE_DIR.OBJ	"CC ''all_cflags' CREATE_DIR.C"		CREATE_DIR.C
$ CALL MAKE FATALERR.OBJ	"CC ''all_cflags' FATALERR.C"		FATALERR.C
$ CALL MAKE GETHOSTNAME.OBJ	"CC ''all_cflags' GETHOSTNAME.C"	GETHOSTNAME.C
$ CALL MAKE GETLOGIN.OBJ	"CC ''all_cflags' GETLOGIN.C"		GETLOGIN.C
$ CALL MAKE HPWD.OBJ		"CC ''all_cflags' HPWD.C"		HPWD.C
$ CALL MAKE LNM.OBJ		"CC ''all_cflags' LNM.C"		LNM.C
$ CALL MAKE LOWERCASE.OBJ       "CC ''all_cflags' LOWERCASE.C"		LOWERCASE.C
$ CALL MAKE PRINT_EVENT.OBJ	"CC ''all_cflags' PRINT_EVENT.C"	PRINT_EVENT.C
$ CALL MAKE PSEUDO_ROOT.OBJ	"CC ''all_cflags' PSEUDO_ROOT.C"	PSEUDO_ROOT.C
$ CALL MAKE STRDUP.OBJ  	"CC ''all_cflags' STRDUP.C"		STRDUP.C
$ CALL MAKE STRCASECMP.OBJ	"CC ''all_cflags' STRCASECMP.C"		STRCASECMP.C
$ CALL MAKE TIMER.OBJ		"CC ''all_cflags' TIMER.C"		TIMER.C
$ CALL MAKE TZSET.OBJ		"CC ''all_cflags' TZSET.C"		TZSET.C
$ CALL MAKE USLEEP.OBJ		"CC ''all_cflags' USLEEP.C"		USLEEP.C
$ CALL MAKE VALIDATE.OBJ	"CC ''all_cflags' VALIDATE.C"		VALIDATE.C
$ CALL MAKE VMS_STAT.OBJ	"CC ''all_cflags' VMS_STAT.C"		VMS_STAT.C
$!
$! Alpha needs some addional definitions
$!
$ If Cpu_Model .gt. 1024
$    Then
$	CALL MAKE STRINGS.OBJ		"MACRO/OBJ=STRINGS.OBJ ''mac_options' ARCH_DEFS+STRINGS.MAR"	STRINGS.MAR
$    Else
$	CALL MAKE STRINGS.OBJ		"MACRO ''mac_options' STRINGS.MAR"	STRINGS.MAR
$ Endif
$!
$! Only compile for X11R4 or later
$!
$ if oldx.eq.1
$  then
$     CALL MAKE XMISC.OBJ	"CC ''all_cflags' XMISC.C"	XMISC.C
$ endif
$!
$! Old DEC C (earlier than 5.2) or VAX C
$!
$ if oldc.eq.1
$  then
$     CALL MAKE TEMPNAM.OBJ	"CC ''all_cflags' TEMPNAM.C"	TEMPNAM.C
$ endif
$!
$! IOCTL for UCX (real TCP stacks have this anyhow)
$!
$ if ucx.eq.1
$    then
$       CALL MAKE IOCTL.OBJ		"CC ''all_cflags' IOCTL.C"		IOCTL.C
$ endif
$ if vv62.eq.0 .or. .not.decc
$   then
$    CALL MAKE GETOPT.OBJ	"CC ''all_cflags' GETOPT.C"	GETOPT.C
$ endif
$ if vv7.eq.0 .or. .not.decc
$   then
$    CALL MAKE DIRECTORY.OBJ    "CC ''all_cflags' DIRECTORY.C"  DIRECTORY.C        
$    CALL MAKE GETPWNAM.OBJ     "CC ''all_cflags' GETPWNAM.C"   GETPWNAM.c
$    CALL MAKE GETPWUID.OBJ     "CC ''all_cflags' GETPWUID.C"   GETPWUID.C
$    CALL MAKE POPEN.OBJ	"CC ''all_cflags' POPEN.C"	POPEN.C
$    CALL MAKE PUTENV.OBJ       "CC ''all_cflags' PUTENV.C"	PUTENV.C
$    CALL MAKE RANDOM.OBJ	"CC ''all_cflags' RANDOM.C"	RANDOM.C
$    CALL MAKE RAND48.OBJ       "CC ''all_cflags' RAND48.C"	RAND48.C 
$    CALL MAKE UNAME.OBJ	"CC ''all_cflags' UNAME.C"	UNAME.C
$    CALL MAKE UNIX_TIMES.OBJ	"CC ''all_cflags' UNIX_TIMES.C"	UNIX_TIMES.C
$    CALL MAKE UNLINK.OBJ       "CC ''all_cflags' UNLINK.C"     UNLINK.C
$ endif
$!
$ if f$search("XVMSUTILS.''OLB'") .Nes. ""
$ then
$	lib_options = "/REPLACE/LOG"
$ else
$	lib_options = "/CREATE/LOG"
$ endif
$!
$ write sys$output "Building VMS UNIX Emulation Library"
$ CALL MAKE XVMSUTILS.OLB	"LIBRARY ''lib_options' XVMSUTILS.OLB *.OBJ"	*.OBJ
$endif
$!
$!
$ exit
$!
$ Clobber:	! Delete executables, Purge directory and clean up object files and listings
$ Delete/noconfirm/log *.olb;*
$ Delete/noconfirm/log [-.lib]xvmsutils.olb;*
$!
$ Clean:	! Purge directory, clean up object files and listings
$ Purge
$ Delete/noconfirm/log *.lis;*
$ Delete/noconfirm/log *.obj;*
$!
$ exit
$!
$ Install:
$ exit
$!
$MAKE: SUBROUTINE   !SUBROUTINE TO CHECK DEPENDENCIES
$ V = 'F$Verify(0)
$! P1 = What we are trying to make
$! P2 = Command to make it
$! P3 - P8  What it depends on
$
$ If F$Search(P1) .Eqs. "" Then Goto Makeit
$ Time = F$CvTime(F$File(P1,"RDT"))
$arg=3
$Loop:
$	Argument = P'arg
$	If Argument .Eqs. "" Then Goto Exit
$	El=0
$Loop2:
$	File = F$Element(El," ",Argument)
$	If File .Eqs. " " Then Goto Endl
$	AFile = ""
$Loop3:
$	OFile = AFile
$	AFile = F$Search(File)
$	If AFile .Eqs. "" .Or. AFile .Eqs. OFile Then Goto NextEl
$	If F$CvTime(F$File(AFile,"RDT")) .Ges. Time Then Goto Makeit
$	Goto Loop3
$NextEL:
$	El = El + 1
$	Goto Loop2
$EndL:
$ arg=arg+1
$ If arg .Le. 8 Then Goto Loop
$ Goto Exit
$
$Makeit:
$ Set Verify
$ 'P2
$ VV='F$Verify(0)
$Exit:
$ If V Then Set Verify
$ENDSUBROUTINE
