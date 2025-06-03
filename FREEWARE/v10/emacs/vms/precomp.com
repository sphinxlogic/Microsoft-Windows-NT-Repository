$! Generated automatically from PRECOMP.COM_IN by configure.
$!
$! VMS command file to define the `ccom' command.
$! This version allows the current directory to be part of a search list.
$!
$ srcdir_dev := EMACS_LIBRARY:
$ srcdir_dir := 000000.VMS
$ vmssrcdir := EMACS_LIBRARY:[000000.VMS]
$! This is just to make absolutelly sure we don't get any local definition
$! of __result
$ __result = ""
$ delete/symbol/local __result
$! Let's shorten the paths as much as possible...
$ @'vmssrcdir'canonicaldir 'vmssrcdir' __result
$ vmssrcdir = __result
$ @'vmssrcdir'canonicaldir 'srcdir_dev'['srcdir_dir'.-.src] __result
$ srcdir = __result
$ @'vmssrcdir'canonicaldir 'srcdir_dev'['srcdir_dir'.-.oldXMenu] __result
$ oldxmenudir = __result
$
$ CC := CC
$ CFLAGS := /PREFIX=ALL /NESTED=INCLUDE/DEBUG
$ C_SWITCH_MACHINE := 
$ C_SWITCH_X_MACHINE := 
$ C_SWITCH_SYSTEM := /NOOPTIMIZE /STANDARD=VAXC
$ C_SWITCH_X_SYSTEM := 
$ C_SWITCH_SITE := 
$ C_SWITCH_X_SITE := 
$ C_DEBUG_SWITCH := 
$ sys_includes := 
$ x_includes := DECW$INCLUDE:
$
$ if sys_includes .nes. "" then -
	define /nolog sys 'sys_includes','srcdir',sys$disk:[]
$ if sys_includes .eqs. "" then -
	define /nolog sys 'srcdir',sys$disk:[]
$ if x_includes .nes. "" then -
	define /nolog x11 'x_includes'
$! sh log sys
$! sh log x11
$
$ ccom :== 'CC' /define=("""emacs""",HAVE_CONFIG_H"'extra_defs'")-
/include=(SYS$DISK:[],'srcdir','oldxmenudir','vmssrcdir') 'C_SWITCH_MACHINE' -
'C_SWITCH_X_MACHINE' 'C_SWITCH_SYSTEM' 'C_SWITCH_X_SYSTEM' 'C_SWITCH_SITE' -
'C_SWITCH_X_SITE' 'CFLAGS' 'C_DEBUG_SWITCH' "'extra_compile_flags'"
$ extra_defs = ""
$ extra_compile_flags = ""
$ exit
