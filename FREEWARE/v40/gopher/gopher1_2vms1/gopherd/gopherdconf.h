/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:13:27 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/gopherdconf.h,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: gopherdconf.h
 * Header file for routines in gopherdconf.c
 *********************************************************************
 * Revision History:
 * $Log: gopherdconf.h,v $
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/



#ifndef GOPHERDCONF_H
#define GOPHERDCONF_H

#include "boolean.h"
#include "STRstring.h"
#include "ext.h"       /** Filename extensions **/
#include "site.h"      /** Hostname/IP security **/


/**************************************************************************
 * A structure that defines all the changable parameters of a gopher server
 * Read from the file gopherd.conf
 */

struct gdconf_struct {
     ExtArray     *Extensions;
     SiteArray    *Sites;
     Accesslevel  Defaccess;       /*** Default access level for unknowns   **/

     boolean      RunFromInetd;    /*** -I option			    **/
     boolean      Caching;         /*** -C option			    **/
     String       *Logfile;        /*** The filename for the logfile	    **/
     String       *BummerMsg;      /*** Message given to non-secure sites   **/
     String       *Data_Dir;       /*** Where our data directory is	    **/
     String       *Hostname;       /*** A FQDN for the host		    **/
     int          Port;            /*** The Port we're running at	    **/
     boolean      chroot;          /*** Are we chroot()ing?		    **/

     boolean      Securityon;      /*** Are we restricting access or not?   **/
     boolean	  SortDir;	   /*** Do we sort directories by default?  **/
     int	  FTPPort;	   /*** Where do we accept aFTP gateways?   **/
     int	  EXECPort;	   /*** Where do we accept EXEC scripts?    **/
     int	  SRCHPort;	   /*** Where do we accept searches?	    **/
#ifdef BUILTIN_SEARCH
#ifdef NEXTSEARCH
     boolean	  SortNeXT;	   /*** Do we sort NeXT searches by default?**/
#endif
#ifdef WAISSEARCH
     boolean	  SortWAIS;	   /*** Do we sort WAIS searches by default?**/
#endif
#ifdef SHELLSEARCH
     boolean	  SortShell;       /*** Do we sort Shell searches by dflt?  **/
#endif
#ifdef GREPSEARCH
     boolean	  SortGREP;	   /*** Do we sort GREP searches by default?**/
#endif
#ifdef CMD1SEARCH
     boolean	  SortCMD1;	   /*** Do we sort CMD1 searches by default?**/
#endif
#endif
     String       *Admin;          /*** Administrator info		    **/
     String       *AdminEmail;     /*** E-mail address of administrator	    **/
     
     String       *Site;           /*** Description of site		    **/
     String       *Org;            /*** Name of Organization		    **/
     String       *Loc;            /*** Location of site		    **/
     String       *Geog;           /*** Latitude and Longitude		    **/
     int          TZ;              /*** Timezone			    **/
     int	  ReadTimeout;	   /*** Network READTIMEOUT value	    **/
     String	  *TimeZoneText;    /*** Timezone in Text Format	    **/
     String	  *Hidden_Prefix;   /*** Files starting w/ this string are 
						ignored			    **/
     String	  *Link_Prefix;	   /*** Files starting w/ this string are 
						.LINKS files		    **/
     String	  *LookAside;	   /*** Name for the lookaside directories  **/
     String	  *DName;	   /*** Default name for files w/o lookaside**/
     double	  MaxLoad;	   /*** Maximum Load Average		    **/
     String	  *DHead;	   /*** Default printfile() header	    **/
     String	  *DFoot;	   /*** Default printfile() footer	    **/
#ifdef VMS
     String       *Scratch_Dir;    /*** Where our scratch directory is	    **/
     String       *Spawn_Init;     /*** Command file for spawned processes  **/
     String	  *RestartLnm;	   /*** Logical name to watch for RESTART   **/
     int	  rollover;	   /*** Rollover period for log file	    **/
#define ROLLOVER_NEVER	    0
#define ROLLOVER_ANNUALLY   1
#define ROLLOVER_MONTHLY    2
#define ROLLOVER_DAILY	    3
#define ROLLOVER_HOURLY	    4
#define ROLLOVER_WEEKLY	    5
#endif
};

typedef struct gdconf_struct GDCobj;

#define GDCgetDefAccess(a)      ((a)->Defaccess)
#define GDCgetInetdActive(a)    ((a)->RunFromInetd)
#define GDCsetInetdActive(a,b)  ((a)->RunFromInetd=(b))
#define GDCgetReadTimeout(a)	((a)->ReadTimeout)
#define GDCsetReadTimeout(a,b)	((a)->ReadTimeout=(b))
#define GDCgetFTPPort(a)	((a)->FTPPort)
#define GDCsetFTPPort(a,b)	((a)->FTPPort=(b))
#define GDCgetEXECPort(a)	((a)->EXECPort)
#define GDCsetEXECPort(a,b)	((a)->EXECPort=(b))
#define GDCgetSRCHPort(a)	((a)->SRCHPort)
#define GDCsetSRCHPort(a,b)	((a)->SRCHPort=(b))
#define GDCgetSortDir(a)	((a)->SortDir)
#define GDCsetSortDir(a,b)	((a)->SortDir=(b))
#ifdef BUILTIN_SEARCH
#ifdef NEXTSEARCH
#define GDCgetSortNeXT(a)	((a)->SortNeXT)
#define GDCsetSortNeXT(a,b)	((a)->SortNeXT=(b))
#endif
#ifdef WAISSEARCH
#define GDCgetSortWAIS(a)	((a)->SortWAIS)
#define GDCsetSortWAIS(a,b)	((a)->SortWAIS=(b))
#endif
#ifdef SHELLSEARCH
#define GDCgetSortShell(a)	((a)->SortShell)
#define GDCsetSortShell(a,b)	((a)->SortShell=(b))
#endif
#ifdef GREPSEARCH
#define GDCgetSortGREP(a)	((a)->SortGREP)
#define GDCsetSortGREP(a,b)	((a)->SortGREP=(b))
#endif
#ifdef CMD1SEARCH
#define GDCgetSortCMD1(a)	((a)->SortCMD1)
#define GDCsetSortCMD1(a,b)	((a)->SortCMD1=(b))
#endif
#endif
#define GDCgetCaching(a)        ((a)->Caching)
#define GDCsetCaching(a,b)      ((a)->Caching=(b))

#define GDCgetDataDir(a)        STRget((a)->Data_Dir)
#define GDCsetDataDir(a,b)      STRset((a)->Data_Dir,(b))
#define GDCsetLogfile(a,b)      STRset((a)->Logfile,(b))
#define GDCgetHostname(a)       STRget((a)->Hostname)
#define GDCsetHostname(a,b)     STRset((a)->Hostname,(b))
#define GDCgetPort(a)           ((a)->Port)
#define GDCsetPort(a,b)         ((a)->Port=(b))
#define GDCgetchroot(a)         ((a)->chroot)
#define GDCsetchroot(a,b)       ((a)->chroot=(b))
#define GDCgetBummerMsg(a)      STRget((a)->BummerMsg)
#define GDCsetBummerMsg(a,b)    STRset((a)->BummerMsg,(b))

#define GDCgetAdmin(a)		STRget((a)->Admin)
#define GDCsetAdmin(a,b)	STRset((a)->Admin,(b))
#define GDCgetAdminEmail(a)	STRget((a)->AdminEmail)
#define GDCsetAdminEmail(a,b)	STRset((a)->AdminEmail,(b))
#define GDCgetSite(a)		STRget((a)->Site)
#define GDCsetSite(a,b)		STRset((a)->Site,(b))
#define GDCgetOrg(a)		STRget((a)->Org)
#define GDCsetOrg(a,b)		STRset((a)->Org,(b))
#define GDCgetLoc(a)		STRget((a)->Loc)
#define GDCsetLoc(a,b)		STRset((a)->Loc,(b))
#define GDCgetGeog(a)		STRget((a)->Geog)
#define GDCsetGeog(a,b)		STRset((a)->Geog,(b))
#define GDCgetTZ(a)		(a)->TZ
#define GDCsetTZ(a,b)		((a)->TZ=(b))
#define GDCgetTimeZone(a)	STRget((a)->TimeZoneText)
#define GDCsetTimeZone(a,b)	STRset((a)->TimeZoneText,(b))
#define GDCgetHiddenPrefix(a)   STRget((a)->Hidden_Prefix)
#define GDCsetHiddenPrefix(a,b) STRset((a)->Hidden_Prefix,(b))
#define GDCgetLinkPrefix(a)     STRget((a)->Link_Prefix)
#define GDCsetLinkPrefix(a,b)   STRset((a)->Link_Prefix,(b))
#define GDCgetLookAside(a)	STRget((a)->LookAside)
#define GDCsetLookAside(a,b)	STRset((a)->LookAside,(b))
#define GDCgetDName(a)		STRget((a)->DName)
#define GDCsetDName(a,b)	STRset((a)->DName,(b))
#define GDCgetDHead(a)		STRget((a)->DHead)
#define GDCsetDHead(a,b)	STRset((a)->DHead,(b))
#define GDCgetDFoot(a)		STRget((a)->DFoot)
#define GDCsetDFoot(a,b)	STRset((a)->DFoot,(b))
#define GDCgetMaxLoad(a)	(a)->MaxLoad
#define GDCsetMaxLoad(a,b)	((a)->MaxLoad=(b))
#ifdef VMS
#define GDCgetScratchDir(a)     STRget((a)->Scratch_Dir)
#define GDCsetScratchDir(a,b)   STRset((a)->Scratch_Dir,(b))
#define GDCgetSpawnInit(a)      STRget((a)->Spawn_Init)
#define GDCsetSpawnInit(a,b)    STRset((a)->Spawn_Init,(b))
#define GDCgetRestart(a)	STRget((a)->RestartLnm)
#define GDCsetRestart(a,b)	STRset((a)->RestartLnm,(b))
#define GDCgetRollover(a)	(a)->rollover
#define GDCsetRollover(a,b)	((a)->rollover=(b))
#else
#define GDCgetLogfile(a)	STRget((a)->Logfile)
#endif
/*
 * Real live functions in gopherdconf.c
 */
#ifdef VMS
char	     *GDCgetLogfile(/* gdc */);
#endif
GDCobj       *GDCnew();
void         GDCinit(/* gdc, FILE* gdc.conf file */);
void         GDCdestroy(/* gdc */);

boolean      GDCignore(/* gdc, char *filename */);

boolean      GDCCanRead(/* gdc, hostname, ipnum */);
boolean      GDCCanBrowse(/* gdc, hostname, ipnum */);
boolean      GDCCanSearch(/* gdc, hostname, ipnum */);

boolean      GDCExtension(/*gdc, ext, Gtype, Prefix*/);


#endif /* GOPHERDCONF_H */

