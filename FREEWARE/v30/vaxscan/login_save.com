$ !
$ !  logicals and command for major SCAN directories
$ !
$ set noverify
$ ASSIGN	scan$$disk:[scan]	scan$:
$ REPORT	:==	set def scan$$disk:[scan.report]
$ ASSIGN	scan$$disk:[scan.report]	report$:
$ PLAN		:==	set def scan$$disk:[scan.plan]
$ ASSIGN	scan$$disk:[scan.plan]	plan$:
$! PROTO	:==	set def scan$$disk:[scan.proto]
$! ASSIGN	scan$$disk:[scan.proto]	proto$:
$ TOOLS		:==	 set def scan$$disk:[scan.tools]
$ ASSIGN	scan$$disk:[scan.tools]	tools$:
$ COM		:==	 set def scan$$disk:[scan.com]
$ ASSIGN	scan$$disk:[scan.com]	com$:
$ TEST		:==	 set def scan$$disk:[scan.test]
$ ASSIGN	scan$$disk:[scan.test]	test$:
$ FTEST		:==	 set def scan$$disk:[scan.ftest]
$ ASSIGN	scan$$disk:[scan.ftest]	ftest$:
$! ASSIGN	scan$$disk:[scan.pme]	pme$:
$! PME	:==	 set def scan$$disk:[scan.pme]
$! ASSIGN	scan$$disk:[scan.ftestout]	ftestout$:
$! ASSIGN	scan$$disk:[scan.ftestmst]	ftestmst$:
$ DOC		:==	 set def scan$$disk:[scan.doc]
$ ASSIGN	scan$$disk:[scan.doc]	doc$:
$ KIT		:==	 set def scan$$disk:[scan.kit]
$ ASSIGN	scan$$disk:[scan.kit]	kit$:
$ KCOM		:==	 set def scan$$disk:[scan.kit.com]
$ ASSIGN	scan$$disk:[scan.kit.com]	kcom$:
$ KCUR		:==	 set def scan$$disk:[scan.kit.v11_8]
$ ASSIGN	scan$$disk:[scan.kit.v11_8]	kcur$:
$ ! To shorten BACKUP command line in scanfiles.dat for kitbuild
$ ASSIGN	scan$$disk:[scan.kit.v11_8]	k$:
$ ASSIGN	scan$$disk:[scan.dtm]	dtm$:
$ ASSIGN	scan$$disk:[scan.tcs]	tcs$:
$ ASSIGN	scan$$disk:[scan.mms]	mms$:
$ ASSIGN	scan$$disk:[scan.lrm]	lrm$:
$ ASSIGN	scan$$disk:[scan.example]	example$:
$ ASSIGN	scan$$disk:[scan.bug]	bug$:
$ ASSIGN	scan$$disk:[scan.demo]	demo$:
$ bug		:==	set def bug$:
$ !
$ !  logicals and commands SCAN compiler
$ !
$ ASSIGN	scan$$disk:[scan.compiler.src]	csrc$:
$ CSRC	:==	set def csrc$
$ ASSIGN	scan$$disk:[scan.compiler.vcg]	vcg$:
$ VCG	:==	set def vcg$:
$ ASSIGN	scan$$disk:[scan.compiler.com]	ccom$:
$ CCOM	:==	 set def ccom$:
$ ASSIGN	csrc$: scan$library
$ ASSIGN	scan$$disk:[scan.compiler.auto]	scan$auto
$ ASSIGN	scan$$disk:[scan.compiler.pat]	scan$pat
$ ASSIGN	scan$$disk:[scan.compiler.obj]	cobj$
$ COBJ 	:==	 set def cobj$:
$ ASSIGN	scan$$disk:[scan.compiler.dobj]	cdobj$
$ CDOBJ :==	 set def cdobj$:
$ ASSIGN	scan$$disk:[scan.compiler.lis]	clis$
$ CLIS 	:==	 set def clis$:
$ ASSIGN	scan$$disk:[scan.menu]  	scan$menu
$ menu :== 	@ scan$menu:scanmenu.com
$ @ccom$:deflog
$ !
$ !  logicals and commands SCAN runtime library
$ !
$ ASSIGN	scan$$disk:[scan.rtl.src]	rsrc$:
$ RSRC	:==	set def rsrc$
$ ASSIGN	scan$$disk:[scan.rtl.com]	rcom$:
$ RCOM	:==	 set def rcom$:
$ ASSIGN	scan$$disk:[scan.rtl.src]	rtl$library:
$ ASSIGN	scan$$disk:[scan.rtl.src]	rtlin:
$ ASSIGN	scan$$disk:[scan.rtl.src]	rtl$:
$ ASSIGN	scan$$disk:[scan.rtl.auto]	rtl$auto:
$ ASSIGN	scan$$disk:[scan.rtl.auto]	rtlml:
$ ASSIGN	scan$$disk:[scan.rtl.obj]	robj$:
$ ROBJ	:==	set def robj$
$ ASSIGN	scan$$disk:[scan.rtl.dobj]	rdobj$:
$ RDOBJ	:==	set def rdobj$
$ ASSIGN	scan$$disk:[scan.rtl.lis]	rlis$:
$ RLIS	:==	set def rlis$
$
$!+
$!    DTM logicals
$!-
$
$ define	dtmlib$		scan$$disk:[scan.dtm.dtmlib] 
$ define	dtm$template	scan$$disk:[scan.dtm.template] 
$ define	dtm$scratch	scan$$disk:[scan.dtm.scratch] 
$
$!+
$!    Use V4.2 BLISS compiler
$!-
$
$ define	bliss32		sys$system:bliss32_old.exe
$
$ !
$ ! BLISS Batch Command and general WORK queue submission command
$ !
$ BLSUB		:==	 @COM$:BLSUB.COM
$ COMSUB 	:==	 @COM$:COMSUB.COM
$ SETCOM 	:==	 @scan$$disk:[scan.compiler.com]setcom
$ SETRTL 	:==	 @scan$$disk:[scan.rtl.com]setrtl
