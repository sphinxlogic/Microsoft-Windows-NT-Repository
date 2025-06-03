/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

/*  This file is the interface to system specific files */

#ifdef AIX
#define _ALL_SOURCE
#define POSIX
#endif

#ifdef POSIX
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif

#include <stdio.h>
#include <string.h>

#define USING_INPUT_BUFFER
#define DONE_WITH_INPUT_BUFFER

#include "config.h"
#include "buffer.h"
#include "sysdep.h"
#include "display.h"
#include "file.h"
#include "screen.h"
#include "misc.h"
#include "slang.h"
#include "hooks.h"

/* These are hooks for porting to other systems */

int (*X_Read_Hook) (void);
int (*X_Input_Pending_Hook) (void);
void (*X_Get_Term_Size_Hook)(int *, int *);
int (*X_Init_Term_Hook) (void);
void (*X_Reset_Term_Hook) (void);

extern char *get_cwd(void);
static int sys_input_pending(int *);

int Ignore_User_Abort = 1;	       /* Abort char triggers S-Lang error */

unsigned char KeyBoard_Xlate[256] = 
{
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 
 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 
 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 
 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 
 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 
 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 
 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157,
 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202,
 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217,
 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232,
 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247,
 248, 249, 250, 251, 252, 253, 254, 255
};

#if defined   (msdos)
#  define SLASH_CHAR '\\'
#  include	"ibmpc.c"
#else
# if defined (__os2__)
#    define SLASH_CHAR '\\'
#    include "os2.c"
# else
#    if defined (VMS)
#      define SLASH_CHAR ']'
#      include "vms.c"
#    else
#      if defined (__GO32__)
#        define SLASH_CHAR '\\'
#        include "i386.c"
#      else
#        if defined (unix)
#          define SLASH_CHAR '/'
#          include "unix.c"
#        endif
#      endif
#    endif
# endif
#endif

int Input_Buffer_Len = 0;
unsigned char Input_Buffer[MAX_INPUT_BUFFER_LEN];

void map_character(int *fromp, int *top)
{
   int from = *fromp, to = *top;
   if ((from > 255) || (to > 255) || (from < 0) || (to < 0)) return;
   KeyBoard_Xlate[from] = to;
}


/* if input char arrives with hi bit set, it is replaced by 2 characters:
 *   Meta_Char + char with hi bit off.  If Meta_Char is -1, then return 
 * full 8 bits which self inserts */
      
/* By default, 8 bit chars self insert. */	 
int Meta_Char = -1;
#if !defined(pc_system)
int DEC_8Bit_Hack = 64;
#else
int DEC_8Bit_Hack = 0;
#endif

int my_getkey()
{
   char buf[10];
   int i, imax;
   unsigned char ch;
   
   if (Batch)
     {
	fgets(buf, 9 ,stdin);
	return (int) *buf;
     }
   
   if (!Input_Buffer_Len)
     {
	/* if (Batch) ch = (unsigned char) getc(stdin); else ch = sys_getkey(); */
	ch = (unsigned char) KeyBoard_Xlate[sys_getkey()];
	if (ch & 0x80)
	  {
	     i = (ch & 0x7F);
	     if ((i < ' ') && DEC_8Bit_Hack)
	       {
		  i += DEC_8Bit_Hack;
		  ungetkey((int *) &i);
		  ch = 27;
	       }
	     else if (Meta_Char != -1)
	       {
		  ungetkey((int *) &i);
		  ch = Meta_Char;		       /* escape char */
	       }
	  }
	return((int) ch);
     }
   
   ch = Input_Buffer[0];
   if ((ch & 0x80) && ((Meta_Char != -1) || ((ch < 160) && DEC_8Bit_Hack)))
     {
	ch = (ch & 0x7F);
	if ((ch < ' ') && DEC_8Bit_Hack) 
	  {
	     ch += DEC_8Bit_Hack;
	     i = 27;
	  }
	else i = Meta_Char;

	Input_Buffer[0] = ch;
	return ((int) (unsigned int) i);
     }

   USING_INPUT_BUFFER
   
   Input_Buffer_Len--;
   imax = Input_Buffer_Len;
   
   MEMCPY ((char *) Input_Buffer, (char *) (Input_Buffer + 1), imax);

   DONE_WITH_INPUT_BUFFER
   
   return((int) ch);
}


void ungetkey_string(char *s, int n)
{
   /* int i; */
   register unsigned char *bmax, *b, *b1;
   if (n + Input_Buffer_Len > MAX_INPUT_BUFFER_LEN - 3) return;

   USING_INPUT_BUFFER
   
   b = Input_Buffer;
   bmax = b + (Input_Buffer_Len - 1);
   b1 = bmax + n;
   while (bmax >= b) *b1-- = *bmax--;
   bmax = b + n;
   while (b < bmax) *b++ = (unsigned char) *s++;
   Input_Buffer_Len += n;
   
   DONE_WITH_INPUT_BUFFER
}

void buffer_keystring(char *s, int n)
{
   if (n + Input_Buffer_Len > MAX_INPUT_BUFFER_LEN - 3) return;
   
   USING_INPUT_BUFFER
   MEMCPY ((char *) Input_Buffer + Input_Buffer_Len, s, n);
   Input_Buffer_Len += n;
   DONE_WITH_INPUT_BUFFER
}

void ungetkey(int *ci)
{
   char ch;
   ch = (char) *ci;
   ungetkey_string(&ch, 1);
}

int input_pending (int *tsecs)
{
   int n;
   int c;
   
   if (Input_Buffer_Len) return Input_Buffer_Len;
   
   n = sys_input_pending (tsecs);
   if (n)
     {
	c = my_getkey ();
	ungetkey (&c);
     }
   return n;
}


void flush_input()
{
   int quit = SLKeyBoard_Quit;
   Input_Buffer_Len = 0;
   SLKeyBoard_Quit = 0;
#ifdef msdos
   while (input_pending(&Number_Zero)) if (!my_getkey()) my_getkey();
#else
#ifdef __os2__
   sys_flush_input();
#endif
   while (input_pending(&Number_Zero)) my_getkey();
#endif
   SLKeyBoard_Quit = quit;
}

#include <time.h>

unsigned long sys_time(void)
{
   return((unsigned long) time((time_t *) 0));
}

char *get_time()
{
    char *the_time;
#ifndef __GO32__
    time_t clock;

    clock = time((time_t *) 0);
    the_time = (char *) ctime(&clock);
#else
   the_time = djgpp_current_time ();
#endif
   /* returns the form Sun Sep 16 01:03:52 1985\n\0 */
   the_time[24] = '\0';
   return(the_time);
}

char *slash2slash(char *dir)
{
#ifndef VMS
   register char *p = dir, ch;
   
   while ((ch = *p) != 0)
     {
	if ((ch == '/') || (ch == '\\')) *p = SLASH_CHAR;
      	p++;
     }
#endif 
   return(dir);
}

/* given a canonical filename, return pointer to its name */
char *extract_file(char *file)
{
   char *f;
   
   f = file + strlen(file);
   while ( f-- > file) if (*f == SLASH_CHAR) return f + 1;
   return(file);
}


#ifndef VMS
/* this routine returns a Static pointer which is considered volatile */
char *expand_filename(char *file)
{
   register char *p;
   char  *last, *p1;
#if defined (pc_system)
   static char work[256];
#else
   static char work[500];
#endif

   *work = 0;
   /* the following combinations indicate non-relative path names:
    *	"//"	path from the root dir
    *	"~/"	path from the $HOME dir
    * for dos, os2 only
    *   "x:/"	path from "x:/" dir
    *   "x:"	same as "x:/"
    */
   p = slash2slash(file) + strlen(file);
   while (p > file) {
      if ( *p == SLASH_CHAR ) {
	 if ( *(p-1) == SLASH_CHAR ) {   /* "//" combination */
	    strcpy(work, p);
	    file = work;
	    break;
	 } else if ( *(p-1) == '~' ) { /* "~/" combination */
	    if ( (p1 = getenv("HOME")) == NULL) p1 = "/";
	    strcpy( work, p1 );
	    p1 = slash2slash(work) + strlen(work);
	    if ( *(--p1) == SLASH_CHAR ) *p1 = '\0';
	    strcat( work, p );
	    for (file = work; *file & (*file != SLASH_CHAR); file++ ); /* nil */
	    break;
	 }
#if defined (pc_system)		       /* DOS, OS/2 stuff */
      } else if ( *p == ':' ) {	       /* "c:" or "c:/" combination */
	 strcpy( work, (p-1) );
	 file = (work+2);	       /* start file past the drive spec */
	 p++;
	 if ( *p != SLASH_CHAR ) {	       /* "c:" combination */
	    strcpy( file, "\\");
	    strcat( work, p );
	 }
	 break;
#endif /* pc_system */
      }
      p--;
   }
   
   if ( *work == '\0' ) {	       /* no special combinations */
      if ( *file != SLASH_CHAR ) {
	 strcpy(work, get_cwd());      /* assume relative dir */
	 slash2slash(work);
      }
      strcat(work, file);
      file = work;
   }

   /* remove ../ and ./ stuff */

   p = last = file;
   while (*p != 0) 
     {
	if ( *p == SLASH_CHAR ) last = p;
	else if ( *p == '.' ) 
	  {
	     p++;
	     if ( *p == '.' ) 
	       {
		  p++;
		  if ( *p == SLASH_CHAR ) 
		    {
		       while ((last > file) && (*(--last) != SLASH_CHAR))
		       	 ;/* find parent dir */
	       
		       p1 = last;
		       while ( *p )  *(p1++) = *(p++);
		       *p1 = 0;
		       p = last;
		    }
	       } 
	     else if ( *p == SLASH_CHAR )  
	       {
		  p1 = last;
		  while ( *p )  *(p1++) = *(p++);
		  *p1 = 0;
		  p = last;
	       }
	     else if (*p == 0) break;
	  }
	p++;
     }
   
   return(work);
}

#endif /* ! VMS */

#ifdef sequent
char *my_strstr(char *a, char *b)
{
   register char *bb, *aa, *amax;
   
   if (*b == 0) return(a);
   
   bb = b; while (*bb) bb++;
   aa = a; while (*aa++);
   
   amax = aa - (bb - b);
   
   while (a < amax)
     {
	bb = b;
	while ((a < amax) && (*a != *bb)) a++;
	if (a == amax) return((char *) NULL);
	
	aa = a;
	while (*aa && (*aa == *bb)) aa++, bb++;
	if (! *bb) return(a);
   
	a++;
     }
   return((char *) NULL);
}

#endif


void deslash(char *dir)
{
#ifndef VMS
   int n;

   if ((n = strlen(dir)) > 1) {
      n--;
#if defined (pc_system)
      if ( (dir[n] == '\\' || dir[n] == '/') && dir[n - 1] != ':' ) 
      	dir[n] = '\0';
#else
      if ( dir[n] == '/' )
      	dir[n] = '\0';      
#endif
   }
#endif /* !VMS */
}

/* add trailing slash to dir */
void fixup_dir(char *dir)
{
#ifndef VMS
   int n;
 
   if ((n = strlen(dir)) > 1) 
     {
	n--;
#if defined(pc_system) 
      if ( dir[n] != '/' && dir[n] != '\\' ) 
      	strcat(dir, "\\" );
#else
      if ( dir[n] != '/' ) 
      	strcat(dir, "/" );
#endif
     }
#endif /* !VMS */
}

int make_directory(char *path)
{
   char work[256];
   
   strcpy(work, path);
   
   deslash(work);

#if defined (msdos) || (defined (__os2__) && !defined (__EMX__))
   return !mkdir(work);
#else
   return !mkdir(work, 0777);
#endif
}

int delete_directory(char *path)
{
   char work[256];
   
   strcpy(work, path);
   deslash(work);
   return !rmdir(work);
}

 /* ch_dir routine added during OS/2 port in order to
    simplify script writing. */
 
int ch_dir(char *path)
{
#if defined(pc_system) || defined(__os2__)
   char work[256];
  
   strcpy(work, path);
   deslash(work);
   return chdir(work);
#else   
   return chdir(path);
#endif
}

/* generate a random number */
int make_random_number(int *seed, int *max)
{
   if (*seed == -1)		       /* generate seed */
     {
#ifndef unix
	srand((unsigned int)(time(0) & getpid()));
#else
#ifdef _HPUX_SOURCE
        srand48((int)(time(0) & getpid()));
#else
#ifndef HAS_RANDOM
	srand((unsigned int)(time(0) & getpid()));
#else
        srandom((int)(time(0) & getpid()));
#endif
#endif   
#endif   
     }
   else if (*seed != 0)
     {
#ifndef unix
	srand(*seed);
#else
#ifdef _HPUX_SOURCE
        srand48(*seed);
#else
#ifndef HAS_RANDOM
	srand(*seed);
#else
        srandom(*seed);
#endif
#endif   
#endif   
     }

#ifndef unix
   return (int) rand() % *max;
#else
#ifdef _HPUX_SOURCE
   return (int) lrand48() % *max;
#else
#ifndef HAS_RANDOM
   return (int) rand () % *max;
#else
   return (int) random() % *max;
#endif
#endif
#endif   
}

#ifndef __GO32__
#ifdef unix
/* if non-zero, Flow control is enabled */
void enable_flow_control(int *mode)
{
   /* This kills X windows.  For the time being, work around it as follows */
   if (X_Init_Term_Hook != NULL) return;
   Flow_Control = *mode;
   reset_tty();
   init_tty();
}
#endif
#endif

#if defined(pc_system)

/* This routine converts  C:\  --> C:\ and C:\subdir\  -> C:\subdir */
char *msdos_pinhead_fix_dir(char *f)
{
   static char file[256];
   register char ch;
   int n;
   
   if (*f == 0) return f;
   strncpy (file, f, 255); file[255] = 0;
   f = file;
   /* skip past colon */
   while (((ch = *f) != 0) && (ch != ':')) f++;
   
   if (ch == 0)			       /* no colon */
     {
	n = (int) (f - file);
	f = file;
     }
   else
     {
	f++;
	n = strlen (f);
     }
   if (n == 0) 
     {
	*f++ = '\\'; *f = 0;
	return file;
     }
   if ((n == 1) && (*f == '\\')) return file;
   
   f += n - 1;
   if (*f == '\\') *f = 0;
   return file;
}
#endif
