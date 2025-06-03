/********************************************************************
 * 1994-04-09	D.Sherman <dennis_sherman@unc.edu>
 *		Fixed bug revealed by previous fix to setting Rollover.
 *		 Again, Thanks Stan!
 * 1994-04-05	D.Sherman <dennis_sherman@unc.edu>
 *		Fix from Stan Peters <stan_peters@byu.edu> for
 *		 setting Rollover.
 *
/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:13:27 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/gopherdconf.c,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: gopherdconf.c
 * Routines to parse the gopherd.conf file.
 *********************************************************************
 * Revision History:
 * $Log: gopherdconf.c,v $
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#include "gopherd.h"
#include "Malloc.h"
#include "String.h"
#include <stdio.h>

extern boolean DEBUG;

/*********************************************/

GDCobj *
GDCnew()
{
     GDCobj *gdc;

     gdc = (GDCobj *) malloc(sizeof(GDCobj));

     gdc->Extensions = ExtArrayNew();

     gdc->Sites      = SiteArrayNew();
     gdc->Securityon   = FALSE;
     gdc->RunFromInetd = FALSE;
     gdc->SortDir      = TRUE;
     gdc->FTPPort      = -1;
     gdc->EXECPort     = -1;
     gdc->SRCHPort     = -1;
#ifdef BUILTIN_SEARCH
#ifdef NEXTSEARCH
     gdc->SortNeXT     = TRUE;
#endif
#ifdef WAISSEARCH
     gdc->SortWAIS     = TRUE;
#endif
#ifdef SHELLSEARCH
     gdc->SortShell    = TRUE;
#endif
#ifdef GREPSEARCH
     gdc->SortGREP     = TRUE;
#endif
#ifdef CMD1SEARCH
     gdc->SortCMD1     = TRUE;
#endif
#endif
     gdc->Caching      = TRUE;
     gdc->Logfile      = STRnew();	STRset(gdc->Logfile, "");
     gdc->Data_Dir     = STRnew();	STRset(gdc->Data_Dir, DATA_DIRECTORY);
     gdc->Hostname     = STRnew();	STRset(gdc->Hostname, "");
     gdc->Port         = GOPHER_PORT;
     gdc->chroot       = FALSE;
     gdc->Defaccess    = ACC_FULL;
     gdc->BummerMsg    = STRnew();	STRset(gdc->BummerMsg, "");
     gdc->Admin        = STRnew();	STRset(gdc->Admin, "");
     gdc->AdminEmail   = STRnew();	STRset(gdc->AdminEmail, "");
     gdc->Site         = STRnew();	STRset(gdc->Site, "");
     gdc->Org          = STRnew();	STRset(gdc->Org, "");
     gdc->Geog         = STRnew();	STRset(gdc->Geog, "");
     gdc->Loc          = STRnew();	STRset(gdc->Loc, "");
     gdc->TZ           = -1;
     gdc->ReadTimeout  = READTIMEOUT;	/* From CONF.H */
     gdc->TimeZoneText = STRnew();	STRset(gdc->TimeZoneText,"");
     gdc->Hidden_Prefix= STRnew();	STRset(gdc->Hidden_Prefix, "_");
     gdc->Link_Prefix  = STRnew();	STRset(gdc->Link_Prefix, ".");
     gdc->LookAside    = STRnew();	STRset(gdc->LookAside, ".cap");
     gdc->DName	       = STRnew();	STRset(gdc->DName, "");
     gdc->DHead	       = STRnew();	STRset(gdc->DHead, "");
     gdc->DFoot	       = STRnew();	STRset(gdc->DFoot, "");
#ifdef VMS
     gdc->Scratch_Dir  = STRnew();	STRset(gdc->Scratch_Dir,"sys$scratch:");
     gdc->Spawn_Init   = STRnew();	STRset(gdc->Spawn_Init, LOGINCOM);
     gdc->RestartLnm   = STRnew();	STRset(gdc->RestartLnm, RESTART);
     gdc->rollover     = ROLLOVER_NEVER;
#endif
     gdc->MaxLoad      = 0.0;

     return(gdc);
}

void
GDCdestroy(gdc)
  GDCobj *gdc;
{
     ExtArrDestroy(gdc->Extensions);
     SiteArrDestroy(gdc->Sites);

}


boolean
GDCtokens(gdc, token, rest)
  GDCobj *gdc;
  char *token;
  char *rest;
{
     boolean success = TRUE;
     char *cp;

     if (DEBUG)
	  printf("Parsed token '%s', rest of line '%s'\n", token, rest);
     
     if (strcasecmp(token, "ACCESS") == 0) {
	  int moo;
	  success = SiteProcessLine(gdc->Sites, rest, gdc->Defaccess);
	  moo = SiteAccess(gdc->Sites, "default");
	  if (moo != ACC_UNKNOWN)
	       gdc->Defaccess = moo;

	  gdc->Securityon = TRUE;
	  return(success);
     }

#ifdef VMS
     if ((cp = strchr(rest, '!')) != NULL) {
	    *cp = '\0';		/*  Allow VMS-style commentary as well	*/
	    for(--cp;isspace(*cp)||!isprint(*cp);cp--)    /*  Strip trailing non-text	*/
		*cp = '\0';
     }
#endif
     if (strcasecmp(token, "READTIMEOUT")==0)
	    GDCsetReadTimeout(gdc, atoi(rest));
     else
     if (strcasecmp(token, "TIMEZONE")==0) {
	  if ( (strncmp(rest, "-", 1)==0) && isdigit(*(rest+1)) )
	    GDCsetTZ(gdc, -atoi(rest+1));
	  else if ( (strncmp(rest, "+", 1)==0) && isdigit(*(rest+1)) )
		GDCsetTZ(gdc, atoi(rest+1));
	  else if (isdigit(*rest))
		GDCsetTZ(gdc, atoi(rest));
	    else {  /* Mnemonic Timezone */
		GDCsetTZ(gdc,-1);
		GDCsetTimeZone(gdc,rest);		    
		return(success);
	    }
	  if (abs(GDCgetTZ(gdc))<100)
	    GDCsetTZ(gdc,GDCgetTZ(gdc)*100);
     }
     else if (strcasecmp(token, "DATADIRECTORY")==0)
	  GDCsetDataDir(gdc, rest);
     else if (strncasecmp(token, "HIDDEN",6)==0)
	  GDCsetHiddenPrefix(gdc, rest);
     else if (strncasecmp(token, "LINK",4)==0)
	  GDCsetLinkPrefix(gdc, rest);
     else if (strcasecmp(token, "LookAside")==0)
	  GDCsetLookAside(gdc, rest);
     else if (strcasecmp(token, "DName")==0)
	  GDCsetDName(gdc, rest);
     else if (strcasecmp(token, "DHead")==0)
	  GDCsetDHead(gdc, rest);
     else if (strcasecmp(token, "DFoot")==0)
	  GDCsetDFoot(gdc, rest);
#ifdef VMS
     else if (strcasecmp(token, "scratchdir")==0)
	  GDCsetScratchDir(gdc, rest);
     else if (strcasecmp(token, "spawninit")==0)
          GDCsetSpawnInit(gdc, rest);
     else if (strcasecmp(token, "Restart")==0)
	  GDCsetRestart(gdc, rest);
     else if (strcasecmp(token, "Rollover")==0) {
	    if (strncasecmp(rest, "DAILY", 5)==0)
		GDCsetRollover(gdc,ROLLOVER_DAILY);
	    else if (strncasecmp(rest, "MONTHLY", 7)==0)
		GDCsetRollover(gdc,ROLLOVER_MONTHLY);
	    else if (strncasecmp(rest, "HOURLY", 6)==0)
		GDCsetRollover(gdc,ROLLOVER_HOURLY);
	    else if (strncasecmp(rest, "ANNUALLY", 8)==0)
		GDCsetRollover(gdc,ROLLOVER_ANNUALLY);
	    else if (strncasecmp(rest, "WEEKLY", 6)==0)
		GDCsetRollover(gdc,ROLLOVER_WEEKLY);
	    else
		GDCsetRollover(gdc, ROLLOVER_NEVER);
	  }
#endif
     else if (strcasecmp(token, "PORT")==0)
	  GDCsetPort(gdc, atoi(rest));
     else if (strcasecmp(token, "CACHE")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetCaching(gdc,TRUE);
	    else
		GDCsetCaching(gdc, FALSE);
	  }
     else if (strcasecmp(token, "INETD")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetInetdActive(gdc,TRUE);
	    else
		GDCsetInetdActive(gdc, FALSE);
	  }
     else if (strcasecmp(token, "SortDir")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetSortDir(gdc,TRUE);
	    else
		GDCsetSortDir(gdc, FALSE);
	  }
     else if (strcasecmp(token, "FTPPort")==0) {
	    if (strncasecmp(rest, "NONE", 4)==0)
		GDCsetFTPPort(gdc,0);
	    else
		GDCsetFTPPort(gdc, atoi(rest));
	  }
     else if (strcasecmp(token, "EXECPort")==0) {
	    if (strncasecmp(rest, "NONE", 4)==0)
		GDCsetEXECPort(gdc,0);
	    else
		GDCsetEXECPort(gdc, atoi(rest));
	  }
     else if (strcasecmp(token, "SRCHPort")==0) {
	    if (strncasecmp(rest, "NONE", 4)==0)
		GDCsetSRCHPort(gdc,0);
	    else
		GDCsetSRCHPort(gdc, atoi(rest));
	  }
#ifdef BUILTIN_SEARCH
#ifdef NEXTSEARCH
     else if (strcasecmp(token, "SortNeXT")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetSortNeXT(gdc,TRUE);
	    else
		GDCsetSortNeXT(gdc, FALSE);
	  }
#endif
#ifdef WAISSEARCH
     else if (strcasecmp(token, "SortWAIS")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetSortWAIS(gdc,TRUE);
	    else
		GDCsetSortWAIS(gdc, FALSE);
	  }
#endif
#ifdef SHELLSEARCH
     else if (strcasecmp(token, "SortShell")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetSortShell(gdc,TRUE);
	    else
		GDCsetSortShell(gdc, FALSE);
	  }
#endif
#ifdef GREPSEARCH
     else if (strcasecmp(token, "SortGREP")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetSortGREP(gdc,TRUE);
	    else
		GDCsetSortGREP(gdc, FALSE);
	  }
#endif
#ifdef CMD1SEARCH
     else if (strcasecmp(token, "SortCMD1")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetSortCMD1(gdc,TRUE);
	    else
		GDCsetSortCMD1(gdc, FALSE);
	  }
#endif
#endif
     else if (strcasecmp(token, "DO_CHROOT")==0) {
	    if (strncasecmp(rest, "TRUE", 4)==0)
		GDCsetchroot(gdc,TRUE);
	    else
		GDCsetchroot(gdc, FALSE);
	  }
     else if (strcasecmp(token, "ADMIN")==0)
	  GDCsetAdmin(gdc, rest);
     else if (strcasecmp(token, "ADMINEMAIL")==0)
	  GDCsetAdminEmail(gdc, rest);
     else if (strcasecmp(token, "HOSTALIAS")==0)
	  GDCsetHostname(gdc, rest);
     else if (strcasecmp(token, "SITE")==0)
	  GDCsetSite(gdc, rest);
     else if (strcasecmp(token, "ORG")==0)
	  GDCsetOrg(gdc, rest);
     else if (strcasecmp(token, "LOC")==0)
	  GDCsetLoc(gdc, rest);
     else if (strcasecmp(token, "GEOG")==0)
	  GDCsetGeog(gdc, rest);
     else if (strcasecmp(token, "LOGFILE")==0)
	  GDCsetLogfile(gdc, rest);
     else if (strcasecmp(token, "BUMMERMSG")==0)
	  GDCsetBummerMsg(gdc, rest);
     else if (strcasecmp(token, "EXT")==0) {
	  success = ExtProcessLine(gdc->Extensions, rest);
     }
     else if (strcasecmp(token, "IGNORE")==0) {
	  ExtAdd(gdc->Extensions, A_ERROR, "","",rest);
     }
     else if (strcasecmp(token, "MaxLoad")==0)
	  GDCsetMaxLoad(gdc, atof(rest));
     else
	  success = FALSE;
     
     return(success);
}

#ifdef VMS
extern int vaxc$errno_stv;
#endif

void
GDCfromFile(gdc, filename)
  GDCobj *gdc;
  char *filename;
{
     FILE *gdcfile;
     char inputline[256];
     char *cp, *token, *restofline;
     boolean success;


     if ((gdcfile = fopen(filename, "r")) == (FILE *) NULL && !RunFromInetd) {
	  printf("Cannot open file '%s'\n", filename);
#ifdef VMS
	  printf("\t%s%s%s\n", strerror(EVMSERR, vaxc$errno),
			vaxc$errno_stv?"\n\t\t":"",
			vaxc$errno_stv?strerror(EVMSERR, vaxc$errno_stv):"");
#endif
	  exit(-1);
     }

     while (fgets(inputline, sizeof inputline, gdcfile)!= NULL) {
	  ZapCRLF(inputline);
	  
	  if (*inputline == '#' || *inputline == '\0') /** Ignore comments **/
	       continue;

	  cp = strchr(inputline, ':');
	  if (cp == NULL && !RunFromInetd) {
	       printf("Ignored Bad line '%s'\n", inputline);
	       continue;
	  }
	  *cp = '\0';
	  token      = inputline;
	  restofline = cp+1;
	  while (*restofline == ' ' || *restofline == '\t')
	       restofline++;
	  
	  success = GDCtokens(gdc, token, restofline);
	  if (!success && !RunFromInetd) {
	       printf("Ignored Bad line '%s'\n", inputline);
	       continue;
	  }
     }
     
     fclose(gdcfile);
}


boolean GDCCanSearch(gdc, hostname, ipnum)
  GDCobj *gdc;
  char *hostname, *ipnum;
{
     boolean test;

     if (DEBUG) printf("Testing %s/%s for access\n", hostname, ipnum);

     if (gdc->Securityon == FALSE)
	  return(TRUE);

     if ((test = SiteArrCanSearch(gdc->Sites, hostname, ipnum)) != ACC_UNKNOWN)
	  return(test);
     
     if ((gdc->Defaccess & ACC_SEARCH) == ACC_SEARCH)
	  return(TRUE);
     else
	  return(FALSE);

}

boolean GDCCanRead(gdc, hostname, ipnum)
  GDCobj *gdc;
  char *hostname, *ipnum;
{
     boolean test;

     if (DEBUG) printf("Testing %s/%s for access\n", hostname, ipnum);

     if (gdc->Securityon == FALSE)
	  return(TRUE);

     if ((test = SiteArrCanRead(gdc->Sites, hostname, ipnum)) != ACC_UNKNOWN)
	  return(test);
     
     if ((gdc->Defaccess & ACC_READ) == ACC_READ)
	  return(TRUE);
     else
	  return(FALSE);

}


boolean GDCCanBrowse(gdc, hostname, ipnum)
  GDCobj *gdc;
  char *hostname, *ipnum;
{
     boolean test;

     if (DEBUG) printf("Testing %s/%s for access\n", hostname, ipnum);

     if (gdc->Securityon == FALSE)
	  return(TRUE);

     if ((test = SiteArrCanBrowse(gdc->Sites, hostname, ipnum)) != ACC_UNKNOWN)
	  return(test);
     
     if ((gdc->Defaccess & ACC_BROWSE) == ACC_BROWSE)
	  return(TRUE);
     else
	  return(FALSE);

}


/** Is this file ignored? **/
boolean
GDCignore(gdc, filenm)
  GDCobj *gdc;
  char *filenm;
{
     char *gplustype, *prefix, objtype;

     ExtGet(gdc->Extensions, filenm, &objtype, &gplustype, &prefix);

     if (objtype == A_ERROR)
	  return(TRUE);
     else
	  return(FALSE);
}

boolean 
GDCExtension(gdc, ext, Gtype, Prefix)
  GDCobj *gdc;
  char *ext;
  char *Gtype;
  char **Prefix;
{
     char *gplustype;

     ExtGet(gdc->Extensions, ext, Gtype, &gplustype, Prefix);

     if (*Gtype != '\0')
	  return(TRUE);
     else
	  return(FALSE);
}
#ifdef VMS
char log_alq[10];
char log_deq[10];

char
*GDCgetLogfile(gdc)
    GDCobj *gdc;
{
    static
	char    rollover[256];
    char    insert[11];
    time_t  Now;
    char    *mx;
    int	    i;
    char    cdate[26];
    static
	char *DaysofWeek="SunMonTueWedThuFriSat";
    static
	char *Months = "JanFebMarAprMayJunJulAugSepOctNovDec";
#define	DOW	(cdate+0)
#define MMM     (cdate+4)
#define DD      (cdate+8)
#define HH	(cdate+11)
#define YYYY    (cdate+20)

    if (*(STRget(gdc->Logfile))=='\0')
	return(NULL);
    time(&Now);
    strcpy(cdate,(char *)ctime(&Now));
    cdate[3] = cdate[7]= cdate[10]= cdate[13] = cdate[24]= '\0';
    if (GDCgetRollover(gdc)==ROLLOVER_WEEKLY) {
	mx=strstr(DaysofWeek,DOW);
	if (i = ((mx - DaysofWeek)/3)) {
	    Now -= (i * (24 * 60 * 60));
	    strcpy(cdate,(char *)ctime(&Now));
	    cdate[3] = cdate[7]= cdate[10]= cdate[13] = cdate[24]= '\0';
	}
    }
    if (cdate[8]==' ')
	cdate[8] = '0';
    mx=strstr(Months,MMM);
    i = mx - Months;
    sprintf(insert,"%s%02d%s%s",YYYY,(i/3)+1,DD,HH);
    switch(GDCgetRollover(gdc))
    {
    case ROLLOVER_ANNUALLY:	insert[4] = '\0';   i = ALQ_ANNUAL; break;
    case ROLLOVER_MONTHLY:	insert[6] = '\0';   i = ALQ_MONTH;  break;
    case ROLLOVER_WEEKLY:	insert[8] = '\0';   i = ALQ_WEEK;   break;
    case ROLLOVER_DAILY:	insert[8] = '\0';   i = ALQ_DAY;    break;
    case ROLLOVER_HOURLY:	insert[10]= '\0';   i = ALQ_HOUR;   break;
    case ROLLOVER_NEVER:    
    default:			insert[0] = '\0';   i = ALQ_NEVER;
    }
    sprintf(log_alq,"alq=%d",i);
    sprintf(log_deq,"deq=%d",i);
    strcpy(rollover,STRget(gdc->Logfile));
    strcat(rollover,insert);
    return(rollover);
}
#endif
