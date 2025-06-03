/* clear.c */
#include <stdio.h>
#ifdef __svr4__
#include <curses.h>
#endif
#ifdef TERMINFO
#include <term.h>
#endif
#undef putchar

int putchar();

#ifdef TERMCAP
static int ok_to_clear;
#endif

#ifdef TERMINFO
static int ok_to_clear;
#endif

#ifdef TERMCAP
static char clear_screen[128] = 0;
static int lines;
#endif

#ifdef MSDOS
/*
 * IF we assume the standard ANSI terminal driver, we can't go TOO far wrong...
 * but allow a bailout.  Making this assumption, we realize that it's a
 * VT-100 sequence, so for this one file...
 */
#ifndef GENERIC_SCR
#define VMS
#endif
#endif

#ifdef VMS
/*
 * VMS is so heavily targeted to VT-100 screens that we can assume...
 */
static char vt100_clear[128] = { '\033','[','H','\033','[','2','J','\0' };
#endif


#ifdef GENERIC_SCR
/* This is pretty generic... */
#define CLR_LINES	30
#endif

clearinit ()
{
#ifdef TERMINFO
  int i;        
  char *getenv();
  char *name = "TERM";
/* setupterm(getenv(name),1,&i); *//* As of at least SVID Issue 2, Vol. 2 */
  i = setterm(getenv(name));
  ok_to_clear = (i == 1) ? 1 : 0;
  if (i != 1) {
     fprintf(stderr,"Warning: Terminal type unknown\n");
  }
  return (i == 1) ? 0 : -1;
#endif
#ifdef TERMCAP
  char tc[1024];
  char *ptr = clear_screen;
  char *getenv();
  char *name = "TERM";
  
  if (tgetent(tc, getenv(name)) < 1) {
    ok_to_clear = 0;
    return;
  }
  tgetstr("cl", &ptr);
  lines = tgetnum("li");
  ok_to_clear = (clear_screen[0] != 0 && lines > 0);

#endif
}        
        
clear_the_screen ()
{
#ifdef TERMINFO
  if (!ok_to_clear) return;        
  tputs(clear_screen,lines,putchar);
  fflush(stdout);
#endif
#ifdef TERMCAP
  if (!ok_to_clear) return;
  tputs(clear_screen,lines,putchar);
  fflush(stdout);
#endif

#ifdef VMS
  fputs(vt100_clear,stdout);
  fflush(stdout);
#endif
#ifdef GENERIC_SCR
  {
	register int index;

	for(index = 0;index < CLR_LINES ;index++)
		putchar('\n');
  }
  fflush(stdout);
#endif
}
