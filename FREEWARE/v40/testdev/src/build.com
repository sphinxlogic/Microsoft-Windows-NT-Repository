$! BUILD.COM v2.2
$!
$! This procedure attempts to assemble & link the pieces of TESTDEV and some
$! associated files.
$!
$! Format:
$!
$!	@BUILD
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ on error then goto HANDLE
$!
$ this_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE")
$ this_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")
$!
$ define/nolog sys_tools 'this_dev''this_dir'
$ upper = f$parse("SYS_TOOLS",,,"DIRECTORY") - "]" + ".-]"
$ define/nolog target 'f$parse("SYS_TOOLS",,,"DEVICE")''upper'
$!
$! library/create/help target:testdev sys_tools:testdev
$! set file /prot=(s:rwe,o:rwe,g:re,w:re) target:testdev.hlb
$!
$ if (p1 .eqs. "LINK") then goto MAKE_IT
$ message/object=sys_tools:msg		sys_tools:testdev
$ if (f$getsyi("HW_MODEL").ge.1024) then flag = "/NOFLAG"
$ macro/nodebug/object=sys_tools:	sys_tools:testdev 'flag'
$!
$MAKE_IT:
$ link/nodebug/notrace/exec=target:	sys_tools:testdev,sys_tools:msg
$ set file /prot=(s:rwe,o:rwe,g:e,w:e)	target:testdev.exe
$!
$ deassign sys_tools
$ @target:setup
$ deassign target
$!
$ exit
$!
$HANDLE:
$ say := write sys$output
$!
$ say "The build did not complete normally, please review documentation"
$ say "and try again. If all else fails, try contacting the author at:"
$ say ""
$ say "   Germain@Eisner.Decus.Org"
$ say ""
$ say "with as much details as possible (SET VERIFY before @BUILD etc)."
$!
$ exit
