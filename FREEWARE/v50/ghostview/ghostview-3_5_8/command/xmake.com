$! X11 make command file
$! Copyright (C) 1995,1996 Johannes Plass, dipmza@phsyik.uni-mainz.de
$!
$  ALL_TARGETS = "XMU,XAW3D,AL,GS,GV,X11R4_VMS,XPM,XFIG,XDVI,FVWM,MOSAIC,MXRN,XV,UTILITY"
$  ALL_DISTRIB_TARGETS = "XMU,XAW3D,GV"
$
$  set on
$  on control_y		then goto_ emergency_exit
$  on warning		then goto_ emergency_exit
$
$  MAKE_PARAM	=  F$EDIT("''P1',''P2',''P3',''P4',''P5',''P6',''P7',''P8'","UPCASE,COLLAPSE")
$ 
$
$  if MAKE_PARAM-"DISTRIB" .eqs. MAKE_PARAM
$  then 
$     KNOWN_TARGETS=ALL_TARGETS
$  else
$     KNOWN_TARGETS=ALL_DISTRIB_TARGETS
$  endif
$
$  if MAKE_PARAM-"ALL" .nes. MAKE_PARAM then MAKE_PARAM=MAKE_PARAM+KNOWN_TARGETS
$
$!### Usage Info
$
$ if (MAKE_PARAM-"DISTRIB" .eqs. ",,,,,,,") .or. (MAKE_PARAM-"HELP"-"?" .nes. MAKE_PARAM)
$ then
$ type_ sys$input

  X11 Make: Utility to build X11 executables and libraries

  Parameters: [LIST-OF-TARGETS][ALL][HELP][CLEAN][NOLOG]

    - LIST-OF-TARGETS is a list of targets either to be build or cleaned.
      The default is to build the specified targets.
    - 'HELP' or '?' will display this help page.
    - 'CLEAN'   will delete temporary files in the source directories of
                all targets specified in LIST-OF-TARGETS. Temporary files
                are target dependant.
    - 'ALL'     will either rebuild or clean all known targets. All known
                targets are listed below.
    - 'NOLOG'   will suppress (some) messages.
    - 'STATIC'  Build and use static object libraries instead of shared
                ones.

  Examples:
    $ xmake GV,XMU,XAW3D
       will build XMU, then XAW3D, then AL.
    $ xmake GV,XMU,CLEAN
       will remove temporary files in the source directories of AL, XMU

  Known targets:
$ write_ sys$output "    ''KNOWN_TARGETS'"
$ exit
$ endif
$
$!### Message Behaviour
$
$  _ESC_[0,8]=27
$  PROC_IDENT = "  "+_ESC_+"[1mX11 Make"+_ESC_+"[m:"
$  SAY	= "write_ SYS$OUTPUT"
$  if MAKE_PARAM-"NOLOG" .nes. MAKE_PARAM then SAY="! "+SAY
$  PROC_MSG = "'SAY' PROC_IDENT,"
$  DIR_MSG = "'IF_BUILD' 'SAY' ""            in directory "",f$environment(""DEFAULT"")"
$
$  'SAY "" 
$  'PROC_MSG	" Executing ..."
$
$!### Building or Cleaning ?
$
$  IF_STAT  = "! "
$  IF_NOT_STAT  = ""
$  if MAKE_PARAM-"STATIC" .nes. MAKE_PARAM then IF_STAT=""
$  'IF_STAT IF_NOT_STAT  = "! "
$  IF_CLEAN = "! " 
$  if MAKE_PARAM-"CLEAN" .nes. MAKE_PARAM then IF_CLEAN=""
$  IF_BUILD = "" 
$  if MAKE_PARAM-"CLEAN" .nes. MAKE_PARAM then IF_BUILD="! "
$  'IF_CLEAN on error then goto emergency_exit
$
$!### Detecting Architecture
$
$  ARCHITECTURE =       F$EDIT(F$GETSYI("ARCH_NAME"),"UPCASE")
$  IF_VAX       =       "! VAX" - "! ''ARCHITECTURE'"
$  IF_ALPHA     =       "! ALPHA" - "! ''ARCHITECTURE'"
$
$  'IF_VAX	EXE_EXT	= "EXE"
$  'IF_VAX	OPT_EXT	= "OPT"
$  'IF_VAX	OLB_EXT	= "OLB"
$  'IF_ALPHA	EXE_EXT	= "EXE_ALPHA"
$  'IF_ALPHA	OPT_EXT	= "OPT_ALPHA"
$  'IF_ALPHA	OLB_EXT	= "OLB_ALPHA"
$
$!### Detecting Location
$
$  old_dir	= f$environment("DEFAULT")
$  temp		= f$environment("PROCEDURE")
$  tempname	= f$parse(temp,,,"NAME")+f$parse(temp,,,"TYPE")+f$parse(temp,,,"VERSION")
$  this_dir	= temp-tempname
$
$  GO_MAKE	= "set default 'THIS_DIR'"
$  GO_ORIGIN	= "set default 'OLD_DIR'"
$  GO_SOURCE	= "set default 'SOURCE_DIR'"
$
$!### Main Build Routine
$
$	num=-1
$ BUILD_LOOP:
$       num=num+1
$	'GO_MAKE
$	TARGET_NAME = F$ELEMENT(num,",",KNOWN_TARGETS)
$	if TARGET_NAME .eqs. "," then goto_ SMOOTH_EXIT
$	if MAKE_PARAM-TARGET_NAME .nes. MAKE_PARAM
$	then
$          'PROC_MSG " starting production for target ''TARGET_NAME'"
$	   on warning	then goto_ NO_SUCH_TARGET
$	   gosub_ MAKE_'TARGET_NAME'
$          'IF_SOURCE 'PROC_MSG " completed production for target ''TARGET_NAME'"
$	endif
$ GOTO BUILD_LOOP
$
$ !### Target not available
$
$ NO_SUCH_TARGET:
$	'PROC_MSG	" Warning, target ''TARGET_NAME' not reachable"
$	GOTO BUILD_LOOP
$
$
$!###TARGET RULES ########################################################
$
$ MAKE_XMU: !########
$
$       SOURCE_DIR = "[-.XMU]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD 'IF_NOT_STAT TARGET = "XMULIBSHR"
$	'IF_SOURCE 'IF_BUILD 'IF_STAT	  TARGET = "XMULIB"
$	'IF_SOURCE 'IF_BUILD 'IF_NOT_STAT GOSUB_ RENAME_EXE_FILE
$	'IF_SOURCE 'IF_BUILD 'IF_STAT     GOSUB_ RENAME_OLB_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*,*.opt*;*,*.map*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_XAW3D: !######
$
$	SOURCE_DIR = "[-.XAW3D]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD 'IF_NOT_STAT TARGET = "XAW3DLIBSHR"
$	'IF_SOURCE 'IF_BUILD 'IF_STAT     TARGET = "XAW3DLIB"
$	'IF_SOURCE 'IF_BUILD 'IF_NOT_STAT GOSUB_ RENAME_EXE_FILE
$	'IF_SOURCE 'IF_BUILD 'IF_STAT     GOSUB_ RENAME_OLB_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "XAW3D_CLIENT"
$	'IF_BUILD GOSUB_ RENAME_OPT_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*,*.opt*;*,*.map*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_GV: !#########
$
$	SOURCE_DIR = "[-.GV.SOURCE]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "GV"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*,gv_class.dat;*,*.exe*;*,app-*.h;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_AL: !#########
$
$	SOURCE_DIR = "[-.AL.SOURCE]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "AL"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_GS: !#########
$
$	'IF_BUILD 'PROC_MSG	" Warning, no production rules for target ''TARGET_NAME'"
$	'IF_CLEAN 'PROC_MSG	" Warning, no cleaning rules for target ''TARGET_NAME'"
$	RETURN_
$
$ MAKE_XPM: !#########
$
$	SOURCE_DIR = "[-.XPM.LIB]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "XPM"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_OLB_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_ 
$
$ MAKE_FVWM: !#########
$
$	SOURCE_DIR = "[-.FVWM.FVWM]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "FVWM"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN 'GO_MAKE
$	'IF_SOURCE 'IF_CLEAN SOURCE_DIR = "[-.FVWM]"
$       'IF_SOURCE 'IF_CLEAN GOSUB SET_SOURCE_ENV
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "[...]*.*obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_X11R4_VMS: !#########
$
$	SOURCE_DIR = "[-.X11R4_VMS]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "XVMSUTILS"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_OLB_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_ 
$
$ MAKE_XFIG: !#########
$
$	SOURCE_DIR = "[-.XFIG]" 
$	GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "XFIG"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "FIG2DEV"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "[...]*.obj*;*,*.opt*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_XDVI: !#########
$
$ 'IF_VAX SOURCE_DIR = "[-.XDVI]"
$ 'IF_ALPHA SOURCE_DIR = "[-.XDVI]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "XDVI"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_MOSAIC: !#########
$
$	SOURCE_DIR = "[-.MOSAIC]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD @MAKE_MOSAIC_MMS
$	'IF_SOURCE 'IF_BUILD TARGET = "MOSAIC"
$       'IF_SOURCE 'IF_BUILD 'GO_MAKE
$	'IF_SOURCE 'IF_BUILD SOURCE_DIR = "[-.MOSAIC.SRC]"
$       'IF_SOURCE 'IF_BUILD GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "[...]*.obj*;*,[...]*.olb*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_MXRN: !#########
$
$	SOURCE_DIR = "[-.MXRN]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "MXRN"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_XV: !#########
$
$	SOURCE_DIR = "[-.XV]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD @MAKE_XV.COM
$	'IF_SOURCE 'IF_BUILD TARGET = "BGGEN"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "DECOMPRESS"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "VDCOMP"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "XCMAP"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "XV"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "XVPICTOPPM"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_INDIFF_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "[...]*.obj*;*,[...]*.olb*;*,[...]*.hlb*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ MAKE_UTILITY: !#########
$
$	SOURCE_DIR = "[-.UTILITY]"
$       GOSUB SET_SOURCE_ENV
$	'IF_SOURCE 'IF_BUILD XMMK
$	'IF_SOURCE 'IF_BUILD TARGET = "AD2C"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "PATHPATCH"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$	'IF_SOURCE 'IF_BUILD TARGET = "TRIM"
$	'IF_SOURCE 'IF_BUILD GOSUB_ RENAME_EXE_FILE
$       'IF_SOURCE 'IF_CLEAN CLEAN_FILES = "*.obj*;*"
$       'IF_SOURCE 'IF_CLEAN GOSUB_ CLEAN_DIR
$	RETURN_
$
$ !### Internal Routines
$
$ RENAME_OPT_FILE:
$	RENAME_/NOLOG/NOCONF 'TARGET'.'OPT_EXT' X11_LIBRARY:'TARGET'.OPT
$	'PROC_MSG	" renamed ''TARGET'.''OPT_EXT'	-> X11_LIBRARY:''TARGET'.OPT"
$	RETURN_
$
$ RENAME_INDIFF_EXE_FILE:
$	RENAME_/NOLOG/NOCONF 'TARGET'.EXE X11_LIBRARY:'TARGET'.EXE
$	'PROC_MSG	" renamed ''TARGET'.EXE	-> X11_LIBRARY:''TARGET'.EXE"
$	RETURN_
$
$ RENAME_EXE_FILE:
$	RENAME_/NOLOG/NOCONF 'TARGET'.'EXE_EXT' X11_LIBRARY:'TARGET'.EXE
$	'PROC_MSG	" renamed ''TARGET'.''EXE_EXT'	-> X11_LIBRARY:''TARGET'.EXE"
$	RETURN_
$
$ RENAME_INDIFF_OLB_FILE:
$	RENAME_/NOLOG/NOCONF 'TARGET'.OLB X11_LIBRARY:'TARGET'.OLB
$	'PROC_MSG	" renamed ''TARGET'.OLB	-> X11_LIBRARY:''TARGET'.OLB"
$	RETURN_
$
$ RENAME_OLB_FILE:
$	RENAME_/NOLOG/NOCONF 'TARGET'.'OLB_EXT' X11_LIBRARY:'TARGET'.OLB
$	'PROC_MSG	" renamed ''TARGET'.''OLB_EXT'	-> X11_LIBRARY:''TARGET'.OLB"
$	RETURN_
$
$ CLEAN_DIR:
$	'PROC_MSG	" cleaning ''CLEAN_FILES'"
$	'SAY'		"            in directory ",f$environment("DEFAULT")
$	SET NOON
$	DELETE_/LOG/NOCONF 'CLEAN_FILES'
$	SET ON
$	RETURN_
$
$ SET_SOURCE_ENV:
$	IF_SOURCE = ""
$       'GO_MAKE
$       TEMP = SOURCE_DIR +"*.*"
$       TEMP = f$parse(TEMP,,,,)
$       if temp .eqs. ""
$	then
$	   'PROC_MSG	" Warning, sources of target ''TARGET_NAME' not reachable"
$	   IF_SOURCE = "!"
$	   RETURN_
$	endif
$       'GO_SOURCE
$	'DIR_MSG
$	'IF_BUILD on warning	then goto_ emergency_exit
$	'IF_CLEAN on error	then goto_ emergency_exit
$	RETURN_
$
$ !### Smooth Exit
$
$ SMOOTH_EXIT:
$	'GO_ORIGIN
$	'SAY' "" 
$	'PROC_MSG	" Done"
$	exit_
$
$ !### Emergency exit
$
$ EMERGENCY_EXIT:
$	'GO_ORIGIN
$	'SAY' "" 
$	'PROC_MSG	" execution stopped during ''TARGET_NAME' production"
$	exit_ 1
