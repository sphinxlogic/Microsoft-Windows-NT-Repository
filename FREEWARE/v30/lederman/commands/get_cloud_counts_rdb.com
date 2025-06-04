$ WRITE SYS$OUTPUT F$ENVIRONMENT("PROCEDURE")
$!
$! Parse out counts on some X25 DLM circuits and store in a database.
$!
$ ON ERROR THEN CONTINUE
$!
$ quote = """
$!
$ IF F$MODE () .EQS. "BATCH"
$ THEN
$    SET PROCESS /NAME = "Cloud Counts"
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
$ NCP SHOW CIRCUIT X25-CLOUD-INC COUNT TO cloud_counts.tmp
$!
$ OPEN /WRITE /ERROR = nofile outfile cloud_counts_store.com
$!
$ WRITE outfile "$ RUN SYS$SYSTEM:RDO"
$ WRITE outfile "invoke database filename 'user0:[accounting]qna'"
$ WRITE outfile "start_transaction read_write reserving cloud_in for shared write"
$ WRITE outfile "store D in cloud_in using"
$!
$ GOSUB process_file
$!
$ WRITE outfile "end_store"
$ WRITE outfile "commit"
$!
$ NCP SHOW CIRCUIT X25-CLOUD-OUT COUNT TO cloud_counts.tmp
$!
$ WRITE outfile "start_transaction read_write reserving cloud_out for shared write"
$ WRITE outfile "store D in cloud_out using"
$!
$ GOSUB process_file
$!
$ WRITE outfile "end_store"
$ WRITE outfile "commit"
$ WRITE outfile "exit"
$ WRITE outfile "$ EXIT"
$ CLOSE outfile
$!
$ DELETE /NOLOG cloud_counts.tmp;*
$!
$ NCP ZERO CIRCUIT X25-CLOUD-OUT COUNT
$ NCP ZERO CIRCUIT X25-CLOUD-INC COUNT
$!
$! Now store the counts in the database
$!
$ @cloud_counts_store.com
$!
$! Don't need file anymore.
$!
$ DELETE /NOLOG cloud_counts_store.com;*
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
$! SUBROUTINES
$!
$ process_file:
$!
$! Get fields
$!
$ OPEN /READ /ERROR = nofile tmpfile cloud_counts.tmp
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
$!
$ count = F$EDIT (count, "COLLAPSE")
$!
$ IF count .EQS. "CircuitCoun" THEN $ GOTO date_time	! date string
$ IF count .EQS. "Circuit=X2"  THEN $ GOTO readloop1	! skip circ. ident.
$!
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
$ length = length - 22
$ count = F$EXTRACT (22, length, tmpstring)
$ WRITE outfile "    d.date_time = " + quote + count + quote + ";"
$ GOTO readloop1
$!
$ SECONDSSINCELASTZEROED:
$ WRITE outfile "    d.SECONDS_SINCE_LAST_ZEROED = " + count + ";"
$ GOTO readloop1
$!
$ TERMINATINGPACKETSRECEIVED:
$ WRITE outfile "    d.TERMINATING_PACKETS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ ORIGINATINGPACKETSSENT:
$ WRITE outfile "    d.ORIGINATING_PACKETS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ TERMINATINGCONGESTIONLOSS:
$ WRITE outfile "    d.TERMINATING_CONGESTION_LOSS = " + count + ";"
$ GOTO readloop1
$!
$ TRANSITPACKETSRECEIVED:
$ WRITE outfile "    d.TRANSIT_PACKETS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ TRANSITPACKETSSENT:
$ WRITE outfile "    d.TRANSIT_PACKETS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ TRANSITCONGESTIONLOSS:
$ WRITE outfile "    d.TRANSIT_CONGESTION_LOSS = " + count + ";"
$ GOTO readloop1
$!
$ CIRCUITDOWN:
$ WRITE outfile "    d.CIRCUIT_DOWN = " + count + ";"
$ GOTO readloop1
$!
$ INITIALIZATIONFAILURE:
$ WRITE outfile "    d.INITIALIZATION_FAILURE = " + count + ";"
$ GOTO readloop1
$!
$ ADJACENCYDOWN:
$ WRITE outfile "    d.ADJACENCY_DOWN = " + count + ";"
$ GOTO readloop1
$!
$ AVAILABLE:		!! DON'T KNOW WHAT THIS IS, BUT:
$ WRITE outfile "    d.AVAILABLE = " + count + ";"
$ GOTO readloop1
$!
$ PEAKADJACENCIES:
$ WRITE outfile "    d.PEAK_ADJACENCIES = " + count + ";"
$! GOTO readloop1
$!
$! When we hit this field, we're done.
$!
$ endfile1:
$!
$ CLOSE tmpfile
$!
$ RETURN
$!
$! B. Z. Lederman	17-Oct-1991	Store date in Rdb database
$!			29-May-1992	Modify for X25 DLM circuits
