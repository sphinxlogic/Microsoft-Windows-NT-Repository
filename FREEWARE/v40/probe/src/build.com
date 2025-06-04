$! BUILD.COM V2.3B
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
$! In order to generate the REPLAY image, you need a VAXC compiler on a VAX,
$! or to compile /STANDARD=VAXC with DECC. If you lack a C compiler, you can
$! link against the provided object [.VAX]REPLAY.OBJ or [.AXP]REPLAY.OBJ
$! (many thanks to David Mathog for his work on the Alpha port) to create an
$! image which can be put in [PROBE].
$!
$! Alternately, you can use the DCL procedure [PROBE]FORMAT.COM to obtain a
$! rough equivalent of PROBE/PLAYBACK.
$!
$! If you wish to recompile the playback image, do:
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
$ if (f$search("sys_probe:tbl.obj").eqs."") then set command/object=sys_probe:tbl sys_probe:table
$ message/object=sys_probe:msg2		sys_probe:replay
$ if (alpha)
$ then
$    cc/nodebug/object=sys_probe:/standard=vaxc/nomember	sys_probe:replay
$    link/nodebug/notrace/exec=target:	sys_probe:replay,sys_probe:msg2,sys_probe:tbl
$ else
$    cc/nodebug/object=sys_probe:	sys_probe:replay
$    link/nodebug/notrace/exec=target:	sys_probe:replay,sys_probe:msg2,sys_probe:tbl,sys_probe:crtl/opt
$ endif
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:replay.exe
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
