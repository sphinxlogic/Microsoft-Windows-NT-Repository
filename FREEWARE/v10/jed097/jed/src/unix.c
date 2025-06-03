/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#include <stdio.h>
#include "config.h"
#include "sysdep.h"

#include <signal.h>
/* sequent support thanks to Kenneth Lorber <keni@oasys.dt.navy.mil> */
/* SYSV (SYSV ISC R3.2 v3.0) provided by iain.lea@erlm.siemens.de */

#include <sys/time.h>
#include <sys/ioctl.h>
#ifndef sequent
# include <termios.h>
#endif

#ifdef SYSV
# ifndef CRAY
#   include <sys/termio.h>
#   include <sys/stream.h>
#   include <sys/ptem.h>
#   include <sys/tty.h>
# endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define TTY_DESC 2
static int Read_FD = TTY_DESC;
int Flow_Control;
int Abort_Char = 7;		       /* scan code for G (control) */


#ifdef sequent
struct ttystuff
  {
      struct tchars t;
      struct ltchars lt;
      struct sgttyb s;
  };
struct ttystuff OLDTTY;
#else
struct termios OLDTTY;
#endif

     /* this next works on ultrix for setting termios */
#ifdef TCGETS
#define GET_TERMIOS(fd, x) ioctl(fd, TCGETS, x)
#define SET_TERMIOS(fd, x) ioctl(fd, TCSETS, x)
#else
# ifdef sequent
#  define X(x,m)  &(((struct ttystuff*)(x))->m)
#  define GET_TERMIOS(fd, x)	\
	if(ioctl(fd, TIOCGETC, X(x,t))<0 || \
	ioctl(fd, TIOCGLTC, X(x,lt))<0 || \
	ioctl(fd, TIOCGETP, X(x,s))<0)exit_error("Can't get terminal info", 0)
#  define SET_TERMIOS(fd, x)	\
	if(ioctl(fd, TIOCSETC, X(x,t))<0 || \
	ioctl(fd, TIOCSLTC, X(x,lt))<0 || \
	ioctl(fd, TIOCSETP, X(x,s))<0)exit_error("Can't set terminal info", 0)
# else
#  define GET_TERMIOS(fd, x) tcgetattr(fd, x)
#  define SET_TERMIOS(fd, x) tcsetattr(fd, TCSAFLUSH, x)
/* #  define SET_TERMIOS(fd, x) tcsetattr(fd, TCSANOW, x) */
# endif
#endif

static int Baud_Rates[20] = 
{
   0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 
   9600, 19200, 38400, 0, 0, 0, 0
};



static int tty_inited = 0;

void init_tty()
{
#ifdef sequent
    struct ttystuff newtty;
#else
    struct termios newtty;
#endif

   if (Batch) return;
   tty_inited = 1;
   if (X_Init_Term_Hook != NULL)
     {
	Read_FD = (*X_Init_Term_Hook) ();
	return;
     }
   
   tt_enable_cursor_keys();
   GET_TERMIOS(Read_FD, &OLDTTY);
   GET_TERMIOS(Read_FD, &newtty);
#ifdef sequent
   newtty.s.sg_flags &= ~(ECHO);
   newtty.s.sg_flags &= ~(CRMOD);
/*   if (Flow_Control == 0) newtty.s.sg_flags &= ~IXON; */
   newtty.t.t_eofc = 1;
   newtty.t.t_intrc = Abort_Char;	/* ^G */
   newtty.t.t_quitc = 255;
   newtty.lt.t_suspc = 255;   /* to ignore ^Z */
   newtty.lt.t_dsuspc = 255;    /* to ignore ^Y */
   newtty.lt.t_lnextc = 255;
   newtty.s.sg_flags |= CBREAK;		/* do I want cbreak or raw????? */
#else
   
   if (*tt_Baud_Rate == 0)
     {
/* Note:  if this generates an compiler error, simply remove 
   the statement */
	*tt_Baud_Rate = cfgetospeed (&newtty);
	
	
	*tt_Baud_Rate = (*tt_Baud_Rate > 0) && (*tt_Baud_Rate < 19) ?
	              Baud_Rates[*tt_Baud_Rate] : -1;
     }
   
   newtty.c_iflag &= ~(ECHO | INLCR | ICRNL);
#ifdef ISTRIP
   newtty.c_iflag &= ~ISTRIP;
#endif
   newtty.c_oflag &= ~(ONLCR | OPOST);	       /* do not map newline to cr/newline on out */
   if (Flow_Control == 0) newtty.c_iflag &= ~IXON;
   newtty.c_cc[VMIN] = 1;
   newtty.c_cc[VTIME] = 0;
   newtty.c_cc[VEOF] = 1;
   newtty.c_lflag = ISIG | NOFLSH;
   newtty.c_cc[VINTR] = Abort_Char;   /* ^G */
   newtty.c_cc[VQUIT] = 255;
   newtty.c_cc[VSUSP] = 255;   /* to ignore ^Z */
#ifdef VSWTCH
   newtty.c_cc[VSWTCH] = 255;   /* to ignore who knows what */
#endif
#endif /*sequent*/
   SET_TERMIOS(Read_FD, &newtty);
}

void reset_tty()
{
   if (Batch) return;
   if (!tty_inited) return;
   if (X_Init_Term_Hook != NULL)
     {
	if (X_Reset_Term_Hook != NULL) (*X_Reset_Term_Hook) ();
	return;
     }
   
   SET_TERMIOS(Read_FD, &OLDTTY);
   /* This statement ensures init_tty will not try to change output_rate 
      (when coming back from suspension) */
   if (*tt_Baud_Rate == 0) *tt_Baud_Rate = -1;
}

unsigned char sys_getkey()
{
   int n = 450;
   int count = 10;
   unsigned char c;
   if (SLKeyBoard_Quit) return((int) Abort_Char);
   /* sleep for 45 second and try again */
   while (!SLKeyBoard_Quit && !sys_input_pending(&n))
     {
	/* update status line incase user is displaying time */
	if (Display_Time)
	  {
	     JWindow->trashed = 1;
	     update((Line *) NULL, 0, 1);
	  }
     }
   if (SLKeyBoard_Quit) return(Abort_Char);

   if (X_Read_Hook != NULL) return (c = X_Read_Hook ());
   
   while (count-- && (read(Read_FD, &c, 1) < 0) && !SLKeyBoard_Quit) sleep(1);
   
   if (count <= 0)
     {
	exit_error ("getkey(): read failed", 0);
     }

   /* only way for keyboard quit to be non zero is if ^G recived and sigint processed */
   if (SLKeyBoard_Quit) c = Abort_Char;
   SLKeyBoard_Quit = 0;
   return(c);
}
  
#ifndef FD_SET
#define FD_SET(fd, tthis) *(tthis) = 1 << fd
#define FD_ZERO(tthis)    *(tthis) = 0
typedef int fd_set;
#endif

static fd_set Read_FD_Set;

static int sys_input_pending(int *tsecs)
{
   struct timeval wait;
   long usecs, secs;
   int ret;

   if (Input_Buffer_Len || Batch) return(Input_Buffer_Len);
   
   
   secs = *tsecs / 10;
   usecs = (*tsecs % 10) * 100000;
   wait.tv_sec = secs;
   wait.tv_usec = usecs;
   
   if (X_Input_Pending_Hook != NULL) 
     {
	if ((*X_Input_Pending_Hook) ()) return 1;
     }
   
   FD_SET(Read_FD, &Read_FD_Set);
   ret = select(Read_FD + 1, &Read_FD_Set, NULL, NULL, &wait);
   if (ret && (X_Input_Pending_Hook != NULL))
     {
	if ((*X_Input_Pending_Hook) ()) return 1;
	/* Nothing there so try to time out again --- too bad select does
	 * not inform of of how far we got. */
	FD_SET(Read_FD, &Read_FD_Set);
	ret = select(Read_FD + 1, &Read_FD_Set, NULL, NULL, &wait);
	/* try again, it could be more bogus Xpackets.  Event driven systems
	 * are not always the way to go. */
	if (ret) return (*X_Input_Pending_Hook) ();
     }
   return (ret);
}

/*  This is to get the size of the terminal  */
void get_term_dimensions(int *cols, int *rows)
{
#ifdef TIOCGWINSZ
   struct winsize wind_struct;

   if (X_Get_Term_Size_Hook == NULL)
     {
	if ((ioctl(2,TIOCGWINSZ,&wind_struct) < 0)
	    && (ioctl(0, TIOCGWINSZ, &wind_struct) < 0)
	    && (ioctl(1, TIOCGWINSZ, &wind_struct) < 0))
	  {
	     *rows = *cols = 0;
	  }
	else
	  {
	     *cols = (int) wind_struct.ws_col;
	     *rows = (int) wind_struct.ws_row;
	  }
     }
   else (*X_Get_Term_Size_Hook)(cols, rows);

   if (*rows <= 0) *rows = *tt_Screen_Rows;
   if (*cols <= 0) *cols = *tt_Screen_Cols;
#else
   if (X_Get_Term_Size_Hook == NULL)
     {
	*rows = *tt_Screen_Rows;
	*cols = *tt_Screen_Cols;
     }
   else (*X_Get_Term_Size_Hook)(cols, rows);
#endif
}

/* returns 0 on failure, 1 on sucess */
int sys_delete_file(char *filename)
{
    return(1 + unlink(filename));
}

int sys_rename(char *from, char *to)
{
    return(rename(from,to));
}

#ifdef SIGTSTP
extern void sig_sys_spawn_cmd(int);
extern int Signal_Sys_Spawn_Flag;      /* used if something else sends stop */
#endif


void sys_suspend(void)
{   
   signal (SIGTSTP, SIG_DFL);
   if (Signal_Sys_Spawn_Flag) kill(0, SIGSTOP); else kill(0, SIGTSTP);
   signal (SIGTSTP, sig_sys_spawn_cmd);   
}


/* ///////////////////////////////////////////////////////////////////
//  Function:   int sys_System(char *command_line);
//
//  Descript:	a shell wrapper included for DOS compatability
//		removes leading '!' and then uses system()
//
//  Returns:	returns error codes as per system()
///////////////////////////////////////////////////////////////////// */

int
sys_System(char *command_line)
{
   while ( *command_line != '\0' && *command_line == ' ' )
     command_line++;			/* start on 1st non-space */

   if ( *command_line == '!' ) {
      command_line++;			/* advance past this '!' */
      while ( *command_line != '\0' && *command_line == ' ' )
      	command_line++;			/* start on next non-space */
   }
   return system( command_line );
}

/* returns 0 if file does not exist, 1 if it is not a dir, 2 if it is */
int sys_chmod(char *file, int what, int *mode, short *uid, short *gid)
{
   struct stat buf;
   int m;

   if (what)
     {
	chmod(file, *mode);
	chown(file, (uid_t) *uid, (uid_t) *gid);
	return(0);
     }

   if (stat(file, &buf) < 0) switch (errno)
     {
	case EACCES: return(-1); /* es = "Access denied."; break; */
	case ENOENT: return(0);  /* ms = "File does not exist."; */
	case ENOTDIR: return(-2); /* es = "Invalid Path."; */
	default: return(-3); /* "stat: unknown error."; break;*/
     }

   m = buf.st_mode;
   *uid = buf.st_uid;
   *gid = buf.st_gid;
   
/* AIX requires this */
#ifdef _S_IFDIR
#ifndef S_IFDIR
#define S_IFDIR _S_IFDIR
#endif
#endif

   *mode = m & 0777;

   if (m & S_IFDIR) return (2);
   return(1);
}

unsigned long sys_file_mod_time(char *file)
{
   struct stat buf;

   if (stat(file, &buf) < 0) return(0);
   return((unsigned long) buf.st_mtime);
}

/* use berkeley interface
 * #include <sys/dir.h>
*/

static char Found_Dir[256];
static char Found_File[256];
static int File_Len;

#ifdef sequent
# include <sys/dir.h>
# define NEED_D_NAMLEN
#else
# include <dirent.h>
#endif

static DIR *Dirp;

/* These routines should be fixed to work with wildcards like the DOS and VMS
 * versions do.  Unfortunately, it is a sad fact of life that Unix does NOT
 * support wildcards.  Strangely enough, most Unix users are totally unaware
 * of this fact.
 * 
 * To add wild card support, I need to use the regular expression package.
 * Specifically, in sys_findfirst, I would need to scan the file spec for 
 * wild cards making the replacements: . --> \., ? -> ., and * -> .* in that
 * order.  I should also quote the special characters ($, etc...) but I will
 * not worry about this.  Finally, I will have to prefix it with '^' since
 * the match will start at the beginning of the string.
 * 
 *    Remark: to implement something like a real wildcard file renaming routine
 *    would requires even more processing.  Consider something like: 
 *       rename *.c~ *.bak
 *    This mean that the first expression (*.c~) would have to be converted to:
 *      \(.*\)\.c~
 *    and the second (*.bak) would be determined to be: \1.bak  where \1 is
 *    the expression matched by the first wildcard.  
 * 
 * After converting the wildcard file name to a regexp file name, I would then
 * have to compile it and save the compiled expression for use in sys_findnext.
 * There, I would use the regexp string matching routine instead of the 
 * strcmp which is now used.
 * 
 * Mainly, the findfirst/findnext are used by the completion routines.  This
 * means that there is an implicit '.*' attached to the end of the filespec.
 * This will have to be dealt with.
 */
int sys_findnext(char *file)
{
#ifdef NEED_D_NAMLEN
#define dirent direct
#endif

   struct dirent *dp;

   while (1)
     {
	if (NULL == (dp = readdir(Dirp))) return(0);
#ifdef NEED_D_NAMLEN
	dp->d_name[dp->d_namlen] = 0;
#endif
	if (!strncmp(Found_File, dp->d_name, File_Len)) break;
     }
   strcpy(file, Found_Dir); strcat(file, dp->d_name);
   if (2 == file_status(file)) strcat(file, "/");
   return(1);
}


int sys_findfirst(char *the_file)
{
   char *f, *file;

   file = expand_filename(the_file);
   f = extract_file(file);

   strcpy (Found_Dir, file);
   strcpy (Found_File, f);
   File_Len = strlen(f);

   Found_Dir[(int) (f - file)] = 0;

   if (Dirp != NULL) closedir(Dirp);

   if (NULL == (Dirp = (DIR *) opendir(Found_Dir))) return(0);
   strcpy(the_file, file);
   return sys_findnext(the_file);
}
