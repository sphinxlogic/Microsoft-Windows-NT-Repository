$! BUILD.COM v2.2 (modified for VMS Freeware CD)
$!
$! This procedure attempts to assemble/compile/link the pieces of 4 programs:
$! PROBE, ACQ, DMS, REPLAY and some associated files.
$!
$!                          *** IMPORTANT *** 
$! This procedure assumes that VAXC (DECC is untried with this software) is
$! available on your system. If you do not have VAXC or if the compiler you
$! are using generates errors, you can bypass compilation and link against
$! the object file provided with this kit by invoking the build process with
$! the parameter "NOCOMPILE", as in:
$!
$!	@BUILD NOCOMPILE
$!
$! By default, if the parameter is absent or incorrect, compilation is tried.
$! Thus, normal build would be requested by:
$!
$!	@BUILD
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ on error then goto HANDLE
$!
$ define sys_probe 'f$parse(f$environment("PROCEDURE"),,,"DEVICE")''f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")'
$ upper  = f$parse("SYS_PROBE",,,"DIRECTORY") - "]" + ".-]"
$ define target    'f$parse("SYS_PROBE",,,"DEVICE")''upper'
$!
$! library/create/help target:probe sys_probe:probe
$! set file /prot=(s:rwe,o:rwe,g:re,w:re) target:probe.hlb
$!
$ if (p1 .eqs. "LINK") then goto MAKE_IT
$ message/object=sys_probe:msg		sys_probe:probe
$ message/object=sys_probe:msg2		sys_probe:replay
$ set command/object=sys_probe:tbl	sys_probe:table
$!
$ macro/nodebug/object=sys_probe:	sys_probe:probe
$ macro/nodebug/object=sys_probe:	sys_probe:acq
$ macro/nodebug/object=sys_probe:	sys_probe:dms
$ if (p1 .nes. "NOCOMPILE") then cc/nodebug/object=sys_probe:	sys_probe:replay
$!
$MAKE_IT:
$ link/nodebug/notrace/exec=target:	sys_probe:probe,sys_probe:msg,sys_probe:tbl
$ link/nodebug/notrace/exec=target:	sys_probe:acq,sys_probe:msg
$ link/nodebug/notrace/exec=target:	sys_probe:dms,sys_probe:msg
$ link/nodebug/notrace/exec=target:	sys_probe:replay,sys_probe:msg2,sys_probe:tbl,sys_probe:crtl/opt
$!
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:probe.exe
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:acq.exe
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:dms.exe
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:replay.exe
$!
$ deassign sys_probe
$ @target:setup
$ deassign target
$!
$ exit
$!
$HANDLE:
$ say := write sys$output
$!
$ say "The build did not complete normally, please review documentation at"
$ say "the beginning of BUILD.COM and try again. If all else fails, try"
$ say "contacting the author at:"
$ say ""
$ say "   Germain@Eisner.Decus.Org"
$ say ""
$ say "with as much details as possible (SET VERIFY before @BUILD etc)."
$!
$ exit
