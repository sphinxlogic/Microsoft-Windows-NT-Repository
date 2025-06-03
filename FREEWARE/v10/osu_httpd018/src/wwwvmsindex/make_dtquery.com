$ v = 'f$verify(0)'
$!===========================================================================
$!
$!	Make_DTquery.com - Command file for building DTquery.exe
$!	================   F.Macrides (macrides@sci.wfeb.edu)
$!
$!	Place this command file and DTquery.c in the OSU DECthreads server's
$!	[.SCRIPT_CODE] subdirectory and execute the command file.
$!
$!===========================================================================
$ v = f$verify(1)
$ on error then goto end
$!
$ CC /NOLIST/OBJECT=DTquery.obj DTquery.c
$ IF F$SEARCH("CGILIB.OBJ") .EQS. "" THEN -
  CC /NOLIST/OBJECT=CGILIB.OBJ cgilib.c
$ IF F$SEARCH("SCRIPTLIB.OBJ") .EQS. "" THEN -
  CC /NOLIST/OBJECT=SCRIPTLIB.OBJ scriptlib.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
$ LINK /TRACE/NOMAP/EXEC=DTquery.exe -
	DTquery.obj,cgilib.obj,scriptlib.obj,CC_LIBS/opt
$ RENAME DTquery.exe [-.BIN]
$!
$ v = 'f$verify(0)'
$end:
$ v = 'f$verify(0)'
$ exit
