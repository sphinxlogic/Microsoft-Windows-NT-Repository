$!
$!  Command procedure to display the shareable image dependence tree for
$!  and executable or shareable image. Works against VAX and Alpha images,
$!  also works on Alpha VESTed images.
$!
$!  Syntax:
$!    @IMAGETREE <imagename> [FULL]
$!
$!  <imagename> is the name of the shareable image, defaults to SYS$SHARE:.EXE
$!
$!  FULL is a flag which directs the procedure to perform a complete tree
$!	traversal. Since the procedure works by parsing ANALYZE/IMAGE output
$!	this can result in excessive processing. The default is to keep track
$!	of images already analysed and display them as "repeat image"
$!
$!  Note that all dependent images must be accessible. The procedure uses
$!  the same algorithm as the image activator to locate images.
$!
$!  Author: © John Gillings, Digital Customer Support Centre, 23-September-1994
$!
$!  THIS IS UNSUPPORTED SOFTWARE
$!
$ IF p1.EQS."" THEN INQUIRE p1 "Image name"
$ IF p1.EQS."" THEN EXIT
$ SET NOON
$ prefix="''prefix'  "
$ level='level'+1
$ IF level.EQ.1.AND.p2.EQS."" THEN CREATE/NAME_TABLE IMAGE_TREE_TABLE
$ pid=F$GETJPI("","PID")
$ tmpfile="SYS$SCRATCH:IMAGETREE''pid'_''level'.TMP;"
$ tmpfile1="SYS$SCRATCH:IMAGETREE''pid'_''level'.TMP1;"
$ imagfile=F$PARSE(p1,"SYS$SHARE:.EXE")
$ imagname=F$PARSE(imagfile,,,"NAME")
$ IF F$SEARCH(imagfile).EQS.""
$ THEN
$   WRITE SYS$OUTPUT "''prefix'''imagname' (no such file)"
$ ELSE
$   IF p2.EQS."".AND.F$TRNLNM(imagname,"IMAGE_TREE_TABLE").NES.""
$   THEN
$     WRITE SYS$OUTPUT "''prefix'''imagname' (repeat)"
$   ELSE
$     IF p2.EQS."" THEN DEFINE/TABLE=IMAGE_TREE_TABLE 'imagname "TRUE"
$     DEFINE/USER SYS$ERROR NL:
$     DEFINE/USER SYS$OUTPUT NL:
$     ANALYZE/IMAGE/OUTPUT='tmpfile' 'imagfile'/FIXUP
$     ON CONTROL_Y THEN GOTO eof
$     OPEN/READ in'level' 'tmpfile'
$     searchtype: READ/ERROR=eof/END=eof in'level' LINE
$       IF F$LOCATE("image type:",LINE).GE.F$LENGTH(LINE) THEN -
      GOTO searchtype
$     prot=F$FILE_ATTRIBUTES(imagfile,"PRO")-"SYSTEM"-"GROUP"-"OWNER"-"WORLD"
$     prot=F$EDIT(prot,"COLLAPSE")-"="-"="-"="-"="
$     execute=LINE
$     IF F$LOCATE("executable",execute).GE.F$LENGTH(execute)
$     THEN
$       READ/ERROR=eof/END=eof in'level' LINE
$	maj=F$ELEMENT(1,"'",LINE)
$       min=F$ELEMENT(3,"'",LINE)
$       READ/ERROR=eof/END=eof in'level' LINE
$	mat=F$ELEMENT(1,":",LINE)-"ISD$K_"
$	mat=F$EDIT(mat,"COLLAPSE")
$     ENDIF
$     searchver: READ/ERROR=eof/END=eof in'level' LINE
$       IF F$LOCATE("image file identification: ",LINE).GE.F$LENGTH(LINE) THEN -
      GOTO searchver
$     ident=F$EDIT(LINE,"TRIM")-"image file identification: "-""""-"""" 
$     IF F$LOCATE("executable",execute).LT.F$LENGTH(execute)
$     THEN
$       WRITE SYS$OUTPUT "''prefix'''imagname', executable ''ident' (''prot')"
$     ELSE
$       WRITE SYS$OUTPUT "''prefix'''imagname',"+-
		" shareable (''maj',''min',''mat'), ''ident' (''prot')"
$     ENDIF
$     knowfile=imagfile
$     IF F$LOCATE("SYS$SYSROOT:[SYSEXE]",imagfile).EQ.0 THEN -
		knowfile="SYS$SYSTEM:''imagname'.EXE"
$     IF F$LOCATE("SYS$SYSROOT:[SYSLIB]",imagfile).EQ.0 THEN -
		knowfile="SYS$SHARE:''imagname'.EXE"
$     IF F$FILE_ATTRIBUTE(knowfile,"KNOWN")
$     THEN
$       DEFINE/USER SYS$OUTPUT 'tmpfile1'
$	INSTALL LIST/FULL 'knowfile'
$	OPEN/READ/ERROR=EndInstalled install_info 'tmpfile1'
$	READ/END=EndInstalled install_info info
$	READ/END=EndInstalled install_info info
$	READ/END=EndInstalled install_info info
$	info=F$EDIT(info,"TRIM,COMPRESS")
$	info=info-F$ELEMENT(0," ",info)
$	WRITE SYS$OUTPUT "''prefix' Installed''info'"
$	info_loop: READ/END=EndInstalled install_info info
$	IF F$LOCATE("Privileges",info).GE.F$LENGTH(info) THEN GOTO info_loop
$	info=F$EDIT(info,"TRIM,COMPRESS")
$	WRITE SYS$OUTPUT "''prefix' ''info'"
$	READ/END=EndInstalled install_info info
$	info=F$EDIT(info,"TRIM,COMPRESS")
$	IF info.NES."" THEN WRITE SYS$OUTPUT "''prefix'              ''info'"
$	EndInstalled: CLOSE install_info
$	DELETE 'tmpfile1'
$     ENDIF
$     searchlist: READ/ERROR=eof/END=eof in'level' LINE
$       IF F$LOCATE("Shareable Image List",LINE).GE.F$LENGTH(LINE) THEN -
      GOTO searchlist
$     searchfirst:  READ/END=eof/ERROR=eof in'level' LINE
$       IF F$LOCATE(")",LINE).GE.F$LENGTH(LINE) THEN GOTO searchfirst
$       IF F$LOCATE("this image",LINE).LT.F$LENGTH(LINE) THEN GOTO searchfirst
$     fileloop: nextfile=F$ELEMENT(1,"""",LINE)
$       IF nextfile.EQS."""" .OR.nextfile.EQS."" THEN GOTO eof
$       @'F$ENVIRONMENT("PROCEDURE") 'nextfile' 'p2'
$       READ/ERROR=eof/END=eof in'level' LINE
$     GOTO fileloop
$     eof: CLOSE in'level'
$     DELETE 'tmpfile'
$   ENDIF
$ ENDIF
$ IF level.EQ.1.AND.p2.EQS."" THEN DEASSIGN/TABLE=LNM$PROCESS_DIRECTORY IMAGE_TREE_TABLE
$ EXIT
