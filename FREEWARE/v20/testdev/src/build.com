$! BUILD.COM v2.1 (modified for VMS Freeware CD)
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
$ define sys_tools 'f$parse(f$environment("PROCEDURE"),,,"DEVICE")''f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")'
$ upper  = f$parse("SYS_TOOLS",,,"DIRECTORY") - "]" + ".-]"
$ define target    'f$parse("SYS_TOOLS",,,"DEVICE")''upper'
$!
$! library/create/help target:testdev sys_tools:testdev
$! set file /prot=(s:rwe,o:rwe,g:re,w:re) target:testdev.hlb
$!
$ if (p1 .eqs. "LINK") then goto MAKE_IT
$ message/object=sys_tools:msg		sys_tools:testdev
$ macro/nodebug/object=sys_tools:	sys_tools:testdev
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
