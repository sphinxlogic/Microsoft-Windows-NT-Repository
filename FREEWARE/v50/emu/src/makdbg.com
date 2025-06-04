$ IF P1 .EQS. "" THEN INQ P1 "Filename: "
$ SET VERIFY
$ if f$getsyi("NODE_HWTYPE") .eqs. "ALPH" then goto alph
$ MAC/DEB/LIS 'P1
$ INQ INP "Link [Y]? "
$ IF F$EDIT(F$EXTRACT(0,1,INP),"UPCASE") .EQS. "N" 
$	THEN 
$ INQ INP "Replace in Library [Y]? "
$ IF F$EDIT(F$EXTRACT(0,1,INP),"UPCASE") .EQS. "N"  THEN EXIT
$ LIBR/REPL EMU5_lib:emu5_vax 'P1
$ EXIT
$ ENDIF
$ SET VERIFY
$!
$ LINK/DEB 'P1,- 
	EMU5_lib:emu5_vax/LIB, -
	EMU5_lib:emu5_vax/INCLUDE=(EMUMSG),-
	sys$system:sys.stb/sel
$!
$! LINK/DEB 'P1,EMU5_lib:emu5_vax/LIB,sys$system:sys.stb/sel
$ SET NOVERIFY
$ exit
$ alph:
$ MAC/MIG/LIS/DEB/FLAG=NOALIGN/OPTIM=NONE/UNALIGN 'P1
$ SET NOVERIFY
$ INQ INP "Link [Y]? "
$ IF F$EDIT(F$EXTRACT(0,1,INP),"UPCASE") .EQS. "N" 
$	THEN 
$ INQ INP "Replace in Library [Y]? "
$ IF F$EDIT(F$EXTRACT(0,1,INP),"UPCASE") .EQS. "N"  THEN EXIT
$ LIBR/REPL EMU5_lib:emu5_axp 'P1
$ EXIT
$ ENDIF
$ SET VERIFY
$!
$ LINK/SYSEXE/SYSSHR/SYSLIB/DEB -
	'P1,EMU5_lib:emu5_axp/LIB,-
	emu5_lib:koblib_axp/lib
$!
$! LINK/SYSEXE/SYSSHR/SYSLIB/DEB 'P1,EMU5_lib:emu5_axp/LIB/INCLUDE=(EMUMSG),emu5_lib:koblib_axp/lib
$ SET NOVERIFY
