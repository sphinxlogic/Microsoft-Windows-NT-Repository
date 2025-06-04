$ WRITE SYS$OUTPUT F$ENVIRONMENT("PROCEDURE")
$! Parse out counts on Ethernet device and store in database.
$!
$ ON ERROR THEN CONTINUE
$!
$ quote = """
$!
$ IF F$MODE () .EQS. "BATCH"
$ THEN
$    SET PROCESS /NAME = "QNA-0 Counts"
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
$ NCP SHOW LINE QNA-0 COUNT TO qna_0_counts.tmp
$!
$ OPEN /WRITE /ERROR = nofile outfile qna_0_counts_store.com
$!
$ WRITE outfile "$ RUN SYS$SYSTEM:RDO"
$ WRITE outfile "invoke database filename 'user0:[accounting]qna'"
$ WRITE outfile "start_transaction read_write reserving qna_counts for shared write"
$ WRITE outfile "store D in qna_counts using"
$!
$! Get fields
$!
$ OPEN /READ /ERROR = nofile tmpfile qna_0_counts.tmp
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
$ ON ERROR THEN $ CONTINUE
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
$ DATABLOCKSRECEIVED:
$ WRITE outfile "    d.DATA_BLOCKS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ MULTICASTBLOCKSRECEIVED:
$ WRITE outfile "    d.MULTICAST_BLOCKS_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ RECEIVEFAILURE:
$ RECEIVEFAILUREINCLUDING:
$ WRITE outfile "    d.RECEIVE_FAILURE = " + count + ";"
$ GOTO readloop1
$!
$ BYTESRECEIVED:
 WRITE outfile "    d.BYTES_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ MULTICASTBYTESRECEIVED:
$ WRITE outfile "    d.MULTICAST_BYTES_RECEIVED = " + count + ";"
$ GOTO readloop1
$!
$ DATAOVERRUN:
$ WRITE outfile "    d.DATA_OVERRUN = " + count + ";"
$ GOTO readloop1
$!
$ DATABLOCKSSENT:
$ WRITE outfile "    d.DATA_BLOCKS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ MULTICASTBLOCKSSENT:
$ WRITE outfile "    d.MULTICAST_BLOCKS_SENT = " + count + ";"
$ GOTO readloop1
$!
$ BLOCKSSENTMULTIPLECOLLISIONS:
$ WRITE outfile "    d.BLOCKS_SENT_MULTIPLE_COLLISIONS = " + count + ";"
$ GOTO readloop1
$!
$ BLOCKSSENTSINGLECOLLISION:
$ WRITE outfile "    d.BLOCKS_SENT_SINGLE_COLLISION = " + count + ";"
$ GOTO readloop1
$!
$ BLOCKSSENTINITIALLYDEFERRED:
$ WRITE outfile "    d.BLOCKS_SENT_INITIALLY_DEFERRED = " + count + ";"
$ GOTO readloop1
$!
$ BYTESSENT:
$ WRITE outfile "    d.BYTES_SENT = " + count + ";"
$ GOTO readloop1
$!
$ MULTICASTBYTESSENT:
$ WRITE outfile "    d.MULTICAST_BYTES_SENT = " + count + ";"
$ GOTO readloop1
$!
$ SENDFAILURE:
$ SENDFAILUREINCLUDING:
$ WRITE outfile "    d.SEND_FAILURE = " + count + ";"
$ GOTO readloop1
$!
$ COLLISIONDETECTCHECKFAILURE:
$ WRITE outfile "    d.COLLISION_DETECT_CHECK_FAILURE = " + count + ";"
$ GOTO readloop1
$!
$ UNRECOGNIZEDFRAMEDESTINATION:
$ WRITE outfile "    d.UNRECOGNIZED_FRAME_DESTINATION = " + count + ";"
$ GOTO readloop1
$!
$ SYSTEMBUFFERUNAVAILABLE:
$ WRITE outfile "    d.SYSTEM_BUFFER_UNAVAILABLE = " + count + ";"
$ GOTO readloop1
$!
$ USERBUFFERUNAVAILABLE:
$ WRITE outfile "    d.USER_BUFFER_UNAVAILABLE = " + count
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
$ DELETE /NOLOG qna_0_counts.tmp;*
$!
$ NCP ZERO LINE QNA-0 COUNT
$!
$! Now store the counts in the database
$!
$ @qna_0_counts_store.com
$!
$! Don't need file anymore.
$!
$ DELETE /NOLOG qna_0_counts_store.com;*
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
