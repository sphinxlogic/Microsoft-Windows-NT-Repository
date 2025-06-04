$ goto start

This command file writes the command files make.com in this directory, make.com
and sysconf.h in [.lib.t1lib], make.com in [.lib.type1], and make.com in
[.xglyph].  Executing the make.com in this directory will use the other
make.com files to compile the C files and put them in the T1 object library in
this directory, and compile and link xglyph.

Xglyph requires the xaw and xmu libraries; I used the libraries that came with
ghostview.  This command file looks for a required header file in
X11_ROOT:[XAW].  If it does not find it looks for an X11 directory tree
parallel to this tree.  If it does not find it there, it creates the command
file, but you have to edit it to tell it where the files and libraries are
before you can execute it.  I linked ghostview using the static version of xmu
and xaw, so I link to object libraries instead of sharable images.  I renamed
the xaw3d directory to xaw in the x11 tree of ghostview so I did not have to
change xglyph.c.  To run xglyph on V6.* of VMS, you need to define T1LIB_CONFIG
to be the T1LIB.CONFIG file in the xglyph directory; it will also not show the
timing information on V6.*.

I have compiled the library and run xglyph on VMS V6.2, both VAX and Alpha; I
used DECC V5.6.  I use IEEE floating on Alpha and G floating on VAX.

John Hasstedt
Physics Department
State University of New York at Stony Brook
John.Hasstedt@sunysb.edu

Nov-1999

- Extended the functionality to check for xaw/xmu also in the X11 path (which 
  is the most Unix compatible way to store them ;-).
- Added tests for shareable version of the Xaw/Xmu libraries, using the 
  shareable images if the appropriate logicals are defined. If not  the object 
  libraries are used as before.
- Added support for type1afm build
- Adapted to t1lib 0.9.2

Dec-1999

- Added support for absolute pathnames for font files under OpenVMS

Aug-2000

- First cut of an incremental build

Martin Zinser
zinser@decus.de

$start:
$ say = "write sys$output"
$ !
$ say "Getting the version number"
$ t1lib_identifier = "?.?"
$ open/read in configure.
$loop:
$ read/end=done in rec
$ if (f$element(0,"=",rec) .nes. "T1LIB_IDENTIFIER") then goto loop
$ t1lib_identifier = f$element(1,"=",rec)
$done:
$ close in
$ !
$ say "Creating make.com"
$ create make.com
$ open/append out make.com
$ if (f$getsyi("ARCH_NAME") .eqs. "VAX")
$ then
$   float = "g_float"
$ else
$   float = "ieee_float"
$ endif
$ defs = "T1LIB_IDENT=""""""""""""""""""""""" + t1lib_identifier  - 
         + """""""""""""""""""""""" + "," -
         + "GLOBAL_CONFIG_DIR=""""""""""""""""""""""""T1_FONT_DIR""""""""""""""""""""""""" + "," -
         + "T1_AA_TYPE16=""""""""short"""""""",T1_AA_TYPE32=""""""""int"""""""""
$ write out "$ cflags = ""/prefix=all/float=''float'"""
$ write out "$ cflags = cflags + ""/define=(", defs, ")"""
$ write out "$ write sys$output ""Compiling in [.lib.t1lib]"""
$ write out "$ set default [.lib.t1lib]"
$ write out "$ @make.com"
$ write out "$ write sys$output ""Compiling in [.lib.type1]"""
$ write out "$ set default [-.type1]"
$ write out "$ @make.com"
$ write out "$ set default [--]"
$ write out "$ write sys$output ""Creating t1.olb"""
$ write out "$ library/create t1.olb [.lib.t1lib]*.obj,[.lib.type1]*.obj"
$ write out "$ write sys$output ""Creating xglyph.exe"""
$ write out "$ set default [.xglyph]"
$ write out "$ @make.com"
$ write out "$ set default [-]"
$ write out "$ write sys$output ""Creating type1afm.exe"""
$ write out "$ set default [.type1afm]"
$ write out "$ @make.com"
$ write out "$ set default [-]"
$ write out "$ write sys$output ""Done"""
$ write out "$ exit"
$ close out
$ !
$ say "Creating [.lib.t1lib]sysconf.h"
$ cc sys$input:/float='float'/object=test.obj
#include <stdlib.h>
#include <stdio.h>
main()
{
  FILE *f=fopen("[.lib.t1lib]sysconf.h", "w");
  if (!f) exit(EXIT_FAILURE);
  fprintf(f, "#define SIZEOF_CHAR %d\n", sizeof(char));
  fprintf(f, "#define SIZEOF_SHORT %d\n", sizeof(short));
  fprintf(f, "#define SIZEOF_INT %d\n", sizeof(int));
  fprintf(f, "#define SIZEOF_LONG %d\n", sizeof(long));
#ifdef __VAX
  fprintf(f, "#define SIZEOF_LONG_LONG %d\n", sizeof(long));
#else
  fprintf(f, "#define SIZEOF_LONG_LONG %d\n", sizeof(long long));
#endif
  fprintf(f, "#define SIZEOF_FLOAT %d\n", sizeof(float));
  fprintf(f, "#define SIZEOF_DOUBLE %d\n", sizeof(double));
#ifdef __VAX
  fprintf(f, "#define SIZEOF_LONG_DOUBLE %d\n", sizeof(double));
#else
  fprintf(f, "#define SIZEOF_LONG_DOUBLE %d\n", sizeof(long double));
#endif
  fprintf(f, "#define SIZEOF_VOID_P %d\n", sizeof(void *));
  exit(EXIT_SUCCESS);
}
$ link test
$ run test
$ delete test.obj;*,test.exe;*
$ !
$ say "Creating [.lib.t1lib]make.com"
$ copy sys$input: [.lib.t1lib]make.com
$ deck
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
$eod
$ !
$ say "Creating [.lib.type1]make.com"
$ create [.lib.type1]make.com
$ deck
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
$eod
$ !
$ say "Creating [.xglyph]make.com"
$ check_xaw = "X11_ROOT,X11"
$ check_rel = f$parse("[-.x11]") - "].;"
$ if (check_rel .nes. "") 
$ then 
$   define rel_loc "''check_rel'.]"
$   check_xaw = check_xaw + ",rel_loc"
$ endif
$ i = 0
$SEARCH_XAW:
$ dir = f$element(i,",",check_xaw)
$ if (dir.nes.",")
$ then
$   if ( f$search("''dir':[xaw]label.h") .nes. "" ) then xaw_root = dir
$   i = i + 1
$   goto search_xaw
$ endif
$ create [.xglyph]make.com
$ open/append out [.xglyph]make.com
$ if (xaw_root .eqs. "")
$ then
$   say "I could not locate the xaw and xmu files.  You must edit"
$   say "[.xglyph]make.com before you can build xglyph."
$   write out "$ type/nopage sys$input"
$   write out "You must edit this file to delete the exit command and define"
$   write out "the dir symbol for the location of the xmu and xaw libraries."
$   write out "$ exit"
$   write out "$ xaw_root = ""<disk>:[dir.X11"""
$ else
$   write out "$ xaw_root = ""''xaw_root'"""
$ endif
$ copy sys$input: out
$ deck
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
$ call make xglyph.obj "cc /prefix=all/float='float' xglyph.c" -
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
$eod
$ close out
$ say "Creating [.type1afm]make.com"
$ create [.type1afm]make.com
$ open/append out [.type1afm]make.com
$ copy sys$input: out
$ deck
$ call make type1afm.obj "cc/prefix=all type1afm.c" -
            type1afm.c
$ call make type1afm.exe "link type1afm,[-]t1/library" -
            type1afm.obj [-]t1.olb
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
$eod
$ close out
$ exit
