$! make package under OpenVMS
$!
$! In case you want to override the automatic compiler detection
$! specify either DECC, VAXC or GCC as a parameter, e.g. @make GCC
$! To test the program after it's build execute @make "" test
$!
$!
$ echo := write sys$output
$ cflags= ""
$ true  = 1.eq.1
$ false = 0.eq.1
$ 
$ 
$ !
$ ! Check for MMK/MMS
$ !
$ Make = ""
$ If F$Search ("Sys$System:MMS.EXE") .nes. "" Then Make = "MMS"
$ If F$trnlnm("MMK_DIR").nes."" .OR. f$Type(MMK).eqs."STRING" Then Make = "MMK"
$ if Make.eqs.""
$ then	! Would not be convenient to maintain via a .COM
$ 	echo "Sorry, but you need MMS or MMK to build this package"
$ 	exit 42
$ endif
$ 
$ !
$ ! Check for platform
$ !
$ if f$getsyi("HW_MODEL").ge.1024
$ then	! Alpha
$ 	arch = "AXP"
$ else	! VAX
$ 	arch = "VAX"
$ endif
$ 
$ !
$ ! check for compiler
$ !
$ its_decc = (f$search("SYS$SYSTEM:DECC$COMPILER.EXE") .nes. "")
$ its_vaxc = .not.its_decc .and. (f$search("SYS$SYSTEM:VAXC.EXE") .nes. "")
$ its_gnuc = .not.its_decc .and. (f$trnlnm("GNU_CC").nes."")
$ 
$ !
$ ! Exit if no compiler available
$ !
$ if (.not. (its_decc .or. its_vaxc .or. its_gnuc))
$ then	echo "C compiler required to build Deltree"
$   	exit
$ endif
$ 
$ !
$ ! Override if requested from the commandline
$ !
$ if (p1 .eqs. "DECC")
$ then	!
$	its_decc = true
$	its_gnuc = false
$	its_vaxc = false
$ endif
$ if (p1 .eqs. "GCC")
$ then	!
$	its_decc = false
$	its_gnuc = true
$	its_vaxc = false
$ endif
$ if (p1 .eqs. "VAXC")
$ then	! 
$	its_decc = false
$	its_gnuc = false
$	its_vaxc = true
$ endif
$ 
$ 
$ if its_gnuc
$ then	!
$	COMP   = "GCC"
$	cflags = "/nocase" 
$ 	mdef   = "__''arch'__=1"
$	mdef   = mdef + ",GNUC=1"
$ endif
$ if its_vaxc
$ then	!
$ 	COMP   = "CC"
$ 	cflags = "" 
$ 	mdef   = "__''arch'__=1"
$ endif
$ if its_decc
$ then	!
$ 	COMP = "CC"
$	cflags = "/prefix=all" 
$	if (arch .eqs. "VAX") then cflags = "/decc" + cflags
$ 	mdef   = "__''arch'__=1"
$ endif
$ 
$ 
$ p2 = f$edit(p2,"upcase")
$ if (p2.nes."TEST")
$ then	! Compile the program
$ 	echo "''Make' /macro=(''mdef',xcflags=""''cflags'"")"
$	'Make' /macro=('mdef',xcflags="''cflags'")
$ else	! Run the tests
$	'Make' /macro=('mdef') test
$ endif
$exit
