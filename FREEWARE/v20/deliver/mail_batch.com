$	SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
	  + F$VERIFY(0'MAIL_BATCH_VERIFY' .GT. 0)
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$!                          
$	GOTO END_OF_HEADER_COMMENTS
$!
$! A DELIVER utility...
$!
$! This command file enables you to execute a batch job on your home
$! system whilst you are away. To use, put the following lines in your
$! MAIL.DELIVERY file:
$!
$! * * "SUBJECT" T E @deliver$dir:mail_batch
$! * * "SUBJECT" T Q
$!
$! "SUBJECT" should be changed to something obscure. Ensure that you haver both
$! this file and MAIL_BATCH_CLEANUP.COM in the directory pointed to by the 
$! logical DELIVER$DIR. You will also need to add the following line to your 
$! LOGIN.COM file:
$!
$!  $ define deliver$forward_address NODE::NAME
$!
$! where NODE and NAME should be changed to the node and username from
$! which you will use this facility.
$!
$! Now you can simply mail a batch command file to your home system, using
$! the subject chosen above, and your home system will execute the command
$! file and mail the logfile back to you. Useful for checking directories,
$! changing file protection, looking at text files (just TYPE them), etc.
$!
$! Donated to DELIVER_V1_3 kit by
$! Keith Edmunds, June 11, 1988
$!
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
$ 	on error then goto it_broke
$
$ ! Use PID to construct temporary filenames
$
$ 	pid = f$getjpi("","PID")
$
$ ! Now check the batch request came from us
$
$ 	from = f$element(0," ",from)
$ 	fwd_user = f$trnlnm("DELIVER$FORWARD_ADDRESS")
$ 	if fwd_user .eqs. from then goto okay
$ breakin:
$
$ ! Here means either someone picked our subject field for batch requests
$ ! inadvertently, or they are pushing their luck...
$ ! Send the offending mail to me:
$
$ 	mail/noself/subject="Invalid batch request received from ''from'" -
		'message_file' 'fwd_user'
$
$ ! ...and quit
$ 	exit
$
$ okay:
$
$ ! Now construct a .COM file to run our batch job
$ ! First some crude error handling:
$
$ 	open/write batch_file deliver$dir:B_MAIL_'PID'.COM
$	write batch_file "$ set noverify"
$ 	write batch_file "$ define sys$output deliver$dir:B_Mail_''pid'.Log"
$ 	write batch_file "$ ex*it = ""goto b_mail_done"""
$ 	write batch_file "$ error_occured = 0"
$ 	write batch_file "$ on error then goto catch_error"
$ 	write batch_file "$ set verify"
$
$ ! Now copy the text of the mail accross:
$
$ 	open/read input_file 'message_file'
$ copy_file:
$ 	read/end_of_file=copy_done input_file the_line
$ 	write batch_file the_line
$ 	goto copy_file
$
$ ! Now some cleanup code:
$
$ copy_done:
$	write batch_file "$ set noverify"
$ 	write batch_file "$ b_mail_done:"
$ 	write batch_file "$ ex*it:=exit"
$ 	write batch_file "$ goto cleanup"
$ 	write batch_file "$ catch_error:"
$ 	write batch_file "$ error_occured = 1"
$ 	write batch_file "$ show symbol/all"
$ 	write batch_file "$ cleanup:"
$ 	write batch_file "$ deassign sys$output"
$ 	write batch_file "$ close B_Mail_''pid'.Log"
$ !
$ ! Ensure that our batch job cleans up after itself, and tells us of any
$ ! errors:
$ !
$ 	write batch_file "$ submit/noprint/after=""+0:1:00"" -"
$ 	write batch_file "  	/nolog -"
$ 	write batch_file "	deliver$dir:mail_batch_cleanup.com -"
$ 	write batch_file "	/parameter=(''pid')"
$ 	write batch_file "$ exit"               
$
$ ! Close off our files:
$
$ 	close input_file
$ 	close batch_file
$
$ ! ...and submit the job:

$
$ 	submit/noprint/log=deliver$dir:b_mail_'pid'.log -
		deliver$dir:b_mail_'pid'.com
$
$ ! ...and get out.
$  
$ 	goto exit
$
$ it_broke:
$
$ ! There is little we can do if it failed, other than tell ourselves it
$ ! died:
$
$ 	mail/noself/subject="Error occured in ''f$environment(""PROCEDURE"")'" -
		nl: 'fwd_user'
$ exit:
$	EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
