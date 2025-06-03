$!
$! IDENTIFY.COM
$!
$	open/read d nm$data.b32
$loop:	read/error=noid d l
$	if f$locate("nm$gt_version",l) .eq. f$length(l) then goto loop
$	close d
$
$	l = f$element(1,"'",l)
$	if l .eqs. "'" then goto noid
$
$	v1 = f$int(f$elem(0,".",f$extr(1,4,f$elem(0,"-",f$elem(1," ",l)))))
$	v2 = f$int(f$elem(1,".",f$extr(1,4,f$elem(0,"-",f$elem(1," ",l)))))
$	v  = f$fao("!2ZL!1ZL",v1,v2)
$	goto creid
$
$noid:	write sys$output "% Cannot determine ident from NM$DATA"
$	l = "<unknown>"
$	v = "000"
$
$creid:	open/write i ident.opt
$	write i "  ! created automatically by identify.com"
$	write i "  ident=""",l,""""
$	close i
$
$	open/write i ident.rno
$	write i ".COMMENT created automatically by identify.com"
$	write i ".SET TEXTSTRING ident """,l,""""
$	write i ".SET TEXTSTRING vvu """,v,""""
$	close i
$
$	open/write i ident.com
$	write i "$! created automatically by identify.com"
$	write i "$ nm$ident == """,l,""""
$	write i "$ nm$vvu == """,v,""""
$	close i
$
$	purge ident.opt,ident.rno,ident.com
$
$!
$! End IDENTIFY.COM
$!
