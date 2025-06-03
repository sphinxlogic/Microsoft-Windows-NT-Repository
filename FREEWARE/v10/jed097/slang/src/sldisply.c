/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "slang.h"

#ifndef sequent
#include <stdlib.h>
#endif

#ifdef unix
#ifndef sequent
# include <unistd.h>
#endif
#endif


/* Colors:  These definitions are used for the display.  However, the 
 * application only uses object handles which get mapped to this
 * internal representation.  The mapping is performed by the Color_Map
 * structure below. */

#define CHAR_MASK	0x000000FF
#define FG_MASK		0x0000FF00
#define BG_MASK		0x00FF0000
#define ATTR_MASK	0x1F000000
#define BGALL_MASK	0x0FFF0000

/* The 0x10000000 bit represents the alternate character set.  BGALL_MASK does
 * not include this attribute.
 */



#define GET_FG(color) ((color & FG_MASK) >> 8)
#define GET_BG(color) ((color & BG_MASK) >> 16)
#define MAKE_COLOR(fg, bg) (((fg) | ((bg) << 8)) << 8)

int SLtt_Screen_Cols;
int SLtt_Screen_Rows;
int SLtt_Term_Cannot_Insert;
int SLtt_Term_Cannot_Scroll;
int SLtt_Use_Ansi_Colors;

static int Automatic_Margins;
static int No_Move_In_Standout;
static int Worthless_Highlight;

#ifdef linux
static int Linux_Console;
#endif

/* It is crucial that JMAX_COLORS must be less than 128 since the high bit
 * is used to indicate a character from the ACS (alt char set)
 */
#define JMAX_COLORS 16
#define JNORMAL_COLOR 0

typedef struct 
{
   SLtt_Char_Type fgbg;
   SLtt_Char_Type mono;
   char *custom_esc;
} Ansi_Color_Type;

#define RGB1(r, g, b)   ((r) | ((g) << 1) | ((b) << 2))
#define RGB(r, g, b, br, bg, bb)  ((RGB1(r, g, b) << 8) | (RGB1(br, bg, bb) << 16))

static Ansi_Color_Type Ansi_Color_Map[JMAX_COLORS] = 
{
   {RGB(1, 1, 1, 0, 0, 0), 0x00000000, NULL},
   {RGB(0, 0, 1, 0, 0, 0), SLTT_REV_MASK, NULL},
   {RGB(0, 1, 0, 0, 0, 0), SLTT_REV_MASK, NULL},
   {RGB(1, 0, 0, 0, 0, 0), SLTT_REV_MASK, NULL},
   {RGB(0, 1, 0, 0, 0, 1), SLTT_REV_MASK, NULL},
   {RGB(1, 0, 0, 0, 0, 1), SLTT_REV_MASK, NULL},
   {RGB(0, 0, 1, 0, 1, 0), SLTT_REV_MASK, NULL},
   {RGB(1, 0, 0, 0, 1, 0), SLTT_REV_MASK, NULL},
   {RGB(0, 0, 1, 1, 0, 0), SLTT_REV_MASK, NULL},
   {RGB(0, 1, 0, 1, 0, 0), SLTT_REV_MASK, NULL},
   {RGB(0, 1, 1, 1, 1, 1), SLTT_REV_MASK, NULL},
   {RGB(1, 1, 0, 1, 1, 1), SLTT_REV_MASK, NULL},
   {RGB(1, 0, 1, 1, 1, 1), SLTT_REV_MASK, NULL},
   {RGB(0, 0, 0, 0, 1, 1), SLTT_REV_MASK, NULL},
   {RGB(0, 0, 0, 1, 1, 0), SLTT_REV_MASK, NULL},
   {RGB(0, 0, 0, 1, 0, 1), SLTT_REV_MASK, NULL}
};


/* This is the string to use to use when outputting color information.
 */
static char *Color_Escape_Sequence = "\033[3%d;4%dm";

char *SLtt_Graphics_Char_Pairs;	       /* ac termcap string -- def is vt100 */

   
/* 1 if terminal lacks the ability to do into insert mode or into delete
   mode. Currently controlled by S-Lang but later perhaps termcap. */

static char *UnderLine_Vid_Str;
static char *Blink_Vid_Str;
static char *Bold_Vid_Str;
static char *Ins_Mode_Str; /* = "\033[4h"; */   /* ins mode (im) */
static char *Eins_Mode_Str; /* = "\033[4l"; */  /* end ins mode (ei) */
static char *Scroll_R_Str; /* = "\033[%d;%dr"; */ /* scroll region */
static char *Cls_Str; /* = "\033[2J\033[H"; */  /* cl termcap STR  for ansi terminals */
static char *Rev_Vid_Str; /* = "\033[7m"; */    /* mr,so termcap string */
static char *Norm_Vid_Str; /* = "\033[m"; */   /* me,se termcap string */
static char *Del_Eol_Str; /* = "\033[K"; */	       /* ce */
static char *Del_Char_Str; /* = "\033[P"; */   /* dc */
static char *Del_N_Lines_Str; /* = "\033[%dM"; */  /* DL */
static char *Add_N_Lines_Str; /* = "\033[%dL"; */  /* AL */
static char *Rev_Scroll_Str;
static char *Curs_Up_Str;
static char *Curs_F_Str;    /* RI termcap string */

static char *Start_Alt_Chars_Str;  /* as */
static char *End_Alt_Chars_Str;   /* ae */

static int Len_Curs_F_Str = 5;

/* cm string has %i%d since termcap numbers columns from 0 */
/* char *CURS_POS_STR = "\033[%d;%df";  ansi-- hor and vert pos */
static char *Curs_Pos_Str; /* = "\033[%i%d;%dH";*/   /* cm termcap string */


/* scrolling region */
static int Scroll_r1 = 0, Scroll_r2 = 23;
static int Cursor_r, Cursor_c;	       /* 0 based */

/* current attributes --- initialized to impossible value */
static SLtt_Char_Type Current_Fgbg = 0xFFFFFFFF;

static int Cursor_Set;		       /* 1 if cursor position known, 0
					* if not.  -1 if only row is known
					*/


#define MAX_OUTPUT_BUFFER_SIZE 4096

static unsigned char Output_Buffer[MAX_OUTPUT_BUFFER_SIZE];
static unsigned char *Output_Bufferp = Output_Buffer;

int SLtt_flush_output (void)
{
   int n = (int) (Output_Bufferp - Output_Buffer);
   if (n) 
     {
	write (fileno(stdout), Output_Buffer, n);
	Output_Bufferp = Output_Buffer;
     }
   return n;
}


int SLtt_Baud_Rate;
static void tt_write(char *str, int n)
{
   static unsigned long last_time;
   static int total;
   unsigned long now;
   int ndiff;
   
   if ((str == NULL) || (n <= 0)) return;
   total += n;
   
   while (1)
     {
	ndiff = MAX_OUTPUT_BUFFER_SIZE - (int) (Output_Bufferp - Output_Buffer);
	if (ndiff < n)
	  {
	     MEMCPY (Output_Bufferp, str, ndiff);
	     write (fileno (stdout), Output_Buffer, MAX_OUTPUT_BUFFER_SIZE);
	     Output_Bufferp = Output_Buffer;
	     n -= ndiff;
	     str += ndiff;
	  }
	else 
	  {
	     MEMCPY (Output_Bufferp, str, n);
	     Output_Bufferp += n;
	     break;
	  }
     }
   
   if (((SLtt_Baud_Rate > 150) && (SLtt_Baud_Rate <= 9600))
       && (10 * total > SLtt_Baud_Rate))
     {
	total = 0;
	if ((now = (unsigned long) time(NULL)) - last_time <= 1)
	  {
	     SLtt_flush_output ();
	     sleep((unsigned) 1);
	  }
	last_time = now;
     }
}


void SLtt_write_string(char *str)
{
   if (str == NULL) return;
   tt_write(str, strlen(str));
}


void SLtt_putchar(char ch)
{
   SLtt_normal_video ();
   if (Cursor_Set == 1)
     {
	if (ch >= ' ') Cursor_c++;
	else if (ch == '\b') Cursor_c--;
	else if (ch == '\r') Cursor_c = 0;
	else Cursor_Set = 0;
	
	if ((Cursor_c + 1 == SLtt_Screen_Cols) 
	    && Automatic_Margins) Cursor_Set = 0;
     }
   
   if (Output_Bufferp < Output_Buffer + MAX_OUTPUT_BUFFER_SIZE)
     {
	*Output_Bufferp++ = (unsigned char) ch;
     }
   else tt_write (&ch, 1);
}

/* this is supposed to be fast--- also handles 
   termcap: %d, &i, %., %+, %r strings as well as terminfo stuff */
static int tt_sprintf(char *buf, char *fmt, int x, int y)
{
   register unsigned char *f = (unsigned char *) fmt, *b, ch;
   int offset = 0, tinfo = 0;
   int stack[10];
   int i = 0, z;
   stack[0] = y; stack[1] = x; i = 2;
   
   b = (unsigned char *) buf;
   if (fmt != NULL) while ((ch = *f++) != 0)
     {
	if (ch != '%') *b++ = ch;
	else 
	  {
	     ch = *f++;
	     if (tinfo)
	       {
		  if ((ch <= '3') && (ch >= '0'))
		    {
		       /* map it to termcap.  Since this is terminfo,
			* it must be one of:
			*   %2d, %3d, %02d, %03d
			* 
			* I am assuming that a terminal that understands
			* %2d form will also understand the %02d form.  These
			* only differ by a space padding the field.
			*/
		       
		       /* skip the 'd'-- hope it is there */
		       if (ch == '0') 
			 {
			    ch = *f;
			    f += 2;
			 }
		       else f++;
		    }
	       }
	     
	     if (ch == 'p')
	       {
		  tinfo = 1;
		  ch = *f++;
		  if (ch == '1') stack[i++] = x; else stack[i++] = y;
	       }
	     else if (ch == '\'')   /* 'x' */
	       {
		  stack[i++] = *f++;
		  f++;
	       }
	     else if ((ch == 'd') || (ch == '2') || (ch == '3'))
	       {
		  z = stack[--i];
		  z += offset;
		  if (z >= 100)
		    {
		       *b++ = z / 100 + '0';
		       z = z % 100;
		       goto ten;
		    }
		  else if (ch == 3) 
		    {
		       *b++ = '0';
		       ch = '2';
		    }
		  
		  if (z >= 10)
		    {
		       ten:
		       *b++ = z / 10 + '0';
		       z = z % 10;
		    }
		  else if (ch == 2) *b++ = '0';
		  
		  *b++ = z + '0';
	       }
	     else if (ch == 'i') 
	       {
		  offset = 1;
	       }
	     else if (ch == '+')
	       {
		  if (tinfo) 
		    {
		       z = stack[--i];
		       stack[i-1] += z;
		    }
		  else
		    {
		       ch = *f++;
		       if ((unsigned char) ch == 128) ch = 0;
		       ch = ch + (unsigned char) stack[--i];
		       if (ch == '\n') ch++;
		       *b++ = ch;
		    }
	       }
	     else if (ch == 'r')
	       {
		  stack[0] = x;
		  stack[1] = y;
	       }
	     else if ((ch == '.') || (ch == 'c'))
	       {
		  ch = (unsigned char) stack[--i];
		  if (ch == '\n') ch++;
		  *b++ = ch;
	       }
	     else *b++ = ch;
	  }
     }
   *b = 0;
   return((int) (b - (unsigned char *) buf));
}

static void tt_printf(char *fmt, int x, int y)
{
   char buf[256];
   int n;
   if (fmt == NULL) return;
   n = tt_sprintf(buf, fmt, x, y);
   tt_write(buf, n);
}


void SLtt_set_scroll_region (int r1, int r2)
{
   Scroll_r1 = r1;
   Scroll_r2 = r2;
   tt_printf (Scroll_R_Str, Scroll_r1, Scroll_r2);
   Cursor_Set = 0;
}

void SLtt_reset_scroll_region (void)
{
    SLtt_set_scroll_region(0, SLtt_Screen_Rows - 1);
}

/* the goto_rc function moves to row relative to scrolling region */
void SLtt_goto_rc(int r, int c)
{
   char *s = NULL;
   int n;
   char buf[6];
   
   if (c < 0)
     {
	c = -c - 1;
	Cursor_Set = 0;
     }
   
   /* if (No_Move_In_Standout && Current_Fgbg) SLtt_normal_video (); */
   r += Scroll_r1;
   
   if ((Cursor_Set > 0) || ((Cursor_Set < 0) && !Automatic_Margins))
     {
	n = r - Cursor_r;
	if ((n == -1) && (Cursor_Set > 0) && (Cursor_c == c) 
	    && (Curs_Up_Str != NULL))
	  {
	     s = Curs_Up_Str;
	  }
	else if ((n >= 0) && (n <= 4))
	  {
	     if ((n == 0) && (Cursor_Set == 1) 
		 && ((c > 1) || (c == Cursor_c)))
	       {
		  if (Cursor_c == c) return;
		  if (Cursor_c == c + 1) 
		    {
		       s = buf;
		       *s++ = '\b'; *s = 0;
		       s = buf;
		    }
	       }
	     else if (c == 0)
	       {
		  s = buf;
		  if ((Cursor_Set != 1) || (Cursor_c != 0)) *s++ = '\r';
		  while (n--) *s++ = '\n';
#ifdef VMS
		  /* Need to add this after \n to start a new record.  Sheesh. */
		  *s++ = '\r';
#endif
		  *s = 0;
		  s = buf;
	       }
	     /* Will fail on VMS */
#ifndef VMS
	     else if ((Cursor_Set == 1) && 
		      (Cursor_c >= c) && (c + 3 > Cursor_c))
	       {
		  s = buf;
		  while (n--) *s++ = '\n';
		  n = Cursor_c - c;
		  while (n--) *s++ = '\b';
		  *s = 0;
		  s = buf;
	       }
#endif
	  }
     }
   if (s != NULL) SLtt_write_string(s);
   else tt_printf(Curs_Pos_Str, r, c);
   Cursor_c = c; Cursor_r = r;
   Cursor_Set = 1;
}

void SLtt_begin_insert (void)
{
   SLtt_write_string(Ins_Mode_Str);
}

void SLtt_end_insert (void)
{
   SLtt_write_string(Eins_Mode_Str);
}

void SLtt_delete_char (void)
{
   SLtt_normal_video ();
   SLtt_write_string(Del_Char_Str);
}

void SLtt_erase_line (void)
{
   char *s;
   
   Current_Fgbg = 0xFFFFFFFF;
   SLtt_write_string("\r");
   Cursor_Set = 1; Cursor_c = 0;
   if (SLtt_Use_Ansi_Colors) s = "\033[0m";
   else s = Norm_Vid_Str;
   SLtt_write_string(s);
   SLtt_del_eol();
}

void SLtt_delete_nlines (int n)
{
   int r1, curs;
   char buf[132];
   if (!n) return;
   SLtt_normal_video ();
   if (Del_N_Lines_Str != NULL) tt_printf(Del_N_Lines_Str,n, 0);
   else
   /* get a new terminal */
     {
	r1 = Scroll_r1;
	curs = Cursor_r;
	SLtt_set_scroll_region(curs, Scroll_r2);
	SLtt_goto_rc(Scroll_r2 - Scroll_r1, 0);
	MEMSET(buf, '\n', n);
	tt_write(buf, n);
	/* while (n--) tt_putchar('\n'); */
	SLtt_set_scroll_region(r1, Scroll_r2);
	SLtt_goto_rc(curs, 0);
     }
}

void SLtt_cls (void)
{
   SLtt_normal_video();
   SLtt_reset_scroll_region ();
   SLtt_write_string(Cls_Str);
}

void SLtt_reverse_index (int n)
{
   if (!n) return;
   
   SLtt_normal_video();
   if (Add_N_Lines_Str != NULL) tt_printf(Add_N_Lines_Str,n, 0);
   else
     {
	while(n--) SLtt_write_string(Rev_Scroll_Str);
     }
}


int SLtt_Ignore_Beep = 3;
static char *Visible_Bell_Str;

void SLtt_beep (void)
{
   if (SLtt_Ignore_Beep & 0x1) SLtt_putchar('\007');
   
   if (SLtt_Ignore_Beep & 0x2)
     {
	if (Visible_Bell_Str != NULL) SLtt_write_string (Visible_Bell_Str);
#ifdef linux
	else if (Linux_Console)
	  {
	     SLtt_write_string ("\e[?5h");
	     SLtt_flush_output ();
	     usleep (50000);
	     SLtt_write_string ("\e[?5l");
	  }
#endif
     }
   SLtt_flush_output ();
}

void SLtt_del_eol (void)
{
   if (Current_Fgbg != 0xFFFFFFFF) SLtt_normal_video ();
   SLtt_write_string(Del_Eol_Str);
}

typedef struct
{
   char *name;
   SLtt_Char_Type color;
}
Color_Def_Type;

static Color_Def_Type Color_Defs[16] =
{
   {"black",		0x00000000},
   {"red",		0x00000100},
   {"green",		0x00000200},
   {"brown",		0x00000300},
   {"blue",		0x00000400},
   {"magenta",		0x00000500},
   {"cyan",		0x00000600},
   {"lightgray",	0x00000700},
   {"gray",		0x00000800},
   {"brightred",	0x00000900},
   {"brightgreen",	0x00000A00},
   {"yellow",		0x00000B00},
   {"brightblue",	0x00000C00},
   {"brightmagenta",	0x00000D00},
   {"brightcyan",	0x00000E00},
   {"white",		0x00000F00}
};

void SLtt_set_mono (int obj, char *what, SLtt_Char_Type mask)
{
   if ((obj < 0) || (obj >= JMAX_COLORS))
     {
	return;
     }   
   Ansi_Color_Map[obj].mono = mask & ATTR_MASK;
}


static unsigned char FgBg_Stats[JMAX_COLORS];

void SLtt_set_color (int obj, char *what, char *fg, char *bg)
{
   int i;
   SLtt_Char_Type f = 0xFFFFFFFF, g = 0xFFFFFFFF;
   SLtt_Char_Type attr = 0;
   char *cust_esc;
   
   if ((obj < 0) || (obj >= JMAX_COLORS))
     {
	return;
     }
   
   i = 0; while (i < 16)
     {
	if (!strcmp(fg, Color_Defs[i].name))
	  {
	     f = Color_Defs[i].color;
	     if (f & 0x800)
	       {
		  f &= 0x700;
		  attr = SLTT_BOLD_MASK;
	       }
	     
	     break;
	  }
	i++;
     }
   i = 0; while (i < 16)
     {
	if (!strcmp(bg, Color_Defs[i].name))
	  {
	     g = Color_Defs[i].color;
	     g &= 0x700;
	     g = g << 8;
	     break;
	  }
	i++;
     }
   if ((f == 0xFFFFFFFF) || (g == 0xFFFFFFFF) || (f == g)) return;
   
   cust_esc = Ansi_Color_Map[obj].custom_esc;
   if (cust_esc != NULL) 
     {
	SLFREE (cust_esc);
	FgBg_Stats[(Ansi_Color_Map[obj].fgbg >> 8) & 0x7F] -= 1;
	Ansi_Color_Map[obj].custom_esc = NULL;
     }

   Ansi_Color_Map[obj].fgbg = f | g | attr;
}

void SLtt_set_color_esc (int obj, char *esc)
{
   char *cust_esc;
   SLtt_Char_Type fgbg = 0;
   int i;
   
   if ((obj < 0) || (obj >= JMAX_COLORS))
     {
	return;
     }
   
   cust_esc = Ansi_Color_Map[obj].custom_esc;
   if (cust_esc != NULL) 
     {
	SLFREE (cust_esc);
	FgBg_Stats[(Ansi_Color_Map[obj].fgbg >> 8) & 0x7F] -= 1;
     }
   
   cust_esc = (char *) SLMALLOC (strlen(esc) + 1);
   if (cust_esc != NULL) strcpy (cust_esc, esc);
   
   Ansi_Color_Map[obj].custom_esc = cust_esc;
   if (cust_esc == NULL) fgbg = 0;
   else
     {
	/* The whole point of this is to generate a unique fgbg */
	for (i = 0; i < JMAX_COLORS; i++)
	  {
	     if (FgBg_Stats[i] == 0) fgbg = i;
	     
	     if (obj == i) continue;
	     if ((Ansi_Color_Map[i].custom_esc) == NULL) continue;
	     if (!strcmp (Ansi_Color_Map[i].custom_esc, cust_esc))
	       {
		  fgbg = (Ansi_Color_Map[i].fgbg >> 8) & 0x7F;
		  break;
	       }
	  }
	FgBg_Stats[fgbg] += 1;
     }
   
   fgbg |= 0x80;
   Ansi_Color_Map[obj].fgbg = (fgbg | (fgbg << 8)) << 8;
}


void SLtt_set_alt_char_set (int i)
{
   if (Start_Alt_Chars_Str == NULL) return;
   SLtt_write_string (i ? Start_Alt_Chars_Str : End_Alt_Chars_Str );
}

static void write_attributes (SLtt_Char_Type fgbg)
{
   int bg0, fg0;
   
   if (fgbg == Current_Fgbg) return;
   
   /* Before spitting out colors, fix attributes */
   if ((fgbg & ATTR_MASK) != (Current_Fgbg & ATTR_MASK))
     {
	if (Current_Fgbg & ATTR_MASK)
	  {
	     SLtt_write_string(Norm_Vid_Str);
	  }
	
	if ((fgbg & SLTT_ALTC_MASK) 
	    != (Current_Fgbg & SLTT_ALTC_MASK))
	  {
	     SLtt_set_alt_char_set (fgbg & SLTT_ALTC_MASK);
	  }
	
	if (fgbg & SLTT_ULINE_MASK) SLtt_write_string (UnderLine_Vid_Str);
	if (fgbg & SLTT_BOLD_MASK) SLtt_bold_video ();
	if (fgbg & SLTT_REV_MASK) SLtt_write_string (Rev_Vid_Str);
	if (fgbg & SLTT_BLINK_MASK) SLtt_write_string (Blink_Vid_Str);
     }
   
   if (SLtt_Use_Ansi_Colors)
     {
	fg0 = GET_FG(fgbg);
	bg0 = GET_BG(fgbg);
	tt_printf(Color_Escape_Sequence, fg0, bg0);
     }
   Current_Fgbg = fgbg;
}

static int Video_Initialized;

void SLtt_reverse_video (int color)
{
   SLtt_Char_Type fgbg;
   char *esc;
   
   if (Worthless_Highlight) return;
   if ((color < 0) || (color >= JMAX_COLORS)) return;
   
   if (Video_Initialized == 0)
     {
	if (color == JNORMAL_COLOR)
	  {
	     SLtt_write_string (Norm_Vid_Str);
	  }
	else SLtt_write_string (Rev_Vid_Str);
	Current_Fgbg = 0xFFFFFFFF;
	return;
     }
   
   if (SLtt_Use_Ansi_Colors) 
     {
	fgbg = Ansi_Color_Map[color].fgbg;
	if ((esc = Ansi_Color_Map[color].custom_esc) != NULL)
	  {
	     if (fgbg != Current_Fgbg)
	       {
		  Current_Fgbg = fgbg;
		  SLtt_write_string (esc);
		  return;
	       }
	  }
     }
   else fgbg = Ansi_Color_Map[color].mono;

   if (fgbg == Current_Fgbg) return;
   write_attributes (fgbg);
}



void SLtt_reset_video (void)
{
   SLtt_goto_rc (SLtt_Screen_Rows - 1, 0);
   Cursor_Set = 0;
   SLtt_normal_video ();	       /* MSKermit requires this  */
   SLtt_write_string(Norm_Vid_Str);
   SLtt_set_alt_char_set (0);
   Current_Fgbg = 0xFFFFFFFF;
   Video_Initialized = 0;
   SLtt_flush_output ();
}

void SLtt_normal_video (void)
{
   SLtt_reverse_video(JNORMAL_COLOR);
}

void SLtt_narrow_width (void)
{
    SLtt_write_string("\033[?3l");
}

void SLtt_wide_width (void)
{
    SLtt_write_string("\033[?3h");
}

/* Highest bit represents the character set. */
#define COLOR_OF(x) ((x & 0x7F00) >> 8)
#define COLOR_EQS(a, b) \
   (Ansi_Color_Map[COLOR_OF(a)].fgbg == Ansi_Color_Map[COLOR_OF(b)].fgbg)


#define COLOR_MASK 0x7F00

#define CHAR_EQS(a, b) (((a) == (b))\
			|| ((((a) & ~COLOR_MASK) == ((b) & ~COLOR_MASK))\
			    && COLOR_EQS((a), (b))))


static void send_attr_str(unsigned short *s)
{
   unsigned char out[256], ch, *p;
   register SLtt_Char_Type attr;
   register unsigned short sh;
   int color, last_color = -1;
   
   p = out;
   while (0 != (sh = *s++))
     {
	ch = sh & 0xFF;
	color = (sh & 0xFF00) >> 8;
	if (color != last_color)
	  {
	     if (SLtt_Use_Ansi_Colors) attr = Ansi_Color_Map[color & 0x7F].fgbg;
	     else attr = Ansi_Color_Map[color & 0x7F].mono;
	     
	     if (sh & 0x8000) attr |= SLTT_ALTC_MASK;   /* alternate char set */
	     
	     if (attr != Current_Fgbg)
	       {
		  if ((ch != ' ') ||
		      /* it is a space so only consider it different if it
		       * has different attributes.
		       */
		      (attr & BGALL_MASK) != (Current_Fgbg & BGALL_MASK))
		    {
		       if (p != out)
			 {
			    *p = 0;
			    SLtt_write_string((char *) out);
			    Cursor_c += (int) (p - out);
			    p = out;
			 }
		       if (SLtt_Use_Ansi_Colors && (NULL != Ansi_Color_Map[color & 0x7F].custom_esc))
			 {
			    SLtt_write_string (Ansi_Color_Map[color & 0x7F].custom_esc);
			    Current_Fgbg = attr;
			 }
		       else write_attributes (attr);
		       
		       last_color = color;
		    }
	       }
	  }
	*p++ = ch;
     }
   *p = 0;
   if (p != out) SLtt_write_string((char *) out);
   Cursor_c += (int) (p - out);
}

static void forward_cursor (int n, int row)
{
   char buf[30];
   
   
   if (n <= 4) 
     {
	Cursor_c += n;
	SLtt_normal_video ();
	MEMSET (buf, ' ', n);
	tt_write (buf, n);
     }
   else if (Curs_F_Str != NULL)
     {
	Cursor_c += n;
	n = tt_sprintf(buf, Curs_F_Str, n, 0);
	tt_write(buf, n);
     }
   else SLtt_goto_rc (row, Cursor_c + n);
}


#define SPACE_CHAR (0x20 | (JNORMAL_COLOR << 8))                        

void SLtt_smart_puts(unsigned short *neww, unsigned short *oldd, int len, int row)
{
   register unsigned short *p, *q, *qmax, *pmax, *buf;
   unsigned short buffer[256];
   int n_spaces;
   unsigned short *space_match, *last_buffered_match;
   
   q = oldd; p = neww;
   qmax = oldd + len;
   pmax = p + len;
   
   /* Find out where to begin --- while they match, we are ok */
   for (;;)
     {
	if (q == qmax) return;
	if (!CHAR_EQS(*q, *p)) break;
	q++; p++;
     }

   /*position the cursor */
   SLtt_goto_rc (row, (int) (p - neww));

   /* Find where the last non-blank character on old/new screen is */
   
   while (qmax > q)
     {
	qmax--;
	if (!CHAR_EQS(*qmax, SPACE_CHAR)) 
	  {
	     qmax++;
	     break;
	  }
     }
   
   while (pmax > p)
     {
	pmax--;
	if (!CHAR_EQS(*pmax, SPACE_CHAR))
	  {
	     pmax++;
	     break;
	  }
     }
   
   
   last_buffered_match = buf = buffer;		       /* buffer is empty */
   
   /* loop using overwrite then skip algorithm until done */
   while (1)
     {
	/* while they do not match and we do not hit a space, buffer them up */
	n_spaces = 0;
	while (p < pmax)
	  {
	     if (CHAR_EQS(*q,SPACE_CHAR) && CHAR_EQS(*p, SPACE_CHAR))
	       {
		  /* If *q is not a space, we would have to overwrite it.  
		   * However, if *q is a space, then while *p is also one, 
		   * we only need to skip over the blank field.
		   */
		  space_match = p;
		  p++; q++;
		  while ((p < pmax) 
			 && CHAR_EQS(*q,SPACE_CHAR) 
			 && CHAR_EQS(*p, SPACE_CHAR))
		    {
		       p++;
		       q++;
		    }
		  n_spaces = (int) (p - space_match);
		  break;
	       }
	     if (CHAR_EQS(*q, *p)) break;
	     *buf++ = *p++;
	     q++;
	  }
	*buf = 0;
	
	if (buf != buffer) send_attr_str (buffer);
	buf = buffer;
	
	if (n_spaces && (p < pmax)) 
	  {
	     forward_cursor (n_spaces, row);
	  }
	
	/* Now we overwrote what we could and cursor is placed at position 
	 * of a possible match of new and old.  If this is the case, skip 
	 * some more.
	 */
	
	while ((p < pmax) && CHAR_EQS(*p, *q))
	  {
	     *buf++ = *p++;
	     q++;
	  }
	
	last_buffered_match = buf;
	if (p >= pmax) break;
	
	/* jump to new position is it is greater than 5 otherwise
	 * let it sit in the buffer and output it later.
	 */
	if ((int) (buf - buffer) >= 5) 
	  {
	     forward_cursor (buf - buffer, row);
	     last_buffered_match = buf = buffer;
	  }
     }
   if (buf != buffer)
     {
	if (q < qmax)
	  {
	     if ((buf == last_buffered_match) 
		 && ((int) (buf - buffer) >= 5))
	       {
		  forward_cursor (buf - buffer, row);
	       }
	     else
	       {
		  *buf = 0;
		  send_attr_str (buffer);
	       }
	  }
     }
   if (q < qmax) SLtt_del_eol ();
}  

#if 0   
void SLtt_smart_puts(SLtt_Char_Type *neww,SLtt_Char_Type *oldd, int row, int spc)
{
   SLtt_Char_Type out[250], *mark;
   register SLtt_Char_Type *p, ch, ch1;
   register SLtt_Char_Type *neew = neww, *old = oldd;
   char curs[20];
   int ii,max_len,i, curs_set = 0, curs_len = 0, cc = 0, row1 = row - 1;
   SLtt_Char_Type *new_save;

   
    i = 0;
    ii = 0;
    *curs = 0;
    max_len = Len_Curs_F_Str;
   
   
   /* many times we are scrolling and line to compare is blank.  Treat this
    case special */
   if (spc == 0)
     {
	p = neew;
	while(*p == ' ') p++;
	
	if (*p == 0) return;
	SLtt_goto_rc(row, p - neew + 1);
	old = out;
	ch1 = ' '; 
	while (1)
	  {
	     while (ch = *p++, (ch1 != ch) && ch) *old++ = ch;
	     mark = old;
	     if (!ch) break;
	     *old++ = ch1;
	     while(ch = *p++, (ch == ch1) && ch) *old++ = ch;
	     if (old - mark > max_len)
	       {
		  *mark = 0;
		  send_attr_str(out);
		  if (ch == 0) return;
		  if (mark != out) SLtt_goto_rc(row, p - neew);
		  old = out;
	       }
	     if (!ch) break;
	     p--;
	  }
	*old = 0;
	send_attr_str(out);
	
	if ((Cursor_Set == 0) && !Automatic_Margins) Cursor_Set = -1; 
	goto the_return;
     }
   
   
    /* while they match, go on */
   /* Note that neew - new_save is then column of character */
   new_save = neew + 1;
   while (((ch = *neew++) == *old++) && ch);
   i += neew - new_save;

   if (!ch)
    /* we are at the end of the new, so delete eond of old line */
      {
	 if ((ch1 = *(old - 1)) == ' ')
	   {
	      while (*old++ == ch1);
	      ch1 = *(old - 1);
	   }

	 if (ch1 == 0) return;

	 SLtt_goto_rc(row, i + 1);
	 SLtt_del_eol();
	 return;
      }


    if (i)
      {
	 cc = i + 1;
	 if ((Cursor_Set != 1) || (Cursor_r != row1) || (Cursor_c != cc))
	  curs_len = tt_sprintf(curs, Curs_Pos_Str, row1, i);
	 curs_set = 1;
      }

    while(1)
      {
	 ch1 = 0;
	 p = out;
	 *p++ = ch;
	 while (ch1 = *old++, ch = *neew++, (ch != ch1) && ch) *p++ = ch;
	 mark = p;
         *p++ = ch;
	 if (ch) while (ch = *neew++, ch1 = *old++, (ch == ch1) && ch)
	   {
	      *p++ = ch;
	   }
	 *p = 0;
	 i = p - mark;
	 if (i > max_len)
	   {
	      *mark = 0;
	      if (*curs)
		{
		   if (No_Move_In_Standout) SLtt_normal_video ();
		   tt_write(curs, curs_len);
		   Cursor_Set = 1;  Cursor_r = row1; Cursor_c = cc;
		   *curs = 0;
		}
	      
	      if (!curs_set)
		{
		   SLtt_goto_rc(row, 1);
		   curs_set = 1;
		}

	      send_attr_str(out);
	      
	      if ((Cursor_Set == 0) && !Automatic_Margins) Cursor_Set = -1;
	      if (!ch)
		{
		   if (ch1)
		     {
			old--;  ch = ' ';
			while (ch1 = *old++, (ch1 == ch));
		     }

		   if (ch1 == 0) goto the_return;
		   
		   cc = neew - new_save + 1;
		   if ((Cursor_Set != 1) || (Cursor_r != row1) || (Cursor_c != cc))
		     {
			Cursor_r = row1;
			Cursor_c = cc;

			if (No_Move_In_Standout) SLtt_normal_video ();
			
			if (curs_set && (Curs_F_Str != NULL)) 
			  tt_printf(Curs_F_Str, i, 0);
			else
			  {
			     tt_printf(Curs_Pos_Str, row1, Cursor_c - 1);
			     curs_set = 1;
			  }
			Cursor_Set = 1;
		     }
		   
		   SLtt_del_eol();
		   goto the_return;
		}
	      
	      cc = neew - new_save + 1;
	      if (i && ((Cursor_Set != 1) || 
			(Cursor_r != row1) || (Cursor_c != cc)))
		{
		   if (curs_set && (Curs_F_Str != NULL)) curs_len = tt_sprintf(curs, Curs_F_Str, i, 0);
		   else
		     {
			curs_len = tt_sprintf(curs, Curs_Pos_Str, row1, cc - 1);
			curs_set = 1;
		     }
		}
	   }
	 else
	   {
	      if (*curs)
		{
		   if (No_Move_In_Standout) SLtt_normal_video ();
		   tt_write(curs, curs_len);
		   Cursor_Set = 1; Cursor_r = row1; Cursor_c = cc;
		   *curs = 0;
		}
	      if (!curs_set)
		{
		   SLtt_goto_rc (row, 1);
		   curs_set = 1;
		}
	      
	      send_attr_str(out);
	      if ((Cursor_Set == 0) && !Automatic_Margins) Cursor_Set = -1;
	      
	      if (!ch)
		{
		   if (ch1 == ' ')
		     {
			while (*old == ch1) old++;
			ch1 = *old;
		     }
		   
		   if (ch1) SLtt_del_eol();
		   goto the_return;
		}
	   }
      }
   the_return:
   if (No_Move_In_Standout) SLtt_normal_video ();
}
#endif

/* termcap stuff */

#ifdef unix
#define EXTERN extern
EXTERN char *tgetstr(char *, char **);
EXTERN int tgetent(char *, char *);
EXTERN int tgetnum(char *);
EXTERN int tgetflag(char *);

static char Tbuf[4096];
   
static char *my_tgetstr(char *what, char **p)
{
   register char *w, *w1;
   char *wsave;
   what = tgetstr(what, p);
   if (what != NULL)
     {
	/* lose pad info --- with today's technology, term is a loser if
	   it is really needed */
	while ((*what == '.') || 
	       ((*what >= '0') && (*what <= '9'))) what++;
	if (*what == '*') what++;	
	
	/* lose terminfo padding--- looks like $<...> */
        w = what;
	while (*w) if ((*w++ == '$') && (*w == '<'))
	  {
	     w1 = w - 1;
	     while (*w && (*w != '>')) w++;
	     if (*w == 0) break;
	     w++;
	     wsave = w1;
	     while ((*w1++ = *w++) != 0);
	     w = wsave;
	  }
	if (*what == 0) what = NULL; 
     }
   return(what);
}

static char Tstr_Buf[512];
static int Vt100_Like = 0;

void SLtt_get_terminfo (void)
{
   char *term, *t, ch;
   char *p = Tstr_Buf;
   
   if (NULL == (term = (char *) getenv("TERM")))
     {
	SLang_exit_error("TERM environment variable needs set.");
     }
   if (1 != tgetent(Tbuf, term)) SLang_exit_error("Unknown terminal.");
   
   t = term;
   if (strcmp(t, "vt52") && (*t++ == 'v') && (*t++ == 't')
       && (ch = *t, (ch >= '1') && (ch <= '9'))) Vt100_Like = 1; 
   
   if ((NULL == (Cls_Str = my_tgetstr("cl", &p))) 
       || (NULL == (Curs_Pos_Str = my_tgetstr("cm", &p))))
     {
	SLang_exit_error("Terminal not powerful enough for SLang.");
     }
   
   if ((NULL == (Ins_Mode_Str = my_tgetstr("im", &p)))
       || ( NULL == (Eins_Mode_Str = my_tgetstr("ei", &p)))
       || ( NULL == (Del_Char_Str = my_tgetstr("dc", &p))))
     SLtt_Term_Cannot_Insert = 1;
   
   Visible_Bell_Str = my_tgetstr ("vb", &p);
   Curs_Up_Str = my_tgetstr ("up", &p);
   Rev_Scroll_Str = my_tgetstr("sr", &p);
   Del_N_Lines_Str = my_tgetstr("DL", &p);
   Add_N_Lines_Str = my_tgetstr("AL", &p);
   Scroll_R_Str = my_tgetstr("cs", &p);
   if ((SLtt_Screen_Cols = tgetnum("co")) <= 0) SLtt_Screen_Cols = 80;
   if ((SLtt_Screen_Rows = tgetnum("li")) <= 0) SLtt_Screen_Rows = 24;
   
   if ((Scroll_R_Str == NULL) 
       || (((NULL == Del_N_Lines_Str) || (NULL == Add_N_Lines_Str))
	   && (NULL == Rev_Scroll_Str)))
     SLtt_Term_Cannot_Scroll = 1;
   
   Del_Eol_Str = my_tgetstr("ce", &p);

   Rev_Vid_Str = my_tgetstr("so", &p);
   Bold_Vid_Str = my_tgetstr("md", &p);
   Blink_Vid_Str = my_tgetstr("mb", &p);
   UnderLine_Vid_Str = my_tgetstr("us", &p);
   
   Start_Alt_Chars_Str = my_tgetstr ("as", &p);
   End_Alt_Chars_Str = my_tgetstr ("ae", &p);
   SLtt_Graphics_Char_Pairs = my_tgetstr ("ac", &p);
   
   if (NULL == (Norm_Vid_Str = my_tgetstr("me", &p))) 
     {
	Norm_Vid_Str = my_tgetstr("se", &p);
     }
   
   
   if (NULL != (Curs_F_Str = my_tgetstr("RI", &p)))
     {
	Len_Curs_F_Str = strlen(Curs_F_Str);
     }
   else Len_Curs_F_Str = strlen(Curs_Pos_Str);

   Automatic_Margins = tgetflag ("am");
   No_Move_In_Standout = !tgetflag ("ms");
   
   Worthless_Highlight = tgetflag ("xs");
   
#ifdef linux
   Linux_Console = !strncmp(term, "con", 3);
#endif
}

#endif
/* Unix */

/* specific to vtxxx only */
void SLtt_enable_cursor_keys (void)
{
#ifdef unix
   if (Vt100_Like)
#endif
   SLtt_write_string("\033=\033[?1l");
}

#ifdef VMS
void SLtt_get_terminfo ()
{
   int zero = 0;
   SLtt_set_term_vtxxx(&zero);
}
#endif

/* This sets term for vt102 terminals it parameter vt100 is 0.  If vt100
  is non-zero, set terminal appropriate for a only vt100  
  (no add line capability). */
							   
void SLtt_set_term_vtxxx(int *vt100)
{
   Norm_Vid_Str = "\033[m";
   
   Scroll_R_Str = "\033[%i%d;%dr"; 
   Cls_Str = "\033[2J\033[H";
   Rev_Vid_Str = "\033[7m";
   Bold_Vid_Str = "\033[1m";
   Blink_Vid_Str = "\033[5m";
   UnderLine_Vid_Str = "\033[4m";
   Del_Eol_Str = "\033[K";
   Rev_Scroll_Str = "\033M";
   Curs_F_Str = "\033[%dC";
   Len_Curs_F_Str = 5;
   Curs_Pos_Str = "\033[%i%d;%dH";
   if (*vt100 == 0)
     {
	Ins_Mode_Str = "\033[4h";
	Eins_Mode_Str = "\033[4l";
	Del_Char_Str =  "\033[P";
	Del_N_Lines_Str = "\033[%dM";
	Add_N_Lines_Str = "\033[%dL";
	SLtt_Term_Cannot_Insert = 0;
     }
   else
     {
	Del_N_Lines_Str = NULL;
	Add_N_Lines_Str = NULL;
	SLtt_Term_Cannot_Insert = 1;
     }
   SLtt_Term_Cannot_Scroll = 0;
   No_Move_In_Standout = 0;
}

void SLtt_init_video (void)
{
   /*   send_string_to_term("\033[?6h"); */
   /* relative origin mode */
   SLtt_reset_scroll_region();
   SLtt_end_insert();
   Video_Initialized = 1;
}

void SLtt_bold_video (void)
{
   SLtt_write_string (Bold_Vid_Str);
}
