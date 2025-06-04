$! X11 setup command file
$! Copyright (C) 1995,1996, Johannes Plass, dipmza@phsyik.uni-mainz.de
$!
$  x11_release = "(03/96)"
$!
$! Parameters:
$!	NOLOG		- suppress messages
$!	MINIMAL		- do a minimal setup, just to run the executables
$!	COMPILE		- define XMMK as foreign command
$!			  and all logicals needed during compilation.
$!	INSTALL		- by default logicals are defined in the process table.
$!			  In the presence of this parameter they are defined in the
$!			  system table instead.			  
$!			  Also INSTALLs the libraries.
$!	DISTRIB		- exclude certain definitions ...
$!	STATIC		- Build and use static object libraries rather than shareable 
$!			  images.
$
$  set on
$  on control_y    then goto   emergency_exit
$  on warning      then goto   emergency_exit
$
$  X11_PARAM 	=  F$EDIT("''P1',''P2',''P3',''P4',''P5',''P6',''P7',''P8'","UPCASE,COLLAPSE")
$
$  X11_SAY	= "WRITE_ SYS$OUTPUT"
$  X11_END_MESSAGE = "Runtime Setup Done"
$  if X11_PARAM - "NOLOG" .nes. X11_PARAM THEN X11_SAY = "! "+X11_SAY
$
$  IF_NOT_DIST	= ""
$  if X11_PARAM - "DISTRIB"  .nes. X11_PARAM then IF_NOT_DIST="! Excluded"
$  IF_DIST	= ""
$  'IF_NOT_DIST	  IF_DIST = "! Excluded"
$
$  IF_INST	= "! Excluded"
$  if X11_PARAM - "INSTALL" .nes. X11_PARAM then IF_INST=""
$
$  IF_NOT_STAT	= ""
$  if X11_PARAM - "STATIC"  .nes. X11_PARAM then IF_NOT_STAT="! Excluded"
$  IF_STAT	= ""
$  'IF_NOT_STAT	  IF_STAT = "! Excluded"
$
$			ltab	:= process
$  'IF_INST		ltab	:= system
$			d	:= define_/process/nolog
$			d_tc	:= define_/process/nolog/trans=conc
$			dl	:= define_/'ltab'/nolog
$			dl_tc	:= define_/'ltab'/nolog/trans=conc
$			ESC_[0,8]= 27
$
$!### WHERE ARE WE ?
$
$  temp		= f$environment("PROCEDURE")
$  tempname	= f$parse(temp,,,"NAME")+f$parse(temp,,,"TYPE")+f$parse(temp,,,"VERSION")
$  tempdev	= f$parse(temp,,,"NODE")+f$parse(temp,,,"DEVICE","NO_CONCEAL")
$  tempdir	= f$parse(temp,,,"DIRECTORY","NO_CONCEAL") -
  		  - ">[000000." - "]<000000." - "><000000." - "][000000." -
  		  - ">[" - "]<" - "><" - "]["-
  		  - "<" - ">" - "[" - "]"
$  tempdir	= "[" + tempdir + "]"
$  X11_ROOT	= tempdev+tempdir-tempname-".COMMAND]"
$
$!### BASIC DEFINITIONS
$
$  ARCHITECTURE	=	F$EDIT(F$GETSYI("ARCH_NAME"),"UPCASE")
$  IF_VAX	=	"! VAX" - "! ''ARCHITECTURE'"
$  IF_ALPHA	=	"! ALPHA" - "! ''ARCHITECTURE'"
$
$ 'X11_SAY "  ''ESC_'[1mX11 Setup''ESC_'[m ''x11_release': Executing ..."
$  
$!### BASIC X11 LOGICALS
$
$		DL_TC	X11_ROOT  		'X11_ROOT.]
$		DL	BITMAP_DIRECTORY	X11_ROOT:[BITMAPS]
$  'IF_VAX	DL	X11_LIBRARY		X11_ROOT:[EXE]
$  'IF_ALPHA	DL	X11_LIBRARY		X11_ROOT:[EXE_ALPHA]
$  
$!### redefining DECW$USER_DEFAULTS
$
$ table = f$trnlnm("DECW$USER_DEFAULTS",,,,,"TABLE_NAME")
$ if table .eqs. "" then table = "LNM$PROCESS_TABLE"
$ defaults = "_???_"
$ i = 0
$ repeat_trn:
$    trn = f$trnlnm("DECW$USER_DEFAULTS",table,i)
$    defaults = defaults + "," + trn
$    i = i+1
$    if trn .nes. "" then goto repeat_trn
$
$ x11_defaults = "X11_ROOT:[DEFAULTS]"
$ if defaults-x11_defaults .eqs. defaults
$ then
$    defaults = defaults + x11_defaults - "_???_,"
$    D  DECW$USER_DEFAULTS 'defaults
$ endif
$
$!############################### 
$!### X11 SOFTWARE 
$
$!### XMU/XAW3D LIBRARIES
$
$ if X11_PARAM - "MINIMAL" .eqs. X11_PARAM
$ then
$		DL	X11_DIRECTORY		DECW$INCLUDE
$		DL	XAW_DIRECTORY		X11_ROOT:[XAW3D]
$		DL	XMU_DIRECTORY		X11_ROOT:[XMU]
$  'IF_NOT_DIST	DL	X11R4_VMS_DIRECTORY	X11_ROOT:[X11R4_VMS] 
$  'IF_STAT	DL	XAW3DLIB		X11_LIBRARY:XAW3DLIB.OLB
$  'IF_STAT	DL	XMULIB			X11_LIBRARY:XMULIB.OLB
$ endif
$
$ 'IF_NOT_STAT	DL	XAW3DLIBSHR		X11_LIBRARY:XAW3DLIBSHR.EXE
$ 'IF_NOT_STAT	DL	XMULIBSHR		X11_LIBRARY:XMULIBSHR.EXE
$
$ 'IF_NOT_STAT 'IF_INST	SET NOON
$ 'IF_NOT_STAT 'IF_INST	INSTALL REPLACE XAW3DLIBSHR/OPEN/HEADER
$ 'IF_NOT_STAT 'IF_INST	INSTALL REPLACE XMULIBSHR/OPEN/HEADER
$ 'IF_NOT_STAT 'IF_INST	SET ON
$
$!### GHOSTSCRIPT
$
$ 'IF_NOT_DIST	DL	GS_LIB			"''X11_ROOT'.GS.SYS],''X11_ROOT'.GS.FONTS]"
$ 'IF_NOT_DIST		GS	:==		$X11_LIBRARY:GS
$
$!### GV-VMS
$
$			GV	:==		$X11_LIBRARY:GV
$
$!### ALICE
$
$ 'IF_NOT_DIST		AL	:==		$X11_LIBRARY:AL
$
$!### MOSAIC
$
$ 'IF_NOT_DIST		MOSAIC	:==		$X11_LIBRARY:MOSAIC
$! first index of MOSAIC_HOME must be of form device:[dir] to use the annotation facility
$ 'IF_NOT_DIST		D MOSAIC_HOME		'F$TRNLNM("DECW$USER_DEFAULTS")', -
						DECW$USER_DEFAULTS:
$
$!### XDVI
$
$ 'IF_NOT_DIST	DL 	XDVIFONTS		"TEX_PK_LN03:[%d]%f.%p/TEX_USER_PK_LN03:[%d]%f.%p"
$ 'IF_NOT_DIST	DL 	XDVISIZES		"150/180/210/240/270/300/329/360/432/518/622/746"
$ 'IF_NOT_DIST		XDVI	:==		$X11_LIBRARY:XDVI
$
$!### XFIG
$
$ 'IF_NOT_DIST	DL	XFIGLIBDIR		'X11_ROOT.DATA]
$ 'IF_NOT_DIST		XFIG	:==		$X11_LIBRARY:XFIG
$ 'IF_NOT_DIST		FIG2DEV	:==		$X11_LIBRARY:FIG2DEV
$ 'IF_NOT_DIST		! XFIG_PS_PRINT :==	PRINT/DELETE/QUEUE=my_ps_queue
$
$!### XRN
$
$ 'IF_NOT_DIST		XRN :==			$X11_LIBRARY:MXRN
$
$!### XV
$
$ 'IF_NOT_DIST		BGGEN :==		$X11_LIBRARY:BGGEN
$ 'IF_NOT_DIST		DECOMPRESS :==		$X11_LIBRARY:DECOMPRESS
$ 'IF_NOT_DIST		VDCOMP :==		$X11_LIBRARY:VDCOMP
$ 'IF_NOT_DIST		XCMAP :==		$X11_LIBRARY:XCMAP
$ 'IF_NOT_DIST		XV :==			$X11_LIBRARY:XV
$
$!### SET DISPLAY UTILITY
$
$ 'IF_NOT_DIST		SET_DISPLAY :==		@X11_ROOT:[COMMAND]SET_DISPLAY
$
$!############################### 
$!### Compile Time Definitions 
$
$ IF X11_PARAM - "COMPILE" .nes. X11_PARAM
$ THEN 
$
$    if F$SEARCH("SYS$SHARE:DECW$XTLIBSHRR5.EXE") .nes. ""
$    then
$       IF_X11R5 = ""
$       IF_X11R4 = "!"
$    else
$       IF_X11R5 = "!"
$       IF_X11R4 = ""
$    endif
$
$    CC_COMPILER_EXE="SYS$SYSTEM:DECC$COMPILER.EXE"
$    GET_VERSION_PROC="SYS$UPDATE:DECW$GET_IMAGE_VERSION.COM"
$    If F$SEARCH("''CC_COMPILER_EXE'") .NES. ""
$    then
$       CC_COMPILER_NAME:= DECC
$       CC_COMPILER_COMMAND="CC/DECC"
$       CC_COMPILER_VERSION="unknown"
$       If F$SEARCH("''GET_VERSION_PROC'") .NES. ""
$       then
$          @'GET_VERSION_PROC' 'CC_COMPILER_EXE' _X11TMP_
$          CC_COMPILER_VERSION = "''_X11TMP_'"
$          if "''_X11TMP_'" .nes. "" then DELETE_/SYMBOL/GLOBAL/NOLOG _X11TMP_
$       endif
$    else
$       CC_COMPILER_NAME:= VAXC
$       CC_COMPILER_COMMAND="CC/VAXC"
$       CC_COMPILER_VERSION="unknown"
$    endif
$    CC_COMPILER="__''CC_COMPILER_NAME'__=''CC_COMPILER_COMMAND'"
$
$    STATIC = ""
$    'IF_STAT STATIC = ",STATIC=1"
$
$    IF "''MMK'" .nes. ""
$    THEN
$                               MAKE_UTILITY_NAME="MMK"
$	'IF_X11R5		XMMK :== 'MMK'/IGN=WARN/MAC=("""''CC_COMPILER'""",X11R5=1'STATIC')
$	'IF_X11R4		XMMK :== 'MMK'/IGN=WARN/MAC=("""''CC_COMPILER'""",X11R4=1'STATIC')
$    ELSE
$                               MAKE_UTILITY_NAME="MMS"
$	'IF_VAX 'IF_X11R5	XMMK :== MMS/IGN=WARN/MAC=(__VAX__=1,"""''CC_COMPILER'""",X11R5=1'STATIC')
$	'IF_VAX 'IF_X11R4	XMMK :== MMS/IGN=WARN/MAC=(__VAX__=1,"""''CC_COMPILER'""",X11R4=1'STATIC')
$	'IF_ALPHA 'IF_X11R5	XMMK :== MMS/IGN=WARN/MAC=(__ALPHA__=1,"""''CC_COMPILER'""",X11R5=1'STATIC')
$	'IF_ALPHA 'IF_X11R4	XMMK :== MMS/IGN=WARN/MAC=(__ALPHA__=1,"""''CC_COMPILER'""",X11R4=1'STATIC')
$    ENDIF
$
$    tmp = "@X11_ROOT:[COMMAND]XMAKE"
$    'IF_DIST tmp = tmp + " DISTRIB "
$    'IF_STAT tmp = tmp + " STATIC "
$    XMAKE :==	'tmp'
$
$    X11_END_MESSAGE="Compile Setup Done"
$
$    if X11_PARAM - "NOLOG" .eqs. X11_PARAM
$    then
$                 'X11_SAY "  Architecture:      ",F$GETSYI("ARCH_NAME")," , ", F$GETSYI("VERSION")
$                 'X11_SAY "  Compiler:          ''CC_COMPILER_NAME'"
$                 'X11_SAY "  Compiler Version:  ''CC_COMPILER_Version'"
$                 'X11_SAY "  Make Utility:      ''MAKE_UTILITY_NAME'"
$       'IF_X11R5 'X11_SAY "  X11 Release:       R5"
$       'IF_X11R4 'X11_SAY "  X11 Release:       R4"
$                 'X11_SAY "  X11_ROOT:          ",F$TRNLNM("X11_ROOT")
$!                 show_ symbol XMMK
$                  show_ symbol XMAKE
$                 'X11_SAY "   "
$    endif
$
$ ENDIF
$
$!### EXIT
$
$  'X11_SAY "''ESC_'M''ESC_'[2K''ESC_'[1m  X11 Setup''ESC_'[m ''x11_release': ''X11_END_MESSAGE'"
$  EXIT_
$
$EMERGENCY_EXIT:
$  EXIT_ 
