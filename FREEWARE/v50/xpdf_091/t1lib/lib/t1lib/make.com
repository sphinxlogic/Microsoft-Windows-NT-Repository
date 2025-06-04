$ call make parseafm.obj "cc ''cflags' parseafm" - 
            parseafm.c
$ call make t1aaset.obj "cc ''cflags' t1aaset" - 
            t1aaset.c
$ call make t1afmtool.obj "cc ''cflags' t1afmtool" - 
            t1afmtool.c
$ call make t1base.obj "cc ''cflags' t1base" - 
            t1base.c 
$ call make t1delete.obj "cc ''cflags' t1delete" - 
            t1delete.c
$ call make t1enc.obj "cc ''cflags' t1enc" - 
            t1enc.c
$ call make t1env.obj "cc ''cflags' t1env" - 
            t1env.c
$ call make t1finfo.obj "cc ''cflags' t1finfo" - 
            t1finfo.c
$ call make t1load.obj "cc ''cflags' t1load" - 
            t1load.c
$ call make t1set.obj "cc ''cflags' t1set" - 
            t1set.c
$ call make t1trans.obj "cc ''cflags' t1trans" - 
            t1trans.c
$ call make t1x11.obj "cc ''cflags' t1x11" - 
            t1x11.c
$ call make t1outline.obj "cc ''cflags' t1outline" - 
            t1outline.c
$ exit
$!
$MAKE: SUBROUTINE   !SUBROUTINE TO CHECK DEPENDENCIES
$ V = 'F$Verify(0)
$! P1 = What we are trying to make
$! P2 = Command to make it
$! P3 - P8  What it depends on
$
$ If F$Search(P1) .Eqs. "" Then Goto Makeit
$ Time = F$CvTime(F$File(P1,"RDT"))
$arg=3
$Loop:
$       Argument = P'arg
$       If Argument .Eqs. "" Then Goto Exit
$       El=0
$Loop2:
$       File = F$Element(El," ",Argument)
$       If File .Eqs. " " Then Goto Endl
$       AFile = ""
$Loop3:
$       OFile = AFile
$       AFile = F$Search(File)
$       If AFile .Eqs. "" .Or. AFile .Eqs. OFile Then Goto NextEl
$       If F$CvTime(F$File(AFile,"RDT")) .Ges. Time Then Goto Makeit
$       Goto Loop3
$NextEL:
$       El = El + 1
$       Goto Loop2
$EndL:
$ arg=arg+1
$ If arg .Le. 8 Then Goto Loop
$ Goto Exit
$
$Makeit:
$ VV=F$VERIFY(0)
$ write sys$output P2
$ 'P2
$ VV='F$Verify(VV)
$Exit:
$ If V Then Set Verify
$ENDSUBROUTINE
