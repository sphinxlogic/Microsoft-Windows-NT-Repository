/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#if !defined (msdos) && !defined (__os2__)
#include <signal.h>
#ifndef VMS
#include <sys/types.h>
#endif

#include "config.h"
#include "sig.h"
#include "file.h"
#include "sysdep.h"
#include "slang.h"
#include "misc.h"
#include "screen.h"
#include "cmds.h"
#include "hooks.h"

int last_signal_code;
int signal_in_progress = 0;

#ifndef VMS
#ifdef SIGWINCH
void resize_display(int sig)
{
   int r, c;
   (void) sig;
   get_term_dimensions(&c, &r);
   if (r > MAX_SCREEN_SIZE) r = MAX_SCREEN_SIZE;
   change_screen_size(c, r);
   signal (SIGWINCH, resize_display);
}
#endif
#endif

void my_signal(int sig)
{
    last_signal_code = sig;
    /* set signal to point to default */
    signal(sig, SIG_DFL);

    if (signal_in_progress)
      kill(getpid(), last_signal_code);
   
   if (signal_in_progress) abort();

    signal_in_progress = 1;

    auto_save_all();
    reset_display();
    reset_tty();
    kill (getpid (), last_signal_code);
}

/* a control-G puts us here */
void my_interrupt(int sig)
{
   SLKeyBoard_Quit = 1;
   if (Ignore_User_Abort == 0) SLang_Error = 2;
   signal (SIGINT, my_interrupt);
}

#ifdef SIGTSTP
int Signal_Sys_Spawn_Flag = 0;
/* This should only be called from outside disturbance */
void sig_sys_spawn_cmd(int sig)
{
   Signal_Sys_Spawn_Flag = 1;
   sys_spawn_cmd();
   update(NULL, 1, 0);		       /* force update */
   Signal_Sys_Spawn_Flag = 0;
}
#endif

void init_signals (void)
{
#ifndef VMS
#ifdef SIGWINCH
   if (X_Get_Term_Size_Hook == NULL) (void) signal(SIGWINCH,resize_display);
#endif
#endif
   signal (SIGINT, my_interrupt);
   signal (SIGHUP, my_signal);
   signal (SIGQUIT, my_signal);
   signal (SIGILL, my_signal);
   signal (SIGTRAP, my_signal);
#if 0
   signal (SIGIOT, my_signal);  /* used by abort */
#endif
   signal (SIGFPE, my_signal);
#ifdef SIGBUS
   signal (SIGBUS, my_signal);
#endif
   signal (SIGSEGV, my_signal);
#ifdef SIGSYS
    signal (SIGSYS, my_signal);
#endif
    signal (SIGTERM, my_signal);
#ifdef SIGTSTP
   signal (SIGTSTP, sig_sys_spawn_cmd);
#endif
}
#endif
