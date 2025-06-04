    	.title nm$message
	.ident "26"

!++
! 									 
! Copyright (c) 1985, 1986, 1987, 1988, 1989, 1992, 1993, 1994
! by Digital Equipment Corporation, Maynard, Mass.			 
! 									 
! Facility:	NMAIL
!
! Abstract:	Network mailer
!
! Environment:	VMS
!
! Author:	Dave Porter  (Mu::Porter)
!		Networks and Communications
!
! Created:	15-Mar-1985
!
! Revision history:
!
!   01  26-Sep-1985
!	Change wording of SHOXNO, SHONDEL to reflect the fact
!	that a zero field means that the daemon applies defaults
!
!   02	02-Oct-1985
!	Add SHONDLV message. Change 'unsent' to 'pending' in a 
!	couple of messages, to clarify what is really meant.
!
!   03	19-Oct-1985
!	Change NOTALL to NOMEDDL (to support introduction of /USER 
!	in SHOW and CANCEL commands)
!
!   04	04-Dec-1985
!	Change some messages to reflect changes in daemon report
!	generation routines
!
!   05	04-Dec-1985
!	Add NETHUNG message
!
!   06	02-Apr-1986
!	Add SHOHLD, SHOCMP, SHOERR messages for job status
!
!   07	14-Apr-1986
!	Add TEXTC, NONXQUE messages. Change file I/O errors
!	to use "!AD" instead of "!AS" to avoid problems with
!	local descriptors. Add QSTOP, QWAIT, QDEL messages.
!
!   08	20-Apr-1986
!	Add CANCEL message, and privilege check messages
!
!   09	21-Apr-1986
!	Add CC-related mesages
!
!   10	23-Apr-1986
!	Replace/remove LOGxxx message to reflect new fatal
! 	error logging mechanism.  Change `pending' back to `unsent'
!	to fit better with new state-selection qualifier for SHOW.
!  	Add SHONOSE for SHOW output display use.
!
!   11	26-Jun-1987
!  	Add EOFERR message for SHOW; add XFROM, RBLURT messages for 
!  	error reports; add SHORET, SHONRET for no-return-to-sender
!	options. 
!
!   12	10-Aug-1987
!	Add various messages related to the ANALYZE/REPAIR command
!
!   13	20-Sep-1987
!	Add NULERR for SHOW use            
!
!   14	24-Jan-1989
!	Add messages used for detection of retained jobs
!	in ANALYZE/REPAIR.
!
!   15	30-Jan-1989
!	Add various messages related to support of MAIL-11 3.1
!
!   16	9-Feb-1989
!	Add NOQUES message
!
!   17	12-Feb-1989
!	Make changes needed to support separate return-to-sender
!	messages from daemon to user.
!
!   18	14-Feb-1989
!	Add SHONRPT message
!
!   19	18-Feb-1989
!	Add messages to support the /RETURN option on cancel
!
!   20	21-Feb-1989
!	Add REQUEUE message for broadcast notification
!
!   21	1-Jul-1992
!	Add FAO, PRVUP, PRVDWN fatal errors
!
!   22 	22-Apr-1993
!	Change ANFILE - file numbers can exceed 65535
!
!   23	15-Nov-1993
!	Add ANNORPT, AN2OLD, AN2NEW, ANFILDT
!         
!   24	19-Nov-1993
!	Add messages to support cancellation reason string
!
!   25	8-Dec-1993
!	Add EXTNF, EXTNT, EXTBAD fatal messages
!
!   26	13-Jan-1994
!	Add NOFILES warning
!--         

	.facility /system /prefix=nm$_  nmail,386


                              
        
	.severity info
!
! Informational messages
!
NOJOBS	<no jobs found in Nmail queues>
NOQUES	<no active Nmail queues available>
QSTART	<starting queue !AS>					/fao=1
QSTOP	<stopping queue !AS>					/fao=1
QWAIT	<waiting for queue !AS to stop>				/fao=1
QDEL	<deleting queue !AS>					/fao=1
QRUN	<queue !AS is already started>				/fao=1
CANCEL	<job !SL cancelled>					/fao=1
RELEAS  <job !SL released>					/fao=1
CANPEND	<job !SL cancellation requested>			/fao=1
TEXT	<!AS>							/fao=1
TEXTC	<!AC>							/fao=1

	.severity warning
!
! Lesser errors
!
FILOFLO	<too many files in job; not all will be deleted>
FILOFL2	<too many files in job; not all will be accounted for>
NOREASN	<cancellation reason ignored; old-style message file>
NOFILES	<no files in job>

        .severity error
!
! Miscellaneous error conditions. READ and WRITE can sometimes 
! be reported in the control file, if they occur whilst actually
! sending a message, but the rest are always conventionally used.
! 
READ	<error reading from file !AD>				/fao=2
WRITE	<error writing to file !AD>				/fao=2
SUBMIT	<failed to submit Nmail job>
RESUB	<failed to submit job !AS to queue !AS>			/fao=2
IVNUM	<invalid decimal number '!AS'>				/fao=1
IVTIME	<invalid time '!AS'>					/fao=1
TOOSOON	<interval '!AS' is too small>				/fao=1
TRNLNM	<error translating logical name '!AS'>			/fao=1
NONXJOB	<no such job>
JOBACT	<job is active>
JOBFREE	<job is not being held in queue>
JOBCAN	<job already marked for cancellation>
QUEACT	<queue !AS is not stopped>				/fao=1
QUEFAIL	<failed to start queue !AS>				/fao=1
NONRET	<message is defined as non-returnable>
NOSYSPRV<operation requires SYSPRV privilege>
NONETMBX<operation requires NETMBX privilege>
NOOPER  <operation requires OPER privilege>
NOCMKRNL<operation requires CMKRNL privilege>
MODE	<operation only allowed in interactive mode process>

	.severity error
!
! Network I/O errors. These are all reported in the current control file.
! Later analysis will determine whether they are hard or soft errors
! from the point of view of the particular message.
!
NONET	<network not available>
LOGLINK	<error creating network link to node !AS>		/fao=1
SENDERR	<error sending to node !AS>				/fao=1
TIMEOUT	<no response from slave mail on node !AS>		/fao=1
NETHUNG	<timed out sending to slave mail on node !AS>		/fao=1
NOACCEPT<node !AS cannot accept this message format>		/fao=1
PROTOCOL<network protocol error>
RACE	<expected mailbox AST not yet processed>

	.severity fatal
!
! The following errors are fatal; no attempt is made to report
! them in the control file for any individual mail message.
! 
OPEN	<error opening file !AD>				/fao=2
CLOSE	<error closing file !AD>				/fao=2
SEARCH	<error searching for file !AD>				/fao=2
PARSE	<error parsing !AD>					/fao=2
RMS	<unexpected RMS error>
JBC	<unexpected job controller error>
CLIBUG	<unexpected parse error>
UNXFC	<unexpected MAIL function code (!SL)>			/fao=1
SYNCH	<something didn't happen (flags=^x!XW)>			/fao=1
MBXIO	<mailbox I/O error>
BADFIL	<not a valid control file>
BADALG	<block alignment error in control file>
BADBIO	<fractional block read from control file>
BADFLG	<contradictory flags in control file>
ABORT	<aborted by request>
SYNTAX	<syntax error in '!AS'>					/fao=1
BADTIM	<invalid time value set as Nmail system default>
INSTREE	<error building internal file list (!SL node!%S)> 	/fao=1
NONXQUE	<no such queue>
BADBUF	<disagreeable buffer size specified by MAIL>
FAO	<unexpected formatting error>
PRVUP	<failed to set privilege from exec mode>
PRVDWN	<failed to reset privilege from exec mode>
EXTNF	<failed to activate Nmail extension image !AS>		/fao=1
EXTNT	<no entry-point table found in !AS>			/fao=1
EXTBAD	<malformed entry-point table in !AS>			/fao=1


	.severity success

!
! These are bits and pieces of string which are used to construct
! a failure report, to be mailed back to the original sender.
!
SSUBJ	<Nmail delivery summary>
RSUBJ	<Report on failed mail>
MSUBJ	<Returned to sender>
RBLURB	<Errors were detected when processing your mail message>
RBLURT	< which was entered at !17%D>				/fao=1
CBLURB	<Your mail message of !17%D has been cancelled>		/fao=1
CBLURT	< by request of user !AS at !17%D>			/fao=2
CBLURN	< but no further details are available.>
GBLURB	<The following error message was returned whilst sending to>
TBLURB	<The text of your failed mail message follows:>
ZBLURB	<There was no text in your failed mail message.>
NBLURB	<Text of original message suppressed by user request.>
MBLURB	<Your original message will be returned separately.>
THISAD	< address !AS>						/fao=1
XTO	<To:   !AS>						/fao=1
XCC	<CC:   !AS>						/fao=1
XSUBJ	<Subj: !AS>						/fao=1
XFROM	<From: !AS>						/fao=1
CWHY1	<Reason: !AS>						/fao=1
CWHY2	<      : !AS>						/fao=1
EHARD	<This is a hard error.>
ESOFT	<This is a soft error.>
EEXP	<This is a soft error, but the retry period has expired.>
ECAN	<This is a soft error, but the message has been cancelled.>
EUNKN	<** unknown error severity **>
ENOMOR	<No more attempts to send to this address will be made.>
EMOR	<Attempts to send to this address will continue.>
REQUEUE	<Nmail message requeued as job !SL!/To: !AS>		/fao=2

!
! More bits and pieces used to construct a 'fatal error' report
! which is then sent to the operator console
!
LOG1	<!AC fatal error (oops)>				/fao=1
LOG2	<Job   !AS!/Queue !AS!/Entry !SL>			/fao=3
LOG3	<User  !AS!/File  !AS> 					/fao=2

!
! Stuff used for the 'SHOW' command output
!
SHOHDR	<Queue entry !SL  !+!AS>				/fao=3
SHOHDAL	<Queue entry !SL, user !AS  !AS>			/fao=3
SHOENT	< Message entered at !17%D>				/fao=1
SHOENUK	< Message entered at unknown time>	
SHOTIM	< Attempted !SL time!%S, most recently at !17%D>	/fao=2
SHONEW	< No attempt to send has yet been made>
SHOXAT	< Expires after !+!SL attempt!%S>			/fao=2
SHOXTM	< Expires at !17%D!+>					/fao=2
SHOXOR	< Expires at !17%D or after !SL attempt!%S>		/fao=2
SHOXNO	< Default expiry period>
SHODEL	< Retry interval is !13%D>		        	/fao=1
SHONDEL	< Default retry interval>                        
SHOFORN	< Foreign or non-text message>
SHOFROM	< From !AS>						/fao=1
SHOTO	< To !AS>						/fao=1
SHOCC	< CC !AS>						/fao=1
SHOSUBJ	< Subject "!AS">		 	 		/fao=1
SHOSUBX	< Subject (not shown)!+>				/fao=1
SHOSUM	< Summary: !SL unsent, !SL sent, !SL failed>		/fao=3
SHODLV	< Confirmation of job completion required>
SHONDLV	< No confirmation of job completion>
SHORET	< To be returned to sender on error>
SHONRET	< Not to be returned to sender>
SHONRPT < Delivery failure not reported to sender>
SHOCRTS	< Message is being cancelled>

!
! More small bits used to fill in parts of 'SHOW' displays
!                                          
SHOADR	<Addressee>
SHOSTS	<Status>
SHOLST	<Time>
SHOUNS	<unsent   >
SHODUN	<sent     >
SHOFAIL <failed   >
SHOEXP  <expired  >
SHOCAN	<cancelled>
SHOXEQ	<active>
SHOHLD	<held in queue>
SHOTIL	<held until>
SHOCMP	<completed>
SHOERR	<error in execution>
SHONOSE	<none selected for display>
EOFERR	<(a new message is being posted: please try SHOW again later)>
NULERR	<(no error message text was received from the slave mail)>
NEVTRY	<(no attempt ever made to send this message)>

	.severity warning 

!
! Stuff used in ANALYZE/REPAIR output. 
! Some of these codes are also used as error statuses internally
! to NM$REPAIR, hence the 'warning' severity.
!
ANFILE	<File !AS (!UL,!UL,!UL)>				/fao=4
ANFILDT	< created at !+!17%D>					/fao=2
ANFROM	< from user !AS at !17%D>				/fao=2
ANEMPTY	<  is not a valid control file - it's empty>
ANBADF	<  is not a valid control file - the format is incorrect>
ANOBSF	<  is not a valid control file - unsupported obsolete format>
ANPREOF	<  is not a valid control file - premature EOF encountered>
ANFLMIX	<  is not a valid control file - contradictory flags byte>
ANXVBN	<  is not a valid control file - no VBN pointers in control record>
ANUNKFR	<  is not a valid control file - cannot determine sender>
ANUNDIR	<  is not in the usual Nmail directory [warning only]>
ANMULDIR<  can be reached by !SW paths [warning only]>		/fao=1
ANUNQUE	<  has no corresponding job in the Nmail queue>
ANMULQUE<  appears !SW times in jobs in the Nmail queue>	/fao=1
ANSTUCK	<  is being retained after system error (job !SL)>	/fao=1
ANHELD	<  is being held until explicitly released (job !SL)>	/fao=1
ANWEIRD	<  is in a weird state  - job !SL may need manual fixup>/fao=1
ANLOST	<Job !SL contains a nonexistent or inaccessible file>	/fao=1
AN2FILE	<Job !SL contains !SL files (usual number is one)>	/fao=2
ANNORPT <Return-to-sender is not possible for this message [warning only]>
AN2OLD	<Repair skipped - message is older than the specified /SINCE time>
AN2NEW	<Repair skipped - message is newer than the specified /BEFORE time>
ANASKDEL<Delete this file?>
ANASKSUB<Submit this file?>
ANASKREL<Release this job?>
ANASKCAN<Cancel this job?>
ANASKFLU<Cancel extraneous jobs?>                 
ANDELOK	<File !AS deleted>					/fao=1
ANSUBOK <Job !SL entered in queue !AS> 				/fao=2
ANHOLDOK<Job !SL entered in queue !AS in holding state>		/fao=2
ANRELOK <Job !SL released>					/fao=1
ANRELHOK<Job !SL released to holding state>			/fao=1
ANCANOK	<Job !SL cancelled>					/fao=1
ANZERO	<No errors detected>
ANSUMM	<!SL error!%S detected:>				/fao=1
ANBADCNT<  !SL invalid control file!%S>				/fao=1
ANUNQCNT<  !SL control file!%S not in the queue>		/fao=1
ANMQUCNT<  !SL control file!%S multiply queued>			/fao=1
ANLOSCNT<  !SL lost or inaccessible file!%S>			/fao=1
ANMFJCNT<  !SL job!%S with more than one file>			/fao=1
ANUNDCNT<  !SL file!%S not in the Nmail work directory>		/fao=1
ANMDFCNT<  !SL file!%S multiply entered in the directory>	/fao=1
ANRETCNT<  !SL job!%S retained in queue>   			/fao=1
ANHLDCNT<  !SL job!%S held in queue>				/fao=1
ANWRDCNT<  !SL job!%S in weird state>				/fao=1
ANSIGCNT<  !SL execution error!%S during analysis run>		/fao=1
ANACTION<Corrective action taken:>
ANDELCNT<  !SL file!%S deleted>					/fao=1
ANSUBCNT<  !SL job!%S submitted>				/fao=1
ANRELCNT<  !SL job!%S released>					/fao=1
ANCANCNT<  !SL job!%S cancelled>				/fao=1
ANUNKU	<(unknown)>
ANNETU	<(network)>
XXYES	<Yes>
XXNO	<No>
XXDEL	<Delete>
XXCAN	<Cancel>
REPRESN	<message file recovered when repairing the Nmail queue>

!
! Trace text (note, TRJOB must be first message here, and TRZZZ
! must be the last trace message)
!
TRJOB	< job !SL user !AS>					/fao=2
TREOJ	< end job !SL>						/fao=1
TRCONN	< to !AS>						/fao=1
TRDISC  < >
TRABO	< abort>
TRSEND  " -tx->  !AF"						/fao=2
TRRECV	" <-rx-  !AF"						/fao=2
TRRECN  " <-rx-  !XL"						/fao=1
TRZZZ	<.>

 	.end
