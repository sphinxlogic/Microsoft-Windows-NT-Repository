$!	BUILD_COMMON.COM
$!	V1.1				25-Jan-1995	IfN/Mey
$!+
$! Build SOCKETSHR.
$! Don't use it directly, SET DEFAULT to the implementation specific
$! directory (i.e. SET DEFAULT [.NETLIB]) and type '@BUILD'
$!
$! 17-Mar-1999	J. Malmberg	Fixed.  DECC was specified with /STAND=VAXC
$!				Source has been changed previous to me so
$!				that that would not work.
$! P1	- blank or LINK
$!-
$	save_ver = f$verify(0)
$set ver
$	on control_y then goto exit
$	on error then goto exit
$	set on
$!
$! Trace
$!
$	if f$type(trace) .eqs. "" then trace = ",TRACE"
$!
$! Check architecture
$!
$       arch = "VAX"
$       if f$getsyi("HW_MODEL") .ge. 1024 then $ arch = "ALPHA"
$
$!
$! Check compiler
$!
$	HAVEDECC = f$search("sys$system:decc$compiler.exe") .nes. ""
$       DEFAULTC = f$trnlnm("DECC$CC_DEFAULT")
$       if (DEFAULTC .eqs. "/VAXC")
$	then					! Use VAX C
$!$		cflags = "/list/show=all"
$		cflags = ""
$		if (HAVEDECC) then $ cflags = "/vaxc" + cflags
$		comp   = "VAXC"
$	else					! Use DEC C
$!$		cflags := /stand=vaxc/prefix=all/list/show=all
$!		cflags := /stand=vaxc/prefix=all
$		comp   = "DECC"
$	endif
$!$	mflags	:= /list/show=exp
$	mflags	= ""
$!
$! Read DESCRIP.MMS to get variables:
$!
$!	pack		- name of implementation, e.g. NETLIB
$!	packobjs	- list of objects to be build, comma separated.
$!
$	if f$trnlnm("CHAN") .nes. "" then close chan
$	continued = "F"
$	open/read chan descrip.mms
$ rdloop:
$	read/end=rdfin chan line
$	line2 = f$edit(line,"TRIM,UPCASE,COLLAPSE,UNCOMMENT")
$	if f$extract(0,1,line2) .eqs. "#" then goto rdloop
$	if continued
$	then
$		val = val + line2
$	else
$		sym = f$element(0,"=",line2)
$		val = f$element(1,"=",line2)
$	endif
$	continued = "F"
$	if sym .eqs. ""  .or.  val .eqs. "=" then goto rdloop
$	val = f$element(0,"\",val)
$	if f$element(1,"\",line2) .nes. "\" then continued = "T"
$	'sym' = "''val'"
$	goto rdloop
$ rdfin:
$	if f$trnlnm("CHAN") .nes. "" then close chan
$!
$	objs = "SI_SOCKET.OBJ," + packobjs
$	if P1 .eqs. "LINK" then goto link
$	v = f$verify(1)
$ LIBRARY/CREATE SOCKETLIB_'pack'_'arch'.OLB
$!'f$verify(v)'
$!
$! Compile the files
$!
$	n = 0
$ comploop:
$	file = f$element(n,",",objs)
$	if file .eqs. "," then goto compfin
$	n = n + 1
$	name = f$parse(file,,,"NAME")
$	d = ""
$	if name .eqs. "SI_SOCKET" then d = "[-]"
$	if name .eqs. "NTOH_HTON" then d = "[-]"
$	if name .eqs. "INET" then d = "[-]"
$	if name .eqs. "TRNLNM" then d = "[-]"
$	if name .eqs. "READDB" then d = "[-]"
$	v = f$verify(1)
$ CC 'cflags' /OBJECT='file' 'd''name' /DEFINE=('pack''trace')
$ LIBRARY/INSERT SOCKETLIB_'pack'_'arch'.OLB 'file'
$!'f$verify(v)'
$	goto comploop
$ compfin:
$!
$! On VAX compile the transfer vector
$!
$	if arch .eqs. "VAX"
$	then
$		v = f$verify(1)
$ MACRO /OBJECT=SOCKETSHR_XFR_VAX  [-]SOCKETSHR_XFR_VAX_'comp'.MAR 'mflags'
$!'f$verify(v)'
$	endif
$!
$! Link
$!
$ link:
$	@[-]link
$!
$ exit:	!'f$verify(save_ver)'
