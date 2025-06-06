$! Generated automatically from COMPVMSLIB.COM_IN by configure.
$ save_verif = 'f$verify(0)
$! VMS command file to recompile all VMSLIB .C files which need recompilation.
$! These are all .C files that have no .OBJ files or that are newer then the
$! corresponding .OBJ files.  This file is self contained and does not require
$! you to do anything before running it.
$!
$ CC := CC
$ CFLAGS := /PREFIX=ALL /NESTED=INCLUDE/DEBUG
$ ALLOCA := ,ALLOCA.OBJ
$ C_SWITCH_SYSTEM := /NOOPTIMIZE /STANDARD=VAXC
$ LD := link
$
$ version := 19.22
$ version_us := 19_22
$ configname := ALPHA-DEC-VMS6-1
$
$ sys_includes := 
$
$ vmssrcdir_dev := EMACS_LIBRARY:
$ vmssrcdir_dir := 000000.VMS
$ vmssrcdir := EMACS_LIBRARY:[000000.VMS]
$
$ UTILITIES := hackdebug.exe
$
$ LIBRARIES := VMSLIB.OLB
$ VMSLIB_MODULES := echo.obj, esctrans.obj, filutils.obj, glob.obj, -
hackargv.obj, misc.obj, ndir.obj, pipe.obj, utime.obj
$
$! This is just to make absolutelly sure we don't get any local definition
$! of __result
$ __result = ""
$ delete/symbol/local __result
$! Let's shorten the paths as much as possible...
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-.src] __result
$ srcdir = __result
$
$ ALL_CFLAGS := /define=(HAVE_CONFIG_H="""""" "'extra_defs'") -
/include=(sys$disk:[] "'extra_incl'") 'CFLAGS'
$ CPP_FLAGS := /define=(HAVE_CONFIG_H="""""" "'extra_defs'") -
/include=(sys$disk:[] "'extra_incl'") 'CPPFLAGS' 'CFLAGS'
$ extra_defs :=
$ extra_incl :=
$
$ if sys_includes .nes. "" then -
	define /nolog sys 'sys_includes',sys$disk:[]
$ if sys_includes .eqs. "" then -
	define /nolog sys sys$disk:[]
$
$ on error then goto ende
$ on severe_error then goto ende
$ on control_y then goto ende
$ on warning then continue
$
$ old = f$trnlnm("SYS$DISK")+f$directory()
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ set def 'proc_dir'
$
$ define __srcdir 'vmssrcdir' ! To get shorter lines...
$
$ if f$search("vmslib.olb") .eqs. "" then library/create/object vmslib.olb
$
$ __i = 0
$loop_vmslib:
$ __e = f$element(__i,",",VMSLIB_MODULES)
$ __i = __i + 1
$ if __e .eqs. "," then goto end_vmslib
$ __n = f$parse(__e,,,"NAME")
$ if f$search("__srcdir:"+__n+".c") .nes. ""
$  then
$   write sys$output "Compiling ",__n,"..."
$   'cc' 'cpp_flags' __srcdir:'__n'.c /obj='__n'.obj
$   library/replace/object vmslib.olb '__n'.obj
$   write sys$output "---------------"
$  endif
$ goto loop_vmslib
$end_vmslib:
$
$ if f$search("__srcdir:hackdebug.c") .eqs. "" then goto no_hackdebug
$ write sys$output "Compiling and linking HACKDEBUG..."
$ 'cc' 'cpp_flags' __srcdir:hackdebug.c /obj=hackdebug.obj
$ 'ld' 'ldflags' /exec=hackdebug.exe hackdebug.obj,vmslink.opt/opt
$ write sys$output "---------------"
$no_hackdebug:
$
$ if f$search("__srcdir:routines.tpu") .eqs. "" then goto no_routines
$ write sys$output "Compiling ROUTINES.TPU$SECTION..."
$ save_mesg = f$environment("MESSAGE")
$ set message/nofacility/noidentification/noseverity/notext
$ edit/tpu/nosect/nodisp /command=__srcdir:routines.tpu -
	/out=sys$disk:[]routines.tpu$section
$ set message'save_mesg'
$ write sys$output "---------------"
$no_routines:
$
$ende:
$ __tmp = $status
$ deassign __srcdir
$ set default 'old'
$ exit __tmp + 0*f$verify(save_verif)
