$! Generated automatically from COMPILE.COM_IN by configure.
$ set noverify
$!
$! VMS command file to recompile all Emacs .C files which need recompilation.
$! These are all .C files that have no .OBJ files or that are newer then the
$! corresponding .OBJ files.  This file is self contained and does not require
$! you to do anything before running it.
$!
$! Arguments
$!   P1 any value forces recompilation of all .C files
$!
$! History
$!   Mukesh Prasad	18 Nov 1985
$!     original version
$!   Roland Roberts	30 Oct 1992	roberts@nsrl31.nsrl.rochester.edu
$!     subsidiary file ccom.com rolled into this version.
$!     allows default directory to be a search list (done in precomp.com)
$!       however, should normally be executed from [.vms] with no search list.
$!     added (restored?) option to force recompilation.
$!    
$ if p1 .eqs. "" 
$ then force = 0
$ else force = 1
$ endif
$ srcdir_dev := EMACS_LIBRARY:
$ srcdir_dir := 000000.VMS
$! This is just to make absolutelly sure we don't get any local definition
$! of __result
$ __result = ""
$ delete/symbol/local __result
$! Let's shorten the paths as much as possible...
$ @'srcdir_dev'['srcdir_dir']canonicaldir 'srcdir_dev'['srcdir_dir'] __result
$ vmssrc = __result
$ @'vmssrc'canonicaldir 'srcdir_dev'['srcdir_dir'.-.src] __result
$ define __srcdir '__result'
$
$ @precomp
$ olddef = f$environment("DEFAULT")
$! We KNOW this script is on the [.VMS] subdirectory, so let's move to it
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ set def 'proc_dir'
$ set def [-.src]
$ write sys$output "---------------"
$!
$! If there is any need for a listing file (foo.LIS), do the call like this:
$!
$!	$ call recomp foo.c "''ccom' /list/show=include"
$!
$ call recomp dispnew.c
$ call recomp frame.c
$ call recomp scroll.c
$ call recomp xdisp.c
$ call recomp window.c
$ call recomp term.c
$ call recomp cm.c
$! XOBJ
$ call recomp xterm.c
$ call recomp xfns.c
$ call recomp xfaces.c
$ call recomp xmenu.c
$ call recomp xselect.c
$ call recomp xrdb.c
$! end XOBJ
$ call recomp emacs.c
$ call recomp keyboard.c
$ call recomp macros.c
$ call recomp keymap.c
$ call recomp sysdep.c
$ call recomp buffer.c
$ call recomp filelock.c
$ call recomp insdel.c
$ call recomp marker.c
$! INTERVAL_OBJ
$ call recomp intervals.c
$ call recomp textprop.c
$! end INTERVAL_OBJ
$ call recomp minibuf.c
$ call recomp fileio.c
$ call recomp dired.c
$ call recomp filemode.c
$ call recomp cmds.c
$ call recomp casetab.c
$ call recomp casefiddle.c
$ call recomp indent.c
$ call recomp search.c
$ call recomp regex.c
$ call recomp undo.c
$ call recomp alloc.c
$ call recomp data.c
$ call recomp doc.c
$ call recomp editfns.c
$ call recomp callint.c
$ call recomp eval.c
$ call recomp floatfns.c
$ call recomp fns.c
$ call recomp print.c
$ call recomp lread.c
$ call recomp abbrev.c
$ call recomp syntax.c
$ call recomp mocklisp.c
$ call recomp bytecode.c
$ call recomp process.c
$ call recomp callproc.c
$ call recomp doprnt.c
$! GETLOADAVG_OBJ
$ call recomp getloadavg.c
$! end GETLOADAVG
$ call recomp vmsfns.c
$ call recomp vmsproc.c
$ call recomp vmsmap.c
$! termcapobj
$ call recomp termcap.c
$ call recomp tparam.c
$! end termcapobj
$ call recomp lastfile.c
$ call recomp vmstime.c
$! mallocobj
$ call recomp vmsgmalloc.c
$ call recomp vm-limit.c
$! allocaobj
$ extra_defs = ",""free""=""xfree"",""malloc""=""xmalloc"""
$ call recomp alloca.c
$ extra_defs = ""
$! end allocaobj
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
$ on error then continue
$ file = f$search("__srcdir:"+p1,1)
$ cmd  = p2
$ if cmd .eqs. "" 
$ then
$!   if ccom .eqs. "" then call precomp
$   cmd = ccom
$ endif
$ name = f$parse(file,,,"NAME")
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
