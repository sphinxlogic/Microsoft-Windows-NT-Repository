$ WRITE SYS$OUTPUT F$ENVIRONMENT("PROCEDURE")
$! Try to parse out counts on Sync line to Telenet
$!
$ ON ERROR        THEN $ GOTO exitout
$! ON CONTROL_Y    THEN $ GOTO exitout
$ ON SEVERE_ERROR THEN $ GOTO exitout
$!
$ IF F$MODE() .EQS. "BATCH"
$  THEN
$    SET PROCESS /NAME = "DPV-0 Counts"
$    SET PROCESS /PRIORITY = 1
$ ENDIF
$!
$! Find out if we have the required privileges.
$!
$ savepriv = F$SETPRV("OPER,SYSPRV")
$ IF F$PRIVILEGE("OPER,SYSPRV") THEN GOTO gotpriv
$!
$ WRITE SYS$OUTPUT "You do not have the correct privilege to do this."
$ EXIT 1 + 0*F$VERIFY(in_verify)
$!
$ gotpriv:
$!
$! save repeat time interval, if given
$!
$ interval = P1
$!
$! Define the NCP command
$!
$ NCP :== $NCP
$!
$ bigloop:
$!
$ debug_count = 0
$!
$ NCP SHOW LINE DPV-0 COUNT TO user0:[accounting]dpv_0_counts.temporary
$!
$ IF F$SEARCH("user0:[accounting]dpv_0_counts.seq") .NES. ""
$ THEN
$     OPEN /APPEND /ERROR=nofile outfile user0:[accounting]dpv_0_counts.seq
$ ELSE
$     OPEN /WRITE /ERROR=nofile outfile user0:[accounting]dpv_0_counts.seq
$ ENDIF
$!
$! Get fields
$!
$ OPEN /READ /ERROR=nofile tmpfile user0:[accounting]dpv_0_counts.temporary
$ outstring = F$FAO("!%D ", 0)
$!
$ readloop1:
$!
$ READ /END=endfile1 tmpfile tmpstring
$ tmpstring = F$EDIT(tmpstring, "COMPRESS, TRIM, UPCASE")
$!
$! If the line is too short (there are some blank lines) get the next line
$!
$ IF F$LENGTH(tmpstring) .LE. 1 THEN GOTO readloop1
$!
$! Parse out parts of the line.
$!
$ count = F$ELEMENT(0, " ", tmpstring)
$ IF count .EQS. "LINE" THEN $ GOTO readloop1
$ IF count .EQS. "REJ"  THEN $ GOTO readloop1
$ IF count .EQS. "FRMR" THEN $ GOTO readloop1
$!
$!! text  = F$EXTRACT(F$LOCATE(" ", tmpstring), F$LENGTH(tmpstring), tmpstring)
$!
$ IF F$LOCATE(">", count) .NE. F$LENGTH(count) THEN -
        count = F$EXTRACT(1, F$LENGTH(count), count)
$ number = F$INTEGER(count)
$!
$ debug_count = debug_count + 1
$ outstring = outstring + F$FAO("!9ZL ", number)
$!
$ GOTO readloop1
$!
$ endfile1:
$!
$ WRITE outfile outstring
$ CLOSE outfile
$ CLOSE tmpfile
$ IF debug_count .EQ. 13
$ THEN
$    DELETE/NOLOG user0:[accounting]dpv_0_counts.temporary;*
$ ELSE
$    RENAME user0:[accounting]dpv_0_counts.temporary dpv_counts.wrong
$ ENDIF
$!
$ NCP ZERO LINE dpv-0 COUNT
$!
$!
$! Check to see if we were asked to auto-repeat
$!
$ IF interval .EQS. "" THEN GOTO exitout
$ WAIT 'interval
$ GOTO bigloop
$!
$ exitout:
$!
$ savepriv = F$SETPRV(savepriv)
$!
$ nofile:
$!
$ EXIT $STATUS
$!
$!
$! B. Z. Lederman	20-Oct-1989
$!
$!			15-May-1990	Investigate extra data item
$!			23-May-1990	Reject additional text line
$!! in_verify = 'F$VERIFY(0)'
$ EXIT $STATUS + 0*F$VERIFY(in_verify)
