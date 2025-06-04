$ V = 'F$VERIFY(0)
$!
$! LINK.COM
$!   Creates GDIFF.EXE and friends from object code.
$!
$ DIFFLIB = ""
$ IF F$GETSYI("HW_MODEL") .GE. 1024
$ THEN
$   IF F$SEARCH("DIFFLIB.ALPHA_OLB") .NES. "" THEN DIFFLIB = "DIFFLIB.ALPHA_OLB"
$   OBJ_EXT = ".ALPHA_OBJ"
$ ELSE
$   IF F$SEARCH("DIFFLIB.VAX_OLB") .NES. "" THEN DIFFLIB = "DIFFLIB.VAX_OLB"
$   OBJ_EXT = ".VAX_OBJ"
$ ENDIF
$ IF DIFFLIB .EQS. ""
$ THEN
$   WRITE SYS$OUTPUT "ERROR: No DIFFLIB library found!"
$   EXIT
$ ENDIF
$!
$ SET VERIFY
$ Link/Executable=GDIFF.EXE/Notrace DIFF'OBJ_EXT', 'DIFFLIB'/Library
$ Link/Executable=GCMP.EXE/Notrace CMP'OBJ_EXT', 'DIFFLIB'/Library
$ Link/Executable=GDIFF3.EXE/Notrace DIFF3'OBJ_EXT', 'DIFFLIB'/Library
$ Link/Executable=GSDIFF.EXE/Notrace SDIFF'OBJ_EXT', 'DIFFLIB'/Library
$ EXIT 1+0*F$VERIFY(V)
