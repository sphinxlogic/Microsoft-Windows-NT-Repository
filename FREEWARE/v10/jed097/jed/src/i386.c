/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dir.h>
#include <errno.h>
#include <pc.h>

#include "config.h"
#include "sysdep.h"

#define BIOSKEY i386_bioskey

#include <dos.h>
#include <bios.h>
#include <process.h>

#include "dos_os2.c"

int Abort_Char = 7;		       /* scan code for G (control) */

unsigned int i386_bioskey(int f)
{
   union REGS in, out;
   in.h.ah = f | 0x10;		       /* use enhanced kbd */
   int86(0x16, &in, &out);
   return(out.x.ax & 0xFFFF);
}

/* Here I also map keys to edt keys */
unsigned char sys_getkey()
{
#if 0
   char *keypad_scan = 
      "\x4e\x53\x52\x4f\x50\x51\x4b\x4c\x4d\x47\x48\x49\x37\x4a";
   char *edt_chars = "lnpqrstuvwxyRS";
   int bra = 'O', P = 'P', keypad, weird;
   char *p;
#endif
   char *normal = "!@#$%^&*()-=\t*\0177QWERTYUIOP[]\r*ASDFGHJKL;'`*\\ZXCVBNM<>/";
   int wit = 300;
   unsigned int c, c1, shft;
   unsigned int i;

   if (!kbhit()) while (!sys_input_pending(&wit))
     {
	if (Display_Time)
	  {
	     JWindow->trashed = 1;
	     update((Line *) NULL, 0, 1);
	  }
     }
   
   i = (unsigned int) BIOSKEY(0);
   c = i & 0xFF;
   c1 = i >> 8;
   
   shft = BIOSKEY(2) & 0xF;
#if 0   
   keypad = (c1 > 0x36) && NumLock_Is_Gold;
   /* allow Shift-keypad to give non edt keypad chars */
   keypad = keypad && !(*shift & 0x3);
   
   /* This is for the damned enter and slash keys */
   weird = keypad && ((c1 == 0xE0) && ((c == 0xD) || (c == 0x2F)));
   
   keypad = keypad && (c != 0xE0);  /* excludes small keypad */
#endif
   if (i == 0x0E08) c = 127;
   else if (i == 0x1C0A) c = 13;   /* ^Enter --> Enter */
   else if ((c == 32) && (shft & 0x04))   /* ^ space = ^@ */
     {
	c = 3;
	ungetkey((int *) &c);
	c = 0;
     }
#if 0
   else if (weird || (keypad && (NULL != (p = strchr(keypad_scan, c1)))))
     {
	if (c1 == 0xE0) if (c == 0x2f) c = 'Q'; else c = 'M';
	else c = (unsigned int) edt_chars[(int) (p - keypad_scan)];
	ungetkey((int *) &c);
	ungetkey((int *) &bra);
	c = 27;
     }
#endif
   else if ((!c) || ((c == 0xe0) && (c1 > 0x43)))
     {
#if 0
	if (NumLock_Is_Gold && (c1 == 0x3B))
	  {
	     ungetkey(&P); ungetkey(&bra); c = 27;
	  } 
	/* if key is 5 of keypad, get another char */
	else if ((c1 == 'L') && bios_key_f && (c == 0)) return(sys_getkey());
	else 
	  {
#endif
	     if ((shft == 0x8) && (c == 0) && PC_Alt_Char)
	       {
		  if ((c1 >= 14) && (c1 <= 53))
		    {
		       c1 = (unsigned int) normal[c1];
		       c = PC_Alt_Char;
		    }
		  else if ((c1 >= 120) && (c1 <= 131))
		    {
		       c1 = (unsigned int) normal[c1 - 120];
		       c = PC_Alt_Char;
		    }
		  else if (c1 == 165) /* tab */
		    {
		       c1 = (unsigned int) normal[c1 - 165 + 12];
		       c = PC_Alt_Char;
		    }
	       }
	     else c = 0;
	     ungetkey((int *) &c1);
#if 0
	  }
#endif
     }
   return(c);
}





static int sys_input_pending(int *tsecs)
{
   int count = *tsecs * 5;
   if (Batch || Input_Buffer_Len) return(Input_Buffer_Len);
   
   if (kbhit()) return 1;
   
   while (count > 0)
     {
	delay(20);		       /* 20 ms or 1/50 sec */
	if (kbhit()) break;
	count--;
     }
   
   return (count);
}

/*  This is to get the size of the terminal  */
void get_term_dimensions(int *cols, int *rows)
{
   *rows = ScreenRows();
   *cols = ScreenCols();
}


/* returns 0 if file does not exist, 1 if it is not a dir, 2 if it is */
int sys_chmod(char *file, int what, int *mode, short *dum1, short *dum2)
{
   struct stat buf;
   int m;
   *dum1 = *dum2 = 0;
   if (what)
     {
	chmod(file, *mode);
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

static int cbreak;

void reset_tty()
{   
   setcbrk(cbreak);
}
void init_tty()
{   
   cbreak = getcbrk();
   setcbrk(0);
}


static struct ffblk Dos_DTA;
static char Found_Dir[256], *Found_File;
/* found_File is a pointer into found_Dir such that the
 * full pathname is stored in the following form
 * "c:/dir/path/\0filename.ext\0"
 */

#define lcase(x) if (((x) >= 'A') && ((x) <= 'Z')) (x) |= 0x20

static void dta_fixup_name (char *file)
{
   char *p;

   strcpy (Found_File, Dos_DTA.ff_name);
   p = Found_File;
   while (*p)
     {
	lcase(*p);
	p++;
     }
   
   strcpy(file, Found_Dir);
   strcat(file, Found_File);
   if (Dos_DTA.ff_attrib & FA_DIREC)
     strcat(file, "\\");
}



int sys_findfirst(char *file)
{
   char *f;

   strcpy(Found_Dir, expand_filename(file) );
   Found_File = extract_file( Found_Dir );

   f = Found_File;
   
   while (*f && (*f != '*')) f++;
   if (! *f)
     {
	f = Found_File;
	while (*f && (*f != '.')) f++;
	if (*f) strcat(Found_Dir, "*"); 
	else strcat(Found_Dir, "*.*");
     }
   
   if (findfirst(Found_Dir, &Dos_DTA, FA_RDONLY | FA_DIREC)) 
     {
	*Found_File++ = 0;
	return 0;
     }
   *Found_File++ = 0;
   
   dta_fixup_name(file);
   return(1);
}

int sys_findnext(char *file)
{
   if (findnext(&Dos_DTA)) return(0);
   dta_fixup_name(file);
   return(1);
}

/* This routine is called from S-Lang inner interpreter.  It serves
   as a poor mans version of an interrupt 9 handler */

void i386_check_kbd()
{
   int ch;
   while (kbhit()) 
     {
	if ((ch = sys_getkey()) == Abort_Char) SLang_Error = USER_BREAK;
	ungetkey(&ch);
     }
}

char *djgpp_current_time (void)
{
   union REGS rg;
   unsigned int year;
   unsigned char month, day, weekday, hour, minute, sec;
   char days[] = "SunMonTueWedThuFriSat";
   char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
   static char the_date[26];
   
   rg.h.ah = 0x2A;
   int86(0x21, &rg, &rg);
   year = rg.x.cx & 0xFFFF;
   month = 3 * (rg.h.dh - 1);
   day = rg.h.dl;
   weekday = 3 * rg.h.al;
   
   rg.h.ah = 0x2C;
   int86(0x21, &rg, &rg);
   hour = rg.h.ch;
   minute = rg.h.cl;
   sec = rg.h.dh;
   
   /* we want this form: Thu Apr 14 15:43:39 1994\n  */
   sprintf(the_date, "%.3s %.3s%3d %02d:%02d:%02d %d\n",
	   days + weekday, months + month, 
	   day, hour, minute, sec, year);
   return the_date;
}

	   
   


