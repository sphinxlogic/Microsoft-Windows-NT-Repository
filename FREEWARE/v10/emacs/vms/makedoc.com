$ ! VMS command file to create or update the file `DOC.' which contains
$ ! documentation strings for the functions and variables preloaded in Emacs.
$ ! This command file should be run when you build Emacs for the first time and
$ ! again if any documentation strings change in the source files listed here.
$
$ srcdir_dev := EMACS_LIBRARY:
$ srcdir_dir := 000000.VMS
$ srcdir := EMACS_LIBRARY:[000000.VMS]
$
$ on error then goto done
$ on severe_error then goto done
$ on control_y then goto done
$ on warning then continue
$
$! This is just to make absolutelly sure we don't get any local definition
$! of __result
$ __result = ""
$ delete/symbol/local __result
$! Let's shorten the paths as much as possible...
$ @'srcdir'canonicaldir 'srcdir_dev'['srcdir_dir'.-.src] __result
$ srcsrcdir = __result
$ @'srcdir'canonicaldir 'srcdir_dev'['srcdir_dir'.-.lisp] __result
$ lispdir = __result
$
$ etc := [-.etc]
$
$ old = f$environment("default")
$ on error then goto done
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ set def 'proc_dir' ! Just to make sure we're in the right place!
$ set def [-.src]
$
$ save_verif = f$verify(1)
$ @'srcdir'canonicaldir 'srcdir_dev'['srcdir_dir'.-.lib-src] __result
$ if f$search(__result+"make-docfile.c") .nes. "" then goto version44
$
$ libsrc := [-.lib_src]
$ doit := $sys$disk:'libsrc'make_docfile
$ rest1 := lisp_mode.elc text_mode.elc
$ rest2 := c_mode.elc buff_menu.elc byte_run.elc
$ rest3 := vms_patch.elc map_inp.elc vc_hooks.elc vms_date.elc
$ rest4 := vmsproc.elc vms_patch.elc
$ goto doit
$
$version44:
$ libsrc := [-.lib-src]
$ doit := $sys$disk:'libsrc'make-docfile
$ rest1 := lisp-mode.elc text-mode.elc
$ rest2 := c-mode.elc buff-menu.elc byte-run.elc
$ rest3 := vms-patch.elc map-ynp.elc vc-hooks.elc vms-date.elc
$ rest4 := vmsproc.elc vms-patch.elc
$
$doit:
$ doit -o [-.etc]DOC -d 'srcsrcdir'
$ orig_doit = doit
$ doit := 'doit' -a [-.etc]DOC -d 'srcsrcdir'
$ doit dispnew.obj scroll.obj xdisp.obj window.obj term.obj cm.obj
$ doit ,xterm.obj, xfns.obj, xfaces.obj, xmenu.obj, xselect.obj, xrdb.obj
$ doit emacs.obj keyboard.obj macros.obj keymap.obj sysdep.obj buffer.obj
$ doit filelock.obj insdel.obj marker.obj ,intervals.obj, textprop.obj minibuf.obj fileio.obj
$ doit dired.obj filemode.obj cmds.obj casetab.obj casefiddle.obj indent.obj
$ doit search.obj regex.obj undo.obj alloc.obj data.obj doc.obj editfns.obj
$ doit callint.obj eval.obj floatfns.obj fns.obj print.obj lread.obj
$ doit abbrev.obj syntax.obj , vmsmap.obj mocklisp.obj bytecode.obj process.obj
$ doit callproc.obj doprnt.obj ,getloadavg.obj vmsfns.obj vmsproc.obj
$ doit vmstime.obj termcap.obj, tparam.obj lastfile.obj ,vmsgmalloc.obj, vm-limit.obj ,alloca.obj
$
$ doit := 'orig_doit' -a [-.etc]DOC -d 'lispdir'
$ doit abbrev.elc files.elc
$ doit fill.elc
$ doit float-sup.elc
$ doit frame.elc mouse.elc select.elc scroll-bar.elc
$ doit help.elc indent.elc
$ doit isearch.elc lisp.elc
$ doit loaddefs.el page.elc
$ doit paragraphs.elc paths.el
$ doit register.elc replace.elc
$ doit simple.elc startup.elc
$ doit subr.elc window.elc
$ doit 'rest1'
$ doit 'rest2'
$ doit 'rest3'
$ doit 'rest4'
$
$done:
$ set default 'old'
$! 'f$verify(save_verif)
