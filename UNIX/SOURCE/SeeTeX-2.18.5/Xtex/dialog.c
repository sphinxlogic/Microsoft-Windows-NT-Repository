/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

/*
 * Print an error message with an optional system error number, and
 * optionally quit.
 *
 */

#include <stdio.h>
#include <varargs.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/ShellP.h>

#include <X11/Xaw/AsciiText.h>

#include "xtex.h"
#include "widgets.h"

#ifdef __STDC__
/*
extern char* XtMalloc(int);
*/
extern char* realloc(char *, int);
#else
extern char* malloc();
extern char* realloc();
#endif

#define ERROR_BUFFER_SIZE 2048
static char errorMessageBuffer[ ERROR_BUFFER_SIZE ];

#if defined(VSPRINTF) || defined( vsprintf ) || defined(sun) || defined(hpux) || defined(ultrix)
#  define VSPRINTF
#else
#  undef  VSPRINTF
#endif

void
  DialogMessage(ipStr, addNewLine)
char *ipStr;
int addNewLine;
{
  /*
   * I had problems dealing with string constants as messages ---
   * one can't add a newline to them. So I malloced space.
   * Shankar 04 / 20 / 1992
   */
  if ( errorText ) {
    char *str;
    int len = strlen(ipStr);
    char *p;
    char *currentString;

    Arg argList[20];
    Cardinal args;

    XawTextPosition start;
    XawTextBlock tblk, empty_tblk;
    
    if ( len <= 0 )
      return;
    
    str = XtMalloc ((len + 2) * sizeof (char));
    strcpy (str, ipStr);
    
    tblk.firstPos = 0;
    tblk.length = len;
    tblk.ptr = str;
    tblk.format = FMT8BIT;
    
    if ( len > 0 && addNewLine && str[len-1] != '\n' ) {
      str[len] = '\n';
      tblk.length = len+1;
      str[tblk.length] = 0;
    }
    
    /* empty text block for deletion */
    empty_tblk.firstPos = 0;
    empty_tblk.length = 0;
    empty_tblk.ptr = "";
    empty_tblk.format = FMT8BIT;

    args = 0;
    XtSetArg(argList[args], XtNstring, &currentString); args++;
    XtGetValues(errorText, argList, args);
    
    start = strlen( currentString );
    
    XawTextDisableRedisplay(errorText); 

    while ( start + len > ERROR_BUFFER_SIZE ) {
      
      /* find first line and delete it */
      
      char *eol = strchr( currentString, '\n');
      
      if ( eol != 0 && eol < &currentString[ERROR_BUFFER_SIZE] ) {
	XawTextReplace( errorText, 0, eol-currentString+1, &empty_tblk );
        start -= eol - currentString + 1;
      }
      else {
	XawTextReplace( errorText, 0, start, &empty_tblk );
	start = 0;
	break;
      }
    }
    
    XawTextReplace( errorText, start, start, &tblk );
    XawTextSetInsertionPoint(errorText, start + len);
    XawTextEnableRedisplay(errorText);

    XtFree (str);
  }
}

static void
  pushClearButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  bzero( errorMessageBuffer , ERROR_BUFFER_SIZE );
  
  XawTextSetInsertionPoint(errorText, 0);
  XawTextEnableRedisplay(errorText);
} 

void
  BuildErrorBox()
{
  Arg argList[20];
  Cardinal args;
  
  if (xtexResources.helpText) {
    strncpy(errorMessageBuffer, xtexResources.helpText, ERROR_BUFFER_SIZE-1);
  }
  
  args = 0;
  
  XtSetArg(argList[args], XtNeditType, (XtArgVal) XawtextEdit); args++;
  XtSetArg(argList[args], XtRAsciiType, XawAsciiString); args++;
  
  XtSetArg(argList[args], XtNstring, (XtArgVal) errorMessageBuffer); args++;
  XtSetArg(argList[args], XtNlength, (XtArgVal) ERROR_BUFFER_SIZE); args++;
  
  errorText = 
    XtCreateManagedWidget("dialogText",
			  asciiTextWidgetClass, topPane,
			  argList, args);
}


#ifdef lint

/* VARARGS3 ARGSUSED */
error(quit, e, fmt) int quit, e; char *fmt; {;}
     
     /* VARARGS1 ARGSUSED */
     panic(fmt) char *fmt; { exit(1); /* NOTREACHED */ }
     
#else lint
     
     extern char *ProgName;
     extern int errno;
     extern char *sys_errlist[];
     extern int sys_nerr;
     
#define ERROR_MSG_LEN 1024
     static char ErrorMsgBuffer[ ERROR_MSG_LEN ];
     
     error(va_alist)
     va_dcl
{
  va_list l;
  int quit, e;
  char *fmt;
  char *p;
  
  p = ErrorMsgBuffer;
  
  va_start(l);
  /* pick up the constant arguments: quit, errno, printf format */
  quit = va_arg(l, int);
  e = va_arg(l, int);
  if (e < 0)
    e = errno;
  
  fmt = va_arg(l, char *);
  
#if defined(VSPRINTF)
  (void) vsprintf(p, fmt, l);
#else
  /*	Does someone know how to do this properly? */
  bzero(p, ERROR_MSG_LEN);
  
  {
    struct _iobuf ungodlyhack;
    ungodlyhack._cnt = ERROR_MSG_LEN;
    ungodlyhack._ptr = p;
    ungodlyhack._base = p;
    ungodlyhack._bufsiz = ERROR_MSG_LEN;
    ungodlyhack._flag = _IOSTRG;
    ungodlyhack._file = 255;
    _doprnt(fmt, l, &ungodlyhack);
  }
#endif
  
  va_end(l);
  
  DialogMessage( ErrorMsgBuffer, 1);
}

panic(va_alist)
     va_dcl
{
  va_list l;
  char *fmt;
  char *p;
  
  p = ErrorMsgBuffer;
  sprintf(p, "panic: ");
  p += strlen(p);
  
  va_start(l);
  /* pick up the constant arguments: quit, errno, printf format */
  fmt = va_arg(l, char *);
  
#if defined(VSPRINTF)
  (void) vsprintf(p, fmt, l);
#else
  /*	Does someone know how to do this properly? */
  
  {
    struct _iobuf ungodlyhack;
    ungodlyhack._cnt = ERROR_MSG_LEN;
    ungodlyhack._ptr = p;
    ungodlyhack._base = p;
    ungodlyhack._bufsiz = ERROR_MSG_LEN;
    ungodlyhack._flag = _IOSTRG;
    ungodlyhack._file = 255;
    _doprnt(fmt, l, &ungodlyhack);
  }
#endif
  
  va_end(l);
  
  DialogMessage(ErrorMsgBuffer, 1);
  exit(1);
}

#endif /* lint */
