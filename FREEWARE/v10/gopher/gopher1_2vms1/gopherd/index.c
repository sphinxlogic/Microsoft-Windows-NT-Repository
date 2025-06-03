/********************************************************************
 * $Author: lindner $
 * $Revision: 1.2 $
 * $Date: 1992/12/14 21:36:05 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/index.c,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: index.c
 * Routines to deal with various types of indexes.
 *********************************************************************
 * Revision History:
 * $Log: index.c,v $
 * Revision 1.2  1992/12/14  21:36:05  lindner
 * Fixed problem in ShellIndexQuery, cp wasn't being incremented.
 * Also added special character elimination from GrepIndexQuery
 *
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#include "gopherd.h"
#include <stdio.h>
#include <unixlib.h>

#define WAISTYPE 1
#define NEXTTYPE 2
#define SHLLTYPE 3
#define GREPTYPE 4
#define CMD1TYPE 5

#ifdef VMS
#include <syidef.h>
#define STRerror(a) strerror(EVMSERR, vaxc$errno)
char *Validate_Filespec(char *);
void SearchVersion(int, GopherObj *, GopherDirObj *);
void CreateGO4link(GopherObj *, GopherDirObj *, char *);
void CreateFTPlink(GopherObj *, GopherDirObj *, char *);
#define system(a) vms_system(a)
#else
#define STRerror(a) strerror(a)
#endif

int	    Do_Sort;
extern char BummerMsg[];
extern char *WWW_to_VMS(char *, char);

void 
Do_IndexTrans(int sockfd, char *inputline)
{
     char   *IndexDirectory = NULL;
     char   *SearchString = NULL;
     char   *cp = NULL;
     char   *dbName = NULL;
     String *INDEXHost;
     String *INDEXPath;
     int    INDEXPort=0;
     int    Index_type=0;
     int    status;

     /** First siphon off the directory pathname **/
     Do_Sort = -1;	    /* Do Sort: Not TRUE, Not FALSE */;
     if (*inputline==':') { /* Maybe there's a sort control token here */
	if (strncasecmp(inputline,":sort:",strlen(":sort:"))==0) {
	    inputline += strlen(":sort:");
	    Do_Sort = TRUE;
	}
	else
	if (strncasecmp(inputline,":nosort:",strlen(":nosort:"))==0) {
	    inputline += strlen(":nosort:");
	    Do_Sort = FALSE;
	}
     }

     IndexDirectory = inputline;

     if (UsingHTML)
	  cp = strchr(inputline, '?');
     else
	  if ((cp = strchr(inputline, '\t')) == NULL)
	      cp = strchr(inputline, '?');
     
     if (cp == NULL) {
	  if (UsingHTML) {
	       Index_type = Find_index_type(IndexDirectory);

	       if (Index_type == -1) {
		    writestring(sockfd, "Server error....<P>\r\n");
		    writestring(sockfd, inputline);
		    writestring(sockfd, " is not a valid index<P>\r\n");
		    return;
	       }
	       writestring(sockfd, "<ISINDEX>\r\n");
	       writestring(sockfd, "This is a gopher index of ");
#ifdef VMS
	       writestring(sockfd, inputline);
#else
	       writestring(sockfd, inputline+1);
#endif
	       writestring(sockfd, ". <P>It is a ");
	       switch (Index_type) {
	       case WAISTYPE:
		    writestring(sockfd, "WAIS index.\r\n");
		    break;
	       case NEXTTYPE:
		    writestring(sockfd, "NeXT Digital Librarian index.\r\n");
		    break;
	       case GREPTYPE:
		    writestring(sockfd, "Brute force search of items in this directory.\r\n");
		    break;
#ifdef VMS
	       case CMD1TYPE:
		    writestring(sockfd, "Local command search.\r\n");
		    break;
#endif
	       case SHLLTYPE:
		    writestring(sockfd, "Shell gateway search.\r\n");
		    break;
	       } 
	       writestring(sockfd, "<P>\r\n");
	       return;

	  } else {
	       /** Give up it won't work..... **/
	       writestring(sockfd, ".\r\n");
	       return;
	  } 
     }
     else
	  *cp = '\0';

     
     /** And siphon off the search string **/

     SearchString = cp +1;

     /** Just in case, get rid of anything following a tab **/

     cp = strchr(SearchString, '\t');
     if (cp != NULL)
	  *cp = '\0';

     STRinit(INDEXHost = STRnew());
     STRinit(INDEXPath = STRnew());
     if ((status=Read_hostdata(IndexDirectory,INDEXHost,&INDEXPort,INDEXPath, 
					    dbName, sockfd)) <0) {
	  if (status >= -1) {
	    LOGGopher(sockfd, "Malformed hostdata file for %s/%s", 
					    IndexDirectory, dbName);
	    Abortoutput(sockfd, "Malformed hostdata", "badhostdata");
	  }
	  STRdestroy(INDEXHost);
	  STRdestroy(INDEXPath);
	  return;
     }

#ifndef VMS
     
     /* Doctor up the indexdirectory path if we're not running chroot()
      * we use fixfile to keep things secure....
      */
     if (!dochroot) {
	  IndexDirectory = fixfile(IndexDirectory);
	  
	  cp = (char *) malloc(sizeof(char)*250);
	  strcpy(cp, GDCgetDataDir(Config));
	  strcat(cp, "/");
	  strcat(cp, IndexDirectory);

	  IndexDirectory = cp;
     }
#endif

     /** And call the appropriate query function **/

     Index_type = Find_index_type(IndexDirectory);

     if (DEBUG) 
	  printf("Index %s type is %d\n", IndexDirectory, Index_type);

     switch (Index_type) {

     case NEXTTYPE:
#ifdef NEXTSEARCH
	  if (Do_Sort == -1)
	    Do_Sort = GDCgetSortNeXT(Config);
#endif
	  
	  NeXTIndexQuery(sockfd, SearchString, IndexDirectory, NULL, 
			 INDEXHost, INDEXPort, INDEXPath);
	  break;

     case WAISTYPE:
#ifdef WAISSEARCH
	  if (Do_Sort == -1)
	    Do_Sort = GDCgetSortWAIS(Config);
#endif
	  /*** The selector string has both the directory and the dbname... ***/
	  cp = strrchr(IndexDirectory, '/');
	  
	  if (cp == NULL)
	       dbName = "index";
	  else {
	       dbName= cp+1;
	       *cp='\0';
	  }
	  WaisIndexQuery(sockfd, IndexDirectory, SearchString, dbName, 
			 INDEXHost, INDEXPort, INDEXPath);
	  break;

     case GREPTYPE:
#ifdef GREPSEARCH
	  if (Do_Sort == -1)
	    Do_Sort = GDCgetSortGREP(Config);
#endif
	  GrepIndexQuery(sockfd, IndexDirectory, SearchString, 
			 INDEXHost, INDEXPort, INDEXPath);
	  break;

     case CMD1TYPE:
#ifdef CMD1TYPE
	  if (Do_Sort == -1)
	    Do_Sort = GDCgetSortCMD1(Config);
#endif
	  CMD1IndexQuery(sockfd, IndexDirectory, SearchString, 
			 INDEXHost, INDEXPort, INDEXPath);
	  break;
	  
     case SHLLTYPE:
#ifdef SHELLSEARCH
	  if (Do_Sort == -1)
	    Do_Sort = GDCgetSortShell(Config);
#endif
	  ShellIndexQuery(sockfd, IndexDirectory, SearchString,
			 INDEXHost, INDEXPort, INDEXPath);
	  break;
     
     default:      
	  /**** Error condition, unknown index type... ****/
	  LOGGopher(sockfd, "Unknown index type %d for %s", Index_type,
						IndexDirectory);
	  Abortoutput(sockfd, "Unknown index type", "index");
	  STRdestroy(INDEXHost);
	  STRdestroy(INDEXPath);
	  return;
     }
     
     /** Log it here so we get the query in the logfile **/
     STRdestroy(INDEXHost);
     STRdestroy(INDEXPath);
     LOGGopher(sockfd, "search %s%s%s for %s", IndexDirectory,
			    dbName==NULL?"":"/", dbName==NULL?"":dbName,
						SearchString);
}

/*
 * Try to figure out what each type of object is
 *
 */

int
Find_index_type(char *gopherpath)
{
     char Teststr[512];
     FILE *Testfile;
     char *cp;
     char VMSdirectory[256];

#if defined(CMD1SEARCH) && defined(VMS)
     /*** Check for the VMS CMD1TYPE designator ($) ***/
     if (gopherpath[0] == '$') {
	cp = skip_whitespace(gopherpath+1);
	strcpy(Teststr, cp);
	if ((cp=strchr(Teststr,' '))!=NULL)
	    *cp = '\0';
	/*** Check for a valid CMD1TYPE sub-type ***/
	if ((strcasecmp(Teststr,"SEARCH")==0)
	    || (strcasecmp(Teststr,"SEARCH&")==0)
	    || (strcasecmp(Teststr,"SEARCH|")==0)
	    || (strcasecmp(Teststr,"SEARCH~&")==0)
	    || (strcasecmp(Teststr,"SEARCH~|")==0)
	    || (strcasecmp(Teststr,"SINCE")==0)
	    || (strcasecmp(Teststr,"BEFORE")==0)
	    || (strcasecmp(Teststr,"VERSION")==0)
	    || (strcasecmp(Teststr,"GO4LINK")==0)
	    || (strcasecmp(Teststr,"FTPLINK")==0)) {
	    return(CMD1TYPE);
	}
     }
#endif

#ifdef NEXTSEARCH
     strcpy(Teststr, gopherpath);
     strcat(Teststr, "/.index/index.ixif");

     Testfile = fopen(Teststr, "r");
     if (Testfile != NULL) {
	  /*** Next Index ***/
	  fclose(Testfile);
	  return(NEXTTYPE);
     }
#endif

#ifdef WAISSEARCH
     strcpy(Teststr, gopherpath);
     strcat(Teststr, ".inv");

     Testfile = fopen(Teststr, "r");
     if (Testfile != NULL) {
	  /*** WAIS Index ***/
	  fclose(Testfile);
	  return(WAISTYPE);
     }
#endif

#ifdef SHELLSEARCH
     strcpy(Teststr, gopherpath);
#ifdef VMS
     /* find end of first selector token */
     if ((cp=strchr(Teststr,' '))!=NULL)
	*cp = '\0';
#endif
     if (Teststr[0] == '/') {
         /* Convert to VMS pathspec before testing */
         char *cp1, path[256];
	 strcpy(path, (cp1=WWW_to_VMS(Teststr, A_FILE)) ? cp1 : Teststr);
	 strcpy(Teststr, path);
     }
     Testfile = fopen(Teststr, "r");
     if (Testfile != NULL) {
	  /** Shell script? **/
#ifdef VMS
	if (strcasecmp(Teststr+strlen(Teststr)-strlen(".SHELL"),".SHELL")==0)
	  if (getc(Testfile) == '$')
#else
	  if (getc(Testfile) == '#')
#endif
	       if (getc(Testfile) == '!') {
		    fclose(Testfile);
		    return(SHLLTYPE);
	       }
     }
#endif

#ifdef GREPSEARCH
     strcpy(Teststr, gopherpath);
#ifdef VMS
     if (Teststr[0] == '/') {
         /* Convert to VMS pathspec before testing */
         char *cp1, path[256];
	 strcpy(path, (cp1=WWW_to_VMS(Teststr, A_FILE)) ? cp1 : Teststr);
	 strcpy(Teststr, path);
     }
     if ((cp=strchr(Teststr,'[')) && strchr(cp,']'))
          return(GREPTYPE);
#else
     if (isadir(Teststr) == 1)
	  return(GREPTYPE);
#endif
#endif
     
     return(-1);
}

/*
 * Read in the data from a hostdata file...
 * 
 * Try "<dbname>.hostdata" first, fall back to "hostdata" otherwise
 */

int
Read_hostdata(char *IndexDirectory, String *INDEXHost, int *INDEXPort, 
			    String *INDEXPath, char *dbName, int sockfd)
{
#define	H_PATH	(1<<0)
#define	H_PORT	(1<<3)
#define	H_HOST	(1<<4)
#define	H_ALL (H_PATH | H_PORT | H_HOST)

     FILE *Hostfile;
     char hostdataName[256];
     char inputline[256];
     char *cp;
     int doneflags = 0;
#ifdef VMS
     int  status;
     char VMSdirectory[256];

     if (IndexDirectory[0] == '$') {
          if ((cp = strchr(IndexDirectory, ' '))==NULL)
	    goto No_Hostdata;
	  strcpy(VMSdirectory, skip_whitespace(cp+1));
     }
     else
          strcpy(VMSdirectory, IndexDirectory);
     if (VMSdirectory[0] == '/') {
         /* Convert first pathspec to VMS syntax */
         char *cp1, *cps, path[256];
	 if((cps=strchr(VMSdirectory,' '))!=NULL)
	     *cps = '\0';
	 strcpy(path,
	 	(cp1=WWW_to_VMS(VMSdirectory, A_FILE)) ? cp1 : VMSdirectory);
	 strcpy(VMSdirectory, path);
     }
     if ((cp=strchr(VMSdirectory, ']')) != NULL) {
          *(cp+1) = '\0';
	if (strcmp((cp-3),"...]")==0)
	    strcpy((cp-3),"]");
     }
#endif

     /** Read in the proper hostdata file.... **/

#ifndef VMS
     chdir(IndexDirectory); /** Change into the index directory **/
#else
     if ((status=chdir(VMSdirectory)) <0) {
          LOGGopher(sockfd, "chdir(\"%s\") error: %s",VMSdirectory,
						STRerror(vaxc$errno));
	  Abortoutput(sockfd, "Cannot access that directory", "baddir");
	  return(-2);
     }
#endif
     if (dbName) {		    /* try idx.hostdata */
	sprintf(hostdataName, "%s%s.hostdata", GDCgetHiddenPrefix(Config), 
						    dbName);
	if ((Hostfile = fopen(hostdataName, "r")) != NULL)
	    goto hostdata_setup;
	sprintf(hostdataName, "%s.hostdata", dbName);
	if ((Hostfile = fopen(hostdataName, "r")) != NULL)
	    goto hostdata_setup;
     }
     sprintf(hostdataName, "%shostdata", GDCgetHiddenPrefix(Config));
     if ((Hostfile = fopen(hostdataName, "r")) != NULL)
	goto hostdata_setup;
     strcpy(hostdataName, "%hostdata");
     Hostfile = fopen(hostdataName, "r");

  hostdata_setup:
     if (Hostfile == NULL) {
	  /*** Use the current host/port/data_dir as the default ***/
     No_Hostdata:
	  STRset(INDEXHost, GDCgetHostname(Config));
	  *INDEXPort = GDCgetPort(Config);
	  STRset(INDEXPath, GDCgetDataDir(Config));
	  return(0);
     } 
     inputline[0] = '\0';

     for (;;) {
	  for (;;) {
	       cp = fgets(inputline, 1024, Hostfile);
	       if (inputline[0] != '#' || cp == NULL)
		    break;
	  }
	  
	  /*** Test for EOF ***/
	  if (cp==NULL)
	       break;
	  
	  ZapCRLF(inputline);  /* should zap tabs as well! */
#ifdef VMS
	  Continuation(inputline, Hostfile, 1024, '-');
#endif
	  if (DEBUG)
	    printf("%s: %s\n", hostdataName, inputline);

	  /*** Test for the various field values. **/
	  if (strncasecmp(inputline, GS_HOST, strlen(GS_HOST))==0) {
		doneflags |= H_HOST;
		if ((inputline[strlen(GS_HOST)] == '+' 
			|| inputline[strlen(GS_HOST)] == '*') 
			    && inputline[strlen(GS_HOST)+1] == '\0')
		    STRset(INDEXHost, GDCgetHostname(Config));
		else
		    STRset(INDEXHost, inputline+strlen(GS_HOST));
	  }
	  else 
	  if (strncasecmp(inputline, GS_PORT, strlen(GS_PORT))==0) {
		doneflags |= H_PORT;
		if ((inputline[strlen(GS_PORT)] == '+' 
			|| inputline[strlen(GS_PORT)] == '*') 
			    && inputline[strlen(GS_PORT)+1] == '\0')
			*INDEXPort = GDCgetPort(Config);
		    else
			*INDEXPort = atoi(inputline+strlen(GS_PORT));
	  }
	  else 
	  if (strncasecmp(inputline, GS_PATH, strlen(GS_PATH))==0) {
		doneflags |= H_PATH;
	       STRset(INDEXPath, inputline+strlen(GS_PATH));
	  }
     }
     fclose(Hostfile);
     return((doneflags == H_ALL) ? 0: -1);
}

/*
 * This is a searching function that runs grep across files in a single
 *  directory.
 * On VMS, if EGREP is not defined as a foreign command in SYLOGIN.COM and 
 *  the server is not running under Inetd/MULTINET_SERVER, it uses SEARCH.  
 *  If it is running under Inetd/MULTINET_SERVER, it gets the foreign command 
 *  symbol for EGREP from an initialization command file that  is executed by 
 *  the subprocess before executing the server's DCL command.
 */

void
GrepIndexQuery(int sockfd, char *Indexdir, char *Searchstr, String *INDEXHost, 
		    int INDEXPort, String *INDEXPath)
{
#ifndef GREPSEARCH
     LOGGopher(sockfd, "GREP index %s, srch '%s's", Indexdir, Searchstr);
     Abortoutput(sockfd, "Sorry, no GREP indexing supported", "index");
     return;
#else
     FILE *moocow;
     char command[512];
     char inputline[512];
     char *filename;
     char path[256];
     char *cp;
     GopherObj *gs;
     GopherDirObj *gd;
#ifdef VMS
     char outfile[256];
     int  status;
     char *c2, *c3;

     if (Indexdir[0] == '/') {
         /* Convert to VMS pathspec */
         char *cp1, path[256];
	 strcpy(path, (cp1=WWW_to_VMS(Indexdir, A_FILE)) ? cp1 : Indexdir);
	 strcpy(Indexdir, path);
     }

     c3 = c2 = (char *) malloc(sizeof(char)*strlen(Indexdir)+1);
     strcpy(c3,Indexdir);

     while (strlen(c2)) {
	cp = strpbrk(c2,",");
	if (cp)
	    *cp = '\0';
	if (Validate_Filespec(c2)==NULL) {
	    LOGGopher(sockfd, "GREP Illegal syntax for %s", Indexdir);
	    Abortoutput(sockfd, "Eh? Confusing Request","syntax");
	    free(c3);
	    return;
	}
	c2 += strlen(c2) + ((cp==NULL) ? 0 : 1);
     }
     free(c3);
#endif

     gs = GSnew();
     gd = GDnew(32);

#ifndef VMS
     cp = Searchstr;
     while (*cp != '\0') {
	  if (*cp == ';' ||*cp == '"' || *cp == '`')
	       *cp = '.';
	  cp++;
     }

     sprintf(command, "egrep \"%s\" \"%s\"/*", Searchstr, Indexdir);
#else
     strcpy(outfile, cp=tempnam(GDCgetScratchDir(Config),NULL));
     free(cp);
     /*** If not Inetd/MULTINET_SERVER and no EGREP, use SEARCH ***/
     if (!RunFromInetd && getenv("EGREP") == NULL) {
          while(cp=strpbrk(Searchstr," \t\r\n")) {
	     if (*(cp-1)==',')
	         strcpy(cp,cp+1);
	     else
	         *cp = ',';
          }
          sprintf(command,"$ search/nohead/output=%s/window=0 %s %s",
			outfile, Indexdir, Searchstr);
     }
     /*** Use EGREP ***/
     else {
          cp = Searchstr;
          while (*cp != '\0') {
	       if (*cp == '"' || *cp == '`')
	            *cp = '.';
	       cp++;
          }
          if (Indexdir[strlen(Indexdir)-1] == ']')
               sprintf(command,"$ egrep -il \">%s\" -e \"%s\" \"%s*.*;\"",
			       outfile, Searchstr, Indexdir);
          else
               sprintf(command,"$ egrep -il \">%s\" -e \"%s\" \"%s\"",
			       outfile, Searchstr, Indexdir);
     }
#endif
     if (strlen(Searchstr)==0) {
	writestring(sockfd, ".\r\n");
	LOGGopher(sockfd, "Null Search String");
	GSdestroy(gs);
	GDdestroy(gd);
	return;
     }

     if (DEBUG) 
	  printf("Grep command is %s\n", command);

#ifndef VMS
     moocow = popen(command, "r");
#else
     if (((status=system(command)) &1) != 1) {
	writestring(sockfd, ".\r\n");
	LOGGopher(sockfd, "Couldn't call SEARCH - %s",
				    strerror(EVMSERR,vaxc$errno));
        if (access(outfile, 0) == 0)
	    unlink(outfile);
	GSdestroy(gs);
	GDdestroy(gd);
	return;
     } 
     moocow = fopen(outfile, "r");
#endif
     
     if (moocow == NULL) {
	  LOGGopher(sockfd, "Couldn't open grep command - %s on %s",
				    STRerror(errno), outfile);
	  LOGGopher(sockfd, "  on attempted grep: %s",command);
	  writestring(sockfd, ".\r\n");
	  GSdestroy(gs);
	  GDdestroy(gd);
	  return;
     }

     GSinit(gs);
     cp = inputline;
     while (fgets(cp+1, 512, moocow)) {
	  inputline[0] = A_FILE;	    /*	Assume a File */
	  ZapCRLF(inputline);
	  switch(isadir(cp+1)) {
	  case 1:   inputline[0] = A_DIRECTORY;	    break;
	  case 0:   inputline[0] = A_FILE;	    break;
	  case -1:  continue;
	  }
	  GSsetType(gs, A_FILE);
	  GSsetHost(gs, STRget(INDEXHost));
	  GSsetPort(gs, INDEXPort);
#ifndef VMS
    /*	Have to fix this so type prefixes the path..... */
	  *(strchr(inputline, ':')='\0';
	  filename = strstr(inputline,STRget(INDEXPath)) 
				+ strlen(STRget(INDEXPath));
	  strcpy(path, cp+1);
#else
          /* force names lowercase */
          for(c2 = cp+1; *c2; c2++)
               *c2 = _tolower(*c2);
	  /* create the path and filename */
	  strcpy(path, cp+1);
	  *(strchr(path,']')+1) = *(strchr(inputline,';')) = '\0';
	  c2 = inputline + strlen(inputline) - 1;
          if (*c2 == 'z')
	       *c2 = 'Z';
	  filename = strchr(inputline,']')+1;
#endif
	  if (!GDCignore(Config,filename)) {
	    if (chdir(path)>=0)
		AddFiletoDir(sockfd, gs, gd, filename, cp+1, NULL);
	  }
     }
     if (GDgetNumitems(gd)) {
	  GDaddDateNsize(gd);
	  if (Do_Sort) GDsort(gd);
	  if (UsingHTML)
	       GDtoNetHTML(gd, sockfd);
	  else {
	       GDtoNet(gd, sockfd);
	       writestring(sockfd, ".\r\n");
	  }
     }
     GSdestroy(gs);
     GDdestroy(gd);
#ifndef VMS
     pclose(moocow);
#else
     fclose(moocow);
     unlink(outfile);
#endif
#endif
}

/*
 * This starts up a 1-line command that's defined to be an "index gateway"
 *
 * Currently "SEARCH", "SINCE", "BEFORE", "VERSION", "GO4LINK" and "FTPLINK" 
 * are supported.
 *
 * "SEARCH" performs a case-insensitive search for terms, without any
 * interpretation of printable characters, with leading and trailing spaces
 * trimmed, and with embedded spaces treated as the Boolean OR.  The search
 * corresponds to the expectations of the client's Boldit() routine.  SEARCH
 * may be suffixed by &, |, ~& or ~| to select the VMS /MATCH= operand, and
 * override the default, implicit /MATCH=OR.
 *
 * "SINCE" and "BEFORE" return lists of files modified since or before
 * the supplied term, a date specification.
 *
 * "VERSION" displays version information about the server.  It's not really
 * a "search" even though it acts like one.
 *
 * "GO4LINK" uses a pseudo-SearchString (the Internet address of a Gopher
 * server) to create a link tuple which the client can use to connect to
 * a server there.
 *
 * "FTPLINK" uses a pseudo-SearchString (the Internet address of an FTP
 * archive) to create a link tuple which the client can pass to the server's
 * FTP gateway.
 *
 * The command must write output in the same format as the GrepIndexQuery()
 * output is formatted.
 *
 */

void
CMD1IndexQuery(int sockfd, char *CMDcmd, char *Searchstr, String *INDEXHost, 
		    int INDEXPort, String *INDEXPath)
{
#if !defined(CMD1SEARCH) || !defined(VMS)
     LOGGopher(sockfd, "CMD index %s, srch '%s's", CMDcmd, Searchstr);
     Abortoutput(sockfd, "Sorry, no command indexing supported", "index");
     return;
#else
     FILE *moocow;
     char command[512];
     char inputline[512];
     char *filename;
     char path[256];
     char *cp;
     GopherObj *gs;
     GopherDirObj *gd;
     char outfile[256];
     int  status;
     char *c2, *c3;
     char *beg, *end;
     char VMSSearchstr[256];
     char *match;
     int i;
     int DoDateNsize=TRUE;

     if (filename=strchr((c2 = skip_whitespace(CMDcmd+1)),' '))
	filename++;
     else switch (*c2) {    /*	Some of these CMD1 searches don't need files */
        case 'g':
	case 'G':   goto files_valid;

        case 'f':
	case 'F':   goto files_valid;

	case 'v':
	case 'V':   goto files_valid;

	default:    goto illegal;
     }
     if (filename[0] == '/') {
         /* Convert to VMS pathspec */
         char *cp1, path[256];
	 strcpy(path, (cp1=WWW_to_VMS(filename, A_FILE)) ? cp1 : filename);
	 strcpy(filename, path);
     }

     c3 = c2 = (char *) malloc(sizeof(char)*strlen(filename)+1);
     strcpy(c3,filename);
     while (strlen(c2)) {
	cp = strpbrk(c2,",");
	if (cp)
	    *cp = '\0';
	if (Validate_Filespec(c2)==NULL) {
     illegal:
	    LOGGopher(sockfd, "CMD1 Illegal syntax for %s", CMDcmd);
	    Abortoutput(sockfd, "Eh? Confusing Request","syntax");
	    free(c3);
	    return;
	}
	c2 += strlen(c2) + ((cp==NULL) ? 0 : 1);
     }
     free(c3);

files_valid:     
     /*** Establish the output file for the VMS command ***/
     strcpy(outfile, cp=tempnam(GDCgetScratchDir(Config),NULL));
     free(cp);

     /*** Get the CMD1TYPE sub-type ***/
     cp = skip_whitespace(CMDcmd+1);

     if ((strncasecmp(cp, "SEARCH ", 7)==0)
	|| (strncasecmp(cp, "SEARCH& ", 8)==0) 
	|| (strncasecmp(cp, "SEARCH| ", 8)==0)
	|| (strncasecmp(cp, "SEARCH~& ", 9)==0)
	|| (strncasecmp(cp, "SEARCH~| ", 9)==0)) {
     /*** Determine match qualification, if any ***/
	match = NULL;
	if (strncmp(cp+6,"& ",2)==0)
	    match = "/MATCH=AND";
	else
	if (strncmp(cp+6,"| ",2)==0)
	    match = "/MATCH=OR";
	else
	if (strncmp(cp+6,"~& ",3)==0)
	    match = "/MATCH=NAND";
	else
	if (strncmp(cp+6,"~| ",3)==0)
	    match = "/MATCH=NOR";
     /*** Convert Searchstr for the VMS SEARCH command ***/ 
        beg = Searchstr;
        end = beg + strlen(Searchstr);
        i = 0;
	/*** Trim any leading spaces and prefix a double-quote ***/
	while (*(beg) == ' ' && beg < end) beg++; 
	VMSSearchstr[i++] = '"';
        while (beg < end) {
              /*** Reiterate any embedded double-quotes ***/
              if (*beg == '"') {
	         VMSSearchstr[i++] = '"';
	         VMSSearchstr[i++] = *(beg++);
	      }
	      /*** Replace any embedded spaces with a "," triplet ***/
	      /*** or omit trailing spaces                        ***/
	      else if (*beg == ' ') {
	         while (*beg == ' ' && beg < end) beg++;
		 if (beg < end) {
		    VMSSearchstr[i++] = '"';
		    VMSSearchstr[i++] = ',';
		    VMSSearchstr[i++] = '"';
		 }
	      }
	      /*** Otherwise, use the character as entered ***/
	      else VMSSearchstr[i++] = *(beg++);
        }
	/*** Append a double-quote and terminate ***/
	VMSSearchstr[i++] = '"';
        VMSSearchstr[i] = '\0';
     }
     else
     if (strncasecmp(cp, "VERSION", 7)==0 ||
         strncasecmp(cp, "GO4LINK", 7)==0 ||
         strncasecmp(cp, "FTPLINK", 7)==0)
     {
	/*** No further validation required ***/
     }
     else {
     /*** "SINCE" or "BEFORE" -- Validate the entry as a date ***/
        if (!Validate_Date(Searchstr)) {
	   LOGGopher(sockfd, "Illegal search term \"%s\" for %s", Searchstr,
			     CMDcmd);
	   Abortoutput(sockfd, "Eh? Unusable Search Term(s)", "syntax");
	   return;
        }
     }

     gs = GSnew();
     gd = GDnew(32);

     /*** Format the command string ***/
     if (strncasecmp(cp, "SEARCH", 6)==0) {
         sprintf(command,"$search%s/nohead/output=%s/window=0 %s %s",
	    match==NULL?"":match, outfile, filename, VMSSearchstr);
	 /*** Log the string actually used in the search ***/
         LOGGopher(sockfd, "VMSSearchstr: %s", VMSSearchstr);
     }
     else if (strncasecmp(cp, "SINCE ", 6)==0)
	 sprintf(command, "$dir/nohead/notrail/output=%s/since=%s %s",
			  outfile, Searchstr, filename);
     else if (strncasecmp(cp, "BEFORE ", 7)==0)
	 sprintf(command, "$dir/nohead/notrail/output=%s/before=%s %s",
			  outfile, Searchstr, filename);
     else if (strncasecmp(cp, "VERSION", 7)==0) {
	sprintf(command, "determine version level");
	outfile[0] = '\0';  /* Won't need output file */
     }
     else if (strncasecmp(cp, "GO4LINK", 7)==0) {
	sprintf(command, "create a Gopher Server tuple");
	outfile[0] = '\0';  /* Won't need output file */
     }
     else if (strncasecmp(cp, "FTPLINK", 7)==0) {
	sprintf(command, "create an FTP gateway tuple");
	outfile[0] = '\0';  /* Won't need output file */
     }
     if (DEBUG) 
	  printf("CMD is %s\n", command);

     if (strlen(outfile)==0) {
     /*	One of our non-DCL commands...just VERSION for now... */
	moocow = NULL;
	switch(*cp) {
        case 'g':
	case 'G':
	    GSinit(gs);    
	    CreateGO4link(gs, gd, Searchstr);
	    break;

        case 'f':
	case 'F':
	    GSinit(gs);    
	    GSsetHost(gs, STRget(INDEXHost));
	    GSsetPort(gs, INDEXPort);
	    CreateFTPlink(gs, gd, Searchstr);
	    break;

	case 'v':
	case 'V':
	    GSinit(gs);    
	    GSsetHost(gs, STRget(INDEXHost));
	    GSsetPort(gs, INDEXPort);
	    SearchVersion(sockfd, gs,gd);
	    Do_Sort = DoDateNsize = FALSE;
	    break;

	default:
	    LOGGopher(sockfd, "Unknown non-DCL search %s",cp);
	}
     }
     else {
	if (((status=system(command)) &1) != 1) {
	    writestring(sockfd, ".\r\n");
	    LOGGopher(sockfd,"Couldn't call CMD - %s",strerror(EVMSERR,status));
            if (access(outfile, 0) == 0)
	        unlink(outfile);
	    GSdestroy(gs);
	    GDdestroy(gd);
	    return;
	} 
	moocow = fopen(outfile, "r");
     
	if (moocow == NULL) {
	    LOGGopher(sockfd,"Couldn't open CMD command - %s on %s",
				    strerror(errno), outfile);
	    LOGGopher(sockfd, "  on attempted CMD: %s",command);
	    writestring(sockfd, ".\r\n");
	    GSdestroy(gs);
	    GDdestroy(gd);
	    return;
	}

	GSinit(gs);
	cp = inputline;
	while (fgets(cp+1, 512, moocow)) {
	    inputline[0] = A_FILE;	    /*	Assume a File */
	    ZapCRLF(inputline);
	    switch(isadir(cp+1)) {
	    case 1:   inputline[0] = A_DIRECTORY;   break;
	    case 0:   inputline[0] = A_FILE;	    break;
	    case -1:  continue;
	    }
	    GSsetType(gs, A_FILE);
	    GSsetHost(gs, STRget(INDEXHost));
	    GSsetPort(gs, INDEXPort);
            /* force names lowercase */
            for(c2 = cp+1; *c2; c2++)
                 *c2 = _tolower(*c2);
	    /* create the path and filename */
	    strcpy(path, cp+1);
	    *(strchr(path,']')+1) = *(strchr(inputline,';')) = '\0';
	    c2 = inputline + strlen(inputline) - 1;
            if (*c2 == 'z')
	         *c2 = 'Z';
	    filename = strchr(inputline,']')+1;
	    if (!GDCignore(Config,filename)) {
		if (chdir(path)>=0)
		    AddFiletoDir(sockfd, gs, gd, filename, cp+1, NULL);
	    }
	}
     }
     if (GDgetNumitems(gd)) {
	  if (DoDateNsize)  GDaddDateNsize(gd);
	  if (Do_Sort)	    GDsort(gd);
	  if (UsingHTML)
	       GDtoNetHTML(gd, sockfd);
	  else {
	       GDtoNet(gd, sockfd);
	       writestring(sockfd, ".\r\n");
	  }
     }
     GDdestroy(gd);
     GSdestroy(gs);
     if (moocow!=NULL) {
	fclose(moocow);
	unlink(outfile);
     }
#endif
}

#ifdef VMS
void
SearchVersion(int sockfd, GopherObj *gs, GopherDirObj *gd)
{
    FILE *fp;
    static long i;
    static char path[256];
    char *x, *cp;
    char image[512];
    struct dsc$descriptor_s
         dsc$tstamp = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
				/* Structures IHDdef & IHIdef extrapolated from
				    $IHDDEF and $IHIDEF, respectively, as
				    found in SYS$LIBRARY:LIB.MLB, VMS v5.2  */
#ifndef __ALPHA
    struct IHDdef
	{
	    short   IHD$W_SIZE;
	    short   IHD$W_ACTIVOFF;
	    short   IHD$W_SYMDBGOFF;
	    short   IHD$W_IMGIDOFF;
	    short   IHD$W_PATCHOFF;
	    short   IHD$W_VERSION_ARRAY_OFF;
	/*  There's more, but we don't need it... */
	} *ihd$;

    struct IHIdef
	{
	    char    IHI$T_IMGNAM[40];
	    char    IHI$T_IMGID[16];
	    char    IHI$Q_LINKTIME[8];
	    char    IHI$T_LINKID[16];
	} *ihi$;
#else
    struct IHIdef
	{
	    char    IHI$Q_LINKTIME[8];
	    char    IHI$T_IMGNAM[40];
	    char    IHI$T_IMGID[16];
	    char    IHI$T_LINKID[16];
	} *ihi$;
#endif
    struct itmlst
	itmlist[3] = {  { 8, SYI$_VERSION, &path[100], (int *) &i},
			{ 31, SYI$_HW_NAME, &path[120], (int *) &i},
			{ 0, 0, 0, 0 } };


    bzero(image, sizeof(image));
    bzero(path, sizeof(path));
    GSsetNum(gs, 0);
    GSsetType(gs, path[0] = A_FILE);
    strcat(path,pname);
    if ((cp = strrchr(path, ';')) != 0)
         *cp = '\0';
    GSsetPath(gs, path);
    if ((fp = fopen(pname, "r")) == NULL)
	GSsetType(gs, A_ERROR);
    else {			/*  Read in 1st block of Image Header */
	if (fread(image, sizeof(char), sizeof(image), fp) == 0)
	    GSsetType(gs, A_ERROR);
	else {			/* Pick up offset to Image Name & Ident */
#ifndef __ALPHA
	    ihd$ = (struct IHDdef *)image;
	    i = ihd$->IHD$W_IMGIDOFF;
	    ihi$ = (struct IHIdef *)(image+i);
#else
	    ihi$ = (struct IHIdef *)(image+192);
#endif
	    strcpy(path,"%fn [%ts, %sz]");
	    GSsetTitle(gs,path); GSaddDateNsize(gs);
	    sprintf(path, "Image:         \"%s\"", GSgetTitle(gs));
	    GSsetTitle(gs,path);
	    GSsetPath(gs, BummerMsg);
	    GDaddGS(gd, gs);
	    strcpy(path,"Image Name:    \"");
	    strncat(path,ihi$->IHI$T_IMGNAM+1,ihi$->IHI$T_IMGNAM[0]);
	    strcat(path, "\"");
	    GSsetTitle(gs,path);    GDaddGS(gd, gs);
	    strcpy(path,"Image File ID: \"");
	    strncat(path,ihi$->IHI$T_IMGID+1,ihi$->IHI$T_IMGID[0]);
	    strcat(path, "\"");
	    GSsetTitle(gs,path);    GDaddGS(gd, gs);
	    bzero(path,sizeof(path)-100);
	    strcpy(path,"Link date/time: ");
	    dsc$tstamp.dsc$a_pointer = path+strlen(path);
	    dsc$tstamp.dsc$w_length = sizeof(path)-strlen(path);
	    LIB$FORMAT_DATE_TIME(&dsc$tstamp, &(ihi$->IHI$Q_LINKTIME), 0, &i, 0);
	    *(dsc$tstamp.dsc$a_pointer + i) = '\0';
	    if ((*dsc$tstamp.dsc$a_pointer) == ' ')
		memmove(dsc$tstamp.dsc$a_pointer,dsc$tstamp.dsc$a_pointer+1,i-1);
	    GSsetTitle(gs,path);    GDaddGS(gd, gs);
	    strcpy(path,"Linker ID:     \"");
	    strncat(path,ihi$->IHI$T_LINKID+1,ihi$->IHI$T_LINKID[0]);
	    strcat(path, "\"");
	    GSsetTitle(gs,path);    GDaddGS(gd, gs);
	}
	fclose(fp);
    }
    if (SS$_NORMAL!=SYS$GETSYIW(0,0,0,itmlist,0,0,0))
	GSsetType(gs, A_ERROR);
    else {
	path[120+i] = '\0';
	for (i=107; path[i]==' '; path[i--] = '\0');
        for(cp = &path[100]; *cp; cp++)
               *cp = _tolower(*cp);
	sprintf(path, "Host System:    %s, VMS %s",&path[120],&path[100]);
	GSsetTitle(gs,path);
	GSsetPath(gs, BummerMsg);
	GDaddGS(gd, gs);
    }

    if (GSgetType(gs)==A_ERROR)
	LOGGopher(sockfd,"Could not report version information");
    else
	LOGGopher(sockfd,"Reported version");
    {
#ifdef MULTINET
#define TCPIP_AGENT   "TGV Multinet"
#endif
#ifdef UCX
#define TCPIP_AGENT   "UCX"
#endif
#ifdef WOLLONGONG
#define TCPIP_AGENT   "Wollongong"
#endif
#ifdef TCPIP_AGENT
    sprintf(path, "TCP/IP agent:   %s", TCPIP_AGENT);
    GSsetTitle(gs,path);    
    GDaddGS(gd, gs);
#endif
    }

    sprintf(path, "Revision Level: %s-%s", GOPHERD_VERSION, PATCH_LEVEL);
    GSsetTitle(gs,path);    
    GDaddGS(gd, gs);

#define VCFG(a,b)\
{ sprintf(path,(a),(b));GSsetTitle(gs,path);GDaddGS(gd,gs); }

#define IFAVAIL(a) (strlen((a))==0?"N/A":(a))

    VCFG("HostAlias:      %s", IFAVAIL(GDCgetHostname(Config)));
    VCFG("Port:           %d", GDCgetPort(Config));
    VCFG("Site:           %s", IFAVAIL(GDCgetSite(Config)));
    VCFG("Admin:          %s", IFAVAIL(GDCgetAdmin(Config)));
    VCFG("AdminEmail:     %s", IFAVAIL(GDCgetAdminEmail(Config)));
    VCFG("Org:            %s", IFAVAIL(GDCgetOrg(Config)));
    VCFG("Loc:            %s", IFAVAIL(GDCgetLoc(Config)));
    VCFG("Geog:           %s", IFAVAIL(GDCgetGeog(Config)));
    if (GDCgetTZ(Config)==-1)
	VCFG("TimeZone:       %s", GDCgetTimeZone(Config))
    else
	VCFG("TimeZone:       %d", GDCgetTZ(Config));
    VCFG("ReadTimeout:    %d", GDCgetReadTimeout(Config));
    VCFG("InetD:          %s", GDCgetInetdActive(Config)==TRUE?"TRUE":"FALSE");
    VCFG("SortDir:        %s", GDCgetSortDir(Config)==TRUE?"TRUE":"FALSE");
    if (!GDCgetFTPPort(Config))
	VCFG("FTPPort:        %s", "NONE")
    else
	VCFG("FTPPort:        %d", GDCgetFTPPort(Config));
    if (!GDCgetEXECPort(Config))
	VCFG("EXECPort:       %s", "NONE")
    else
	VCFG("EXECPort:       %d", GDCgetEXECPort(Config));
    if (!GDCgetSRCHPort(Config))
	VCFG("SRCHPort:       %s", "NONE")
    else
	VCFG("SRCHPort:       %d", GDCgetSRCHPort(Config));
#ifdef NEXTSEARCH
    VCFG("SortNeXT:       %s", GDCgetSortNeXT(Config)==TRUE?"TRUE":"FALSE");
#endif
#ifdef WAISSEARCH
    VCFG("SortWAIS:       %s", GDCgetSortWAIS(Config)==TRUE?"TRUE":"FALSE");
#endif
#ifdef GREPSEARCH
    VCFG("SortGREP:       %s", GDCgetSortGREP(Config)==TRUE?"TRUE":"FALSE");
#endif
#ifdef CMD1SEARCH
    VCFG("SortCMD1:       %s", GDCgetSortCMD1(Config)==TRUE?"TRUE":"FALSE");
#endif
#ifdef SHELLSEARCH
    VCFG("SortShell:      %s", GDCgetSortShell(Config)==TRUE?"TRUE":"FALSE");
#endif
    VCFG("Cache:          %s", GDCgetCaching(Config)==TRUE?"TRUE":"FALSE");
    VCFG("MaxLoad:        %g", GDCgetMaxLoad(Config));
    VCFG("Do_chroot:      %s", GDCgetchroot(Config)==TRUE?"TRUE":"FALSE");
    VCFG("BummerMsg:      \"%s\"", IFAVAIL(GDCgetBummerMsg(Config)));
    VCFG("DName:          \"%s\"", IFAVAIL(GDCgetDName(Config)));
    VCFG("DHead:          \"%s\"", IFAVAIL(GDCgetDHead(Config)));
    VCFG("DFoot:          \"%s\"", IFAVAIL(GDCgetDFoot(Config)));
    VCFG("Hidden:         \"%s\"", IFAVAIL(GDCgetHiddenPrefix(Config)));
    VCFG("Link:           \"%s\"", IFAVAIL(GDCgetLinkPrefix(Config)));
    VCFG("Lookaside:      %s", IFAVAIL(GDCgetLookAside(Config)));
    VCFG("DataDirectory:  %s", IFAVAIL(GDCgetDataDir(Config)));
    VCFG("LogFile:        %s", IFAVAIL(GDCgetLogfile(Config)));
    VCFG("ScratchDir:     %s", IFAVAIL(GDCgetScratchDir(Config)));
    VCFG("SpawnInit:      %s", IFAVAIL(GDCgetSpawnInit(Config)));
    VCFG("Restart:        %s", IFAVAIL(GDCgetRestart(Config)));
    x =  "Rollover:       %s";
    switch(GDCgetRollover(Config)) {
    case ROLLOVER_DAILY:    VCFG(x,"DAILY");	    break;
    case ROLLOVER_HOURLY:   VCFG(x,"HOURLY");	    break;
    case ROLLOVER_MONTHLY:  VCFG(x,"MONTHLY");	    break;
    case ROLLOVER_ANNUALLY: VCFG(x,"ANNUALLY");	    break;
    case ROLLOVER_WEEKLY:   VCFG(x,"WEEKLY");	    break;
    case ROLLOVER_NEVER:    
    default:		    VCFG(x,"NEVER");	
    }
}

void
CreateGO4link(GopherObj *gs, GopherDirObj *gd, char *GO4Site)
{
    char GO4buf[256], *cp;

    GSsetType(gs, A_DIRECTORY);
    if (cp=strchr(GO4Site,' ')) {
	*cp = '\0';
	GSsetPort(gs,atoi(++cp));
    }
    else
	GSsetPort(gs,70);   /* Hard coded since that's the Gopher standard,
				and we have to take a guess here */
    GSsetHost(gs, GO4Site);
    GSsetPath(gs, "");
    bzero(GO4buf, sizeof(GO4buf));
    sprintf(GO4buf, "Link to Gopher host %s on port %d",
			GO4Site, GSgetPort(gs));
    GSsetTitle(gs, GO4buf);
    GDaddGS(gd, gs);
}

void
CreateFTPlink(GopherObj *gs, GopherDirObj *gd, char *FTPSite)
{
    char ftpbuf[256], *cp;

    bzero(ftpbuf, sizeof(ftpbuf));
    sprintf(ftpbuf, "%cftp:%s@/", A_DIRECTORY, FTPSite);
    GSsetType(gs, ftpbuf[0]);
    GSsetPath(gs, ftpbuf+1);
    cp = ftpbuf + strlen(ftpbuf);
    strcat(ftpbuf, "Link to aFTP host ");
    strcat(ftpbuf, FTPSite);
    GSsetTitle(gs, cp);
    GDaddGS(gd, gs);
}
#endif

/*
 *  *date locates a date specification.  Make sure that's what it is, and
 *  return TRUE or FALSE.
 */
boolean
Validate_Date(char *date)
{
#ifdef VMS
    int	status;
    char quadword[8];
    $DESCRIPTOR(dsc$date,"");
    long flags = 0x7f;
#endif
    if (date==NULL)
	return(FALSE);
#ifdef VMS
    dsc$date.dsc$a_pointer = date;
    dsc$date.dsc$w_length = strlen(date);
    status = LIB$CONVERT_DATE_STRING(&dsc$date, quadword,0,&flags,0,0);
    if ((status &1)!=1)
	return(FALSE);
#endif
    return(TRUE);
}

/*
 * This starts up a shell script that's defined to be an index gateway
 * 
 * The shell script should write out standard gopher directory protocol.
 */

void
ShellIndexQuery(int sockfd, char *Script, char *Searchstring,
	String *INDEXHost, int INDEXPort, String *INDEXPath)
{
#ifndef SHELLSEARCH
     LOGGopher(sockfd, "SHELL index %s, srch '%s's", Script, Searchstring);
     Abortoutput(sockfd, "Sorry, no SHELL indexing supported", "index");
     return;
#else
     GopherDirObj *gd;
     char Command[512];
     FILE  *Searchprocess;
     char *cp;
#ifdef VMS
     char outfile[256];
     char *cfg;
     int status;

     if((cp=strchr(Script,' '))!=NULL) {
         char *cp1, path[256];
	 *cp = '\0';
         if(Script[0] == '/')
             /* Convert to first pathspec to VMS syntax */
	     sprintf(path, "%s ",
	     		   (cp1=WWW_to_VMS(Script, A_FILE)) ? cp1 : Script);
	 else
	     sprintf(path, "%s ", Script);
         if(*(cp+1) == '/')
             /* Convert to second pathspec to VMS syntax */
	     strcat(path, (cp1=WWW_to_VMS(cp+1, A_FILE)) ? cp1 : cp+1);
	 else
	     strcat(path, cp+1);
	 *cp = ' ';
	 strcpy(Script, path); 
     }

     strcpy(outfile, Script);
     if ((cp=strchr(outfile,' '))!=NULL)
	*cp = '\0';
     if (Validate_Filespec(outfile)==NULL) {
	LOGGopher(sockfd, "SHELL Illegal syntax for %s", Script);
	Abortoutput(sockfd, "Eh? Confusing Request","syntax");
	return;
     }
#endif     

     gd = GDnew(32);

     /*** Clean up the arguments, remove  ; and " and `**/
     cp = Searchstring;
     while (*cp != '\0') {
	  if (*cp == ';' ||*cp == '"' || *cp == '`')
	       *cp = '.';
	  cp++;
     }
	  
#ifndef VMS
     sprintf(Command, "\"%s\" \"%s\"", Script, Searchstring);

     Searchprocess = popen(Command, "r");
#else
     strcpy(outfile, cp=tempnam(GDCgetScratchDir(Config),NULL));
     free(cp);
     cfg = (char *) malloc(sizeof(char)*strlen(GDCgetDataDir(Config)+1));
     strcpy(cfg, GDCgetDataDir(Config));
     if ((cp=strchr(cfg,':'))!=NULL)
	*cp='\0';
     sprintf(Command, "$ @%s %s \"%s\" \"%s\" %d %s", Script, outfile, 
				    Searchstring, STRget(INDEXHost), INDEXPort,
					cfg);
LOGGopher(sockfd, "Command: %s", Command);
     free(cfg);
     if (((status=system(Command)) &1) != 1) {
	writestring(sockfd, ".\r\n");
	LOGGopher(sockfd,"Couldn't call Shell Script - %s",
					    strerror(EVMSERR,status));
        if (access(outfile, 0) == 0)
	    unlink(outfile);
	GDdestroy(gd);
	return;
     }
     Searchprocess = fopen(outfile, "r");
#endif

     if (Searchprocess == NULL) {
	  LOGGopher(sockfd, "Couldn't open shell command - %s on %s",
				    STRerror(errno), outfile);
	  LOGGopher(sockfd, "  on attempted shell: %s", Command);
	  writestring(sockfd, ".\r\n");
	  GDdestroy(gd);
	  return;
     }

     GDfromNet(gd, fileno(Searchprocess), NULL);
     if (GDgetNumitems(gd)) {
	  GDaddDateNsize(gd);
	  if (Do_Sort) GDsort(gd);
	  if (UsingHTML)
	       GDtoNetHTML(gd, sockfd);
	  else {
	       GDtoNet(gd, sockfd);
	       writestring(sockfd, ".\r\n");
	  }
     }
     GDdestroy(gd);
#ifndef VMS
     pclose(Searchprocess);
#else
     fclose(Searchprocess);
     unlink(outfile);
#endif
#endif
}

#ifndef WAISSEARCH

void
WaisIndexQuery(int sockfd, char *index_directory, char *SearchWords, 
		    char *new_db_name, String *INDEXHost, int INDEXPort, 
			    String *INDEXPath)
{
     LOGGopher(sockfd, "WAIS index %s, srch '%s', Db %s", index_directory,
				SearchWords, new_db_name);
     Abortoutput(sockfd, "Sorry, no WAIS indexing supported", "index");
     return;
}
#endif 

#ifndef NEXTSEARCH
void
NeXTIndexQuery(int sockfd, char *SearchWords, char *ZIndexDirectory, 
		char *DatabaseNm /*** Not used by the NeXT indexer... ***/, 
			String *INDEXHost, int INDEXPort)
{
     LOGGopher(sockfd, "NeXT index %s, srch '%s'", ZIndexDirectory,
				SearchWords);
     Abortoutput(sockfd, "Sorry, no NeXT indexing supported", "index");
     return;
}
#endif
