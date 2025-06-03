.TITLE		NEWS_MSG
.FACILITY	NEWS,1044/PREFIX=NEWS__
.IDENT		"V4.8"
!++
!
! DESCRIPTION:
!
!   These are the message codes used by the NEWS utility.
!
! AUTHOR:   	    M. Madison
!   	    	    COPYRIGHT © 1992-1994, MADGOAT SOFTWARE.
!   	    	    ALL RIGHTS RESERVED.
!
! CREATION DATE: 09-MAR-1989
!
! MODIFICATION HISTORY:
!
!   17-MAR-1989	V1.0-001    Madison 	Initial coding.
!   02-JUN-1989	V1.0-002    Madison 	Added news__usingcurgroup.
!   05-JUN-1989	V1.0-003    Madison 	Added news__noartspec.
!   14-FEB-1990 V1.0-004    Madison 	news__newgroup changed a bit.
!   16-FEB-1990	V2.0	    Madison 	Added msgs for read/thread.
!   21-FEB-1990	V2.0-1	    Madison 	Added SETARCHERR
!   23-FEB-1990	V2.1-2	    Madison 	Added MARKCOUNT, NEWCOUNT
!   24-FEB-1990 V2.1-3	    Madison 	Fix MARKCOUNT, NEWCOUNT.
!   01-MAR-1990	V2.2-4	    Madison 	Add FWDERR, ADDRSYNTX, CANCELLED.
!   23-MAY-1990	V2.2-5	    Madison 	Get rid of !/ in UNEXPRSP.
!   28-JUN-1990	V2.3-6	    Madison 	Add KWDSYNTAX, RECONNECTING.
!   13-NOV-1990	V2.4	    Madison 	Add NOPARENT.
!   09-JAN-1991	V2.5	    Madison 	Add NOMOREMATCH, NOSUBJSPEC.
!   11-OCT-1991	V3.0	    Madison 	Add SUBSCRIBED.
!   04-NOV-1991	V3.0-1	    Madison 	Add BODYERR.
!   04-JUN-1992	V3.1	    Madison 	Add NOREPLYADDR.
!   11-SEP-1992	X4.0	    Madison 	Add GRPSET, DIDSUB, DIDUNSUB
!   17-SEP-1992	X4.0-1	    Madison 	Add NOIGHDSPEC.
!   15-FEB-1993	V4.0-2	    Madison 	Remove all !AZ references.
!   29-JUN-1993	X4.3	    Madison 	Add LISTREPLYERR.
!   09-OCT-1993	V4.5	    Madison 	Add NOSERVICE.
!   10-NOV-1993	V4.5-1	    Madison 	Split GRPSET message into two parts.
!   15-JAN-1994	V4.5-2	    Madison 	Add NOSUCHARTICLE.
!   28-MAR-1994	V4.6-1	    Madison 	Add WRITERR.
!   16-MAY-1994	V4.8	    Madison 	Add NOMOREGRP.
!--

.SEVERITY   	FATAL
NOCONNECT   	<could not connect to NNTP server !AD>/FAO=2
NOSERVICE   	<NNTP server !AD not available>/FAO=2

.SEVERITY   	ERROR
CFGERR	    	<error reading NEWS configuration information>
UNEXPRSP    	<unexpected response from server: "!3ZL !AD">/FAO=3
PROTOERR    	<NNTP protocol violation error>
NONETADR    	<no network address found>
SENDERR	    	<error sending to NNTP server>
RCVERR	    	<error receiving from NNTP server>
BUFALLOCERR 	<error allocating buffer>
PAGERR	    	<error in display paging routines>
COMPOSERR   	<error occurred during article composition>
KWDSYNTAX   	<unknown keyword "!AD">/FAO=2
CMDERR	    	<error parsing NEWS command>
CAPTIVE	    	<operation not allowed from captive account>
ATTERR	    	<error attaching to process>
PRINTERR    	<error creating print job>
NOPOSTING   	<posting is not allowed by server>
POSTERR	    	<error occured while posting message>
MAILERR	    	<error occurred while sending mail>
REPLYERR    	<error processing reply>
SPAWNERR    	<error creating subprocess>
NOPRINT	    	<no print job in progress>
NOTYOURS    	<your address (!AD) does not match author (!AD)>/FAO=4
BODYERR	    	<error opening article body file !AD>/FAO=2
NOREPLYADDR 	<missing or invalid destination address for reply>
RCRDERR	    	<error reading newsrc file !AD>/FAO=2
RCWRTERR    	<error writing newsrc file !AD>/FAO=2
LISTREPLYERR	<error parsing reply to NNTP LIST command>
WRITERR	    	<error opening file !AD for writing>/FAO=2

.SEVERITY   	WARNING
NONEW	    	<no new articles found>
NOMORE	    	<no more articles in this group or thread>
NOMORENEW   	<no more new articles>
NOCURART    	<no article currently active>
NOSUCHGRP   	<group name !AD not found>/FAO=2
NOTSUBSCR   	<you are not subscribed to group !AD>/FAO=2
NOCURGROUP  	<there is no current group set>
NOGRMATCH   	<no group names matched pattern "!AD">/FAO=2
EDNOTFOUND  	<callable editor !AD not found>/FAO=2
HDRNOTFOUND 	<header !AD not currently set for viewing>/FAO=2
INVARTRNG   	<invalid article range specification "!AD">/FAO=2
OUTRNG	    	<article !UL out of range; articles !UL to !UL are available>/FAO=3
CREATERR    	<error creating message file !AD>/FAO=2
EDITERR	    	<could not find edit routine !AD in shareable image !AD>/FAO=4
SUBPERR	    	<error status returned from subprocess>
OPENERR	    	<error creating file !AD>/FAO=2
EXTRERR	    	<error extracting message !UL>/FAO=1
SIGFERR	    	<error processing signature file !AD>/FAO=2
UPDERR	    	<error updating information for group !AD>/FAO=2
ARTRERR	    	<error retrieving article !AD:!UL>/FAO=3
NOARTSPEC   	<please specify article number, /FIRST, or /LAST>
NOTHREAD    	<no thread established; you must read an article first>
NOSUBJECT   	<article has no subject; no thread to follow>
NOMATCH	    	<no strings matched "!AD">/FAO=2
KEYDEFERR   	<error in key definition>
SETARCHERR  	<error setting archive directory>
FWDERR    	<error occurred while forwarding mail>
ADDRSYNTX   	<syntax error in address list>
NOPARENT    	<there is no parent process to attach to>
NOMOREMATCH 	<no more matching articles found>
NOSUBJSPEC   	<no subject pattern(s) specified>
NOCANCEL    	<will not cancel article !UL>/FAO=1
NOIGHDSPEC  	<no ignore header type was specified>/FAO=0
NOSUCHARTICLE	<no such article exists on server>
NOMOREGRP 	<no more groups to visit>/FAO=0

.SEVERITY   	INFORMATION
NEWGROUP    	<found new group !AD>/FAO=2
EOARTICLE   	<end of article>
EOLIST	    	<end of list>
NOTIMPL	    	<command not yet implemented>
NOOPNPROF   	<could not open NEWS profile>
ALRDYSUB    	<you are already subscribed to group !AD>/FAO=2
HDRALRDY    	<header !AD already set for viewing>/FAO=2
EOGROUP	    	<end of new messages in group !AD>/FAO=2
MUSTMAIL    	<will mail reply; followups are directed to article poster>
CONNECTING  	<connecting to NNTP server !AD>/FAO=2
RECONNECTING  	<reconnecting to NNTP server !AD>/FAO=2
UPDATING    	<locating unread articles; please wait...>
USINGCURGROUP  	<message will be posted to group !AD>/FAO=2
ALRDYIGNORE 	<already ignoring specified string>
MARKCOUNT   	<set !UL article!%S !ADseen>/FAO=3
NEWCOUNT    	<!UL unseen message!%S in newsgroup !AD>/FAO=3
CANCELLED   	<message cancelled>
SUBSCRIBED  	<added subscription to newsgroup !AD>/FAO=2
GRPSET	    	<current newsgroup now !AD>/FAO=2
GRPINFO	    	<articles [!UL,!UL], !UL unseen>/FAO=3
EXTRACTED   	<extracted !UL article!%S from newsgroup !AD>/FAO=3

.SEVERITY   	SUCCESS
ALLDONE	    	<leaving NEWS utility>
SPAWNED	    	<process !AD (pid !XL) spawned>/FAO=3
DIDSUB	    	<successfully added subscription to newsgroup !AD>/FAO=2
DIDUNSUB    	<successfully removed subscription to newsgroup !AD>/FAO=2

.END
