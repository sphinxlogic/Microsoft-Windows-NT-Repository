$ if f$search ("sav_src.dir").nes.""
$ then	if f$search ("src.dir").nes."" then rename src.dir work.dir	/log
$	rename sav_src.dir src						/log
$ endif
$ @addheader
$ rename src.dir sav_src
$ rename work.dir src
$ backup AA.README,CTCS.COM,DESCRIP.MMS,EMON$CFG.CFG,EMON$HELP.HLB,-
ESTCS.PAS,HISTO.TXT,INSTALL.TXT,LINK.COM,-
FREEWARE_README.TXT,FREEWARE_DEMO,relform.txt,-
MAKE.COM,MKFILTER.COM,MKFREEWARE.com,MKKIT.COM,MKSRC.COM,-
MKSRC.DAT,NET$EVENT_LOCAL.NCL,-
!PEEK_ETH.EXE,PEEK_PROTO.EXE,READ_INTCTR.EXE,-
[.ALPHA]*.*;,[.DOC],[.DRIVER_PATCH],[.SRC],[.TEST],[.VAX], -
c$include:TYPES.H	-	! last since search-lists create havoc
	$1$dua7:[anonymous.freeware_cd]emon021-2.bck/save_set/prot=w:R/log
$!
$ rename src.dir work	/log
$ rename sav_src.dir src/log
