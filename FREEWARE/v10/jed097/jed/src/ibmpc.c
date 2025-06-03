/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <process.h>
#include "config.h"
#include "sysdep.h"

#ifdef HAS_MOUSE
#include "pcmouse.h"
#endif

#include <dir.h>
#include <dos.h>
#include <bios.h>
#include <io.h>
#include <conio.h>
#include <time.h>
#include <stddef.h>

/* for stat */
#include <sys/stat.h>
#include <errno.h>


#ifdef __cplusplus
#define _DOTS_ ...
#else
#define _DOTS_ 
#endif

void interrupt (*oldint9)(_DOTS_);

static unsigned char far *shift = (unsigned char far *) 0x417;
static unsigned int far *kbptr = (unsigned int far *) 0x41a;
static unsigned int far *kbnext = (unsigned int far *) 0x41c;
static unsigned int far *kbbeg = (unsigned int far *) 0x480;
static unsigned int far *kbend = (unsigned int far *) 0x482;

int Abort_Char = 34;

#include "dos_os2.c"

/* Macro to get char from BIOS not dos */
void interrupt int9_handler(_DOTS_)
{
   unsigned int scan = (unsigned int) Abort_Char; /* 34 = scan code for ^G */
   unsigned char s, s1;
   unsigned int offset, f1_scan = 0x3B00;  /* f1 key */
   unsigned int *p;
   unsigned int numlock = 0x45;
   

   s1 = *shift & 0xF;  /* ignoring caps, ins, num lock, scroll lock */
   s = inp(0x60);
   if (s1 & 0x04)      /* control key */
     {
	if (s == scan)
	  {
	     if (Ignore_User_Abort == 0) SLang_Error = 2;
	     SLKeyBoard_Quit = 1;
	  }
	else if (s == 28)   /* Control - enter, ignore it!! */
	  {
	     s = inp(0x61);
	     outportb(0x61, s | 0x80);
	     outportb(0x61, s);
	     outportb(0x20, 0x20);
	     return;
	  }
     }
   else if (NumLock_Is_Gold && ((s & 0x7F) == numlock))
     {
	if (s == numlock)
	  {
	     offset = *kbnext;
	     offset += 2;
	     if (offset == *kbend) offset = *kbbeg;
	     if (offset != *kbptr)  /* buffer not full */
	       {
		  p = (unsigned int *) (0x400L + (*kbnext));
		  *p = f1_scan;
		  *kbnext = offset;

	       }
	  }
	s = inp(0x61);
	outportb(0x61, s | 0x80);
	outportb(0x61, s);
	outportb(0x20, 0x20);
	return;
     }
   (*oldint9)();
}

void init_int9_handler(void)
{
   oldint9 = getvect(9);
   setvect(9, int9_handler);
}

void restore_int9_handler(void)
{
   setvect(9, oldint9);
}

static void set_ctrl_break(int state)
{
   static int prev = 0;

   asm {
      mov dl, byte ptr prev
      mov ax, state
      cmp ax, 0
      jne L1
      mov ah, 33h
      mov al, 0
      mov dl, byte ptr prev
      int 21h
      xor ax, ax
      mov al, dl
      mov prev, ax
      mov dl, 0
   }
   L1:
   asm {
      mov al, 1
      mov ah, 33h
      int 21h
   }   
}

#if 0
/* Here I just flag the error and return to the user-- hopefully jed will
   catch it and user will know what to do. */
static char *int24_errs[] = 
{
   "write protect",
   "bad unit",
   "drive not ready",
   "unknown",
   "CRC error",
   "request error",
   "seek error",
   "unknown media",
   "sector not found",
   "printer out of paper",
   "general write failure",
   "general read failure",
   "general failure", "reserved", "reserved", "invalid disk change"
};
 
int int24_handler(int err, int ax, int bp, int si)
{
   unsigned int di;
   
   di = _DI;
   (void) ax;
   (void) bp;
   msg_error(int24_errors[di & 0x000F]);
}
#endif

static int ctrl_break(void)
{
   msg_error("Control Break ignored!");
   return(1);
}


#define BIOSKEY(x) bioskey((x) | bios_key_f)
static int bios_key_f;
void init_tty()
{
   bios_key_f = peekb(0x40,0x96) & 0x10; /* enhanced keyboard flag */
   set_ctrl_break(0); 
   ctrlbrk(ctrl_break);
   init_int9_handler();
}

void reset_tty()
{
   restore_int9_handler();
   set_ctrl_break(1);
   if (Batch) return;
   SLtt_reset_video();
}


/* Here I also map keys to edt keys */
unsigned char sys_getkey()
{
   char *keypad_scan = 
      "\x4e\x53\x52\x4f\x50\x51\x4b\x4c\x4d\x47\x48\x49\x37\x4a";
   char *normal = "!@#$%^&*()-=\t*\0177QWERTYUIOP[]\r*ASDFGHJKL;'`*\\ZXCVBNM<>/";
   char *edt_chars = "lnpqrstuvwxyRS";
   int bra = 'O', P = 'P', keypad, weird;
   char *p;
   unsigned int c, c1, shft;
   unsigned int i;
   
   weird = 300;
   if (BIOSKEY(1) == 0) while (!sys_input_pending(&weird))
     {
	if (Display_Time)
	  {
	     JWindow->trashed = 1;
	     update((Line *) NULL, 0, 1);
	  }
     }
#ifdef HAS_MOUSE
   /* This can only be set by the mouse */
   if (Input_Buffer_Len) return my_getkey ();
#endif
   
   i = BIOSKEY(0);
   c = i & 0xFF;
   c1 = i >> 8;
   
   keypad = (c1 > 0x36) && NumLock_Is_Gold && (bios_key_f == 0x10);
   /* allow Shift-keypad to give non edt keypad chars */
   shft = *shift & 0xF;
   keypad = keypad && !(shft & 0x3);
   
   
   /* This is for the damned enter and slash keys */
   weird = keypad && ((c1 == 0xE0) && ((c == 0xD) || (c == 0x2F)));
   
   keypad = keypad && (c != 0xE0);  /* excludes small keypad */
   
   if ((c == 8) && ((i >> 8) == 0x0e)) c =127;
   else if ((c == 32) && (shft & 0x04))    /* ^ space = ^@ */
     {
	c = 3;
	ungetkey((int *) &c);
	c = 0;
     }
   else if (weird || (keypad && (NULL != (p = strchr(keypad_scan, c1)))))
     {
	if (c1 == 0xE0) if (c == 0x2f) c = 'Q'; else c = 'M';
	else c = (unsigned int) edt_chars[(int) (p - keypad_scan)];
	ungetkey((int *) &c);
	ungetkey((int *) &bra);
	c = 27;
     }
   else if ((!c) || ((c == 0xe0) && (bios_key_f) && (c1 > 0x43)))
     {
	if (NumLock_Is_Gold && (c1 == 0x3B))
	  {
	     ungetkey(&P); ungetkey(&bra); c = 27;
	  }
	/* if key is 5 of keypad, get another char */
	else if ((c1 == 'L') && bios_key_f && (c == 0)) return(sys_getkey());
	else 
	  {
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
	  }
     }
   return(c);
}

/* sleep for *tsecs tenths of a sec waiting for input */
static int sys_input_pending(int *tsecs)
{
   int count = *tsecs * 5;
   
   if (Batch || Input_Buffer_Len) return(Input_Buffer_Len);
   
   if (count)
     {
	while(count > 0)
	  {
	     delay(20);		       /* 20 ms or 1/50 sec */
	     if (BIOSKEY(1)
#ifdef HAS_MOUSE
		 || mouse_input()
#endif
		 ) break;
	     count--;
	  }
	return(count);
     }
   else return(
	       BIOSKEY(1)
#ifdef HAS_MOUSE
	       || mouse_input()
#endif
	       );
	       
}

/* Thanks to Robert Schmidt - robert@alkymi.unit.no - Buuud@IRC
   for some of code below */
void get_term_dimensions(int *w, int *h)
{
   *h = 0;

   /* Get BIOS's screenwidth, this works on ALL displays. */
   *w = *((int *)MK_FP(0x40, 0x4a));
   
   /* Use Ralf Brown test for EGA or greater */
   asm mov ah, 12h
   asm mov bl, 10h
   asm mov bh, 0xFF  /* EGA or greater will change this */
   asm int 10h
   asm cmp bh, 0xFF
   asm je L1
	/* if EGA or compatible: Get BIOS's number of rows. */      
      *h = *(char *)MK_FP(0x40, 0x84) + 1;
   L1:
      if (*h <= 0) *h = 25;
}

int sys_chmod(char *file, int what, int *mode, short *dum1, short *dum2)
{
   int flag = 0, m = *mode;
   (void) dum1; (void) dum2;
   
   file = msdos_pinhead_fix_dir (file);
   
   asm mov ah, 43h
   asm mov al, byte ptr what
   asm mov cx, m
   asm push ds
   asm lds dx, dword ptr file
   asm int 21h
   asm pop ds
   asm mov m, cx
   asm jnc L1
   asm mov flag, ax
   
   /* Here if carry flag is set */
   if (flag == 0x2) return(0);     /* file not found */
   if (flag == 0x3) return(-2);
	/* msg_error("Path does not exist."); */
	/* else return (-3); */
	/* sprintf(buf, "chmod: Unknown Error. %d", out.x.ax);
	   msg_error(buf);
	*/
   return(-1);
    
   /* carry flag is 0 */
   L1:
   if (what == 0)
     {
	*mode = m;
     }

   if (m & 0x10)
     {
	/* msg_error("File is a directory."); */
	return(2);
     }

   return(1);
}

typedef struct Dos_DTA_Type
{
   unsigned char undoc[21];
   unsigned char attr;
   unsigned int time;
   unsigned int date;
   unsigned char low_size[2];
   unsigned char high_size[2];
   char name[13];
} Dos_DTA_Type;

static Dos_DTA_Type Dos_DTA;

static void set_dta (void)
{
   Dos_DTA_Type *dummy = &Dos_DTA;
   
   asm mov ah, 0x1A
   asm push ds
   asm lds dx, dword ptr dummy
   asm int 21h
   asm pop ds
}


static int File_Attr;

#define HIDDEN 0x2
#define SYSTEM 0x4
#define SUBDIR 0x10
#define READON 0x1

static char Found_Dir[256];

#define lcase(x) if (((x) >= 'A') && ((x) <= 'Z')) (x) |= 0x20

void dta_fixup_name(char *file)
{
   int dir;
   char *p, name[13];

   strcpy(file, Found_Dir);
   strcpy(name, Dos_DTA.name);
   dir = (Dos_DTA.attr & SUBDIR);
   p = name;
   while (*p)
     {
	lcase(*p);
	p++;
     }
   strcat(file, name);
   if (dir) strcat(file, "\\");
}

int sys_findfirst(char *thefile)
{
   char *f, the_path[256], *file, *f1;
   char *pat, *fudge;
   
   set_dta();
   File_Attr = READON | SUBDIR;

   file = expand_filename(thefile);
   f1 = f = extract_file(file);
   strcpy (Found_Dir, file);
   
   
   Found_Dir[(int) (f - file)] = 0;

   strcpy(the_path, file);
   
   while (*f1 && (*f1 != '*')) f1++;
   if (! *f1)
     {
	while (*f && (*f != '.')) f++;
	if (*f) strcat(the_path, "*"); else strcat(the_path, "*.*");
     }
   pat = the_path;
   
   /* Something is very wrong after this returns.  
    * thefile gets trashed for some reason.  Here I fudge until I figure 
    *  out what is going on 
    * 
    * Note: This has been fixed.  I am just too lazy to remove the fudge. 
    */
   fudge = thefile;
   
   asm mov ah, 0x4e
   asm mov cx, File_Attr
   asm push ds
   asm lds dx, dword ptr pat
   asm int 21h
   asm pop ds
   asm jc L1 
   
   thefile = fudge;
   
   /* fprintf(stderr, "asm:%lu\t|%s|\n", thefile, thefile); */
   
   dta_fixup_name(file);
   strcpy(thefile, file);
   return(1);
   L1:
   return 0;
}

int sys_findnext(char *file)
{
   asm mov ah, 0x4F
   asm int 21h
   asm jc L1
   dta_fixup_name(file);
   return(1);
   L1:
   return(0);
}



#if 0
unsigned long sys_file_mod_time(char *file)
{
   struct stat buf;
   
   if (stat(file, &buf) != 0) return(0);
   return ((unsigned long) buf.st_mtime);
   close(fd);
   return ((unsigned long) buf.st_mtime);
   /*
   asm mov ah, 57h
   asm mov al, 0
   asm mov bx, fd
   asm int 21h
   asm mov cx, d
   asm mov dx, t
   close(fd);
   fprintf(stderr, "Time ok.");
   return(1); */
}
#endif

/* Here we do a find first followed by calling routine to conver time */
unsigned long sys_file_mod_time(char *file)
{
   struct time t;
   struct date d;
   /* struct tm *local; */
   time_t secs;
   unsigned int dat, tim;
   
   File_Attr = READON | SUBDIR;
   
   set_dta();

   asm mov ah, 0x4e
   asm mov cx, File_Attr
   asm push ds
   asm lds dx, dword ptr file
   asm int 21h
   asm pop ds
   asm jnc A_LABEL
   return 0;
   
   A_LABEL:
   tim = Dos_DTA.time;
   dat = Dos_DTA.date;
   t.ti_min = (tim >> 5) & 63;
   t.ti_hour = (tim >> 11) & 31;
   t.ti_hund = 0;
   t.ti_sec = 2 * (tim & 31);
   d.da_day = dat & 31;
   d.da_mon = (dat >> 5) & 15;
   d.da_year = 1980 + ((dat >> 9) & 0x7F);
   secs = dostounix(&d, &t);
   return((unsigned long) secs);
}

