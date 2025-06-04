$! DTDOC.COM
$!
$! Build documentation, run as an indirect command file.
$! Change at your own risk:
$	dousage		=	0
$!
$! The document maintenence programs are assumed to be stored in BIN:
$! which has been assigned by the user.
$!
$	getrno := $bin:getrno.exe	! Extract runoff from C source
$	getkwk := $bin:getkwk.exe	! Make kwik index runoff source
$	fixdoc := $bin:fixdoc.exe	! Cleanup after runoff
$	kwik   := $bin:kwik.exe		! Key word index program
$	was_verify := 'f$verify(1)'
$	set default [.source]
$	create usage.tmp		! Create a dummy usage file
$	eod
$! Extract documentation from library source modules.
$	getrno -c >rno.tmp -h dtlibh.rno -r *.h *.c	! Documentation
$	getkwk -c >getkwk.tmp *.h *.c			! Index source
$	kwik -t 16 -w 64 <getkwk.tmp >kwik.tmp		! Index kwik'ed
$	copy/concat dtlibi.rno,kwik.tmp index.tmp	! Index .rno
$	delete getkwk.tmp;*,kwik.tmp;*			! Delete temp files
$	if 'dousage' then -
	    getrno -cu >usage.tmp -h dtlibu.rno -r *.h *.c
$ 	copy/concat rno.tmp,usage.tmp,index.tmp dtlib.rno
$ 	delete rno.tmp;*,usage.tmp;*,index.tmp;*
$! Make the manual.
$	runoff/underline/out=doc.tmp	dtlib.rno
$	fixdoc <doc.tmp			>dtlib.doc
$	delete doc.tmp;*
$	purge *.rno				! Cleanup after ourselves
$	purge *.doc
$	rename dtlib.rno [-]			! Move .rno and .doc files
$	rename dtlib.doc [-]			! to [.dtlib] directory
$	set default [-]
$! Build readme.1st and decus.txt
$! The "teco" command cleans up Runoff file attributes.
$	runoff/right=8/out=readme.1st readme.rno
$	teco readme.1st
$	purge readme.1st
$	runoff/right=8/out=decus.txt/varient=abstract readme.rno
$	teco decus.txt
$	purge decus.txt
$	if .not. 'was_verify' then 'f$verify(0)'	! Documentation built
