	Name:	DECThreads NNTP Server for OpenVMS
	Author: Ruslan R. Laishev <Laishev@SMTP.DeltaTel.RU>	
		http://www.levitte.org/~rlaishev/

	Abstract: DECThreads NNTP server is a news server 
		as described in RFC 977, + XOVER support

	Prerequisite: 	OpenVMS 6.1 or later,
			DECC 5.6 or later,
			any TCP/IP support package (UCX,MULTINET,TCPWare),
			MadGoat Make utility (MMK),
			MADGoat NetLib 2.2 or later

	Build:		$UnZip DNNTP.ZIP
			$MMK
			
			Edit NNTP.CONF,NNTP_START
			Copy *.exe,.conf,*.com to home directory of NNTP
			Copy *.hlb to sys$help directory
			Start NNTP Server by NNTP_START and wait while server download
			..newsgroups list from your ISP

			$nc :== $dev:[dir]nntpcp.exe
			$nc

			Use 'nc' for select and set cashed groups

	Revisions history:
....

06-OCT-1997
	- Some changes....
09-OCT-1997
	- Posting in moderated groups (Fido7.*,gnu.*)
10-NOV-1997
	- Expiration, posting in moderated groups.

18-DEC-1997
v0.91	- Ported to NETLIB 2.2 (Thanks Matt Madison for adding NETLIB_M_FLUSH)
	- Warning ! Changed form of stored messages, for exclude problem kill
	..nntp$msg.db (not nntp$grp.db, etc).
19-DEC-1997
	- Fixed problem with expiration processing
	- Fixed problem with NEXT command
	- Fixed problem with BODY
21-DEC-1997
	- Fixed problem with posting to moderated groups
22-DEC-1997
	- Fixed problem with nntp_read_mline
05-JAN-1998
	- Fixed problem in nntp_read_mline: zero length buffer 
	..in call netlib_read caused for infinite looping
	- Fixed problem in nntp_read_mline: "buffer overflow" status changed to 
	.."insufficient memory"
05-DEC-1998
	- Fixed incorrectly incremented gkeyp->First in nntp_expire.c. 
09-DEC-1998
	- Fixed garbage output during execution NEWGROUPS command
12-DEC-1998
	- Change TimeOut parameter for each type of thread
	- Change assuming First/Last counter of a article number
	..in empty group
15-DEC-1998
	- Add check access by IP address/IP name
19-DEC-1998
	- Change expiration procedure behavior: resting in group alway 1 article,
	..even group expiried and switching in passive
2-FEB-1998
	- Change calling expiration procedure, it's call now from main loop;
	..now expiraion procedure is started at MidNight (23:59:59)
	- ExpireInterval parameter is removed from nntp.conf
8-FEB-1998
	- Fixed problem with sys$schdwk (:) in nntp_main.c
5-MAR-1998:0.94
	- Several changes in NNTP.h (Pthreads.h/Pthread_d4.h)
	..added $PTHREAD_CREATE macros
17-APR-1998:0.95
	- Fixed bug in nntp_read_mline: add check for terminator 
	.. after first netlib_read
	- Add default specifications in nntp_db.c
8-MAY-1998
	- NNTP_DB.C: restore record size checking (see MRS != 0)

18-Jun-1998
	- Some change in the NNTP_SUCK.C and NNTPCP.C modules
	- betta 0.96

28-Jun-1998
	- Some change in the NNTP_SUCK.C and NNTPCP.C modules
	- Some changes in the NNTP.H
	- Add update of a newsgroups list at first start of the server
	- Some changes for improving of performance
	- prerelease 0.97
                 

28-Aug-1998
	- 1.0 A lot of changes

	--TO DO:
		.	User authentification by OpenVMS
		.	WEB Interface with search facilities

