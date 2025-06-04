$ WRITE SYS$OUTPUT F$ENVIRONMENT("PROCEDURE")
$! Parse out counters for LAT
$!
$ SET NOVERIFY
$!
$ ON SEVERE_ERROR THEN $ GOTO exitout
$!
$ IF F$MODE() .EQS. "BATCH"
$ THEN
$    SET PROCESS /NAME = "LAT Counts"
$    SET PROCESS /PRIORITY = 1
$ ELSE
$    ON CONTROL_Y THEN $ GOTO exitout
$ ENDIF
$!
$! Find out if we have the required privileges.
$!
$ savepriv = F$SETPRV ("CMKRNL")
$ IF F$PRIVILEGE ("CMKRNL") THEN GOTO gotpriv
$!
$ WRITE SYS$OUTPUT "You do not have the correct privilege to do this."
$ EXIT
$!
$ gotpriv:
$!
$! save repeat time interval, if given
$!
$ interval = P1
$!
$! Define the LAT command
$!
$ LAT :== $LATCP
$! REFORMAT :== $SYS_VAXNET:REF
$!
$ bigloop:
$!
$ IF F$SEARCH ("user0:[accounting]lat_counts.temporary") .NES. "" THEN -
	DELETE /NOLOG user0:[accounting]lat_counts.temporary;*
$!
$ debug_count = 0
$!
$ SPAWN /NOLOG /OUT = user0:[accounting]lat_counts.temporary -
	LAT SHOW COUNT /NODE
$! reformat user0:[accounting]lat_counts.temporary
$ DEFINE /USER SYS$OUTPUT NL:
$ EDIT /TPU /NOINIT /NOSECT /NODISP /COMMAND = new_util:fix_crlf_silent -
	user0:[accounting]lat_counts.temporary
$!
$ IF F$SEARCH ("user0:[accounting]lat_counts.seq") .NES. ""
$ THEN
$     OPEN /APPEND /ERROR = nofile outfile user0:[accounting]lat_counts.seq
$ ELSE
$     OPEN /WRITE /ERROR = nofile outfile user0:[accounting]lat_counts.seq
$ ENDIF
$!
$! Get fields
$!
$ OPEN /READ /ERROR = nofile tmpfile user0:[accounting]lat_counts.temporary
$ outstring = F$FAO("!%D ", 0)
$!
$ readloop1:
$!
$ READ /END = endfile1 tmpfile tmpstring
$ tmpstring = F$EDIT (tmpstring, "COMPRESS, TRIM, UPCASE")
$!
$! If the line is too short (there are some blank lines) get the next line
$!
$ IF F$LENGTH (tmpstring) .LE. 1 THEN $ GOTO readloop1
$!
$! Parse out parts of the line.
$!
$ count = F$ELEMENT (0, " ", tmpstring)
$!
$! Reject lines which don't have counts.  No good lexical for this
$! so:
$!
$ IF count .EQS. "LCP"      THEN $ GOTO readloop1
$ IF count .EQS. "PROTOCOL" THEN $ GOTO readloop1
$ IF count .EQS. "REPEAT"   THEN $ GOTO readloop1
$!
$!! text  = F$EXTRACT(F$LOCATE(" ", tmpstring), F$LENGTH(tmpstring), tmpstring)
$!
$ IF F$LOCATE (">", count) .NE. F$LENGTH (count) THEN -
        count = F$EXTRACT (1, F$LENGTH (count), count)
$ number = F$INTEGER (count)
$!
$ debug_count = debug_count + 1
$ outstring = outstring + F$FAO ("!9ZL ", number)
$!
$ GOTO readloop1
$!
$ endfile1:
$!
$ WRITE outfile outstring
$ CLOSE outfile
$ CLOSE tmpfile
$ IF debug_count .GT. 15
$ THEN
$    RENAME /NOLOG user0:[accounting]lat_counts.temporary;0 -
	user0:[accounting]lat_counts.wrong
$    PURGE /NOLOG /KEEP = 10 user0:[accounting]lat_counts.wrong
$ ENDIF
$!
$ LAT SET COUNT /ZERO
$!
$! Check to see if we were asked to auto-repeat
$!
$ IF interval .EQS. "" THEN $ GOTO exitout
$ WAIT 'interval
$ GOTO bigloop
$!
$ exitout:
$!
$ SET NOON
$!
$ savepriv = F$SETPRV (savepriv)
$!
$ EXIT $STATUS
$!
$ nofile:
$!
$ WRITE SYS$OUTPUT "Problem with statistics file"
$ GOTO exitout
$!
$!
$! B. Z. Lederman	20-Oct-1989
$!			23-May-1990	reject text lines with no counters
$!			26-Sep-1991	Delete temp file when done!
