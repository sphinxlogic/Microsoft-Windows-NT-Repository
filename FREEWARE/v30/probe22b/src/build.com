$! BUILD.COM V2.2B
$!
$! This procedure can be used to assemble/compile/link the pieces of four
$! programs: PROBE, ACQ, DMS, REPLAY and some associated files.
$!
$! By default, the programs PROBE, ACQ and DMS are either assembled (on VAX)
$! or compiled (on Alpha). To do this:
$!
$!	@BUILD
$!
$!                          *** IMPORTANT *** 
$!
$! In order to generate the REPLAY image, you need a VAXC compiler installed
$! on your system. DECC is untried with this software, although it may work
$! with /STANDARD=VAXC or some other directive. If you do not have VAXC or if
$! the compiler you are using produces errors, you will not be able to invoke
$! PROBE/PLAYBACK.
$!
$! On a VAX, you can link against the provided object [PROBE.VAX]REPLAY.OBJ
$! to create an image which can be put in [PROBE]. On Alpha, the fallback is
$! to use the DCL procedure [PROBE]FORMAT.COM to obtain a rough equivalent of
$! PROBE/PLAYBACK. If you wish to recompile the playback image, do:
$!
$!	@BUILD PLAYBACK
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ say := write sys$output
$ alpha = f$getsyi("HW_MODEL").ge.1024
$!
$ on error then goto HANDLE
$!
$ this_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE")
$ this_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")
$!
$ define sys_probe 'this_dev''this_dir'
$ upper  = f$parse("SYS_PROBE",,,"DIRECTORY") - "]" + ".-]"
$ define target    'f$parse("SYS_PROBE",,,"DEVICE")''upper'
$!
$ if (p1 .eqs. "PLAYBACK") then goto MAKE_REPLAY
$ if (p1 .eqs. "RELINK") then goto LINK_PROBE
$!
$! library/create/help target:probe sys_probe:probe
$! set file /prot=(s:rwe,o:rwe,g:re,w:re) target:probe.hlb
$!
$ message/object=sys_probe:msg		sys_probe:probe
$ set command/object=sys_probe:tbl	sys_probe:table
$!
$ if (alpha) then flag = "/NOFLAG"	! otherwise blank
$!
$ macro/nodebug/object=sys_probe:	sys_probe:probe	'flag'
$ macro/nodebug/object=sys_probe:	sys_probe:acq	'flag'
$ macro/nodebug/object=sys_probe:	sys_probe:dms	'flag'
$!
$LINK_PROBE:
$ link/nodebug/notrace/exec=target:	sys_probe:probe,sys_probe:msg,sys_probe:tbl
$ link/nodebug/notrace/exec=target:	sys_probe:acq,sys_probe:msg
$ link/nodebug/notrace/exec=target:	sys_probe:dms,sys_probe:msg
$!
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:probe.exe
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:acq.exe
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:dms.exe
$!
$ goto DONE
$!
$MAKE_REPLAY:
$ if (alpha)
$ then
$    say "PLAYBACK compilation is not supported on Alpha (DECC)"
$    say "Use the FORMAT.COM DCL procedure to examine a Probe recording"
$ else
$    message/object=sys_probe:msg2	sys_probe:replay
$    cc/nodebug/object=sys_probe:	sys_probe:replay
$    link/nodebug/notrace/exec=target:	sys_probe:replay,sys_probe:msg2,sys_probe:tbl,sys_probe:crtl/opt
$    set file /prot=(s:rwe,o:rwe,g:e,w:e) target:replay.exe
$ endif
$!
$DONE:
$ deassign sys_probe
$ @target:setup
$ deassign target
$!
$ exit
$!
$HANDLE:
$ say "The build did not complete normally, please review documentation at"
$ say "the beginning of BUILD.COM and try again. If all else fails, try"
$ say "contacting the author at:"
$ say ""
$ say "   Germain@Eisner.Decus.Org"
$ say ""
$ say "with as much details as possible (SET VERIFY before @BUILD etc)."
$!
$ exit
