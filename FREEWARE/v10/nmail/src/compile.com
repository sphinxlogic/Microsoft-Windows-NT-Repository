$	v = 'f$verify(0)'
$!
$! COMPILE.COM - compile Nmail
$!
$! P1 = module name (for single-module compilation)
$!      defaults to "*"
$!
$! P2 = target architecture ("VAX" or "ALPHA")
$!	defaults to the type of the running system
$!
$! P3 = "/NOLIST" (default)
$!      "/LIST" 
$!
$! P4 = ""  (default)
$!      any Bliss command qualifiers
$!
$	set noon
$
$	modnam = "*"
$	if p1 .nes. "" then modnam = f$edit(p1,"upcase,trim")
$
$	host = f$edit(f$getsyi("arch_name"),"upcase,trim")
$	target = host
$	if p2 .nes. "" then target = f$edit(p2,"upcase,trim")
$	if target .nes. "VAX" .and. target .nes. "ALPHA" then exit 20 ! badparam
$
$	listq =	"/nolist"
$	if p3 .nes. "" then listq = p3
$
$	otherq = ""
$	if p4 .nes. "" then otherq = p4
$
$	lib    = "nm$library_" + target + ".l32"
$	olb    = "nmail_" + f$edit(target,"lowercase") + ".olb"
$	syslib = ""
$
$	if host .nes. target
$	then
$	    ___p_alpha = "migration"
$	    ___p_vax   = "vax"
$	    ___q_alpha = "a32"
$	    ___q_vax   = "b32"
$	    bliss   = "bliss /"   + ___q_'target'
$	    macro   = "macro /"   + ___p_'target'
$	    message = "message /" + target
$	    library = "library /" + target
$	    syslib  = target + "$library:"
$	endif
$
$	___x_vax   = "/check=(align,noshare) /syntax=2"
$	___x_alpha = "/check=(align,share,address)"
$	otherq = otherq + ___x_'target'
$
$	if f$search("''modnam'.obj") .nes. "" then delete 'modnam'.obj;*
$	if f$search("''modnam'.lis") .nes. "" then delete 'modnam'.lis;*
$	if f$search("''modnam'.req") .nes. "" then delete 'modnam'.req;*
$
$	if modnam .nes. "*" then goto single
$
$	if f$search(lib) .nes. "" then delete 'lib';*
$	if f$search(olb) .nes. "" then delete 'olb';*
$
$	call mesg nm$message
$	call bliss nm$library /library/nolist
$	rename nm$library.l32 'lib'
$	delete nm$message.req;*
$	call bliss nm$cancel
$	if target .eqs. "VAX"   then call macro nm$chmode
$	if target .eqs. "ALPHA" then call bliss nm$chmode
$	call bliss nm$clint
$	call bliss nm$data
$	call bliss nm$errorio
$	call bliss nm$fileio
$	call bliss nm$mail
$	call bliss nm$netio
$	call bliss nm$quesubs
$	call bliss nm$release
$	call bliss nm$repair
$	call bliss nm$reportio
$	call bliss nm$send
$	call bliss nm$show
$	call bliss nm$start
$	call bliss nm$stop
$	call bliss nm$subs
$	call bliss nm$symbiont
$	call bliss nm$trace
$	call bliss nm$uss
$
$	library/create/object 'olb' *.obj
$	if $status then delete *.obj;*
$	exit 1
$
$single:
$	lang = "bliss"
$	if modnam .eqs. "NM$CHMODE" .and. target .eqs. "VAX" then lang = "macro"
$	if modnam .eqs. "NM$MESSAGE" then lang = "mesg"
$	if modnam .eqs. "NM$LIBRARY" then goto 10
$	call 'lang' 'modnam'
$	library/replace/object 'olb' 'modnam'.obj
$	if $status then delete 'modnam'.obj;*
$	exit 1
$10:	call bliss nm$library /library/nolist
$	rename nm$library.l32 'lib'
$	purge 'lib'
$	exit 1
$
$bliss:	subroutine
$	write sys$output f$edit(p1,"lowercase")
$	if syslib .nes. "" then define /user sys$library 'syslib'
$	if p1 .nes. "NM$LIBRARY" .and. lib .nes. "" then define /user nm$library 'lib'
$	bliss 'listq' 'otherq' 'p1''p2'
$	endsubroutine
$
$macro:	subroutine
$	write sys$output f$edit(p1,"lowercase")
$	if syslib .nes. "" then define /user sys$library 'syslib'
$	macro 'listq' 'p1''p2'
$	endsubroutine
$
$mesg:	subroutine
$	write sys$output f$edit(p1,"lowercase")
$	message /nolist /nosymbols /sdl 'p1''p2'
$	sdl /nolist /language=(bliss='p1'.req) 'p1'
$	delete 'p1'.sdl;*
$	endsubroutine
$!
$! End COMPILE.COM
$!
