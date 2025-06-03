$! Generated automatically from COMPXMENU.COM_IN by configure.
$ if p1 .eqs. "" 
$ then force = 0
$ else force = 1
$ endif
$ olddef = f$environment("DEFAULT")
$! We KNOW this script is on the [.VMS] subdirectory, so let's move to it
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ set def 'proc_dir'
$ set def [-.oldXMenu]
$
$ vmssrcdir_dev := EMACS_LIBRARY:
$ vmssrcdir_dir := 000000.VMS
$ vmssrcdir := EMACS_LIBRARY:[000000.VMS]
$! This is just to make absolutelly sure we don't get any local definition
$! of __result
$ __result = ""
$ delete/symbol/local __result
$! Let's shorten the paths as much as possible...
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-.oldXMenu] __result
$ define __srcdir '__result'
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
$
$ sys_includes := 
$ x_includes := DECW$INCLUDE:
$
$ all_cflags = C_SWITCH_SITE + C_SWITCH_SYSTEM + C_SWITCH_X_SITE +-
C_SWITCH_X_SYSTEM + C_SWITCH_MACHINE + CFLAGS +-
"/define=(""EMACS_BITMAP_FILES"")
$ ccom = cc + " " + all_cflags
$
$ EXTRA = "insque.obj"
$
$ if sys_includes .nes. "" then -
	define /nolog sys 'sys_includes',sys$disk:[]
$ if sys_includes .eqs. "" then -
	define /nolog sys sys$disk:[]
$ if x_includes .nes. "" then -
	define /nolog x11 'x_includes'
$ @'vmssrcdir'canonicaldir 'vmssrcdir_dev'['vmssrcdir_dir'.-.src.bitmaps] __result
$ define /nolog bitmaps '__result'
$
$ call recomp   Activate.c
$ call recomp	AddPane.c
$ call recomp	AddSel.c
$ call recomp	ChgPane.c
$ call recomp	ChgSel.c
$ call recomp	Create.c
$ call recomp	DelPane.c
$ call recomp	DelSel.c
$ call recomp	Destroy.c
$ call recomp	Error.c
$ call recomp	EvHand.c
$ call recomp	FindPane.c
$ call recomp	FindSel.c
$ call recomp	InsPane.c
$ call recomp	InsSel.c
$ call recomp	Internal.c
$ call recomp	Locate.c
$ call recomp	Post.c
$ call recomp	Recomp.c
$ call recomp	SetAEQ.c
$ call recomp	SetFrz.c
$ call recomp	SetPane.c
$ call recomp	SetSel.c
$ call recomp   XDelAssoc.c
$ call recomp   XLookAssoc.c
$ call recomp   XCrAssoc.c
$ call recomp   XDestAssoc.c
$ call recomp   XMakeAssoc.c
$ if EXTRA .nes. "" then call recomp	'EXTRA'
$
$ if f$search("LIBXMENU11.OLB") then delete LIBXMENU11.OLB.*
$ library/insert/create LIBXMENU11.OLB -
	Activate.obj, AddPane.obj, AddSel.obj, 	ChgPane.obj, ChgSel.obj, -
	Create.obj, DelPane.obj, DelSel.obj, Destroy.obj, Error.obj, -
	EvHand.obj, FindPane.obj, FindSel.obj, InsPane.obj, InsSel.obj, -
	Internal.obj, Locate.obj, Post.obj, Recomp.obj, SetAEQ.obj, -
	SetFrz.obj, SetPane.obj, SetSel.obj, XDelAssoc.obj, XLookAssoc.obj, -
	XCrAssoc.obj, XDestAssoc.obj, XMakeAssoc.obj
$ if (EXTRA .nes. "")
$  then
$   library/insert LIBXMENU11.OLB 'EXTRA'
$  endif
$
$ deassign __srcdir
$ set default 'olddef'
$ exit
$!
$! VMS subroutine file to recompile a .C file which needs recompilation.
$! This is a .C files that has no .OBJ file or that is newer
$! than the corresponding .OBJ file.  This file is self contained
$! and does not require you to do anything before running it.
$!
$ recomp: subroutine
$ if p1 .eqs. "" then goto sub_exit
$ on error then continue
$ name = f$parse(p1,,,"NAME")
$ file = f$search("__srcdir:"+name+".c",1)
$ cmd  = p2
$ if cmd .eqs. "" 
$ then
$   if "''ccom'" .eqs. "" then call precomp
$   cmd = ccom
$ endif
$ obj  = name + ".OBJ"
$ if (f$search(obj) .nes. "") .and. (.not. force)
$ then
$   if f$cvtime(f$file(file,"RDT")) .les. f$cvtime(f$file(obj,"RDT")) then -
	goto sub_exit
$ endif
$! obj = f$parse(file,,,"DEVICE") + f$parse(file,,,"DIRECTORY") + obj
$ write sys$output "Compiling ''name'..."
$ on error then goto error_exit
$ 'cmd' 'file' /object='obj'
$ if f$search(obj) .nes. "" then purge /nolog 'obj'
$error_exit:
$ write sys$output "---------------"
$sub_exit:
$ exit
$!
$ endsubroutine
