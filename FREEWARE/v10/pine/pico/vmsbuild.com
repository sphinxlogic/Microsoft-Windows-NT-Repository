$ IF F$LOCATE("HEBREW", P1) .LT. F$LENGTH(P1)
$ THEN
$	CC_DEF = "/define=(HEBREW,ANSI_DRIVER)"
$ ELSE
$	CC_DEF = "/define=(ANSI_DRIVER)"
$ ENDIF
$ IF F$LOCATE("VAX", F$GETSYI("HW_NAME")) .EQS. F$LENGTH(F$GETSYI("HW_NAME"))
$ THEN
$	CC_DEF = "''CC_DEF'/STANDARD=VAXC"
$ ELSE
$	DEFINE SYS SYS$LIBRARY	! For the Inlcude to not fail on VAX.
$!
$ ENDIF
$ SET VERIFY
$ CC'CC_DEF' attach
$ CC'CC_DEF' ansi
$ CC'CC_DEF' basic
$ CC'CC_DEF' bind
$ CC'CC_DEF' browse
$ CC'CC_DEF' buffer
$ CC'CC_DEF' composer 
$ CC'CC_DEF' display
$ CC'CC_DEF' file 
$ CC'CC_DEF' fileio
$ CC'CC_DEF' line
$ CC'CC_DEF' pico
$ CC'CC_DEF' random 
$ CC'CC_DEF' region 
$ CC'CC_DEF' search
$ CC'CC_DEF' spell 
$ CC'CC_DEF' window 
$ CC'CC_DEF' word
$ CC'CC_DEF' main
$ CC'CC_DEF' osdep_VMS
$!
$ LIBRARY/OBJECT/CREATE/INSERT PICO *.OBJ
$!
$ IF F$EXTRACT(0, 3, F$GETSYI("HW_NAME")) .EQS. "VAX"
$ THEN
$	LINK/EXE=PICO MAIN,PICO/LIBR,SYS$INPUT:/OPT
	SYS$SHARE:VAXCRTL/SHARE
$ ELSE
$	LINK/EXE=PICO MAIN,PICO/LIBR
$ ENDIF
$ DELETE *.OBJ;*
$ SET NOVER
