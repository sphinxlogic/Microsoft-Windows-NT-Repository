$	SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
	  + F$VERIFY(0'MAIL_BATCH_CLEANUP_VERIFY' .GT. 0)
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$!                          
$	GOTO END_OF_HEADER_COMMENTS
$!
$! A DELIVER utility...
$
$! Provides support to MAIL_BATCH.COM
$
$! Keith Edmunds, June 11, 1988
$
$ 
$! Edit History:
$!
$! 11 June 1988    	Original version (T.01)
$! 28 July 1988		Cosmetic changes, tidied up (V1.0)
$!
$END_OF_HEADER_COMMENTS:
$!
$	version = "V1.0"
$!
$! Reset commands that the user may have redefined.
$!
$	APPEND		= "APPEND"
$	CLOSE		= "CLOSE"
$	CONVERT		= "CONVERT"
$	CREATE		= "CREATE"
$	DEASSIGN	= "DEASSIGN"
$	DEFINE		= "DEFINE"
$	DELETE		= "DELETE"
$	EDIT		= "EDIT"
$	EXIT		= "EXIT"
$	GOTO		= "GOTO"
$	IF		= "IF"
$	INQUIRE		= "INQUIRE"
$	MAIL		= "MAIL"
$	ON		= "ON"
$	OPEN		= "OPEN"
$	PURGE		= "PURGE"
$	READ		= "READ"
$	RENAME		= "RENAME"
$	SEARCH		= "SEARCH"
$	SET		= "SET"
$	SORT		= "SORT"
$	SPAWN		= "SPAWN"
$	SUBMIT		= "SUBMIT"
$	TYPE		= "TYPE"
$	WAIT		= "WAIT"
$	WRITE		= "WRITE"
$!
$
$ 	on error then goto it_broke
$
$! Firstly, we mail the logfile from our received batch job back to us:
$
$ 	fwd_user = f$trnlnm("DELIVER$FORWARD_ADDRESS")
$ 	mail/noself/subject="Batch results" deliver$dir:b_mail_'P1'.log -
		'fwd_user'
$
$! Now delete all temporary files we used:
$                       
$ 	delete deliver$dir:b_mail_'P1'.*;*
$ 	goto exit
$
$! If something went wrong, tell us where the files are:
$
$ it_broke:
$ 	mail/noself/subject="Do: $ dir DELIVER$DIR:*''pid'" sys$input 'fwd_user'
$ deck
	Your mailed batch job failed somewhere. On your home node, type in the
DCL command given in the subject of this mail. Examination of any files listed
there will possibly help identify what went wrong.
$ eod
$ exit:
$	EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
