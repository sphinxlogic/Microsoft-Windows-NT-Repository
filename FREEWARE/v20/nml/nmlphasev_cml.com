$!
$!	Bidouille pour activer cml ou nml selon que l'appelant est
$!	phase IV ou V
$!
$!	auteur:	gg	24-jan-1995
$!
$ set ver
$	nwopt= ""
$	if f$mode().eqs."NETWORK"
$	then	x= f$trnlnm("SYS$NET")
$		if x.nes.""
$		then	opt= x-f$element (0,"/",x) - "/"
$			len= f$length (opt)
$			i= 0
$			nwopt= ""
$			v:= 'f$verify (0)'
$		bi:	if i.ge.len then goto print
$			nwopt= f$fao ("!2XL", f$cvui (8*i,8,opt))+nwopt
$			i= i+1
$			goto bi
$		print:	v= f$verify (v)
$			write sys$output "Network options: ", nwopt
$		else	show log/j/p
$		endif
$	endif
$!
$	vers = f$extract (f$length(nwopt)-8, 8, nwopt)
$	phase= 'f$extract (0,2,vers)
$!	sh sym/all
$	if phase.eq.4
$	then
$!		set process/dump
$!		define nml$log 255
$!		define nmlphasev_dns_directory ".public_names"
$!		define nmlphasev_identification "My identification"
$!		define nmlphasev_circuit_name sva-0
$		r sys$system:nmlphasev_nml
$	else	r sys$system:cml
$	endif
