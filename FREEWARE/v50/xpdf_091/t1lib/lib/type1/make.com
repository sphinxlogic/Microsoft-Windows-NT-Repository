$ call make arith.obj "cc ''cflags' arith" - 
            arith.c
$ call make bstring.obj "cc ''cflags' bstring" - 
            bstring.c
$ call make curves.obj "cc ''cflags' curves" - 
            curves.c
$ call make fontfcn.obj "cc ''cflags' fontfcn" - 
            fontfcn.c
$ call make hints.obj "cc ''cflags' hints" - 
            hints.c
$ call make lines.obj "cc ''cflags' lines" - 
            lines.c
$ call make objects.obj "cc ''cflags' objects" - 
            objects.c
$ call make paths.obj "cc ''cflags' paths" - 
            paths.c
$ call make regions.obj "cc ''cflags' regions" - 
            regions.c
$ call make scanfont.obj "cc ''cflags' scanfont" - 
            scanfont.c
$ call make spaces.obj "cc ''cflags' spaces" - 
            spaces.c
$ call make t1io.obj "cc ''cflags' t1io" - 
            t1io.c
$ call make t1snap.obj "cc ''cflags' t1snap" - 
            t1snap.c
$ call make t1stub.obj "cc ''cflags' t1stub" - 
            t1stub.c
$ call make token.obj "cc ''cflags' token" - 
            token.c
$ call make type1.obj "cc ''cflags' type1" - 
            type1.c
$ call make util.obj "cc ''cflags' util" - 
            util.c
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
