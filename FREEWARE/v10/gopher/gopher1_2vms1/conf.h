/*
 *******************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 91/03/25 21:11:32 $
 * $Source: /export/mermaid/staff/lindner/gopherd/RCS/conf.h,v $
 *******************************************************************
 */

/*
**  You'll find all the user configurable parameters here
*/


/*
** This specifies the default port to run at.  Port 70 is the official 
** port number. You can run it on different ports if you want though.  
** This can be overridden on the command line. 
*/

#define GOPHER_PORT	70

/*
** If your hostname command returns the Fully Qualified Domain Name
** (i.e. it looks like foo.bar.edu and not just foo) then make
** the domain name a null string.  Otherwise put in the rest of
** your domain name that `hostname` doesn't return.
*/

#ifndef DOMAIN_NAME
#define DOMAIN_NAME ""
#endif

/*
** This is the default place where all the data is going to reside.
*/

#ifndef DATA_DIRECTORY
#define DATA_DIRECTORY "GOPHER_DATADIR"
#endif

#ifdef VMS
/*
** These are the default allocations to be used for the log file depending
** upon log file rollover selections; edit these if logfiles are fragmenting
** too much.
*/
#define	ALQ_NEVER	1000
#define	ALQ_ANNUAL	1000
#define ALQ_MONTH	500
#define	ALQ_WEEK	250
#define	ALQ_DAY		200
#define	ALQ_HOUR	3
/*
** This is the name of a system table logical that can be used to stop
** and/or restart the server.
*/

#ifndef RESTART
#define RESTART "GOPHER_RESTART"
#endif

/*
** This is the name of a system table logical that can be used to specify
** a command file containing foreign command and logical definitions, to
** be executed by spawned subprocess when the gopher server is run from
** Inetd/MULTINET_SERVER.  Such processes are created with LOGINOUT.EXE
** as the image, but they do not execute SYS$MANAGER:SYLOGIN.COM.
*/

#ifndef LOGINCOM
#define LOGINCOM "GOPHER_LOGIN"
#endif
#endif /* VMS */

/*
** This is the default file where the configuration options are found.
** 
** It can be overridden on the command line.
*/

#ifndef CONF_FILE
#ifdef VMS
#define CONF_FILE	"GOPHER_CONFIG"
#else
#define CONF_FILE	"/usr/local/etc/gopherd.conf"
#endif
#endif
/*
** Uncomment this out if you are running a system that doesn't have
** dirent directory routines.  Note that Next (and Mach) have the
** dirent routines, but they don't seem to work.
*/

#define BROKENDIRS

#if defined(NeXT) || defined(n16)
#define BROKENDIRS
#endif


/*
**
** This #define is what will be transmitted by the server when it wants to
** refuse service to a client
**
*/

#define BUMMERSTR "We cannot allow off campus connections to this server. Sorry"


/*
** Uncomment this out if you're compiling on a NeXT machine.
**
** Would be nice if I could figure out why the varargs stuff dies without
** this!
*/

#ifdef NeXT
#define __STRICT_BSD__ /**/
#endif

/*
** Uncomment this out if you want to use allow the gopher data server to
** also act as a gopher index server.
*/

#define BUILTIN_SEARCH /**/

/*
** Uncomment *one* of the first two search engines for Unix.  
**    (Neither has been ported to VMS as of yet).
*/

/*#define NEXTSEARCH /**/
/*#define WAISSEARCH /**/
#define SHELLSEARCH /**/
#define GREPSEARCH /**/
#define CMD1SEARCH /**/

/*
** Uncomment this out if you want to use load restricting.
*/

#define LOADRESTRICT /**/

/*
 * This is the default time to cache directory entries on the server.
 */

#define CACHE_TIME 180  /** Default cache time of three minutes. **/


/*
 * The load average at which to restrict connections
 */

#ifdef VMS
#define MAXLOAD 5.0
#else
#define MAXLOAD 10.0
#endif

/*
 * Return type for signal()
 */

#define SIGRETTYPE void

/*
 * Timeout for network reads (5 minutes)
 */

#define READTIMEOUT (5 * 60)


