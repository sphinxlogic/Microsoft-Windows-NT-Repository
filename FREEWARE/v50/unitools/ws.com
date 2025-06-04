$ verflg = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!  Working Set Information.
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!------------------------------------------------------------------------------
$
$ pid = ""
$ ctx = ""
$
$ say -
"			Working Set Information"
$ say ""
$ say -
"				  WS    WS    WS     WS   Pages Page"
$ say -
"Username    Processname   State   Extnt Quota Deflt  Size in WS faults  Image"
$ say ""
$
$ if p1 .nes. ""  then  pid = p1
$START:
$ if p1 .eqs. ""  then  pid = f$pid (ctx)
$ if pid .eqs. ""  then  goto  END
$ username	= f$getjpi ("''pid'","USERNAME")
$ if username .eqs. ""  then  goto  START
$ Procnam	= f$getjpi ("''pid'","PRCNAM")
$ imgnam	= f$parse (f$getjpi ("''pid'","IMAGNAME"),,,"NAME")
$ state		= f$getjpi ("''pid'","STATE")
$ wsdef		= f$getjpi ("''PID'","DFWSCNT")
$ wsquota	= f$getjpi ("''pid'","WSQUOTA")
$ wsextent	= f$getjpi ("''pid'","WSEXTENT")
$ wssize	= f$getjpi ("''pid'","WSSIZE")
$ glbpags	= f$getjpi ("''pid'","GPGCNT")
$ procpags	= f$getjpi ("''pid'","PPGCNT")
$ pagflts	= f$getjpi ("''pid'","PAGEFLTS")
$ pages		= glbpags + procpags
$
$ say f$fao ("!AS!15AS!5AS!5(6SL)!7SL!AS", -
		username,procnam,state,wsextent,wsquota,wsdef, -
		wssize,pages,pagflts," "+imgnam)
$ if p1 .eqs. ""  then  goto  START
$ 
$END:
$ EXIT 1 + 0 * f$verify(verflg)
