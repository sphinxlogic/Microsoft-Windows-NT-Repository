$ in_verify = 'F$VERIFY(0)'
$!
$! Detect and reset 'synchronizing' X.25 DECnet links.
$! Comments at end of file.
$!
$ ON ERROR THEN $ GOTO exitout
$ ON CONTROL_Y THEN $ GOTO exitout
$ ON SEVERE_ERROR THEN $ GOTO exitout
$!
$ IF F$MODE() .EQS. "BATCH" THEN SET PROCESS/NAME="Reset X.25 Cir."
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
$ doloop:
$!
$! Get information about our current state
$!
$ NCP SHOW KNOWN CIRCUITS TO showcircuits.temporaryfile
$ OPEN /READ /ERROR=nofile tmpfile showcircuits.temporaryfile
$!
$ readloop:
$!
$ READ /END=endfile tmpfile tmpstring
$ tmpstring = F$EDIT(tmpstring, "COMPRESS, TRIM, UPCASE")
$!
$! If the line is too short (there are some blank lines) get the next line
$!
$ IF F$LENGTH(tmpstring) .LE. 1 THEN GOTO readloop
$!
$! Parse out parts of the line.  If it's not an X.25 line, skip it.
$!
$ circuit = F$ELEMENT(0, " ", tmpstring)
$ select = F$EXTRACT(0, 4, CIRCUIT)
$ IF select .NES. "X25-" THEN GOTO readloop
$ state   = F$ELEMENT(1, " ", tmpstring)
$ sync    = F$ELEMENT(2, " ", tmpstring)
$!
$! Find lines which have to be fixed.
$!
$ IF sync .EQS. "-SYNCHRONIZING" THEN GOTO fixit
$ IF sync .EQS. "-FAILED" THEN GOTO fixit
$!
$ GOTO readloop
$!
$ fixit:
$!
$! Try to reset the circuit
$!
$ NCP SET CIRCUIT 'circuit STATE OFF
$ WAIT 00:00:02
$ NCP SET CIRCUIT 'circuit STATE ON
$!
$ GOTO readloop
$!
$ endfile:
$ CLOSE tmpfile
$ DELETE/NOLOG showcircuits.temporaryfile;*
$!
$! Check to see if we were asked to auto-repeat
$!
$ IF interval .EQS. "" THEN GOTO exitout
$ WAIT 'interval
$ GOTO doloop
$!
$ exitout:
$!
$ savepriv = F$SETPRV(savepriv)
$!
$ nofile:
$!
$ EXIT $STATUS + 0*F$VERIFY(in_verify)
$!
$! Automatic detection of X.25 DECnet links going into synchronizing
$! state and reset (hopefully).
$!
$! B. Z. Lederman	04-Oct-1989
