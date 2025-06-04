/*****************************************************************************
 * F.Macrides 20-Jun-1994  Don't quit on attempt to page beyond end of
 *			    document (wait for an explicit quit command).
 ****************************************************************************/

/********************************************************************
  Changes from Alan Coopersmith's patches:
    - make sure A_CHARTEXT is defined before we try to use it
 ********************************************************************/

/********************************************************************
 * lindner
 * 3.24
 * 1994/06/09 22:13:40
 * /home/mudhoney/GopherSrc/CVS/gopher+/gopher/pager.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 92, 93, 94 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: pager.c
 * neato pager..
 *********************************************************************
 * Revision History:
 * pager.c,v
 * Revision 3.24  1994/06/09  22:13:40  lindner
 * More language conversions
 *
 * Revision 3.23  1994/06/09  16:35:57  lindner
 * Fix typos
 *
 * Revision 3.22  1994/06/09  04:37:06  lindner
 * (F.Macrides) Fixed PagerNextPage() and PagerSearch() to not add extra
 * blank lines following lines that are exactly equal to the screen width
 * on VMS.  Added code to disallow 'm'ail if SecureMode or NoShellMode
 * and the NOMAIL symbol was defined on compilation.
 *
 * Revision 3.21  1994/06/03  06:12:19  lindner
 * Fix for redisplay after using help
 *
 * Revision 3.20  1994/05/19  14:08:05  lindner
 * use fast malloc on VMS VAXC
 *
 * Revision 3.19  1994/05/18  03:59:41  lindner
 * Change to FIOsystem() for VMS
 *
 * Revision 3.18  1994/05/17  05:48:06  lindner
 * Massive internationalization change
 *
 * Revision 3.17  1994/05/14  04:13:47  lindner
 * Internationalization...
 *
 *
 *********************************************************************/

/** Note, uses Global "Searchstring" **/

#include "gopher.h"
#include "CURcurses.h"
#include "fileio.h"
#include "Malloc.h"

#ifdef VMS
#include <stat.h>
#else
#include <sys/stat.h>
#endif


/** Argv/argc for searched words.. **/
static char *words[50];   
static int  wordcount = 0;

/** Optional search string **/
static char slashstring[128];

/** bytecount positions for each page..  (non-dynamic.. yet...) **/
static int  positions[4000];  /** Bad programmer **/
static int  currentpage;


/*
 * This is the beginning of a built in pager..  
 * it is very pretty..
 *
 */

void
PagerHelp(cur)
  CursesObj *cur;
{
     static char **helpmenu = NULL;

     if (helpmenu == NULL) {
	  helpmenu = (char**) malloc(sizeof(char*) * 9);
	  helpmenu[0] = Gtxt("u, ^G, left : Return to menu",19);
	  helpmenu[1] = Gtxt("space, down : Move to the next page",20);
	  helpmenu[2] = Gtxt("b, up       : Move to the previous page",21);
	  helpmenu[3] = Gtxt("/           : Search for text",22);
	  helpmenu[4] = Gtxt("m           : mail current document",23);
	  helpmenu[5] = Gtxt("s           : save current document",24);
	  helpmenu[6] = Gtxt("p           : print current document",25);
	  helpmenu[7] = Gtxt("D           : download current document",26);
	  helpmenu[8] = NULL;
     }
     
     CURDialog(cur, Gtxt("Pager Help",18), helpmenu);
}

void
PagerInitGlobals()
{
     currentpage = 0;
     wordcount = 0;
     slashstring[0] = '\0';
}


void
PagerTitles(cur, gs, totalbytes)
  CursesObj *cur;
  GopherObj *gs;
  int totalbytes;
{
     int i;

     /* Okay, first draw the top... ***/
     
     move(0,0);
     wboldout(stdscr);
     waddstr(stdscr, GSgetTitle(gs));
     wboldend(stdscr);

     wprintw(stdscr, " (%dk)", totalbytes/1024);


     wmove(stdscr, 1,0);
     waddch(stdscr, CURgetBox_ul(cur));

     for (i=0; i<COLS-2; i++)
	  waddch(stdscr, CURgetBox_hline(cur));

     waddch(stdscr, CURgetBox_ur(cur));


     /*  And now the bottom **/

     wmove(stdscr, LINES-2, 0);
     waddch(stdscr, CURgetBox_ll(cur));
     for (i=0; i<COLS-2; i++)
	  waddch(stdscr, CURgetBox_hline(cur));
     waddch(stdscr, CURgetBox_lr(cur));

     /****/
     wmove(stdscr, LINES-1,0);
     CURbutton(cur, stdscr, Gtxt("PageDown: <SPACE>",115), FALSE);
     waddch(stdscr, ' ');
     CURbutton(cur, stdscr, Gtxt("Help: ?",96), FALSE);
     waddch(stdscr, ' ');
     CURbutton(cur, stdscr, Gtxt("Return to Menu: u",134), FALSE);
}


void
PagerPercent(bytes, totalbytes)
  int bytes;
  int totalbytes;
{
     int per = (totalbytes? ((100 * bytes)/totalbytes) : 0);
     
     wmove(stdscr, 0, COLS-5);
     if (totalbytes != 0)
	  wprintw(stdscr, "%d%%", per);
     if (per < 100)
	  waddch(stdscr, ' ');
     if (per < 10)
	  waddch(stdscr, ' ');
}

/*
 * Turn the search terms into a argv style thing..
 */

void
PagerParseSearchstring()
{
     char *MungeSearchstr = Searchstring;
     int  numchars;
     char theline[256];

     wordcount = 0;

     if (Searchstring == NULL)
	  return;

     while (isspace(*MungeSearchstr)) /** Strip off spaces **/
	  MungeSearchstr++;
	  
     for (wordcount=0; wordcount<40; wordcount++) {
	  
	  while (isspace(*MungeSearchstr)) /** Strip off spaces **/
	       MungeSearchstr++;
	  
	  numchars = sreadword(MungeSearchstr, theline, 40);
	  MungeSearchstr += numchars;
	  
	  if (numchars == 0)
	       break;
	  
	  if (strcasecmp(theline, "and")==0 ||
	      strcasecmp(theline, "or")==0 ||
	      strcasecmp(theline, "not")==0) {
	       wordcount--;
	  } else {
	       if (words[wordcount] != NULL)
		    free(words[wordcount]);
	       
	       words[wordcount] = strdup(theline);
	  }
     }
}



void
PagerPrintLine(inputline)
  char *inputline;
{
     int lowwordnum = -1, i;
     char *cp, *lowword;
     int wlen;

     /** Just print it if no search terms **/
     if ((wordcount  == 0 && slashstring[0] == '\0') ||
	 RCsearchBolding(GlobalRC) == FALSE) {
	  waddstr(stdscr, inputline);
	  return;
     }

     /** Find the first word in the line **/

     while (*inputline!='\0') {
	  lowword = NULL;

	  if (slashstring[0] != '\0')
	       cp = strcasestr(inputline, slashstring);
	  else 
	       cp = NULL;

	  if (cp != NULL) {
	       lowword    = cp;
	       lowwordnum = -1;
	  }

	  for (i=0; i< wordcount; i++) {
	       cp = strcasestr(inputline, words[i]);
	       if (cp != NULL)
		    if (cp < lowword || lowword == NULL) {
			 lowword = cp;
			 lowwordnum = i;
		    }
	  }

	  if (lowword == NULL) {
	       /** No search terms, spit it out **/
	       waddstr(stdscr, inputline);
	       return;
	  }
	  else {
	       /** add non bolded stuff **/
	       for (cp = inputline; cp < lowword; cp++)
#if defined(GINTERNATIONAL) && defined(A_CHARTEXT)
		    waddch(stdscr, *cp & A_CHARTEXT);
#else    
	       waddch(stdscr, *cp);
#endif
	       
	       inputline = lowword;
	       
	       /*** Bolded stuff ***/
	       standout();
	       if (lowwordnum == -1)
		    wlen = strlen(slashstring);
	       else
		    wlen = strlen(words[lowwordnum]);

	       for (cp = inputline; cp < inputline + wlen; cp++)
		    waddch(stdscr, *cp);
	       inputline += wlen;

	       standend();
	  }
     }
}


void
PagerSeekPage(thefile, gotopage, bytecount)
  FILE *thefile;
  int  gotopage;
  int  *bytecount;
{
     if (gotopage < currentpage) {
	  fseek(thefile, positions[gotopage], 0);
	  *bytecount = positions[gotopage];
	  currentpage = gotopage;
     }
}


void
PagerNextPage(cur, thefile, theline, bytecount, totalbytes)
  CursesObj *cur;
  FILE      *thefile;
  char      *theline;
  int       *bytecount;
  int       totalbytes;
{
     int i;
     char *cp = NULL;
#ifdef VMS
     boolean check_next_line = FALSE;
#endif

     /** Save current bytecount position **/
     positions[currentpage++] = *bytecount;

     wmove(stdscr, 2, 0);

     /* Display a screen of text */
     for (i=0; i<LINES-4; i++) {
     read_again:
	  cp = fgets(theline, COLS+1, thefile);
	  if (cp == NULL)
	       break;
#if defined(VMS) && defined(VMSRecords)
	  *bytecount = ftell(thefile);
 	  if (check_next_line == TRUE) {
 	      check_next_line = FALSE;
 	      if (strlen(cp) == 1 && cp[0] == '\n')
 	          goto read_again;
 	  }
 	  if (strlen(cp) == COLS && cp[COLS-1] != '\n')
 	      check_next_line = TRUE;

#else
	  *bytecount += strlen(theline);
#endif

	  ZapCRLF(theline);
	  /** Bold the terms... **/

	  wmove(stdscr, i+2, 0);
	  wclrtoeol(stdscr);
	  PagerPrintLine(theline);
     }
     while (i<LINES-4) {
	  wmove(stdscr, i+2, 0);
	  wclrtoeol(stdscr);
	  i++;
     }

     /*** Display buttons ***/
     wmove(stdscr, LINES - 1, 0);
     wclrtoeol(stdscr);
     CURbutton(cur, stdscr, Gtxt("Help: ?",96), FALSE);
     waddstr(stdscr, "  ");
     CURbutton(cur, stdscr, Gtxt("Exit: u",86), FALSE);
     waddstr(stdscr, "  ");
     if (*bytecount != totalbytes) {
          CURbutton(cur, stdscr, Gtxt("PageDown: Space",116), FALSE);
          waddstr(stdscr, "  ");
     }
     if (currentpage > 1)
          CURbutton(cur, stdscr, Gtxt("PageUp: b",117), FALSE);
}

/*
 * Seek to the page with the specific search term...
 */

boolean
PagerSearch(thefile, theline, bytecount, search)
  FILE *thefile;
  char *theline;
  int  *bytecount;
  char *search;
{
     int i;
     char *cp = NULL;
#ifdef VMS
     boolean check_next_line = FALSE;
#endif

     
     do {
	  /** Save current bytecount position **/
	  positions[currentpage++] = *bytecount;

	  
	  for (i=0; i<LINES-4; i++) {
	  search_again:
	       cp = fgets(theline, COLS+1, thefile);
	       if (cp == NULL)
		    break;
	       
#if defined(VMS) && defined(VMSRecords)
	       *bytecount = ftell(thefile);
	       if (check_next_line == TRUE) {
	           check_next_line = FALSE;
		   if (strlen(cp) == 1 && cp[0] == '\n')
		       goto search_again;
	       }
	       if (strlen(cp) == COLS && cp[COLS-1] != '\n')
	           check_next_line = TRUE;
#else
	       *bytecount += strlen(theline);
#endif
	       
	       if (strcasestr(cp, search) != NULL)
		    return(TRUE);
	  }
     } while (cp != NULL);

     return(FALSE);
}

void
PagerBuiltin(cur, gs)
  CursesObj *cur;
  GopherObj *gs;
{
     int         ch;
     FILE        *thefile;
     char        *theline;
     char        *Dialogmess[3];
     char        command[MAXSTR];
     boolean     done = FALSE;
     int         bytecount = 0;
     int         totalbytes = 0;
     int         savedpagenum;
     struct stat buf;

     PagerInitGlobals();
     
     /*** Initialize an array for the screen ... ***/
     theline = (char *) malloc(sizeof(char)*(COLS+1));
     *theline = '\0';

     stat(GSgetLocalFile(gs), &buf);
     totalbytes = buf.st_size;
     
     PagerParseSearchstring();

     clear();
     PagerTitles(cur, gs, totalbytes);
     refresh();

     thefile = fopen(GSgetLocalFile(gs), "r");

     if (thefile == NULL) {
	  CursesErrorMsg(Gtxt("Cannot Open requested file..",71));
	  return;
     }

     PagerNextPage(cur, thefile, theline, &bytecount, totalbytes);

     refresh();

     /* Get a keystroke */
     while (done==FALSE) {
	  PagerPercent(bytecount, totalbytes);
	  refresh();

	  ch = CURgetch(cur);

	  switch (ch) {
#ifdef VMS
	  case '\032': /* ^Z */
#endif
	  case KEY_LEFT:
	  case 'u':
	  case 'q':
	  case '\007':
	       done = TRUE;
	       break;

          case '\006':
	  case '\n':
	  case ' ':
	  case KEY_DOWN:
	  case KEY_NPAGE:
	       if (bytecount < totalbytes)
	            PagerNextPage(cur, thefile, theline, &bytecount, 
                         totalbytes);
	       break;

          case '\002':
	  case KEY_UP:
	  case KEY_PPAGE:
	  case 'b':
	       if (currentpage > 1) {
		    PagerSeekPage(thefile, currentpage-2, &bytecount);
		    PagerNextPage(cur, thefile, theline, &bytecount, 
                         totalbytes);
	       } 
               break;

          case '^':
               if (currentpage > 1) {
                    PagerSeekPage(thefile, 0, &bytecount);
                    PagerNextPage(cur, thefile, theline, &bytecount, 
                         totalbytes);
               }
               break;

          case '$':
               if (bytecount < totalbytes)
                    do
                         PagerNextPage(cur, thefile, theline, &bytecount,
                              totalbytes);
                    while (bytecount < totalbytes);

               break;

          case '\037':
	  case KEY_HELP:
          case 'h':
	  case '?':
	       PagerHelp(cur);
	       touchwin(stdscr);
	       break;

	  case 'p':
	       if (SecureMode || NoShellMode) {
		    CursesErrorMsg(Gtxt("Sorry, you are not allowed to print files",161));
		    touchwin(stdscr);
		    break;
	       }
	       if (!RCprintCommand(GlobalRC, GSgetLocalView(gs), 
		   GSgetLocalFile(gs), command) || 
		   !strncasecmp(command, "- none -", 8) || 
		   strlen(command) == 0) {
		    CursesErrorMsg(Gtxt("Sorry, no method to print this document",156));
		    touchwin(stdscr);
		    break;
	       }
	       Dialogmess[0] = strdup(Gtxt("The filename is:",174));
	       Dialogmess[1] = strdup(GSgetLocalFile(gs));
	       Dialogmess[2] = NULL;

	       if (CURDialog(cur, Gtxt("Print current document",122), Dialogmess) != -1) {
		    if (FIOsystem(command))
			CursesErrorMsg(Gtxt("Encountered printing problem, sorry...",84));
	       }

	       free(Dialogmess[0]);
	       free(Dialogmess[1]);

	       touchwin(stdscr);
	       clearok(curscr, TRUE);

	       break;

	  case 's':
	       if (!(SecureMode || NoShellMode))
		    Save_file(gs, NULL, GSgetLocalView(gs));
	       else
		    CursesErrorMsg(Gtxt("Sorry, you are not allowed to save files",162));

	       touchwin(stdscr);
	       break;

	  case 'D':
	       Download_file(gs);
	       wclear(stdscr);
	       PagerTitles(cur, gs, totalbytes);
	       PagerSeekPage(thefile, currentpage-1, &bytecount);
	       PagerNextPage(cur, thefile, theline, &bytecount, totalbytes);

	       break;

	  case 'm':
#ifdef NOMAIL
	       if (SecureMode || NoShellMode) {
		    CursesErrorMsg(Gtxt("Sorry, you are not allowed to do this", 64));
		    touchwin(stdscr);
		    break;
	       }
#endif

	       GSmail(gs);
	       wclear(stdscr);
	       PagerTitles(cur, gs, totalbytes);
	       PagerSeekPage(thefile, currentpage-1, &bytecount);
	       PagerNextPage(cur, thefile, theline, &bytecount, totalbytes);
	       break;

	  case '/':
	       /** Search ... ***/
	       if ((CURGetOneOption(CursesScreen, "",
				   Gtxt("Search text for:",141), 
				    slashstring) < 0) ||
				   (slashstring[0] == '\0')) {
		    touchwin(stdscr);
		    break;
	       }

	  case 'n':
	       /** Next occurrence, and fall through from above.. **/
	       if (slashstring[0] == '\0') {
		    CursesErrorMsg(Gtxt("Use '/' to define search a first...",178));
		    touchwin(stdscr);
                    break;
	       }

	       savedpagenum = currentpage;

	       if (ch == '/')
		    PagerSeekPage(thefile, currentpage-1, &bytecount);

	       if (!PagerSearch(thefile, theline, &bytecount, slashstring)) {
		    CursesErrorMsg(Gtxt("Couldn't find text",82));
		    touchwin(stdscr);
		    PagerSeekPage(thefile, savedpagenum-1, &bytecount);
		    PagerNextPage(cur, thefile, theline, &bytecount, 
                         totalbytes);
		    touchwin(stdscr);
		    break;
	       }

	       /* We found it... */
	       PagerSeekPage(thefile, currentpage-1, &bytecount);
	       PagerNextPage(cur, thefile, theline, &bytecount, totalbytes);
	       touchwin(stdscr);
	       break;

	  default:
	       CURBeep(cur);
	       break;
	  }
	  
     }


     /*** Clean up.. ***/

     fclose(thefile);
     free(theline);
}


