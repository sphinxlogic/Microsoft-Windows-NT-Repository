/* hfile.inc - placed into all .h files to set up for PVCS 
   $Header:   E:/pcdirs/vcs/netevent.h_v   1.0   15 Jan 1990 19:29:26   bkc  $
   Revision History ----------------------------------------------------
   $Log:   E:/pcdirs/vcs/netevent.h_v  $
 * 
 *    Rev 1.0   15 Jan 1990 19:29:26   bkc
*/


/*
*  Events for event processing in NCSA Telnet.
*  Used for netgetevent().
*/


#define USERCLASS	1	/* the user program will accept these events */
#define ICMPCLASS	2	/* ICMP in netsleep will look for these */
#define ERRCLASS    4	/* the user may or may not read these error messages */
#define SCLASS		8	/* the background server will take these */
#define CONCLASS    0x10	/* the application manages connections with these */

#define ERR1	1		/* an error message is waiting, ERRCLASS */

#define IREDIR	1		/* ICMP redirect, ICMPCLASS */

#define CONOPEN 1		/* connection has opened, CONCLASS */
#define CONDATA 2       /* there is data available on this connection */
#define CONCLOSE 3		/* the other side has closed its side of the connection */
#define CONFAIL 4		/* connection open attempt has failed */

#define UDPDATA 1		/* UDP data has arrived on listening port, USERCLASS */
#define DOMOK	2		/* domain name ready */
#define DOMFAIL 3		/* domain name lookup failed */
#define FTPCOPEN 20     /* FTP command connection has opened */
#define FTPCLOSE 21     /* FTP command connection has closed */
#define FTPBEGIN 22     /* FTP transfer beginning, dat =1 for get, 0 for put */
#define FTPEND   23     /* FTP transfer ending */
#define FTPLIST  24     /* FTP file listing taking place */
#define FTPUSER  25     /* FTP user name has been entered */
#define FTPPWOK	26		/* FTP password verified */
#define FTPPWNO 27		/* FTP password failed */
#define RCPBEGIN 30		/* RCP beginning */
#define RCPEND 31		/* RCP ending */

#define UDPTO 1			/* UDP request from DOMAIN timed out, SCLASS */
#define FTPACT 2		/* FTP transfer is active, keep sending */
#define TCPTO  3		/* TCP for DOMAIN timed out */
#define RCPACT 4		/* rcp is active, needs CPU time */
#define RETRYCON 5		/* retry connection packet, might be lost */
#define DOMNEXT 6               /* search next domain list entry */
#define E_CLOCK 7
#ifdef	SCRIPT
#define	SCRIPT_EVENT	1	/* script next step */
#define	SCRIPT_DATA	2	/* received some data */
#define	SCRIPT_PROC	3	/* just process stuff */
#define	SCRIPT_CLOSE	4	/* connection was closed */
#define	SCRIPT_FORCE	5	/* user forced connection closed */
#define	SCRIPT_DOMAIN	6	/* domain name lookup ok */
#define	SCRIPT_FUNC	8
/* int Script_Event(int type, void *twin, unsigned int data); */
#endif
