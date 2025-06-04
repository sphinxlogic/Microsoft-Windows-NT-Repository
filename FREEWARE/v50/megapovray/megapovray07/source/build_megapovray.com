$!
$! BUILD_MEGAPOVRAY.COM
$! Written By:  Robert Alan Byer
$!              Vice-President
$!              A-Com Computing, Inc.
$!              ByerRA@aol.com
$!              byer@mail.all-net.net
$!
$!
$! This script is used to compile and link MEGAPovRAY v0.7 for OpenVMS using 
$! DEC C.
$!
$! The script accepts the following parameters.
$!
$!	P1	ALL		Just Build "Everything".
$!              ZLIB		Just Build The ZLIB-xxx.OLB Library.
$!	 	LIBPNG 		Just Build The LIBPNG-xxx.OLB Library.
$!		MEGAPOVRAYLIB	Just Build The MEGAPOVRAY-xxx.OLB Library.
$!		XMEGAPOVRAYLIB	Just Build The XMEGAPOVRAY-xxx.OLB Library.
$!		MEGAPOVRAY	Just Build MEGAPOVRAY Executable.
$!              MEGAXPOVRAY     Just Build The XMEGAPOVRAY Executable.
$!
$!	P2	DEBUG		Build With Debugger Information.
$!		NODEBUG		Build Withoug Debugger Information.
$!
$! The default is "ALL" and "NODEBUG".
$!
$!
$! Check To Make Sure We Have Valid Command Line Parameters.
$!
$ GOSUB CHECK_OPTIONS
$!
$! Check To See If We Are On An AXP Machine.
$!
$ IF (F$GETSYI("CPU").LT.128)
$ THEN
$!
$!  We Are On A VAX Machine So Tell The User.
$!
$   WRITE SYS$OUTPUT "Compiling On A VAX Machine."
$!
$!  Define The Machine Type.
$!
$   MACHINE_TYPE = "VAX"
$!
$!  Define The Compile Command For VAX.
$!
$   CC = "CC/PREFIX=ALL/''OPTIMIZE'/''DEBUGGER'/G_FLOAT"
$!
$!  Define The ZLIB Library Name.
$!
$   ZLIB_NAME = "SYS$DISK:[]ZLIB-VAX.OLB"
$!
$!  Define The LIBPNG Library Name.
$!
$   LIBPNG_NAME = "SYS$DISK:[]LIBPNG-VAX.OLB"
$!
$!  Define The MEGAPOVLIB Library.
$!
$   MEGAPOVLIB_NAME = "SYS$DISK:[]MEGAPOVLIB-VAX.OLB"
$!
$!  Define The XMEGAPOVLIB Library.
$!
$   XMEGAPOVLIB_NAME = "SYS$DISK:[]XMEGAPOVLIB-VAX.OLB"
$!
$! Else, We Are On An AXP Machine.
$!
$ ELSE
$!
$!  We Are On A AXP Machine So Tell The User.
$!
$   WRITE SYS$OUTPUT "Compiling On A AXP Machine."
$!
$!  Define The Machine Type.
$!
$   MACHINE_TYPE = "AXP"
$!
$!  Define The Compile Command For AXP.
$!
$   CC = "CC/PREFIX=ALL/''OPTIMIZE'/''DEBUGGER'/FLOAT=IEEE_FLOAT" + -
         "/IEEE_MODE=DENORM_RESULTS/NEST=PRIMARY/NAME=(AS_IS,SHORTENED)"
$!
$!  Define The ZLIB Library Name.
$!
$   ZLIB_NAME = "SYS$DISK:[]ZLIB-AXP.OLB"
$!
$!  Define The LIBPNG Library Name.
$!
$   LIBPNG_NAME = "SYS$DISK:[]LIBPNG-AXP.OLB"
$!
$!  Define The MEGAPOVLIB Library.
$!
$   MEGAPOVLIB_NAME = "SYS$DISK:[]MEGAPOVLIB-AXP.OLB"
$!
$!  Define The XMEGAPOVLIB Library.
$!
$   XMEGAPOVLIB_NAME = "SYS$DISK:[]XMEGAPOVLIB-AXP.OLB"
$!
$! End Of The Machine Check.
$!
$ ENDIF
$!
$! Check To See What We Are To Do.
$!
$ IF (BUILDALL.EQS."TRUE")
$ THEN
$!
$!  Build "Everything".
$!
$   GOSUB BUILD_ZLIB
$   GOSUB BUILD_LIBPNG
$   GOSUB BUILD_MEGAPOVRAYLIB
$   GOSUB BUILD_XMEGAPOVRAYLIB
$   GOSUB BUILD_MEGAPOVRAY
$   GOSUB BUILD_XMEGAPOVRAY
$!
$! Else...
$!
$ ELSE
$!
$!  Build Just What The User Wants Us To Build.
$!
$   GOSUB BUILD_'BUILDALL'
$!
$! Time To Exit The Build Check.
$!
$ ENDIF
$!
$! Time To EXIT.
$!
$ EXIT
$!
$! Build The ZLIB Library.
$!
$ BUILD_ZLIB:
$!
$! Tell The User What We Are Doing.
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Compling The ",ZLIB_NAME," Library."
$ WRITE SYS$OUTPUT "Using Compile Command: ",CC,"/INCLUDE=(SYS$DISK:[.ZLIB])"
$ WRITE SYS$OUTPUT ""
$!
$! Check To See If We Already Have A "ZLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(ZLIB_NAME).EQS."")
$ THEN
$!
$!  Guess Not, Create The Library.
$!
$   LIBRARY/CREATE/OBJECT 'ZLIB_NAME'
$!
$! End The Library Check.
$!
$ ENDIF
$!
$! Define The Files That Are Necessary For The ZLIB Library.
$!
$ ZLIB_FILES = "ADLER32,COMPRESS,CRC32,GZIO,UNCOMPR,DEFLATE,TREES," + -
               "ZUTIL,INFLATE,INFBLOCK,INFTREES,INFCODES,INFUTIL," + -
               "INFFAST"
$!
$!  Define A File Counter And Set It To "0".
$!
$ ZLIB_FILE_COUNTER = 0
$!
$! Top Of The File Loop.
$!
$ NEXT_ZLIB_FILE:
$!
$! O.K, Extract The File Name From The File List.
$!
$ ZLIB_FILE_NAME = F$ELEMENT(ZLIB_FILE_COUNTER,",",ZLIB_FILES)
$!
$! Check To See If We Are At The End Of The File List.
$!
$ IF (ZLIB_FILE_NAME.EQS.",") THEN GOTO ZLIB_FILE_DONE
$!
$! Increment The Counter.
$!
$ ZLIB_FILE_COUNTER = ZLIB_FILE_COUNTER + 1
$!
$! Create The Source File Name.
$!
$ ZLIB_SOURCE_FILE = "SYS$DISK:[.ZLIB]" + ZLIB_FILE_NAME + ".C"
$!
$! Create The Object File Name.
$!
$ ZLIB_OBJECT_FILE = "SYS$DISK:[.ZLIB]" + ZLIB_FILE_NAME + ".OBJ"
$!
$! Check To See If The File We Want To Compile Actually Exists.
$!
$ IF (F$SEARCH(ZLIB_SOURCE_FILE).EQS."")
$ THEN
$!
$!  Tell The User That The File Dosen't Exist.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "The File ",ZLIB_SOURCE_FILE," Dosen't Exist."
$   WRITE SYS$OUTPUT ""
$!
$!  Exit The Build.
$!
$   EXIT
$!
$! End The File Check.
$!
$ ENDIF
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	",ZLIB_SOURCE_FILE
$!
$! Compile The File.
$!
$  CC/INCLUDE=(SYS$DISK:[.ZLIB])/OBJECT='ZLIB_OBJECT_FILE' 'ZLIB_SOURCE_FILE'
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'ZLIB_NAME' 'ZLIB_OBJECT_FILE'
$!
$! Go Back And Do It Again.
$!
$ GOTO NEXT_ZLIB_FILE
$!
$! All Done Compiling.
$!
$ ZLIB_FILE_DONE:
$!
$! Delete The Object Files.
$!
$ DELETE/NOCONFIRM/NOLOG SYS$DISK:[.ZLIB]*.OBJ;*
$!
$! That's It, Time To Return From Where We Came From.
$!
$ RETURN
$!
$! Build The LIBPNG Library.
$!
$ BUILD_LIBPNG:
$!
$! Tell The User What We Are Doing.
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Compling The ",LIBPNG_NAME," Library."
$ WRITE SYS$OUTPUT "Using Compile Command: ",CC,"/INCLUDE=(SYS$DISK:[.LIBPNG],SYS$DISK:[.ZLIB])"
$ WRITE SYS$OUTPUT ""
$!
$! Check To See If We Already Have A "LIBPNG-xxx.OLB" Library...
$!
$ IF (F$SEARCH(LIBPNG_NAME).EQS."")
$ THEN
$!
$!  Guess Not, Create The Library.
$!
$   LIBRARY/CREATE/OBJECT 'LIBPNG_NAME'
$!
$! End The Library Check.
$!
$ ENDIF
$!
$! Define The Files That Are Necessary For The LIBPNG Library.
$!
$ LIBPNG_FILES = "PNG,PNGSET,PNGGET,PNGRUTIL,PNGTRANS,PNGWUTIL,PNGREAD," + -
                 "PNGMEM,PNGWRITE,PNGRTRAN,PNGWTRAN,PNGRIO,PNGWIO," + -
                 "PNGERROR,PNGPREAD"
$!
$!  Define A File Counter And Set It To "0".
$!
$ LIBPNG_FILE_COUNTER = 0
$!
$! Top Of The File Loop.
$!
$ NEXT_LIBPNG_FILE:
$!
$! O.K, Extract The File Name From The File List.
$!
$ LIBPNG_FILE_NAME = F$ELEMENT(LIBPNG_FILE_COUNTER,",",LIBPNG_FILES)
$!
$! Check To See If We Are At The End Of The File List.
$!
$ IF (LIBPNG_FILE_NAME.EQS.",") THEN GOTO LIBPNG_FILE_DONE
$!
$! Increment The Counter.
$!
$ LIBPNG_FILE_COUNTER = LIBPNG_FILE_COUNTER + 1
$!
$! Create The Source File Name.
$!
$ LIBPNG_SOURCE_FILE = "SYS$DISK:[.LIBPNG]" + LIBPNG_FILE_NAME + ".C"
$!
$! Create The Object File Name.
$!
$ LIBPNG_OBJECT_FILE = "SYS$DISK:[.LIBPNG]" + LIBPNG_FILE_NAME + ".OBJ"
$!
$! Check To See If The File We Want To Compile Actually Exists.
$!
$ IF (F$SEARCH(LIBPNG_SOURCE_FILE).EQS."")
$ THEN
$!
$!  Tell The User That The File Dosen't Exist.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "The File ",LIBPNG_SOURCE_FILE," Dosen't Exist."
$   WRITE SYS$OUTPUT ""
$!
$!  Exit The Build.
$!
$   EXIT
$!
$! End The File Check.
$!
$ ENDIF
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	",LIBPNG_SOURCE_FILE
$!
$! Compile The File.
$!
$  CC/INCLUDE=(SYS$DISK:[.LIBPNG],SYS$DISK:[.ZLIB]) -
     /OBJECT='LIBPNG_OBJECT_FILE' 'LIBPNG_SOURCE_FILE'
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'LIBPNG_NAME' 'LIBPNG_OBJECT_FILE'
$!
$! Go Back And Do It Again.
$!
$ GOTO NEXT_LIBPNG_FILE
$!
$! All Done Compiling.
$!
$ LIBPNG_FILE_DONE:
$!
$! Delete The Object Files.
$!
$ DELETE/NOCONFIRM/NOLOG SYS$DISK:[.LIBPNG]*.OBJ;*
$!
$! That's It, Time To Return From Where We Came From.
$!
$ RETURN
$!
$! Build The MEGAPOVRAYLIB-xxx Library.
$!
$ BUILD_MEGAPOVRAYLIB:
$!
$! Tell The User What We Are Doing.
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Compling The ",MEGAPOVLIB_NAME," Library."
$ WRITE SYS$OUTPUT "Using Compile Command: ",CC,"/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG])/DEFINE=(""VMS=1"",""DECC=1"")"
$ WRITE SYS$OUTPUT ""
$!
$! Check To See If We Already Have A "MEGAPOVLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(MEGAPOVLIB_NAME).EQS."")
$ THEN
$!
$!  Guess Not, Create The Library.
$!
$   LIBRARY/CREATE/OBJECT 'MEGAPOVLIB_NAME'
$!
$! End The Library Check.
$!
$ ENDIF
$!
$! Check To See SYS$DISK:[]CONFIG.H File.
$!
$ IF (F$SEARCH("SYS$DISK:[]CONFIG.H").NES."")
$ THEN
$!
$!  The File Exists So Delete It.
$!
$   DELETE/NOCONFIRM/NOLOG SYS$DISK:[]CONFIG.H;*
$!
$! Time To End The SYS$DISK:[]CONFIG.H File Check.
$!
$ ENDIF
$!
$! Check To See If The SYS$DISK:[.UNIX]UNIXCONF.H File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]UNIXCONF.H").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The SYS$DISK:[.UNIX]UNIXCONF.H
$!  File And We Can't Compile Without It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The SYS$DISK:[.UNIX]UNIXCONF.H File."
$   WRITE SYS$OUTPUT "This File Is Necessary To Compile MEGAPovRAY For OpenVMS."
$   WRITE SYS$OUTPUT ""
$!
$!  Since We Don't Have The File, Exit.
$!
$   EXIT
$!
$! Time To End The SYS$DISK:[.UNIX]UNIXCONF.H File Check.
$!
$ ENDIF
$!
$! Copy SYS$DISK:[.UNIX]UNIXCONF.H To SYS$DISK:[]CONFIG.H
$!
$ COPY SYS$DISK:[.UNIX]UNIXCONF.H SYS$DISK:[]CONFIG.H
$!
$! Check To See If The SYS$DISK:[.UNIX]UNIX.C File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]UNIX.C").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find SYS$DISK:[.UNIX]UNIX.C File.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "The SYS$DISK:[.UNIX]UNIX.C File Dosen't Exist."
$   WRITE SYS$OUTPUT ""
$!
$!  Since The File Dosen't Exist, Exit.
$!
$   EXIT
$!
$! End The SYS$DISK:[.UNIX]UNIX.C File Check.
$!
$ ENDIF
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	SYS$DISK:[.UNIX]UNIX.C"
$!
$! Compile The SYS$DISK:[.UNIX]UNIX.C File.
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT=SYS$DISK:[]UNIX.OBJ SYS$DISK:[.UNIX]UNIX.C
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'MEGAPOVLIB_NAME' SYS$DISK:[]UNIX.OBJ
$!
$! The Top Of The File Loop.
$!
$ NEXT_MEGAPOVLIB_FILE:
$!
$! Define The Files That Are Necessary For The MEGAPOVLIB Library.
$!
$ MEGAPOVLIB_FILES = F$ELEMENT(0,";",F$ELEMENT(1,"]",F$SEARCH("SYS$DISK:[]*.C",1)))
$!
$! O.k., Extract The File Name From The File List.
$!
$ MEGAPOVLIB_FILE_NAME = F$ELEMENT(0,".",MEGAPOVLIB_FILES)
$!
$! Check To See If We Are At The End Of The File List.
$!
$ IF (MEGAPOVLIB_FILE_NAME.EQS."]") THEN GOTO MEGAPOVLIB_FILE_DONE
$!
$! Create The Source File Name.
$!
$ MEGAPOVLIB_SOURCE_FILE = "SYS$DISK:[]" + MEGAPOVLIB_FILE_NAME + ".C"
$!
$! Create The Object File Name.
$!
$ MEGAPOVLIB_OBJECT_FILE = "SYS$DISK:[]" + MEGAPOVLIB_FILE_NAME + ".OBJ"
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	",MEGAPOVLIB_SOURCE_FILE
$!
$! Compile The File.
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT='MEGAPOVLIB_OBJECT_FILE' 'MEGAPOVLIB_SOURCE_FILE'
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'MEGAPOVLIB_NAME' 'MEGAPOVLIB_OBJECT_FILE'
$!
$! Go Back And Do It Again.
$!
$ GOTO NEXT_MEGAPOVLIB_FILE
$!
$! All Done Compiling.
$!
$ MEGAPOVLIB_FILE_DONE:
$!
$! Delete The Object Files.
$!
$ DELETE/NOCONFIRM/NOLOG SYS$DISK:[]*.OBJ;*
$!
$! That's All, Time To Return From Where We Came From.
$!
$ RETURN
$!
$! Build The XMEGAPOVRAYLIB-xxx Library.
$!
$ BUILD_XMEGAPOVRAYLIB:
$!
$! Tell The User What We Are Doing.
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Compling The ",XMEGAPOVLIB_NAME," Library."
$ WRITE SYS$OUTPUT "Using Compile Command: ",CC,"/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG])/DEFINE=(""VMS=1"",""DECC=1"")"
$ WRITE SYS$OUTPUT ""
$!
$! Check To See If We Have The Logical X11 Defined.
$!
$ IF (F$TRNLNM("X11").EQS."")
$ THEN
$!
$!  Define The Logical X11.
$!
$   DEFINE X11 DECW$INCLUDE
$!
$! Time To End The X11 Logical Check.
$!
$ ENDIF
$!
$! Check To See If We Already Have A "XMEGAPOVLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(XMEGAPOVLIB_NAME).EQS."")
$ THEN
$!
$!  Guess Not, Create The Library.
$!
$   LIBRARY/CREATE/OBJECT 'XMEGAPOVLIB_NAME'
$!
$! End The Library Check.
$!
$ ENDIF
$!
$! Check To See SYS$DISK:[]CONFIG.H File.
$!
$ IF (F$SEARCH("SYS$DISK:[]CONFIG.H").NES."")
$ THEN
$!
$!  The File Exists So Delete It.
$!
$   DELETE/NOCONFIRM/NOLOG SYS$DISK:[]CONFIG.H;*
$!
$! Time To End The SYS$DISK:[]CONFIG.H File Check.
$!
$ ENDIF
$!
$! Check To See If The SYS$DISK:[.UNIX]XWINCONF.H File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]XWINCONF.H").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The SYS$DISK:[.UNIX]XWINCONF.H
$!  File And We Can't Compile Without It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The SYS$DISK:[.UNIX]XWINCONF.H File."
$   WRITE SYS$OUTPUT "This File Is Necessary To Compile X MegaPOV-RAY For OpenVMS."
$   WRITE SYS$OUTPUT ""
$!
$!  Since We Don't Have The File, Exit.
$!
$   EXIT
$!
$! Time To End The SYS$DISK:[.UNIX]XWINCONF.H File Check.
$!
$ ENDIF
$!
$! Copy SYS$DISK:[.UNIX]XWINCONF.H To SYS$DISK:[]CONFIG.H
$!
$ COPY SYS$DISK:[.UNIX]XWINCONF.H SYS$DISK:[]CONFIG.H
$!
$! Check To See If The SYS$DISK:[.UNIX]UNIX.C File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]UNIX.C").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find SYS$DISK:[.UNIX]UNIX.C File.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "The SYS$DISK:[.UNIX]UNIX.C File Dosen't Exist."
$   WRITE SYS$OUTPUT ""
$!
$!  Since The File Dosen't Exist, Exit.
$!
$   EXIT
$!
$! End The SYS$DISK:[.UNIX]UNIX.C File Check.
$!
$ ENDIF
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	SYS$DISK:[.UNIX]UNIX.C"
$!
$! Compile The SYS$DISK:[.UNIX]UNIX.C File.
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.UNIX],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT=SYS$DISK:[]UNIX.OBJ SYS$DISK:[.UNIX]UNIX.C
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'XMEGAPOVLIB_NAME' SYS$DISK:[]UNIX.OBJ
$!
$! Check To See If The SYS$DISK:[.UNIX]XWINDOWS.C File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]XWINDOWS.C").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find SYS$DISK:[.UNIX]XWINDOWS.C File.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "The SYS$DISK:[.UNIX]XWINDOWS.C File Dosen't Exist."
$   WRITE SYS$OUTPUT ""
$!
$!  Since The File Dosen't Exist, Exit.
$!
$   EXIT
$!
$! End The SYS$DISK:[.UNIX]XWINDOWS.C File Check.
$!
$ ENDIF
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	SYS$DISK:[.UNIX]XWINDOWS.C"
$!
$! Compile The SYS$DISK:[.UNIX]XWINDOWS.C File.
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.UNIX],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT=SYS$DISK:[]XWINDOWS.OBJ SYS$DISK:[.UNIX]XWINDOWS.C
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'XMEGAPOVLIB_NAME' SYS$DISK:[]XWINDOWS.OBJ
$!
$! Top Of The File Loop.
$!
$ NEXT_XMEGAPOVLIB_FILE:
$!
$! Define The Files That Are Necessary For The XMEGAPOVLIB Library.
$!
$ XMEGAPOVLIB_FILES = F$ELEMENT(0,";",F$ELEMENT(1,"]",F$SEARCH("SYS$DISK:[]*.C",1)))
$!
$! O.K, Extract The File Name From The File List.
$!
$ XMEGAPOVLIB_FILE_NAME = F$ELEMENT(0,".",XMEGAPOVLIB_FILES)
$!
$! Check To See If We Are At The End Of The File List.
$!
$ IF (XMEGAPOVLIB_FILE_NAME.EQS."]") THEN GOTO XMEGAPOVLIB_FILE_DONE
$!
$! Create The Source File Name.
$!
$ XMEGAPOVLIB_SOURCE_FILE = "SYS$DISK:[]" + XMEGAPOVLIB_FILE_NAME + ".C"
$!
$! Create The Object File Name.
$!
$ XMEGAPOVLIB_OBJECT_FILE = "SYS$DISK:[]" + XMEGAPOVLIB_FILE_NAME + ".OBJ"
$!
$! Tell The User What We Are Compiling.
$!
$  WRITE SYS$OUTPUT "	",XMEGAPOVLIB_SOURCE_FILE
$!
$! Compile The File.
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.UNIX],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT='XMEGAPOVLIB_OBJECT_FILE' 'XMEGAPOVLIB_SOURCE_FILE'
$!
$! Add It To The Library.
$!
$ LIBRARY/REPLACE/OBJECT 'XMEGAPOVLIB_NAME' 'XMEGAPOVLIB_OBJECT_FILE'
$!
$! Go Back And Do It Again.
$!
$ GOTO NEXT_XMEGAPOVLIB_FILE
$!
$! All Done Compiling.
$!
$ XMEGAPOVLIB_FILE_DONE:
$!
$! Delete The Object Files.
$!
$ DELETE/NOCONFIRM/NOLOG SYS$DISK:[]*.OBJ;*
$!
$! All Done, Time To Return From Where We Came From.
$!
$ RETURN
$!
$! Build MEGAPOVRAY.EXE
$!
$ BUILD_MEGAPOVRAY:
$!
$! Check To See If We Have The "ZLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(ZLIB_NAME).EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "ZLIB" Library, So We Are Gong To
$!  Just Create It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The ",ZLIB_NAME," Library.  Since It Is"
$   WRITE SYS$OUTPUT "Needed To Build The MEGAPOVRAY Executable We Will Compile It Now."
$!
$!  Build The "ZLIB" Library.
$!
$   GOSUB BUILD_ZLIB
$!
$! End The ZLIB Library Check.
$!
$ ENDIF
$!
$! Check To See If We Have The "LIBPNG-xxx.OLB" Library...
$!
$ IF (F$SEARCH(LIBPNG_NAME).EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "LIBPNG" Library, So We Are Gong To
$!  Just Create It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The ",LIBPNG_NAME," Library.  Since It Is"
$   WRITE SYS$OUTPUT "Needed To Build The MEGAPOVRAY Executable We Will Compile It Now."
$!
$!  Build The "LIBPNG" Library.
$!
$   GOSUB BUILD_LIBPNG
$!
$! End The LIBPNG Library Check.
$!
$ ENDIF
$!
$! Check To See If We Have The "MEGAPOVLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(MEGAPOVLIB_NAME).EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "MEGAPOVLIB" Library, So We Are Gong To
$!  Just Create It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The ",MEGAPOVLIB_NAME," Library.  Since It Is"
$   WRITE SYS$OUTPUT "Needed To Build The MEGAPOVRAY Executable We Will Compile It Now."
$!
$!  Build The "MEGAPOVLIB" Library.
$!
$   GOSUB BUILD_MEGAPOVRAYLIB
$!
$! End The MEGAPOVLIB Library Check.
$!
$ ENDIF
$!
$! Check To See SYS$DISK:[]CONFIG.H File.
$!
$ IF (F$SEARCH("SYS$DISK:[]CONFIG.H").NES."")
$ THEN
$!
$!  The File Exists So Delete It.
$!
$   DELETE/NOCONFIRM/NOLOG SYS$DISK:[]CONFIG.H;*
$!
$! Time To End The SYS$DISK:[]CONFIG.H File Check.
$!
$ ENDIF
$!
$! Check To See If The SYS$DISK:[.UNIX]UNIXCONF.H File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]UNIXCONF.H").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The SYS$DISK:[.UNIX]UNIXCONF.H
$!  File And We Can't Compile Without It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The SYS$DISK:[.UNIX]UNIXCONF.H File."
$   WRITE SYS$OUTPUT "This File Is Necessary To Compile MegaPOV-Ray For OpenVMS."
$   WRITE SYS$OUTPUT ""
$!
$!  Since We Don't Have The File, Exit.
$!
$   EXIT
$!
$! Time To End The SYS$DISK:[.UNIX]UNIXCONF.H File Check.
$!
$ ENDIF
$!
$! Copy SYS$DISK:[.UNIX]UNIXCONF.H To SYS$DISK:[]CONFIG.H
$!
$ COPY SYS$DISK:[.UNIX]UNIXCONF.H SYS$DISK:[]CONFIG.H
$!
$! Check To Make Sure We Have The POVRAY.C File.
$!
$ 
$ IF (F$SEARCH("SYS$DISK:[]POVRAY.C").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "POVRAY.C" File, So We Are Gong To
$!  Exit As We Can't Compile Without It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The SYS$DISK:[]POVRAY.C File.  Since It's"
$   WRITE SYS$OUTPUT "Needed To Build MEGAPOVRAY.EXE We Will Exit Now."
$   WRITE SYS$OUTPUT ""
$!
$!  Since We Can't Compile Without, Time To Exit.
$!
$   EXIT
$!
$! End The POVRAY.C File Check.
$!
$ ENDIF
$!
$! Well, Since It Looks Like We Have Everything, Tell The User What We
$! Are Going To Do.
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Building SYS$DISK:[]MEGAPOVRAY.EXE-",MACHINE_TYPE
$ WRITE SYS$OUTPUT "Using Compile Command: ",CC,"/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG])/DEFINE=(""VMS=1"",""DECC=1"")"
$ WRITE SYS$OUTPUT ""
$!
$! Compile POVRAY.C
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT=SYS$DISK:[]MEGAPOVRAY.OBJ-'MACHINE_TYPE' SYS$DISK:[]POVRAY.C
$!
$! Check To See If We Are On A VAX.
$!
$ IF (MACHINE_TYPE.EQS."VAX")
$ THEN
$!
$!  We Are On A VAX So Use The VAX Link Options.
$!
$   LINK/'DEBUGGER'/'TRACEBACK'/EXE=SYS$DISK:[]MEGAPOVRAY.EXE-VAX -
        SYS$DISK:[]MEGAPOVRAY.OBJ-'MACHINE_TYPE','MEGAPOVLIB_NAME'/LIBRARY, -
        'LIBPNG_NAME'/LIBRARY,'ZLIB_NAME'/LIBRARY, -
        SYS$COMMON:[SYSLIB]VAXCRTLG.OLB/LIBRARY
$!
$! Else...
$!
$ ELSE
$!
$!  Else We Are On A AXP, So Use The AXP Link Options.
$!
$   LINK/'DEBUGGER'/'TRACEBACK'/EXE=SYS$DISK:[]MEGAPOVRAY.EXE-AXP -
        SYS$DISK:[]MEGAPOVRAY.OBJ-'MACHINE_TYPE','MEGAPOVLIB_NAME'/LIBRARY, -
        'LIBPNG_NAME'/LIBRARY,'ZLIB_NAME'/LIBRARY
$!
$! Time To End The VAX Machine Check.
$!
$ ENDIF
$!
$! That's All, Time To Return From Where We Came From.
$!
$ RETURN
$!
$! Build XMEGAPOVRAY.EXE
$!
$ BUILD_XMEGAPOVRAY:
$!
$! Check To See If We Have The "ZLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(ZLIB_NAME).EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "ZLIB" Library, So We Are Gong To
$!  Just Create It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The ",ZLIB_NAME," Library.  Since It Is"
$   WRITE SYS$OUTPUT "Needed To Build The XMEGAPOVRAY Executable We Will Compile It Now."
$!
$!  Build The "ZLIB" Library.
$!
$   GOSUB BUILD_ZLIB
$!
$! End The ZLIB Library Check.
$!
$ ENDIF
$!
$! Check To See If We Have The "LIBPNG-xxx.OLB" Library...
$!
$ IF (F$SEARCH(LIBPNG_NAME).EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "LIBPNG" Library, So We Are Gong To
$!  Just Create It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The ",LIBPNG_NAME," Library.  Since It Is"
$   WRITE SYS$OUTPUT "Needed To Build The XMEGAPOVRAY Executable We Will Compile It Now."
$!
$!  Build The "LIBPNG" Library.
$!
$   GOSUB BUILD_LIBPNG
$!
$! End The LIBPNG Library Check.
$!
$ ENDIF
$!
$! Check To See If We Have The "XMEGAPOVLIB-xxx.OLB" Library...
$!
$ IF (F$SEARCH(XMEGAPOVLIB_NAME).EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "XMEGAPOVLIB" Library, So We Are Gong To
$!  Just Create It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The ",XMEGAPOVLIB_NAME," Library.  Since It Is"
$   WRITE SYS$OUTPUT "Needed To Build The XMEGAPOVRAY Executable We Will Compile It Now."
$!
$!  Build The "XMEGAPOVLIB" Library.
$!
$   GOSUB BUILD_XMEGAPOVRAYLIB
$!
$! End The XMEGAPOVLIB Library Check.
$!
$ ENDIF
$!
$! Check To See If We Have The Logical X11 Defined.
$!
$ IF (F$TRNLNM("X11").EQS."")
$ THEN
$!
$!  Define The Logical X11.
$!
$   DEFINE X11 DECW$INCLUDE
$!
$! Time To End The X11 Logical Check.
$!
$ ENDIF
$!
$! Check To See SYS$DISK:[]CONFIG.H File.
$!
$ IF (F$SEARCH("SYS$DISK:[]CONFIG.H").NES."")
$ THEN
$!
$!  The File Exists So Delete It.
$!
$   DELETE/NOCONFIRM/NOLOG SYS$DISK:[]CONFIG.H;*
$!
$! Time To End The SYS$DISK:[]CONFIG.H File Check.
$!
$ ENDIF
$!
$! Check To See If The SYS$DISK:[.UNIX]XWINCONF.H File Exists.
$!
$ IF (F$SEARCH("SYS$DISK:[.UNIX]XWINCONF.H").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The SYS$DISK:[.UNIX]XWINCONF.H
$!  File And We Can't Compile Without It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The SYS$DISK:[.UNIX]XWINCONF.H File."
$   WRITE SYS$OUTPUT "This File Is Ncessary To Compile XMEGAPOVRAY For OpenVMS."
$   WRITE SYS$OUTPUT ""
$!
$!  Since We Don't Have The File, Exit.
$!
$   EXIT
$!
$! Time To End The SYS$DISK:[.UNIX]XWINCONF.H File Check.
$!
$ ENDIF
$!
$! Copy SYS$DISK:[.UNIX]XWINCONF.H To SYS$DISK:[]CONFIG.H
$!
$ COPY SYS$DISK:[.UNIX]XWINCONF.H SYS$DISK:[]CONFIG.H
$!
$! Check To Make Sure We Have The POVRAY.C File.
$!
$ 
$ IF (F$SEARCH("SYS$DISK:[]POVRAY.C").EQS."")
$ THEN
$!
$!  Tell The User We Can't Find The "POVRAY.C" File, So We Are Gong To
$!  Exit As We Can't Compile Without It.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "Can't Find The SYS$DISK:[]POVRAY.C File.  Since It's"
$   WRITE SYS$OUTPUT "Needed To Build The XMEGAPOVRAY Executable We Will Exit Now."
$   WRITE SYS$OUTPUT ""
$!
$!  Since We Can't Compile Without, Time To Exit.
$!
$   EXIT
$!
$! End The POVRAY.C File Check.
$!
$ ENDIF
$!
$! Well, Since It Looks Like We Have Everything, Tell The User What We
$! Are Going To Do.
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Building SYS$DISK:[]XMEGAPOVRAY.EXE-",MACHINE_TYPE
$ WRITE SYS$OUTPUT "Using Compile Command: ",CC,"/INCLUDE=(SYS$DISK:[],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG])/DEFINE=(""VMS=1"",""DECC=1"")"
$ WRITE SYS$OUTPUT ""
$!
$! Compile POVRAY.C
$!
$ CC/INCLUDE=(SYS$DISK:[],SYS$DISK:[.UNIX],SYS$DISK:[.ZLIB],SYS$DISK:[.LIBPNG]) -
    /DEFINE=("VMS=1","DECC=1") -
    /OBJECT=SYS$DISK:[]XMEGAPOVRAY.OBJ-'MACHINE_TYPE' SYS$DISK:[]POVRAY.C
$!
$! Check To See If We Are On A VAX.
$!
$ IF (MACHINE_TYPE.EQS."VAX")
$ THEN
$!
$!  We Are On A VAX So Use The VAX Link Options.
$!
$   LINK/'DEBUGGER'/'TRACEBACK'/EXE=SYS$DISK:[]XMEGAPOVRAY.EXE-VAX -
        SYS$DISK:[]XMEGAPOVRAY.OBJ-'MACHINE_TYPE', -
        SYS$DISK:[]XMEGAPOVRAY-'MACHINE_TYPE'.OPT/OPT
$!
$! Else...
$!
$ ELSE
$!
$!  Else We Are On A AXP, So Use The AXP Link Options.
$!
$   LINK/'DEBUGGER'/'TRACEBACK'/EXE=SYS$DISK:[]XMEGAPOVRAY.EXE-AXP -
        SYS$DISK:[]XMEGAPOVRAY.OBJ-'MACHINE_TYPE', -
        SYS$DISK:[]XMEGAPOVRAY-'MACHINE_TYPE'.OPT/OPT
$!
$! Time To End The VAX Machine Check.
$!
$ ENDIF
$!
$! That's All, Time To Return From Where We Came From.
$!
$ RETURN
$!
$! Check The User's Options.
$!
$ CHECK_OPTIONS:
$!
$! Check To See If We Are To "Just Build Everything."
$!
$ IF (P1.EQS."").OR.(P1.EQS."ALL")
$ THEN
$!
$!  P1 Is Blank Or "ALL", So Just Build Everything.
$!
$   BUILDALL = "TRUE"
$!
$! Else
$!
$ ELSE
$!
$!  Check To See If P1 Has A Valid Arguement.
$!
$   IF (P1.EQS."ZLIB").OR.(P1.EQS."LIBPNG").OR.(P1.EQS."MEGAPOVRAYLIB").OR. -
       (P1.EQS."XMEGAPOVRAYLIB").OR.(P1.EQS."MEGAPOVRAY").OR. -
       (P1.EQS."XMEGAPOVRAY")
$   THEN
$!
$!    A Valid Arguement.
$!
$     BUILDALL = P1
$!
$!   Else...
$!
$   ELSE
$!
$!    Tell The User We Don't Know What They Want.
$!
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "The Option ",P1," Is Invalid.  The Valid Options Are:"
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "    ALL              :  Just Build 'Everything'."
$     WRITE SYS$OUTPUT "    ZLIB             :  Just Build The ZLIB-xxx.OLB Library."
$     WRITE SYS$OUTPUT "    LIBPNG           :  Just Build The LIBPNG-xxx.OLB Library."
$     WRITE SYS$OUTPUT "    MEGAPOVRAYLIB    :  Just Build The MEGAPOVRAYLIB-xxx.OLB Library."
$     WRITE SYS$OUTPUT "    MEAGAXPOVRAYLIB  :  Just Biuld The XMEGAPOVRAYLIB-xxx.OLB Library."
$     WRITE SYS$OUTPUT "    MEAGAPOVRAY      :  Just Build The MEGAPOVRAY Executable."
$     WRITE SYS$OUTPUT "    XMEGAPOVRAY      :  Just Build The XMEGAPOVRAY Executable."
$     WRITE SYS$OUTPUT ""
$!
$!    Time To Exit.
$!
$     EXIT
$!
$!  Time To End The Valid Arguement Check.
$!
$   ENDIF
$!
$! To To End The P1 Check.
$!
$ ENDIF
$!
$! Check To See What The User Entered.
$!
$ IF (BUILDALL.EQS."TRUE").OR.(BUILDALL.EQS."MEGAPOVRAYLIB").OR.-
     (BUILDALL.EQS."XMEGAPOVRAYLIB").OR.(BUILDALL.EQS."MEGAPOVRAY").OR.-
     (BUILDALL.EQS."XMEGAPOVRAY")
$ THEN
$!
$!  Check To See If The "VMS_COMPILE_NAME.H" File Exists.
$!
$   IF (F$SEARCH("SYS$DISK:[]VMS_COMPILE_NAME.H").EQS."")
$   THEN
$!
$!    Ask The User To Enter Their Name.
$!
$     ENTER_USER_NAME:
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "Please Enter Your Name In Quotes."
$     INQUIRE USER_NAME "(Like ""Joe Smith <jsmith@somewhere.net>"")"
$!
$!    Check To See If The User Just Hit <RETURN> And Didn't Enter Anything.
$!
$     IF (USER_NAME.EQS."")
$     THEN
$!
$!      Go Back And Ask Them Again.
$!
$       GOTO ENTER_USER_NAME
$!
$!    Time To End The USER_NAME Check.
$!
$     ENDIF
$!
$!    Open The File "VMS_COMPILE_NAME.H" For Output.
$!
$     OPEN/WRITE COMPILE_NAME SYS$DISK:[]VMS_COMPILE_NAME.H
$!
$!    Write The Data To The File.
$!
$     WRITE COMPILE_NAME "#define DISTRIBUTION_MESSAGE_2 """,USER_NAME,"""
$!
$!    Close The SYS$DISK:[]VMS_COMPILE_NAME.H File.
$!
$     CLOSE COMPILE_NAME
$!
$!  Time To End The "VMS_COMPILE_NAME.H" File Check.
$!
$   ENDIF
$!
$!  Check To See If The "VMS_POVLIB_DIR.H" File Exists.
$!
$   IF (F$SEARCH("SYS$DISK:[]VMS_POVLIB_DIR.H").EQS."")
$   THEN
$!
$!    Ask The User To Enter Their Name.
$!
$     ENTER_POVLIB_DIR_PATH:
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "Please Enter The Directory Path Where You Intend To"
$     WRITE SYS$OUTPUT "Put Your MEGAPOVRAY.INI File.  The Path Must Be In Unix"
$     WRITE SYS$OUTPUT "Format And In Quotes."
$     INQUIRE POVLIB_DIR_PATH "(Like ""/disk$povray/megapovray07"")"
$!
$!    Check To See If The User Just Hit <RETURN> And Didn't Enter Anything.
$!
$     IF (POVLIB_DIR_PATH.EQS."")
$     THEN
$!
$!      Go Back And Ask Them Again.
$!
$       GOTO ENTER_POVLIB_DIR_PATH
$!
$!    Time To End The POVLIB_DIR_PATH Check.
$!
$     ENDIF
$!
$!    Open The File "VMS_POVLIB_DIR.H" For Output.
$!
$     OPEN/WRITE POVLIB_DIR_NAME SYS$DISK:[]VMS_POVLIB_DIR.H
$!
$!    Write The Data To The File.
$!
$     WRITE POVLIB_DIR_NAME "#define POV_LIB_DIR """,POVLIB_DIR_PATH,"""
$!
$!    Close The SYS$DISK:[]VMS_POVLID_DIR.H File.
$!
$     CLOSE POVLIB_DIR_NAME
$!
$!  Time To End The "VMS_POVLIB_DIR.H" File Check.
$!
$   ENDIF
$!
$! Time To End The BUILDALL Check.
$!
$ ENDIF
$!
$! Check To See If We Are To Compile Without Debugger Information.
$!
$ IF ((P2.EQS."").OR.(P2.EQS."NODEBUG"))
$ THEN
$!
$!  P2 Is Either Blank Or "NODEBUG" So Compile Without Debugger Information.
$!
$   DEBUGGER  = "NODEBUG"
$!
$!  Check To See If We Are On An AXP Machine.
$!
$   IF (F$GETSYI("CPU").LT.128)
$   THEN
$!
$!    We Are On A VAX Machine So Set The VAX Optimize Option.
$!
$     OPTIMIZE = "OPTIMIZE"
$!
$!  Else...
$!
$   ELSE
$!
$!    We Are On A AXP Machine, So Set The AXP Optimize Option.
$!
$     OPTIMIZE = "OPTIMIZE=(LEVEL=5,TUNE=HOST)"
$!
$!  That's All For The Machine Check.
$!
$   ENDIF
$!
$!  Define The Linker Traceback Options.
$!
$   TRACEBACK = "NOTRACEBACK"
$!
$!  Tell The User What They Selected.
$!
$   WRITE SYS$OUTPUT ""
$   WRITE SYS$OUTPUT "No Debugger Information Will Be Produced During Compile."
$   WRITE SYS$OUTPUT "Compiling With Compiler Optimization."
$ ELSE
$!
$!  Check To See If We Are To Compile With Debugger Information.
$!
$   IF (P2.EQS."DEBUG")
$   THEN
$!
$!    Compile With Debugger Information.
$!
$     DEBUGGER  = "DEBUG"
$     OPTIMIZE = "NOOPTIMIZE"
$     TRACEBACK = "TRACEBACK"
$!
$!    Tell The User What They Selected.
$!
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "Debugger Information Will Be Produced During Compile."
$     WRITE SYS$OUTPUT "Compiling Without Compiler Optimization."
$!
$!  Else...
$!
$   ELSE
$!
$!    Tell The User Entered An Invalid Option..
$!
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "The Option ",P2," Is Invalid.  The Valid Options Are:"
$     WRITE SYS$OUTPUT ""
$     WRITE SYS$OUTPUT "    DEBUG    :  Compile With The Debugger Information."
$     WRITE SYS$OUTPUT "    NODEBUG  :  Compile Without The Debugger Information."
$     WRITE SYS$OUTPUT ""
$!
$!    Time To EXIT.
$!
$     EXIT
$   ENDIF
$ ENDIF
$!
$! Time To Return To Where We Were.
$!
$ RETURN
