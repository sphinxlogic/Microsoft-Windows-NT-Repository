$!****************************************************************************
$!*                                                                          *
$!*  COPYRIGHT (c) 1991-1993 BY                                              *
$!*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.                  *
$!*  ALL RIGHTS RESERVED.                                                    *
$!*                                                                          *
$!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
$!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
$!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
$!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
$!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
$!*  TRANSFERRED.                                                            *
$!*                                                                          *
$!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
$!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
$!*  CORPORATION.                                                            *
$!*                                                                          *
$!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
$!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                 *
$!*                                                                          *
$!****************************************************************************
$!
$! cvt_mar-to-h.com
$!
$! Converts the contents of the VMS Macro Libraries (all $EQU statements
$! in all $*DEF modules) into VAX C language #define include (.h) files.
$!
$
$!   #################################################################
$!   DO NOT REPLACE THE COMPILER-PROVIDED INCLUDE FILES WITH THE FILES
$!   GENERATED BY THIS PROCEDURE -- THEY ARE NOT DIRECTLY COMPATIBLE.
$!   #################################################################
$
$!
$! I'm not certain this procedure will be useful with DEC C...
$! Comments and questions to Steve Hoffman, STAR::SHoffman; DEC ZKO"
$!
$Cvt_Mar_To_h: SUBROUTINE
$
$! Convert the specified (in p1) macro file into a c format .h file.
$! Delete the (transient) source file when done with the conversion.
$! Drop the results in SYS$SCRATCH:, or if specified, p2.
$
$ Set NoOn
$
$ If f$trnlnm("INP") .nes. "" Then CLOSE INP
$ If f$trnlnm("OUT") .nes. "" Then CLOSE OUT
$
$ linecnt = 0
$
$ devdir = f$parse(p2,"SYS$SCRATCH:",,"DEVICE") +-
   f$parse(p2,"SYS$SCRATCH:",,"DIRECTORY")
$ inpspec = f$parse( ".MAR", p1, "SYS$SCRATCH:" )
$ outspec = f$parse( ".H", p1, devdir )
$ Open/Read INP 'inpspec'
$ Open/Write OUT 'outspec'
$
$ swvers = f$edit(f$getsyi("NODE_SWVERS"),"COLLAPSE")
$ swtype = f$edit(f$getsyi("NODE_SWTYPE"),"COLLAPSE")
$ now = f$cvtime(,"ABSOLUTE")
$
$ write OUT "/*"
$ write OUT "/* ''outspec'"
$ write OUT "/* generated by cvt_mar-to-h.com V1.0-001"
$ write OUT "/*"
$ write OUT "/* Copyright 1991-1993; Digital Equipment Corporation"
$ write OUT "/* All Rights Reserved"
$ write OUT "/*"
$ write OUT "/*   Steve Hoffman"
$ write OUT "/*   star::shoffman; shoffamn@star.enet.dec.com"
$ write OUT "/*"
$ write OUT "/*   ''swtype' ''swvers'"
$ write OUT "/*   ''now'"
$ write OUT "*/"
$
$Cvt_Read_Loop:
$ Read/End=cvt_Read_Done INP text
$ text = f$edit(text,"TRIM,COMPRESS")
$ col0 = f$edit(f$element(0," ",text),"COLLAPSE")
$ if col0 .eqs. "$EQU"
$ Then
$   col1 = f$edit(f$element(1," ",text),"COLLAPSE,UPCASE")
$   col2 = f$edit(f$element(2," ",text),"COLLAPSE,UPCASE")
$   write out "#define ''col1' (''col2')"
$   linecnt = linecnt + 1
$ Endif
$ Goto Cvt_Read_Loop
$!
$cvt_READ_DONE:
$ write OUT "/*"
$ write OUT "/* End ''outspec'"
$ write OUT "*/"
$ Close OUT
$ Close INP
$
$! if we did not write anything interesting into the file,
$! delete it now.  (This catches the .MAR files that contain
$! weird macro stuff, or only some macro call declarations.)
$
$ if linecnt .eq. 0
$ then
$   delspec = f$parse( ";*", outspec )
$   Delete 'delspec'
$ EndIf
$!
$ delspec = f$parse( ";*","''inpspec'", "SYS$SCRATCH:" )
$ Delete 'delspec'
$!
$ Exit
$ EndSubroutine
$
$!####
$ 
$Cvt_Get_Def: SUBROUTINE
$
$! locate all $*DEF files in the specified macro library.
$
$ Set NoOn
$ inpspec = f$parse( ".MLB","''p1'","SYS$LIBRARY:" )
$ outspec = f$parse( ".TMP","''p1'","SYS$SCRATCH:" )
$ Library/Macro/List='outspec'/Only=$*DEF 'inpspec'
$ Exit
$ EndSubroutine
$
$!####
$
$Cvt_Go_Def: SUBROUTINE
$
$! read the library listing (specified by P1, one line at a time),
$! extract the specified file, and convert it into c format using
$! the conversion subroutine.  P2 is the (optional) output directory.
$
$ Set NoOn
$
$ If f$trnlnm("TMP") .nes. "" Then CLOSE TMP
$
$ libspec = f$parse( ".MLB","''p1'", "SYS$LIBRARY:" )
$ inpspec = f$parse( ".TMP","''p1'", "SYS$SCRATCH:" )
$ Open/Read TMP 'inpspec'
$
$Cvt_Go_Loop:
$ Read/End=cvt_Go_Done TMP text
$ text = f$edit(text,"COLLAPSE")
$ If f$extr(0,1,text) .eqs. "$"
$ Then
$   If f$locate("DEF",text) .ne. F$Length(text)
$   Then
$     outspec = f$parse( ".MAR","''text'", "SYS$SCRATCH:" )
$     Librar/Macro/Extra='text'/Out='outspec' 'libspec'
$     Call Cvt_Mar_To_h 'text' 'p2'
$   EndIf
$ EndIf
$ Goto Cvt_Go_Loop
$!
$cvt_Go_DONE:
$ Close TMP
$
$ delspec = f$parse( ";*","''inpspec'", "SYS$SCRATCH:" )
$ Delete 'delspec'
$
$ Exit
$ EndSubroutine
$
$
$!####
$
$ Call Cvt_Get_Def STARLET
$ Call Cvt_Go_Def STARLET 'p1'
$ Call Cvt_Get_Def LIB
$ Call Cvt_Go_Def LIB 'p1'
$
$ Exit
$

