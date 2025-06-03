$	SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
	  + F$VERIFY(0'MAKE_MAIL_DELIVERY_VERIFY' .GT. 0)
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$	ON WARNING THEN GOTO ABNORMAL_EXIT
$!                          
$	VERSION = "V1.0"	
$	GOTO END_OF_HEADER_COMMENTS
$!        
$! This procedure builds a MAIL.DELIVERY of your choice 
$! and provides several helpful hints. Written by Andy Leslie, 27-July-1988
$!
$! NOTE: DELIVER *MUST* be installed with privs if Mail.Delivery's created
$! here are to run unmodified. You should change all V's to D's and W's to F's
$! otherwise.
$!
$! The DELIVER$STARTUP.COM supplied will Install DELIVER with all privs needed
$!
$END_OF_HEADER_COMMENTS:
$!
$! Reset commands that the user may have redefined.
$!
$	APPEND		= "APPEND"
$	CALL		= "CALL"
$ 	CLOSE		= "CLOSE"
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
$
$ 	ESC[0,8]	= 27
$
$ type/PAGE sys$input
                              MAKE_MAIL_DELIVERY

    This Procedure helps you create a MAIL.DELIVERY file.
    A MAIL.DELIVERY file is necessary for you to successfully use DELIVER.
    You may have to re-edit the output before use: see "constraints".

    Hit Control-Y at any time to abort it if you wish.

    Once it has run, sanity check it and then rename the output file from
    NEW_MAIL.DELIVERY to SYS$LOGIN:MAIL.DELIVERY

    Constraints: 20 Chars for "from"
                 18 chars for subject, "to"
                 Addresses containing quotes may not be coped with too well!
    
    Wildcards ("*") can be used anywhere.

    Good Luck.

    Andy Leslie, 

$ user = F$GETJPI("","USERNAME")
$ time = f$time()
$ node=f$trnlnm("SYS$CLUSTER_NODE")
$ if node .eqs. "" then node=f$trnlnm("SYS$NODE")
$ user="''node'''user'"
$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Continue''esc'[m"
$ write sys$output "''esc'[2J''esc'[H"
$ write sys$output " "
$ CLOSE/NOLOG NMD
$ open/WRITE nmd new_mail.delivery
$ write nmd "!=============================================================================="
$ write nmd "! MAIL.DELIVERY   "
$ write nmd "!	   created by " + "''user' at ''time'"
$ write nmd "!"
$ write nmd "! - Parameter file for DELIVER, this MUST be placed in SYS$LOGIN:"
$ write nmd "!"
$ write nmd "!=============================================================================="
$ write nmd "!"
$ write nmd "! Keys:"
$ write nmd "!"
$ write nmd "!<from> <to> <sub> <accept> <action> <parameter>"
$ write nmd "!                     A always"
$ write nmd "!                     X never"
$ write nmd "!                     T if all match"
$ write nmd "!                     F if patterns did not all match"
$ write nmd "!                     O if all match and no previous match"
$ write nmd "!                     B if no match and no previous match"
$ write nmd "!                     E if all match OR no previous match"
$ write nmd "!<from> <to> <sub>  <accept> <action> <parameter>"
$ write nmd "!                               A append contents to file <parameter>"
$ write nmd "!                               C copy contents to file <parameter>"
$ write nmd "!                               D deliver normally"
$ write nmd "!                               V deliver using system privilege"
$ write nmd "!                               E execute command file <parameter>"
$ write nmd "!                               F forward to <parameter>"        
$ write nmd "!                               W forward using system priv to <parameter>"
$ write nmd "!                               Q quit - no action (Always the last action for this item)"
$ write nmd "!"
$ write nmd "! <from>               <to>  <subject>    <accept> <action>   <parameter>"
$ write nmd "!"   
$ type sys$input
  
	If you wish to forward any messages to yourself at a remote
	location, please give the address here

$ read sys$command/prompt="Forwarding Address? " forward_addr
$ type sys$input
  
	If you wish to auto reply using DELIVER_REPLY or DELIVER_MYMAIL
	then give a full directory specification here (no logical names)
	for the location of these files. You will also be asked where you 
	wish logfiles to go, default to both is SYS$LOGIN:.

	This will create a DELIVER$DIR.COM file to re-create the DELIVER$DIR
	and DELIVER$LOG logicals each time DELIVER runs.

        WARNING: DO NOT EDIT or DELETE SYS$LOGIN:DELIVER$DIR.COM EVER!

	If you have already DEFINED these logicals then the translation of 
	this logical will be used.

	This will always be executed by your DELIVER batch jobs and must 
	remain in SYS$LOGIN:.

$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Continue''esc'[m"
$ if f$trnlnm("DELIVER$DIR") .nes. "" then goto dir_known
$ read sys$command/prompt="Full DELIVER$DIR directory spec: [SYS$LOGIN:] " deliver_dir 
$
$Dir_known:
$
$ if "''deliver_dir'" .eqs. "" then deliver_dir = f$trnlnm("DELIVER$DIR")
$
$ if f$trnlnm("DELIVER$LOG") .nes. "" then goto log_known
$ read sys$command/prompt="Full DELIVER$LOG directory spec: [SYS$LOGIN:] " deliver_log
$
$log_known:
$                
$ if f$trnlnm("DELIVER$LOG") .nes. "" then deliver_log = f$trnlnm("DELIVER$LOG") 
$ close/nolog deliver$dirfile
$ open/write deliver$dirfile sys$login:deliver$dir.com
$ write deliver$dirfile "$! DELIVER$DIR.COM - DO NOT EDIT AS THIS WILL BE PURGED!"
$ write deliver$dirfile "$!	   created by " + "''user' at ''time'"
$ if "''deliver_dir'" .eqs. "" then deliver_dir = "SYS$LOGIN:"
$ if "''deliver_log'" .eqs. "" then deliver_log = "SYS$LOGIN:"
$ write deliver$dirfile "$ DEFINE DELIVER$DIR ''deliver_dir'"
$ write deliver$dirfile "$ DEFINE DELIVER$LOG ''deliver_log'"
$ close/nolog deliver$dirfile
$ purge sys$login:deliver$dir.com/nolog        
$ write nmd "!"                 
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!             -*- Define DELIVER$DIR every time -*-"
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"                 
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ""$@DELIVER$DIR.COM""", -
"*", "*", "*", "A E")
$ Call FROM
$ Call FWD_BY_ADDR
$ CALL SEND_NEWMAIL
$ CALL AUTO_REPLY
$ CALL AUTO_FORWARD
$ CALL DELIVER
$ CALL FINISH
$ goto NORMAL_EXIT
$
$
$ FROM: Subroutine       
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$ type/page sys$input
                           

	We will now deal with messages tags on "From's", "To's" and 
	Subjects that you do not wish to reply to. Optionally you may
	elect to forward them to the forwarding address (if given earlier).

    	This facility would normally be used to not reply to distribution 
	lists and standard messages, such as testing for "*lights* so that
	you don't reply to messages about car lights left on in the car park.
                  
$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Continue''esc'[m"
$ write sys$output "''esc'[2J''esc'[H"
$ write sys$output " "
$
$ From_i_loop: 
$ write sys$output " "
$ inquire yesno  -
"Do you wish to enter a ""From"" to not auto-reply to? <Y/N>"
$ if .not. yesno then goto to_i_loop
$ From_Banner:
$ write nmd "!"                 
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!                      -*- <from> <to> <sub>  rules -*-"
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"                 
$ from_loop:
$ read/End_Of_File=To_i_loop -
sys$command/prompt="""From:"" node? (may include *) [^Z to goto next prompt] " from_node 
$ From_node = "''From_node'" - "::"
$ read sys$command/prompt="""From:"" username? (may include *) " From_username 
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", -
"""''From_node'::''From_username' *""","*","*","T V")
$ if forward_addr .eqs. "" then goto from_loop
$ inquire yesno  -
"Do you wish this mail to be forwarded to you? <Y/N>"
$ if yesno then -      
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'", -
"""''From_node'::''From_username' *""","*","*","T W")
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", -
"""''From_node'::''From_username' *""","*","*","T Q")
$ goto From_loop
$ !
$ To_i_loop:                    
$ write sys$output " "
$ inquire yesno  -
"Do you wish to enter a ""To:"" to not auto-reply to? <Y/N>"
$ if .not. yesno then goto Subject_i_loop
$
$ To_loop:
$ read/End_Of_File=subject_i_loop -
 sys$command/prompt="""To:"" (may be user, * or a distribution list) [^Z to goto next prompt] " to 
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)","*","""''to'""","*","T V")
$ if forward_addr .eqs. "" then goto to_loop
$ inquire yesno  -
"Do you wish This mail to be forwarded to you? <Y/N>"
$ if yesno then -
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'" -
,"*","""''to'""","*","T W")
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) " -
,"*","""''to'""","*","T Q")
$ goto To_loop
$
$ Subject_i_loop:                                  
$ write sys$output " "
$ inquire yesno  -
"Do you wish to enter a subject to not auto-reply to? <Y/N>"
$ if .not. yesno then goto from_end
$
$ Subject_loop:
$ read/end_of_file=from_end -
sys$command/prompt="Subject? (may include *) [^Z to goto next prompt] " Subject 
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", - 
"*","*","""''subject'""","T V")
$ if forward_addr .eqs. "" then goto Subject_loop
$ inquire yesno  -
"Do you wish This mail to be forwarded to you? <Y/N>"
$ if yesno then -
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'" -
,"*","*","""''subject'""","T W")
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", -
"*","*","""''subject'""","T Q")
$ goto Subject_loop
$ FROM_End:
$ EndSubroutine              
$ !               
$ FWD_BY_ADDR: SUBROUTINE
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$ Fwd_to_3rd:
$ write sys$output " "
$ inquire yesno  -
"Do you wish to enter a subject to be Forwarded to a 3rd party ? <Y/N>"
$ if .not. yesno then goto fwd_by_addr_end

$
$Fwd_to_3rd_banner:
$ write nmd "!" 
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!     -*- Forwarding by Address + Subject to a 3rd Party -*- "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"                                             
$ fwd_to_3rd_loop:             
$ read/end_of_file= FWD_BY_ADDR_END -
sys$command/prompt="Subject? (may include *) [^Z to goto next prompt] " Subject              
$ read/end_of_file=FWD_BY_ADDR_END -
sys$command/prompt="3rd Party to forward this mail to? " address
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''address'","*","""''subject'""","*","T W")
$ inquire yesno  -                
"Do you wish this mail to be discarded after delivery to the 3rd Party? <Y/N>"
$ if .not. yesno then -      
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ", -
"*","""''subject'""","*","T V")
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ", -
"*","""''subject'""","*","T Q")
$ goto fwd_to_3rd_loop
$ FWD_BY_ADDR_END:
$ ENDSUBROUTINE
$                                 
$ AUTO_FORWARD: SUBROUTINE
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$ Forward_i_Loop:            
$ if forward_addr .eqs. "" then goto auto_forward_end
$ write sys$output " "
$ inquire yesno  -                
"Do you wish to auto-forward remaining messages? <Y/N>"
$ if .not. yesno then goto AUTO_FORWARD_END
$
$ Forward_Banner:
$ write nmd "!" 
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!                          -*- Forwarding Address -*-                         "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"
$ Forward_loop:
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'","*","*","*","A W")
$ !                        
$ AUTO_FORWARD_END:
$ ENDSUBROUTINE
$
$ AUTO_REPLY: SUBROUTINE
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$ write sys$output " "
$
$ inquire yesno "Do you wish to auto-reply to any mail? <Y/N>"
$ if .not. yesno then goto AUTO_REPLY_END
$
$ write nmd "! "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!                                Reply to mail                                "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ""$@deliver$dir:DELIVER_REPLY.COM/OUT=deliver$dir:DELIVER_REPLY.LOG""" , -
"*","*","*","T E")
$ AUTO_REPLY_END:
$ ENDSUBROUTINE
$                                              
$ SEND_NEWMAIL: SUBROUTINE
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$ Allmail_i_loop:                                         
$ type sys$input                        

	You can specify a subject that, if it is used, will cause the 
	DELIVER_MYMAIL procedure to be invoked, which will send ALL NEW 
	MAIL to the sender.

	You might use this if you have a borrowed account and don't wish to
 	auto-forward your mail to it.

$ inquire yesno "Do you wish to be able to use this facility? <Y/N>"
$ if .not. yesno then goto send_newmail_end
$ Allmail_Loop:
$ write nmd "! "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!                                Send on all newmail                          "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"
$  read sys$command/prompt="Chosen Subject (be obtuse!)? " subject 
$ write nmd -
f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ""$@deliver$dir:DELIVER_MYMAIL.COM/OUT=deliver$dir:DELIVER_MYMAIL.LOG""" ,-
 "*","*","""''subject'""","T E")
$ write nmd -
"! Don't bother to deliver this mail" 
$ write nmd -
f$fao("!(20AS) !(18AS) !(18AS) !(4AS)" , "*","*","""''subject'""","T Q")
$ SEND_NEWMAIL_END:
$ ENDSUBROUTINE
$ 
$ FINISH: SUBROUTINE 
$	ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
$ Quit:                                                   
$ write nmd "!"
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!               Finally, explicitely quit              "
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)","*", "*", "*", "T Q")
$ write nmd "! End"
$ FINISH_END:
$ ENDSUBROUTINE
$
$DELIVER: SUBROUTINE
$ write nmd "!"
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!               Deliver all mail not quit from by this point"
$ write nmd "!-----------------------------------------------------------------------------"
$ write nmd "!"
$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)","*", "*", "*", "T V")
$DELIVER_END:
$ ENDSUBROUTINE
$!
$! Normal exit.
$!
$NORMAL_EXIT:
$	STATUS = $STATUS			!save success status
$ type/page sys$input  

	Now review the NEW_MAIL.DELIVER file. If it is correct, rename it
	to SYS$LOGIN:MAIL_DELIVER.COM. Ensure that DELIVER_REPLY.COM,
        DELIVER_REPLY.TXT and DELIVER_MYMAIL.COM are in place and MAIL 
	SET FORW DELIVER%USERNAME.

	DELIVER$DIR:DELIVER.HISTORY holds a history of all those replied to.
	
	You may wish to delete this at the end of each absence.

	- Don't forget to test this before setting it going!

$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Continue''esc'[m"
$ write sys$output "''esc'[2J''esc'[H"
$ write sys$output " "
$	GOTO EXIT
$!
$! Abnormal exit.
$!
$ABNORMAL_EXIT:
$	STATUS = $STATUS			!save failure status
$	IF STATUS THEN STATUS = "%X0800002A"	!force error if neccessary
$ type/page sys$input

	Procedure has aborted

	Your NEW_MAIL.DELIVER file will be in an unfinished state

$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Continue''esc'[m"
$ write sys$output "''esc'[2J''esc'[H"
$ write sys$output " "
$	GOTO EXIT
$!
$! Exit.
$!
$EXIT:
$
$ 	CLOSE/NOLOG NMD
$ 	Write SYS$OUTPUT " "
$	Write SYS$OUTPUT "All Done"
$ 	Write SYS$OUTPUT " "
$	EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
