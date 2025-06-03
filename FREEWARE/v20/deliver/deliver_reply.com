$	SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
	  + F$VERIFY(0'DELIVER_REPLY_VERIFY' .GT. 0)
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$	ON WARNING THEN GOTO ABNORMAL_EXIT
$ 	GoTo AFter_Headers
$ !                                                              
$ !     Provided for use with DELIVER by Andy Leslie
$ !                                      Sandhurst, Surrey, England
$ !
$ ! The purpose of this auto-reply routine is to be able to cope with
$ !  the various quotated address notations - ALLIN1 and ARPAnet for two
$ ! e.g.
$ !	ARPAGATEWAY::"xxxxxx@xxx.xxx.xxx" "Personal Name"
$ !	NODE::MRGATE::"A1::A User" "Personal Name"
$ !          
$ !  as well as ordinary DECnet addresses, including PMR
$ ! e.g.
$ !	NODE::User 		"Personal Name"
$ !
$ !		and MRGATE. Other gateways must be added manually.
$ !
$ !     DELIVER_REPLY.COM
$ !	V1.4	- Andy Leslie, Last updated 28-7-1987.
$ !     
$After_Headers:
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
$	DELETE	= "DELETE"
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
$	WAIT	= "WAIT"
$	WRITE	= "WRITE"
$ ! Redefine OPUT as follows for debugging
$ ! Oput = "Write SYS$OUTPUT"
$ ! Leave it as a comment for now
$ Oput ="!"
$ !
$ dquote="""
$ !
$ ! read sys$input from 
$ ! NODE::MRGATE::"A1::USER"
$ ! NODE::NODE::User "Me"
$ !
$ from=f$edit(from,"COMPRESS,TRIM")
$ !
$ Oput from
$ !
$ address = from
$ address_len = 'f$length(address)'
$ oput "address length:",address_len
$ space_pos = 'f$locate(" ",address)'
$ mrgate_pos = 'f$locate("MRGATE::",address)'
$ oput "mrgate_pos:",mrgate_pos
$ extstr= f$extract(mrgate_pos,99,address)
$ Oput "from mrgate:: on:",extstr                 
$       
$ first_dquote_pos = 'f$locate(dquote,address)'  
$ oput first_dquote_pos
$ extstr= f$extract(first_dquote_pos+1,99,address)
$ Oput "from first quote on:",extstr                 
$ second_dquote_pos = 'f$locate(dquote,extstr)' + first_dquote_pos +1
$ Oput second_dquote_pos
$
$ extstr= f$extract(second_dquote_pos+1,99,address)
$ Oput "from second quote on:",extstr                 
$ third_dquote_pos = 'f$locate(dquote,extstr)' + second_dquote_pos +1
$ Oput third_dquote_pos                
$
$ extstr=f$extract(third_dquote_pos+1,99,address) 
$ Oput "from third quote on:",extstr                 
$ fourth_dquote_pos = 'f$locate(dquote,extstr)'	+ third_dquote_pos    +1
$ Oput fourth_dquote_pos                     
$ !
$ !
$Remove_Personal_name:
$ 
$ If first_dquote_pos +1 .eq. address_len then goto remove_poor_mans_routing
$ ! No Quotes at all!
$
$ oput f$extract(first_dquote_pos-1 ,1,address)
$ oput f$extract(0,second_dquote_pos+1, ADDRESS)
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
$ 	QUOTE_POS=F$LOCATE(DQUOTE,ADDRESS)
$	
$	STRING = F$EXTRACT(POS+2,999,ADDRESS)
$	MRSTRING = F$EXTRACT(POS+2,6,address)
$                                  
$ oput address
$ oput string
$ oput mrstring                                  
$ oput f$extract(quote_pos,999,address) 
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
$ search_string = address
$ if mrstring .eqs. "MRGATE" then search_string = f$extract(quote_pos,999,address) 
$ write sys$output "Final address was: ",address
$ ! Check for replies sent list's presence
$ if F$SEARCH("DELIVER$DIR:DELIVER.HISTORY") .nes. "" then goto search_list
$ open/write HISTORY DELIVER$DIR:DELIVER.HISTORY
$ goto add_list
$ !
$ ! Check the list for this sender
$ search_list:
$ search/nooutput DELIVER$DIR:DELIVER.HISTORY 'search_string
$ if $status .eq. 1 then exit
$ open/append HISTORY DELIVER$DIR:DELIVER.HISTORY
$ !
$ add_list:
$ write HISTORY FROM
$ close HISTORY 
$ VMS_VERSION = F$GETSYI("VERSION")                
$ SYS_VERSION = F$EXTRACT(1,1,VMS_VERSION)
$ IF  (SYS_VERSION .EQ. 5) THEN goto V5
$ V4:
$	mail/noself -
	/subject="Auto-Reply from my DELIVER Mail-daemon" -
   	DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
$	Goto Normal_Exit
$ V5:
$ ! Note that "/personal_name=" is a VMS V5 feature
$	mail/noself -
	/pers="DELIVER Mail-Daemon" -
	/subject="Auto-Reply from my DELIVER Mail-daemon" -
   	DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
$
$ !                     
$ ! Normal exit.
$ !
$NORMAL_EXIT:
$	STATUS = $STATUS     			!save success status
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
$      	EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
