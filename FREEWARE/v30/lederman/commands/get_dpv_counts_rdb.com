$ WRITE SYS$OUTPUT F$ENVIRONMENT("PROCEDURE")
$! Parse out counts on DPV-11 device and store in database.
$!
$ ON ERROR THEN CONTINUE
$!
$ quote = """
$!
$ IF F$MODE () .EQS. "BATCH"
$ THEN
$    SET PROCESS /NAME = "DPV-0 Counts"
$ ELSE
$    ON CONTROL_Y THEN $ GOTO exitout
$ ENDIF
$!
$! Find out if we have the required privileges.
$!
$ savepriv = F$SETPRV ("OPER,SYSPRV")
$ IF F$PRIVILEGE ("OPER,SYSPRV") THEN GOTO gotpriv
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
$! Define the NCP command
$!
$ NCP :== $NCP
$!
$ bigloop:
$!
$ NCP SHOW LINE DPV-0 COUNT TO dpv_0_counts.tmp
$!
$ OPEN /WRITE /ERROR = nofile outfile dpv_0_counts_store.com
$!
$ WRITE outfile "$ RUN SYS$SYSTEM:RDO"
$ WRITE outfile "invoke database filename 'user0:[accounting]qna'"
$ WRITE outfile "start_transaction read_write reserving dpv_counts for shared write"
$ WRITE outfile "store D in dpv_counts using"
$!
$! Get fields
$!
$ OPEN /READ /ERROR = nofile tmpfile dpv_0_counts.tmp
$!
$ readloop1:
$!
$ ON ERROR THEN $ GOTO endfile1			! reset after goto error
$!
$ READ /END = endfile1 tmpfile tmpstring
$ length = F$LENGTH (tmpstring)
$ IF length .LT. 15 THEN $ GOTO readloop1	! some lines too short
$!
$ count = F$EXTRACT (0, 12, tmpstring)			! count portion
$ IF count .EQS. "            " THEN $ GOTO readloop1	! blank field
$ count = F$EDIT (count, "COLLAPSE")
$ IF count .EQS. "LineCounter" THEN $ GOTO date_time	! date string
$ length = length - 14
$ field = F$EXTRACT (14, length, tmpstring)		! type of count
$ field = F$EDIT (field, "COLLAPSE, TRIM, UPCASE")
$!
$ IF F$LOCATE (">", count) .NE. F$LENGTH (count) THEN -	! counter overflow
        count = F$EXTRACT (1, F$LENGTH (count), count)	! remove
$!
$ IF F$LOCATE (",", field) .NE. F$LENGTH (field)	! remove commas
$ THEN
$    tmpstring = field
$    field = F$ELEMENT (0, ",", tmpstring) + F$ELEMENT (1, ",", field)
$ ENDIF
$!
$ IF F$LOCATE (":", field) .NE. F$LENGTH (field)	! remove colon
$ THEN
$    field = F$ELEMENT (0, ":", field)
$ ENDIF
$!
$ ON WARNING THEN $ GOTO goto_error	! in case of error
$!
$ GOTO 'field'				! process this counter
$!
$ goto_error:
$ WRITE SYS$OUTPUT "Unknown field " + field
$ GOTO readloop1:
$!
$ date_time:
$ length = length - 20
$ count = F$EXTRACT (20, length, tmpstring)
$ WRITE outfile "    d.date_time = " + quote + count + quote + ";"
$ GOTO readloop1
$!
$ SECONDSSINCELASTZEROED:
$ WRITE outfile "    d.SECONDS_SINCE_LAST_ZEROED = " + count + ";"
$ GOTO readloop1
$!
$ Secondssincelastzeroed:
$ WRITE outfile "    d.Seconds_since_last_zeroed = " + count + ";"
$ GOTO readloop1
$!
$ Bytesreceived:
$ WRITE outfile "    d.Bytes_received = " + count + ";"
$ GOTO readloop1
$!
$ Bytessent:
$ WRITE outfile "    d.Bytes_sent = " + count + ";"
$ GOTO readloop1
$!
$ Datablocksreceived:
$ WRITE outfile "    d.Data_blocks_received = " + count + ";"
$ GOTO readloop1
$!
$ Datablockssent:
$ WRITE outfile "    d.Data_blocks_sent = " + count + ";"
$ GOTO readloop1
$!
$ Dataerrorsinbound:
$ Dataerrorsinboundincluding:
$ WRITE outfile "    d.Data_errors_inbound = " + count + ";"
$ GOTO readloop1
$!
$ Dataerrorsoutbound:
$ DATAERRORSOUTBOUNDINCLUDING:
$ WRITE outfile "    d.Data_errors_outbound = " + count + ";"
$ GOTO readloop1
$!
$ Remotereplytimeouts:
$ WRITE outfile "    d.Remote_reply_timeouts = " + count + ";"
$ GOTO readloop1
$!
$ Localreplytimeouts:
$ WRITE outfile "    d.Local_reply_timeouts = " + count + ";"
$ GOTO readloop1
$!
$ Remotebuffererrors:
$ Remotebuffererrorsincluding:
$ WRITE outfile "    d.Remote_buffer_errors = " + count + ";"
$ GOTO readloop1
$!
$ Localbuffererrors:
$ Localbuffererrorsincluding:
$ WRITE outfile "    d.Local_buffer_errors = " + count + ";"
$ GOTO readloop1
$!
$ Remoteprocesserrors:
$ Remoteprocesserrorsincluding:
$ WRITE outfile "    d.Remote_process_errors = " + count + ";"
$ GOTO readloop1
$!
$ localprocesserrors:
$ localprocesserrorsincluding:
$ WRITE outfile "    d.local_process_errors = " + count
$!
$! When we hit this field, we're done.
$!
$ endfile1:
$!
$ WRITE outfile "end_store"
$ WRITE outfile "commit"
$ WRITE outfile "exit"
$ WRITE outfile "$ EXIT"
$ CLOSE outfile
$ CLOSE tmpfile
$ DELETE /NOLOG dpv_0_counts.tmp;*
$!
$ NCP ZERO LINE DPV-0 COUNT
$!
$! Now store the counts in the database
$!
$ @dpv_0_counts_store.com
$!
$! Don't need file anymore.
$!
$ DELETE /NOLOG dpv_0_counts_store.com;*
$!
$! Check to see if we were asked to auto-repeat
$!
$ IF interval .EQS. "" THEN GOTO exitout
$ WAIT 'interval'
$ GOTO bigloop
$!
$ exitout:
$!
$ savepriv = F$SETPRV (savepriv)
$!
$ nofile:
$!
$ EXIT $STATUS
$!
$!
$! B. Z. Lederman	17-Oct-1991	Store date in Rdb database
