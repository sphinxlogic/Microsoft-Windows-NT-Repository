$	v = 'f$verify(0)'
$!
$! BUILD.COM - the compleat Nmail build procedure
$!
$	set noon
$	write sys$output "Nmail build procedure"
$	if f$mode() .eqs. "BATCH" then set process/name="Nmail Build"
$	@identify  		! make ident files
$	@ident			! just so we can display ident
$	write sys$output nm$ident
$	delete/symbol/global nm$ident
$	delete/symbol/global nm$vvu
$	write sys$output "[vax]"
$	@compile * vax		! compile vax sources
$	@kitlink * vax		! link vax images
$	write sys$output "[alpha]"
$	@compile * alpha	! compile alpha sources
$	@kitlink * alpha	! link alpha images
$	write sys$output "[common]"
$	@runoff	 		! make documentation
$!
$! End of BUILD.COM
$!
$exit:	exit (f$verify(v)*0)+1
