$	SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
	  + F$VERIFY(0'DELIVER_MYMAIL_VERIFY' .GT. 0)
$	SET ON
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$	ON WARNING THEN GOTO ABNORMAL_EXIT
$ 	GoTo After_Headers
$ !
$ ! The purpose of this DELIVER_MYMAIL routine is to reply to a message whose
$ ! subject is specified in my mail.delivery, with all new mail since I last 
$ ! read mail on my home node. There are two methods of getting newmail, see
$ ! below.
$ !
$ ! The entry in MAIL.DELIVERY looks like this, excluding "$ !" on each line
$ ! It should go after your auto-forward line and before your auto-reply line.
$ !    WARNING: DO NOT BE OBVIOUS IN YOUR CHOICE OF THE MAGIC SUBJECT.
$ !============================================================================
$ !! Send a reply containing all new mail
$ !!
$ !*                       *   "Chosensubj"    T E "$@dir:DELIVER_MYMAIL.COM"
$ !*                       *   "Chosensubj"    T Q
$ !============================================================================
$ !          
$ !	V1.0	- Andy Leslie, Last updated 1-4-1988.
$ !
$After_headers:
$ !
$ ! Put all your DELIVER command files EXCEPT MAIL.DELIVERY in DELIVER$DIR
$ ! MAIL.DELIVERY must go in SYS$LOGIN
$ !
$ ! Reset commands that the user may have redefined.
$ !
$	APPEND	= "APPEND"
$	CLOSE	= "CLOSE"
$	CONVERT	= "CONVERT"
$	CREATE	= "CREATE"
$	DEFINE	= "DEFINE"       
$	DELETE 	= "DELETE"
$	EXIT	= "EXIT"
$	GOTO	= "GOTO"
$	IF	= "IF"
$	MAIL	= "MAIL"
$	ON	= "ON"
$	OPEN	= "OPEN"
$	PURGE	= "PURGE"                                       
$	READ	= "READ"
$	RENAME	= "RENAME"
$	SEARCH	= "SEARCH"
$	SET	= "SET"
$	SPAWN	= "SPAWN"
$	SUBMIT	= "SUBMIT"
$	TYPE	= "TYPE"         
$	WAIT   	= "WAIT"
$	WRITE	= "WRITE"
$       
$
$ dquote="""
$ !
$ from=f$edit(from,"COMPRESS,TRIM")
$ address = from
$ address_len = 'f$length(address)'
$ space_pos = 'f$locate(" ",address)'
$ mrgate_pos = 'f$locate("MRGATE::",address)'
$ extstr= f$extract(mrgate_pos,99,address)
$       
$ first_dquote_pos = 'f$locate(dquote,address)'  
$ extstr= f$extract(first_dquote_pos+1,99,address)
$ second_dquote_pos = 'f$locate(dquote,extstr)' + first_dquote_pos +1
$
$ extstr= f$extract(second_dquote_pos+1,99,address)
$ third_dquote_pos = 'f$locate(dquote,extstr)' + second_dquote_pos +1
$
$ extstr=f$extract(third_dquote_pos+1,99,address) 
$ fourth_dquote_pos = 'f$locate(dquote,extstr)'	+ third_dquote_pos    +1
$ !
$Remove_Personal_name:
$ 
$ If first_dquote_pos +1 .eq. address_len then goto remove_poor_mans_routing
$ ! No Quotes at all!
$
$ ! First quote is pers
$ If f$extract(first_dquote_pos-1 ,1,address) .nes. " " then goto endif1
$ 	address = f$element(0," ", ADDRESS)
$ 	goto remove_poor_mans_routing
$ endif1:
$ ! First quote is routing
$ If f$extract(first_dquote_pos-1 ,1,address) .nes. ":" then goto endif2
$ 	address = f$extract(0,second_dquote_pos+1, ADDRESS)
$ 	goto remove_poor_mans_routing
$ endif2:
$
$ ! address = f$extract(0,pers_dquote_pos, ADDRESS) ! Remove Personal Name
$ 
$REMOVE_POOR_MANS_ROUTING: 
$       
$	POS = 'F$LOCATE("::",ADDRESS)' 
$ 	MRGATE_POS=F$LOCATE("MRGATE",ADDRESS) 
$ 	QUOTE_POS=F$LOCATE(QUOTE,ADDRESS)
$	
$	STRING = F$EXTRACT(POS+2,999,ADDRESS)
$	MRSTRING = F$EXTRACT(POS+2,6,address)
$                                  
$      	IF MRSTRING .EQS. "MRGATE" THEN goto remote_address_found
$ !		deals with NODE::MRGATE::"A User @REO"
$	IF STRING - "::" .EQS. STRING THEN goto remote_address_found
$  	IF POS .gt. quote_pos THEN goto remote_address_found
$ !		deals with "A1::A User"
$	address=string
$	GOTO REMOVE_POOR_MANS_ROUTING
$
$ remote_address_found:
$ qfrom = address
$ from  = address
$ write sys$output "Final address to send all newmail to was: ",address
$ !      
$ ! Now to extract and send the NEWMAIL folder to the sender of the magic
$ !  subject line.
$ ! There are two ways of handling this. 
$ ! 1) copies all the mail into a deliver$temp folder
$ ! 2) doesn't
$ ! The difference is that the EXTRACT/ALL
$ ! directly from Newmail places all the newmail into the mail
$ ! folder - thus you only get the mail since last you executed this 
$ ! procedure. take your pick and change the goto if need be.
$ !
$ goto method_1   ! All newmail
$ ! goto method_2 ! Only newmail since last I did this
$                                                        
$ Method_1:
$ !      
$ mail
sel newmail
copy/all/noconfirm deliver$temp
select deliver$temp
extract/all/mail sys$scratch:deliver$temp_mail.mai;
select deliver$temp
delete/all
exit
$ !
$ goto MAIL_IT
$ !
$ Method_2:
$ !                                                      
$ mail
sel newmail
copy/all/noconfirm deliver$temp
extract/all/mail sys$scratch:deliver$temp_mail.mai;
exit
$
$MAIL_IT:
$ VMS_VERSION = F$GETSYI("VERSION")                
$ SYS_VERSION = F$EXTRACT(1,1,VMS_VERSION)
$ IF  (SYS_VERSION .EQ. 5) THEN goto V5
$ V4:
$	mail/noself sys$scratch:deliver$temp_mail.mai;  -
	/subject="Newmail from the DELIVER-daemon" -
   	DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
$	Goto Normal_Exit
$ V5:
$ ! Note that "/personal_name=" is a VMS V5 feature
$	mail/noself sys$scratch:deliver$temp_mail.mai;  -
	/pers="DELIVER Mail-Daemon" -
	/subject="Newmail from the DELIVER-daemon" -
   	DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
$
$ delete sys$scratch:deliver$temp_mail.mai;*
$ !                                                      
$ ! Normal exit.
$ !
$NORMAL_EXIT:
$	STATUS = $STATUS			!save success status
$	GOTO EXIT                                               
$ !
$ ! Abnormal exit.
$ !
$ABNORMAL_EXIT:
$	STATUS = $STATUS			!save failure status
$	IF STATUS THEN STATUS = "%X0800002A"	!force error if neccessary
$	GOTO EXIT
$ !
$ ! Exit.
$ !                     
$EXIT:
$	EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
