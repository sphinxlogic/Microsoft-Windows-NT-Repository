$! Parse out counts on X25-PROTOCOL DTE and store in database.
$!
$ ON ERROR THEN CONTINUE
$!
$ quote = """
$!
$ IF F$MODE () .EQS. "BATCH"
$ THEN
$    SET PROCESS /NAME = "X25-P Counts"
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
$ NCP SHOW MODU X25-P DTE 311061701114 COUNT TO x25_p_counts.tmp
$!
$ OPEN /WRITE /ERROR = nofile outfile x25_p_counts_store.com
$!
$ WRITE outfile "$ RUN SYS$SYSTEM:RDO"
$ WRITE outfile "invoke database filename 'user0:[accounting]qna'"
$ WRITE outfile "start_transaction read_write reserving x25p_counts for shared write"
$ WRITE outfile "store D in x25p_counts using"
$!
$! Get fields
$!
$ OPEN /READ /ERROR = nofile tmpfile x25_p_counts.tmp
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
$ IF count .EQS. "DTE         " THEN $ GOTO readloop1	! skip this
$ IF count .EQS. "Network     " THEN $ GOTO readloop1	! skip this
$ count = F$EDIT (count, "COLLAPSE")
$ IF count .EQS. "ModuleX25-P" THEN $ GOTO date_time	! date string
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
$ ON ERROR THEN $ CONTINUE
$ WRITE SYS$OUTPUT "Unknown field " + field
$ GOTO readloop1:
$!
$ date_time:
$!
$ length = length - 35
$ count = F$EXTRACT (35, length, tmpstring)
$ WRITE outfile "    d.date_time = " + quote + count + quote + ";"
$ GOTO readloop1
$!
$ BYTESRECEIVED:
$ WRITE outfile "    d.BYTES_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ BYTESSENT:
$ WRITE outfile "    d.BYTES_SENT = " + count + ";"
$ GOTO readloop1
$!
$ CALLSRECEIVED:
$ WRITE outfile "    d.CALLS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ CALLSSENT:
$ WRITE outfile "    d.CALLS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ DATABLOCKSRECEIVED:
$ WRITE outfile "    d.DATA_BLOCKS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ DATABLOCKSSENT:
$ WRITE outfile "    d.DATA_BLOCKS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ FASTSELECTSRECEIVED:
$ WRITE outfile "    d.FAST_SELECTS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ FASTSELECTSSENT:
$ WRITE outfile "    d.FAST_SELECTS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ LOCALLYINITIATEDRESETS:
$ WRITE outfile "    d.LOCALLY_INITIATED_RESETS = " + count + ";"
$ GOTO readloop1
$!
$ MAXIMUMCHANNELSACTIVE:
$ WRITE outfile "    d.MAXIMUM_CHANNELS_ACTIVE = " + count + ";"
$ GOTO readloop1
$!
$ MAXIMUMSWITCHEDCIRCUITSACTIVE:
$ WRITE outfile "    d.MAXIMUM_SWITCHED_CIRCUITS = " + count + ";"
$ GOTO readloop1
$!
$ NETWORKINITIATEDRESETS:
$ WRITE outfile "    d.NETWORK_INITIATED_RESETS = " + count + ";"
$ GOTO readloop1
$!
$ RECEIVEDCALLRESOURCEERRORS:
$ WRITE outfile "    d.RECEIVED_CALL_RESOURCE_ER = " + count + ";"
$ GOTO readloop1
$!
$ REMOTELYINITIATEDRESETS:
$ WRITE outfile "    d.REMOTELY_INITIATED_RESETS = " + count + ";"
$ GOTO readloop1
$!
$ RESTARTS:
$ WRITE outfile "    d.RESTARTS = " + count + ";"
$ GOTO readloop1
$!
$! Put this field last even though it's first to try to force
$! DCL to read more of the labels in
$!
$ SECONDSSINCELASTZEROED:
$ WRITE outfile "    d.SECONDS_SINCE_LAST_ZEROED = " + count + ";"
$ GOTO readloop1
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
$ DELETE /NOLOG x25_p_counts.tmp;*
$!
$ NCP ZERO MODU X25-P KNOWN DTE COUNT
$!
$! Now store the counts in the database
$!
$ @x25_p_counts_store.com
$!
$! Don't need file anymore.
$!
$ DELETE /NOLOG x25_p_counts_store.com;*
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
$! B. Z. Lederman	15-Jul-1992
