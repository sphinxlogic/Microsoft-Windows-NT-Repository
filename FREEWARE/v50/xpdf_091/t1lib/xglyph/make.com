$ xaw_root = "X11"
$ if (f$getsyi("ARCH_NAME") .eqs. "VAX")
$ then
$   float = "g_float"
$   olb_dir = "exe"
$ else
$   float = "ieee_float"
$   olb_dir = "exe_alpha"
$ endif
$ if (xaw_root.nes."X11") then define x11 'xaw_root',decw$include:
$ define x11_directory decw$include:
$ define xmu_directory 'xaw_root':[xmu]
$ define xaw_directory 'xaw_root':[xaw]
$ define olb_directory 'xaw_root':['olb_dir']
$ open/write  optf xglyph.opt
$ write optf "xglyph.obj"
$ write optf "[-]t1.olb/library"
$ write optf "sys$share:decc$shr.exe/share"
$ write optf "sys$share:decw$xextlibshr/share"
$ write optf "sys$share:decw$xlibshr/share"
$ write optf "sys$share:decw$xtlibshrr5/share"
$ if (f$trnlnm("xaw3dlibshr").nes."")
$ then
$   write optf "xaw3dlibshr/share"
$ else
$   write optf "olb_directory:xaw3dlib/library"
$ endif
$ if (f$trnlnm("xmulibshr").nes."")
$ then
$   write optf "xmulibshr/share"
$ else
$   write optf "olb_directory:xmulib/library"
$ endif
$ close optf
$ call make xglyph.obj "cc /nolist/prefix=all/float='float' xglyph.c" -
            xglyph.c
$ call make xglyph.obj "link xglyph.opt/option" -
            xglyph.obj [-]t1.olb
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
