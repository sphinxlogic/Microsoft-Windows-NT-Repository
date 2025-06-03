/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#ifdef unix
# ifdef SYSV
#   include <sys/types.h>
#   include <fcntl.h>
# endif
# include <sys/file.h>
#endif
#ifdef msdos
# include <dir.h>
# include <io.h>
  extern unsigned _stklen = 10000U;
#endif

#ifndef O_RDWR
#ifndef VMS
# include <fcntl.h>
#endif
#endif

#include "config.h"
#include "file.h"
#include "buffer.h"
#include "display.h"
#include "sysdep.h"
#include "sig.h"
#include "misc.h"
#include "keymap.h"
#include "screen.h"
#include "ledit.h"
#include "slang.h"
#include "search.h"
#include "text.h"
#include "hooks.h"

char Jed_Root_Dir[256];

/* a hook to parse some command line args. */
int (*X_Argc_Argv_Hook)(int, char **);

typedef struct
{
   jmp_buf b;
} jmp_buf_struct;

extern jmp_buf_struct Jump_Buffer, *Jump_Buffer_Ptr;

int Batch = 0;

char *get_cwd(void)
{
   static char cwd[256];

#if defined (sequent)
   if(!getwd(cwd)) return "";
#else 
#if defined (__EMX__)
   _getcwd2(cwd, 254);		       /* includes drive specifier */
#else
   getcwd(cwd, 254);		       /* djggp includes drive specifier */
#endif
#endif
#ifndef VMS
   slash2slash(cwd);
   fixup_dir(cwd);
#endif
   return(cwd);
}

#if 0
void load_init_file(void)
{
   FILE *fp;
#ifdef unix
   char *file = ".jedrc";
#else
   char *file = "jed.rc";
#endif
   char initfile[256], *home;

   home = getenv("JED_HOME");
#ifndef VMS
   if ((home != NULL) || (NULL != (home = getenv("HOME"))))
     {
	strcpy(initfile, home);
	fixup_dir(initfile);
     }
   else initfile[0] = 0;
#else
   if (home == NULL) home = "SYS$LOGIN:";
   strcpy(initfile, home);
#endif

   strcat(initfile, file);
   if ((fp = fopen(initfile,"r")) == NULL) return;
   fclose(fp);
   SLang_load_file(initfile);
}
#endif 

void (*X_Init_Global_Structures_Hook)(void);

int Main_Argc;
char **Main_Argv;

int main(int argc, char **argv)
{
   char *cmd_hook = "command_line_hook", *jl, *jr;
   int i, fd;
   
#ifdef __EMX__
   _response(&argc, &argv);
   _wildcard(&argc, &argv);
#endif

#ifdef msdos
   define_word("0-9a-zA-Z\200-\232\240-\245\341-\353");
#else
   define_word("0-9a-zA-Z\277-\326\330-\336\340-\366\370-\376");
#endif

   tt_get_terminfo();

   /* If this hook is defined, let it peel off what ever arguments
      it wants.  It should return the number of remaining arguments */
   if (X_Argc_Argv_Hook != NULL) 
     {
	i = (*X_Argc_Argv_Hook)(argc, argv) - 1;
	argv[i] = argv[0];
	argc -= i;
	argv += i;
     }
   
   if (argc > 1)
     {
	if (!strcmp(argv[1], "-batch"))
	  {
	     Batch = 1;
	     SLang_Traceback = 1;
	  }
	else if (!strcmp(argv[1], "-script"))
	  {
	     Batch = 2;
	     SLang_Traceback = 1;
	  }
     }
   

#ifdef unix
#ifdef __GO32__
   fd = 2;
#else
   if (Batch) fd = 2;
   else 
     {
	if ((fd = open("/dev/tty", O_RDWR)) >= 0)
	  {
	     dup2(fd, 2);  /* tty uses 2 as the descriptor */
	  }
	else fd = 2;
	  /* exit_error("Unable to open /dev/tty.", 0); */
   
     }
#endif
#else
   fd = 2;			       /* 2 is stderr, assume it is ok */
#endif

   
   init_tty();
   
   init_display(1);         /* sets up virtual screen */

   Jump_Buffer_Ptr = &Jump_Buffer;
   
   /* incase something goes wrong before we even get started... */
   if (setjmp(Jump_Buffer_Ptr->b) != 0)
     {
	/* hmm.... just exit I guess */
	exit_error("main: Fatal Error", 0);
	return -1;
     }


   /* This order here is crucial! */

   init_keymaps();
   

   if (NULL == (CBuf = make_buffer()))
     {
	exit_error("main: Allocation Failure", 0);
     }
   CLine = NULL;
   strcpy(CBuf->name, "*scratch*");
   *CBuf->file = 0;
   
   strcpy(CBuf->dir, get_cwd());
   
   init_minibuffer();

   set_file_modes();
   
   /* what if someone pipes something to jed, allow it */
   
   if (!isatty(0))   /* 1 if stdin is a terminal, 0 otherwise */
     {
	set_buffer("*stdin*");
	read_file_pointer(fileno(stdin));
	bob();
	fclose(stdin);
	dup2(fd, 0);	
     }
   
   if (CLine == NULL) make_line(25);
   Point = 0;
   
   window_buffer(CBuf);
   if (!Batch) tt_cls();
#ifndef VMS
#ifndef msdos
#if !defined (__GO32__) && !defined (__os2__)
   init_signals();
#endif
#endif
#endif

#ifdef VMS
   jr = "JED_ROOT:";
#else
   jr = (char *) getenv("JED_ROOT");
#endif
   
#ifdef JED_ROOT
   if ((jr == NULL) && (file_status(JED_ROOT) == 2))
     {
	jr = JED_ROOT;
     }
#endif

   if (jr != NULL) 
     {
	strcpy(Jed_Root_Dir, jr);
	strcpy(Jed_Library, jr);
#ifndef VMS
   	fixup_dir(Jed_Library);
	strcat(Jed_Library, "lib");
#else
	strcat(Jed_Library, "[lib]");
#endif
     }
   
   jl = (char *) getenv("JED_LIBRARY");
	
   if (jl == NULL)
     {
	if (*Jed_Library == 0)
	  {
	     jl = extract_file(argv[0]);
	     i = (int) (jl - argv[0]);
	     strncpy(Jed_Library, argv[0], i);
	     Jed_Library[i] = 0;
	  }
     }
   else strcpy(Jed_Library, jl);

   
   SLang_load_file("site");
   
   /* set up command line args */
   Main_Argc = argc;
   Main_Argv = argv;
   

   if (SLang_is_defined(cmd_hook))
     {
	SLang_run_hooks(cmd_hook, NULL, NULL);
     }
   else if (!SLang_Error && (argc > 1 + Batch))
     {
	find_file_in_window(argv[1 + Batch]);
     }
   
	

   /* after we have possible loaded key definitions, we can fix up
      the minibuffer map. This way user definitions are used. */

   if (NULL == (Mini_Map = SLang_create_keymap("Mini_Map", Global_Map))) exit_error("main: malloc error", 0);
   SLang_undefine_key("^M", Mini_Map);
   SLang_define_key1("^M", (VOID *) exit_minibuffer, SLKEY_F_INTRINSIC, Mini_Map);
   SLang_define_key1("^I", (VOID *) mini_complete, SLKEY_F_INTRINSIC, Mini_Map);
   SLang_define_key1(" ", (VOID *) mini_complete, SLKEY_F_INTRINSIC, Mini_Map);
   The_MiniBuffer->keymap = Mini_Map;

   /* edit_loop */
   if (!Batch) jed();  /* never returns */
   exit_error ("Batch End with no exit", 0);
   return(-1);
}
