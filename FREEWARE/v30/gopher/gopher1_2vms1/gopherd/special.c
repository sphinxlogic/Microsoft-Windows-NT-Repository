/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:13:27 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/special.c,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: special.c
 * routines to deal with special types of files, compressed, scripts, etc.
 *********************************************************************
 * Revision History:
 * $Log: special.c,v $
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#include "gopherd.h"


/* Check to see if this file needs special treatment before heading
 * back to the client... We will check for:
 *	Compressed files	if so, zcat first
 *	Shellscript		if so, "do it"
 * Note: it would be somewhat non-portable to check of a binary
 *  (we'd need to check for so many different magic numbers; the
 *  shell script designation should be sufficient, since the script
 *  can call an executable anyway
 * Recognized elsewhere:
 *	.snd			needs special processing on client
 *	uuencoded		needs special processing on client
 * Other filetypes we could check for:
 *	GIF		->	Bring up GIF previewer
 *	Postscript	->	Bring up Postscript previewer
 */

static int ispipe;
#ifdef VMS
static char outfile[256];
#endif

FILE *
specialfile(fp, pathname)
  FILE *fp;
  char *pathname;
{
     FILE *pp;
     char buf[256], s[256];
#ifdef VMS
     char *cp;
     char *Validate_Filespec(char *);
     int status;
#define system(a) vms_system(a)
#endif
     long i;

     ispipe = 0;

     /* Keep track of where we are */
     i = ftell(fp);
     rewind(fp);
     
     /* Grab the first three bytes, and rewind */
     if (fgets(s, 255, fp) == NULL) {
	  fseek(fp, i, 0);
	  return (FILE *)(ispipe=0);
     }

     fseek(fp, i, 0);

#ifdef GOPHER_ZCOMPRESS
     /* Compressed? */
     if (iscompressed(s)) {
	  if (dochroot)
	       sprintf(buf, "%s \"%s\"\n", ZCATCMD, pathname);
	  else
	       sprintf(buf, "%s \"%s/%s\"\n", ZCATCMD, Data_Dir, pathname);

	  if (! (pp = popen(buf, "r")))
	       return (FILE *)(ispipe=0);
	  ispipe = 1;
	  return pp;
     }
#endif
     /* Script? */
#ifdef VMS
     if (isshellscript(s, pathname)) {
	  strcpy(outfile, pathname);
	  if ((cp=strchr(outfile,' '))!=NULL)
	    *cp = '\0';
	  if (Validate_Filespec(outfile)==NULL) {
	    outfile[0] = '\0';
	    return (FILE *)(ispipe=0);
	  }
     strcpy(outfile, cp=tempnam(GDCgetScratchDir(Config),NULL));
     free(cp);
     sprintf(buf, "$ @%s/output=%s", pathname, outfile);
     if (EXECargs != NULL) {
	strcat(buf, " \"");
	strcat(buf, EXECargs);
	strcat(buf, "\"");
     }
#else
     if (isshellscript(s)) {
	  s[strlen(s)-1] = '\0';
	  if (dochroot)
	       sprintf(buf, "\"%s\" %s\n", pathname, (EXECargs == NULL) ? "" : EXECargs);
	  else
	       sprintf(buf, "\"%s/%s\" %s\n", GDCgetDataDir(Config), 
				pathname, (EXECargs == NULL) ? "" : EXECargs);
#endif
	  if (DEBUG)
	       fprintf(stderr, "Executing %s", buf);
#ifndef VMS
	  if (! (pp = popen(buf, "r")))
	       return (FILE *)(ispipe=0);
	  ispipe = 1;
	  
	  if (DEBUG)
	       fprintf(stderr, "Zcat popen is okay\n");
	  
	  return pp;
#else
	  if (((status=system(buf)) &1) == 1) {
	    pp = fopen(outfile, "r");
	    ispipe = 1;
	    return pp;
	  }
	  else {
	    unlink(outfile);
	    outfile[0] = '\0';
	  }
	  
#endif
     }
     return (FILE *)(ispipe=0);
}


iscompressed(s)
  char *s;
{
     if (s[0] == '\037' && s[1] == '\235')
	  return 1;
     else
	  return 0;
}


#ifdef VMS

isshellscript(char *s, char *path)
{
     if (strncasecmp(path+strlen(path)-strlen(".SCRIPT"),".SCRIPT")!=0)
	return 0;
     if (! strncmp(s, "$!", 2))
#else

isshellscript(s)
  char *s;
{
     if (! strncmp(s, "#!/", 3))
#endif
	  return 1;
     else
	  return 0;
}


int
Specialclose(fp)
  FILE *fp;
{
     if (ispipe)
#ifdef VMS
	{
	  ispipe = fclose(fp);
	  if (strlen(outfile)!=0)
	    unlink(outfile);
	  outfile[0] = '\0';
	  return(ispipe);
	}
#else
	  return(pclose(fp));
#endif
     else
	  return(fclose(fp));
}
