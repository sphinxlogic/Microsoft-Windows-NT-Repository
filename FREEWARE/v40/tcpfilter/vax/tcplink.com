$ vaxc= p2
$ noaudit=p1
$ if noaudit
$ then sfx:= _noaudit
$ else sfx= ""
$ endif
$ if vaxc
$ then	csfx:= _vaxc
$	lnkopt:=, [-.src]vaxc/option
$ else	csfx= ""
$	lnkopt= ""
$ endif
$ link/exe=tcpfilter.exe tcpfilter'csfx',init_driver'sfx',-
		[-.src]c.opt/option 'lnkopt'
