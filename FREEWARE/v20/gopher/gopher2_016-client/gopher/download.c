/********************************************************************
 * lindner
 * 3.19
 * 1994/05/17 05:47:54
 * /home/mudhoney/GopherSrc/CVS/gopher+/gopher/download.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: download.c
 * Functions relating to downloading data
 *********************************************************************
 * Revision History:
 * download.c,v
 * Revision 3.19  1994/05/17  05:47:54  lindner
 * Massive internationalization change
 *
 * Revision 3.18  1994/05/14  04:13:39  lindner
 * Internationalization...
 *
 * Revision 3.17  1994/05/06  02:26:31  lindner
 * Fix for binhex downloads on vms and use dump command for binary files
 *
 * Revision 3.16  1994/04/25  20:48:19  lindner
 * Proper casts
 *
 * Revision 3.15  1994/04/01  04:48:10  lindner
 * Use FIOsystem() for downloading
 *
 * Revision 3.14  1993/11/29  01:10:26  lindner
 * Do not let user attempt to download an 'i' (Info) menu item.  (Beckett)
 *
 * Revision 3.13  1993/10/26  18:44:12  lindner
 * Fix for people trying to download the help file
 *
 * Revision 3.12  1993/10/07  05:11:00  lindner
 * Fixed spawned process error checking on Unix, add third arg to Save_File
 *
 * Revision 3.11  1993/09/21  03:38:50  lindner
 * change getcwd to getwd
 *
 * Revision 3.10  1993/09/03  03:30:40  lindner
 * The test for spawn success should be !system().  Restore initial
 * default directory, and related mods, to circumvent problems in the
 * logic of this module.  These are preliminary mods, simply to make it
 * actually work.  They do not yet cache temporary files under every
 * circumstance in which the 'D' command might be used, but do for the
 * typical patterns of use. (F.Macrides)
 *
 * Revision 3.9  1993/08/16  17:57:58  lindner
 * Fix for sys$scratch for VMS
 *
 * Revision 3.8  1993/08/09  20:39:55  lindner
 * fix for VMS
 *
 * Revision 3.7  1993/07/30  17:36:54  lindner
 * More secure download in /tmp
 *
 * Revision 3.6  1993/07/29  17:21:05  lindner
 * eliminate non-used variables
 *
 * Revision 3.5  1993/07/23  04:36:03  lindner
 * LocalFile mods
 *
 * Revision 3.4  1993/07/20  23:11:25  lindner
 * downloading now caches the downloaded file too
 *
 * Revision 3.3  1993/04/30  16:01:19  lindner
 * kermit binary mods
 *
 * Revision 3.2  1993/03/24  16:57:37  lindner
 * Fixes for new SaveFile()
 *
 * Revision 3.1.1.1  1993/02/11  18:02:57  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.4  1993/01/14  21:59:30  lindner
 * Filenames generated for zmodem now are a bit better.. should work better
 * on VMS
 *
 * Revision 1.3  1993/01/12  20:42:04  lindner
 * Added <stat.h> stuff for VMS, also changed text download for VMS from
 * cat to type
 *
 * Revision 1.2  1993/01/11  19:26:56  lindner
 * Mods to make it work under VMS
 *
 * Revision 1.1  1993/01/07  22:47:20  lindner
 * Initial revision
 *
 *
 *********************************************************************/


#include "gopher.h"
#ifdef VMS
#include <stat.h>
#else
#include <sys/stat.h>
#endif

#include "fileio.h"		/* For FIOsystem() */

static char *DLnames[] = {
     "Zmodem",
     "Ymodem",
     "Xmodem-1K",
     "Xmodem-CRC",
     "Kermit",
     "Text",
     NULL
     };

static char *DLcmds[] = { /* Cmds for ascii files: FILE */
     "sz ",
     "sb ",
     "sx -k ",
     "sx ",
     "kermit -q -s ",
#ifdef VMS
     "type ",
#else
     "cat -v ",
#endif
     NULL
     };

static char *DLcmdB[] = {     /* Cmds for binary files */
     "sz ",
     "sb ",
     "sx -k ",
     "sx ",
     "kermit -q -i -s ",
#ifdef VMS
     "dump ",
#else
     "cat -v ",
#endif
     NULL
     };

void
Download_file(gs)
  GopherObj *gs;
{
     int    choice;
     char   tmpfilename[512], *cp;
     char   command[512];
     char   curcwd[512];
     int    start, end;
     struct stat buf;

     switch (GSgetType(gs)) {
     case A_DIRECTORY:
     case A_CSO:
     case A_ERROR:
     case A_INDEX:
     case A_TELNET:
     case A_TN3270:
     case A_INFO:
	  CursesErrorMsg(Gtxt("Sorry, can't download that!",150));
	  return;
     }

     choice = CURChoice(CursesScreen, GSgetTitle(gs), DLnames, 
			Gtxt("Choose a download method",74), -1);
     
     if (choice == -1)
	  return;
     
     
     /*** Get a reasonable tmp file name ***/
     cp = GSgetPath(gs);
     if (cp != NULL) {
	  if ((cp = strrchr(cp,'/')) != NULL)
	       strcpy(tmpfilename, cp+1);
	  else
	       strcpy(tmpfilename,GSgetTitle(gs));
     } else {
	  strcpy(tmpfilename,GSgetTitle(gs));
     }	  
	  

#ifdef VMS
     VMSfile(tmpfilename);
#else
     UNIXfile(tmpfilename);
#endif

     for (cp=tmpfilename; *cp != '\0'; cp++) {
	  switch (*cp) {
	  case ' ':
	  case '\"':
	  case '\'':
	       *cp = '_';
	  }
     }

     getwd(curcwd);
#ifdef VMS
     if (chdir("SYS$SCRATCH")!=0) {
	  CursesErrorMsg(Gtxt("Can't write to SYS$SCRATCH!",69));
	  return;
     }
#else
     if (chdir("/tmp")!=0) {
	  CursesErrorMsg(Gtxt("Can't write to the /tmp directory!",70));
	  return;
     }
#endif

     /** Make sure we don't overwrite an existing file ... **/
     while (stat(tmpfilename, &buf) == 0) {
	  int len = strlen(tmpfilename);

	  if (tmpfilename[len-1] == '-') {
	       tmpfilename[len] = tmpfilename[len] + 1;
	  } else
	       strcat(tmpfilename, "-1");
     }

     /*** Retrieve the file ***/
     Save_file(gs, tmpfilename, NULL);

     /*** Check to see which method they're using to download ***/
     
     if (stat(tmpfilename, &buf) < 0) {
	  CursesErrorMsg(Gtxt("File didn't transfer successfully",88));
	  return;
     }

     /*** Now start the download ... ***/
     if (GSisText(gs, NULL))
	  strcpy(command, DLcmds[choice]);
     else
	  strcpy(command, DLcmdB[choice]);

     strcat(command, tmpfilename);
     
     CURexit(CursesScreen);

     if (choice == 5) {
	  printf(Gtxt("Start your capture now...\n\n",171));
	  printf(Gtxt("Press <RETURN> when you're ready\n",121));
	  fflush(stdout);
	  getchar();
     } else {
	  printf(Gtxt("Start your download now...\n",172));
	  fflush(stdout);
     }

     start = time(NULL);

     if (FIOsystem(command))
	  printf(Gtxt("\nDownload could not be completed, sorry... \n",183));
     else {
	  end = time(NULL);
	  if (end == start)
	       end++;
     
	  printf(Gtxt("\nDownload Complete. %d total bytes, %d bytes/sec\n",182),
		 (int)buf.st_size, ((int)buf.st_size)/(end-start));
     }

     unlink(tmpfilename);
     chdir(curcwd);
     printf(Gtxt("Press <RETURN> to continue",121));
     fflush(stdout);
     getchar();
     CURenter(CursesScreen);
     
}
