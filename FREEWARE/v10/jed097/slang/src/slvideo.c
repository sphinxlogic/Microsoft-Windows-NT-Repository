/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>

#ifdef msdos
#include <conio.h>
#include <bios.h>
#include <mem.h>
#endif

#ifdef __GO32__
#include <pc.h>
#define GO32_DPMI
#endif

#ifdef __os2__
#define INCL_BASE
#define INCL_NOPM
#define INCL_VIO
#define INCL_KBD
#include <os2.h>
#endif

#include <dos.h>

#include "config.h"
#include "slang.h"

int SLtt_Term_Cannot_Insert;
int SLtt_Term_Cannot_Scroll;
int SLtt_Ignore_Beep = 3;
int SLtt_Use_Ansi_Colors;

int SLtt_Screen_Rows = 25;
int SLtt_Screen_Cols = 80;

static int Attribute_Byte = 0;
static int Scroll_r1 = 0, Scroll_r2 = 25, Cursor_Row = 1, Cursor_Col = 1;
static int One60;
static int Msdos_Insert_Mode = 0;
static int Rev_Vid_Flag;
static int IsColor = 0;
static int Blink_Killed;	       /* if high intensity background 
					* enabled.
					*/

#ifndef GO32_DPMI
static unsigned char *Video_Base;
#endif

#ifdef msdos
int SLtt_Msdos_Cheap_Video = 0;
static int Video_Status_Port;

#define SNOW_CHECK\
   if (SLtt_Msdos_Cheap_Video)\
     { while ((inp(0x3DA) & 0x08)); while (!(inp(0x3DA) & 0x08)); }

struct SREGS SegRegs;
#endif

typedef struct 
{
   int fg, bg;
} Ansi_Color_Type;

#define JMAX_COLORS 16
#define JNORMAL_COLOR 0

static Ansi_Color_Type Ansi_Color_Map[JMAX_COLORS] = 
{
   {7, 0}, {0, 7}, {0, 7}, {0, 7}, {0, 7},
   {7,0},  			       /* operator */
   {7,0},			       /* numbers */
   {7,0},			       /* strings */
   {7,0},			       /* comments */
   {7,0},			       /* keyword */
   {7,0},			       /* delimeters */
   {7,0},			       /* preprocess */
   {7,0},			       /* preprocess */
   {7,0},			       /* preprocess */
   {7,0},			       /* preprocess */
   {7,0}			       /* preprocess */
};



void SLtt_write_string (char *s)
{
   fputs(s, stdout);
}


void SLtt_set_scroll_region(int r1, int r2)
{
    Scroll_r1 = r1;
    Scroll_r2 = r2;
}

void SLtt_reset_scroll_region(void)
{
    Scroll_r1 = 0;
    Scroll_r2 = SLtt_Screen_Rows;
}

void SLtt_goto_rc(int r, int c)
{
#ifdef __GO32__
   if (r > SLtt_Screen_Rows) r = SLtt_Screen_Rows; 
   if (c > SLtt_Screen_Cols) c = SLtt_Screen_Cols; 
   ScreenSetCursor(Scroll_r1 + r, c);
   Cursor_Row = r;
   Cursor_Col = c;
#else
# ifdef __os2__
   if (r > SLtt_Screen_Rows) r = SLtt_Screen_Rows;
   if (c > SLtt_Screen_Cols) c = SLtt_Screen_Cols;
   VioSetCurPos(Scroll_r1 + r, c, 0);
   Cursor_Row = r;
   Cursor_Col = c;
# else
   /* if (r > SLtt_Screen_Rows - 1) r = SLtt_Screen_Rows - 1; */
   asm mov ax, r
   asm mov bx, SLtt_Screen_Rows
   asm dec bx
   asm cmp ax, bx
   asm jle L1
   asm mov ax, bx
   L1:
   /* if (c > SLtt_Screen_Cols - 1) c = SLtt_Screen_Cols - 1; */
   asm mov cx, SLtt_Screen_Cols
   asm dec cx
   asm mov bx, c
   asm cmp bx, cx
   asm jle L2
   asm mov bx, cx
   L2:
   asm mov Cursor_Row, ax
   asm mov Cursor_Col, bx
   
   asm add ax, Scroll_r1
   asm xor dx, dx
   asm mov dh, al
   asm mov dl, bl
   asm xor bx, bx
   asm mov ax, 0x200
   asm int 0x10
# endif /* __os2__ */
#endif
}

static void wherexy(void)
{
#ifdef __GO32__
   ScreenGetCursor(&Cursor_Row, &Cursor_Col);
#else
# ifdef __os2__
   VioGetCurPos((USHORT*) &Cursor_Row, (USHORT*) &Cursor_Col, 0);
# else
   
   asm mov ah, 3
   asm mov bh, 0
   asm int 10h
   asm xor ax, ax
   asm mov al, dh
   asm mov Cursor_Row, ax
   asm xor ax, ax
   asm mov al, dl
   asm mov Cursor_Col, ax
#endif /* __os2__ */
#endif
}

#ifdef __os2__
# define MAXCOLS 256
static char tempBuffer [MAXCOLS*2];
#endif

#ifdef GO32_DPMI
static void go32_dpmi_viowrite_n(unsigned short sh, int n, int r, int c)
{
   int ch, at, cmax = c + n;
   ch = sh & 0xFF; at = sh >> 8;
   
   while (c < cmax) ScreenPutChar(ch, at, c++, r);
}

static void go32_dpmi_viowrite_str(unsigned char *s, int n, int a, 
				 int r, int c)
{
   unsigned char *smax = s + n;
   while (s < smax) ScreenPutChar((int) *s++, a, c++, r);
}
#endif

void SLtt_begin_insert(void)
{
#ifndef GO32_DPMI
#ifdef __GO32__
   unsigned short *pmin;
#endif
   unsigned short *p; 
   int n;
   
   Msdos_Insert_Mode = 1;
   wherexy();
   
   /* move the stuff over 1 column */
   n = SLtt_Screen_Cols - Cursor_Col;
#ifdef __os2__
    n = 2 * (n - 1);
    VioReadCellStr (tempBuffer, (USHORT*) &n, Cursor_Row, Cursor_Col, 0);
    VioWrtCellStr (tempBuffer, n, Cursor_Row, Cursor_Col + 1, 0);
#else   
   
   p = (unsigned short *) (Video_Base + ((One60 * Cursor_Row
					  + 2 * (SLtt_Screen_Cols - 1))));
#ifdef __GO32__
   pmin = (unsigned short *) (Video_Base +
			      (One60 * Cursor_Row +
			       2 * Cursor_Col));
   
   while(p-- > pmin) *(p + 1)= *p; 
#else
   SNOW_CHECK;
   asm mov ax, ds
   asm mov bx, di
   asm mov dx, si
   
   asm mov cx, n
   asm les di, p
   asm lds si, p
   asm sub si, 2
   asm std
   asm rep movsw

   asm mov ds, ax
   asm mov di, bx
   asm mov si, dx
#endif
#endif  /* __os2__ */
#endif /* GO32_DPMI */
}

void SLtt_end_insert(void)
{
   Msdos_Insert_Mode = 0;
}

void SLtt_delete_char()
{
#ifndef GO32_DPMI
#ifdef __GO32__
   register unsigned short *p1;
#endif
   unsigned short *p;
   int n;
   
   wherexy();
   /* move the stuff over 1 column */
#ifdef __os2__
    n = 2 * (SLtt_Screen_Cols - Cursor_Col - 1);
    VioReadCellStr (tempBuffer, (USHORT*)&n, Cursor_Row, Cursor_Col + 1, 0);
    VioWrtCellStr (tempBuffer, n, Cursor_Row, Cursor_Col, 0);
    return;
#else
   
   p = (unsigned short *) (Video_Base +  
			   (One60 * Cursor_Row  + 2 * Cursor_Col));
   n = SLtt_Screen_Cols - Cursor_Col - 1;
   
#ifdef __GO32__
    while(n--)
      {
	 p1 = p + 1;
	 *p = *p1;
	  p++;
      }
#else
   
   SNOW_CHECK;
   asm mov ax, ds
   asm mov bx, si
   asm mov dx, di
   
   asm mov cx, n
   asm les di, p
   asm lds si, p
   asm add si, 2
   asm cld
   asm rep movsw
   
   asm mov ds, ax
   asm mov si, bx
   asm mov di, dx
#endif
#endif /* __os2__ */
#endif /* not GO32_DPMI */   
}

/* This function is ONLY called when exiting editor.  It sets attribute byte
   to Black & White */
void SLtt_erase_line(void)
{
#ifdef __os2__
   USHORT w;
   Attribute_Byte = 7;
   w = (Attribute_Byte << 8) | 32;
   VioWrtNCell ((BYTE*)&w, SLtt_Screen_Cols, Cursor_Row, 0, 0);
#else
#ifdef GO32_DPMI
   unsigned short w = (7 << 8) | 32;
   Attribute_Byte = 7;
   go32_dpmi_viowrite_n (w, SLtt_Screen_Cols, Cursor_Row, 0);
#else
   
   unsigned short *p = (unsigned short *) (Video_Base + One60 * Cursor_Row);
   unsigned short w;
#ifdef __GO32__
   register unsigned short *pmax = p + SLtt_Screen_Cols;
   
   Attribute_Byte = 7;
   w = (Attribute_Byte << 8) | 32;
   while (p < pmax) *p++ = w;
#else
   Attribute_Byte = 7;
   w = (Attribute_Byte << 8) | 32;
   SNOW_CHECK;
   asm mov dx, di
   
   asm mov ax, w
   asm mov cx, SLtt_Screen_Cols
   asm les di, p
   asm cld
   asm rep stosw
   
   asm mov di, dx
#endif  /* __GO32__ */
#endif  /* GO32_DPMI */
#endif  /* __os2__ */
   Rev_Vid_Flag = -1;		       /* since we messed with attribute byte */
}

void SLtt_delete_nlines(int n)
{
#ifdef __os2__
   char Cell [2];
   SLtt_normal_video ();
   Cell [0] = ' ';
   Cell [1] = Attribute_Byte;
   VioScrollUp (Scroll_r1, 0, Scroll_r2, SLtt_Screen_Cols-1, n, Cell, 0);
#else   
#ifdef __GO32__
   union REGS in;
   SLtt_normal_video();
   in.x.ax = n;
   in.x.cx = 0;
   in.h.ah = 6;
   in.h.ch = Scroll_r1;
   in.h.dl = SLtt_Screen_Cols - 1;
   in.h.dh = Scroll_r2;
   in.h.bh = Attribute_Byte;
   int86(0x10, &in, &in);
#else
   SLtt_normal_video ();
   /* This has the effect of pulling all lines below it up */
   asm mov ax, n
   asm mov ah, 6		       /* int 6h */
   asm xor cx, cx
   asm mov ch, byte ptr Scroll_r1
   asm mov dx, SLtt_Screen_Cols
   asm dec dx
   asm mov dh, byte ptr Scroll_r2
   asm mov bh, byte ptr Attribute_Byte
   asm int 10h
#endif
#endif /* __os2__ */
}


void SLtt_reverse_index(int n)
{
#ifdef __os2__
   char Cell [2];
   SLtt_normal_video ();
   Cell [0] = ' ';
   Cell [1] = Attribute_Byte;
   VioScrollDn (Scroll_r1, 0, Scroll_r2, SLtt_Screen_Cols-1, n, Cell, 0);
#else   
#ifdef __GO32__
   union REGS in;
   SLtt_normal_video();
   in.h.al = n;
   in.x.cx = 0;
   in.h.ah = 7;
   in.h.ch = Scroll_r1;
   in.h.dl = SLtt_Screen_Cols - 1;
   in.h.dh = Scroll_r2;
   in.h.bh = Attribute_Byte;
   int86(0x10, &in, &in);
#else
   SLtt_normal_video();
   asm xor cx, cx
   asm mov ch, byte ptr Scroll_r1
   asm mov dx, SLtt_Screen_Cols
   asm dec dx
   asm mov dh, byte ptr Scroll_r2
   asm mov bh, byte ptr Attribute_Byte
   asm mov ah, 7
   asm mov al, byte ptr n
   asm int 10h
#endif
#endif /* __os2__ */
}

static void invert_display (void)
{
#ifdef msdos
   register unsigned short *pmin, *pmax, sh;
   register unsigned short ch;
   
   pmin = (unsigned short *) Video_Base;
   pmax = pmin + One60 * SLtt_Screen_Rows;
   while (pmin < pmax)
     {
	sh = *pmin;
	ch = sh;
	ch = ch ^ 0xFF00;
	*pmin++ = (ch & 0xFF00) | (sh & 0x00FF);
     }
#else
#ifdef __os2__
   int row, col;
   USHORT length = SLtt_Screen_Cols * 2;
   PCH CellStr = tempBuffer;

   for (row = 0; row < SLtt_Screen_Rows; row++) {
      VioReadCellStr(CellStr, &length, row, 0, 0);
     for (col = 1; col < length; col += 2)
       CellStr[col] ^= 0xff;
     VioWrtCellStr(CellStr, length, row, 0, 0);
   }
#else 
#ifdef __GO32__
   unsigned char buf[2 * 180 * 80];         /* 180 cols x 80 rows */
   unsigned char *bmax, *b;
   
   b = buf + 1;
   bmax = b + 2 * SLtt_Screen_Rows * SLtt_Screen_Cols;
   
   ScreenRetrieve (buf);
   while (b < bmax)
     {
	*b ^= 0xFF;
	b += 2;
     }
   ScreenUpdate(buf);
#endif
#endif
#endif
}

void SLtt_beep (void)
{
   int see, hear;
   
   if (SLtt_Ignore_Beep == 0) return;
   
   hear = (SLtt_Ignore_Beep & 1);   
   see = (SLtt_Ignore_Beep & 2);

#ifdef __os2__
   if (see) invert_display ();
   if (hear) DosBeep(1500, 100); else DosSleep(100);
   if (see) invert_display ();
#else
   if (hear) sound(1500);
   if (see) invert_display ();
   delay(100);
   if (see) invert_display ();
   if (hear) nosound();
#endif
}

void SLtt_del_eol(void)
{
#ifdef __os2__
   int n = SLtt_Screen_Cols - Cursor_Col;
   USHORT w;
 
   if (Rev_Vid_Flag != -1) SLtt_normal_video ();
   w = (Attribute_Byte << 8) | 32;
   VioWrtNCell ((BYTE*)&w, n, Cursor_Row, Cursor_Col, 0);
#else
#ifdef GO32_DPMI
   int n = SLtt_Screen_Cols - Cursor_Col;
   unsigned short w;
   if (Rev_Vid_Flag != -1) SLtt_normal_video ();
   w = (Attribute_Byte << 8) | 32;
   go32_dpmi_viowrite_n (w, n, Cursor_Row, Cursor_Col);
#else
   unsigned short *p = (unsigned short *) (Video_Base +  (One60 * Cursor_Row + 2 * Cursor_Col));
   int n = SLtt_Screen_Cols - Cursor_Col;
   unsigned short w;
#ifdef __GO32__
   unsigned short *pmax = p + n;
   
   if (Rev_Vid_Flag != -1) SLtt_normal_video ();
   w = (Attribute_Byte << 8) + (unsigned short) ' ';
   while (p < pmax) *p++ = w;
#else
   
   if (Rev_Vid_Flag != -1) SLtt_normal_video ();
   w = (Attribute_Byte << 8) + (unsigned short) ' ';
   SNOW_CHECK;
   asm mov dx, di
   
   asm les di, p
   asm mov ax, w
   asm mov cx, n
   asm cld
   asm rep stosw
   
   asm mov di, dx
#endif
#endif /* GO32_DMPI */
#endif /* __os2__ */
}

static void SLtt_reverse_video(int color)
{
   Attribute_Byte = Ansi_Color_Map[color].fg | (Ansi_Color_Map[color].bg << 4);
   Rev_Vid_Flag = color;
}

void SLtt_normal_video(void)
{
   SLtt_reverse_video(JNORMAL_COLOR);
}

#define CGA_STATUS 0x3DA
#define CGA_SETMODE 0x3D8

static unsigned short *video_write(register unsigned char *pp, 
				   register unsigned char *p,
				   register unsigned short *pos)
{
#ifdef __os2__
   int n = p - pp;
   int attr = Attribute_Byte;
   VioWrtCharStrAtt ((CHAR*)pp, n, Cursor_Row, *pos, (BYTE*)&attr, 0);
   *pos += n;
   return pos;
#else
#ifdef GO32_DPMI
   int n = p - pp;
   go32_dpmi_viowrite_str (pp, n, Attribute_Byte, 
			   Cursor_Row,  (int) *pos);
   *pos += n;
   return pos;
#else
#ifdef __GO32__
   while (pp < p)
     {
	*pos++ = (Attribute_Byte << 8) + (unsigned short) (*pp++);
     }
   return pos;
#else
   int n = (int) (p - pp);
   
   asm push si
   asm push ds
   asm push di
   
   /* set up register for BOTH fast and slow */
   asm mov bx, SLtt_Msdos_Cheap_Video
   
   /* These are the registers needed for both fast AND slow */
   asm mov ah, byte ptr Attribute_Byte
   asm mov cx, n
   asm lds si, dword ptr pp
   asm les di, dword ptr pos
   asm cld
   
   asm cmp bx, 0		       /* cheap video test */
   asm je L_fast
   asm mov bx, ax
   asm mov dx, CGA_STATUS
   asm jg L_slow_blank
   
   /* slow video */
   asm cli
   /* wait for retrace */
   L_slow:
   asm in al, dx
   asm test al, 1
   asm jnz L_slow
   
   L_slow1:
   asm in al, dx
   asm test al, 1
   asm jz L_slow1
   
   /* move a character out */
   asm mov ah, bh
   asm lodsb
   asm stosw
   asm loop L_slow
   
   asm sti
   asm jmp done
   
/* -------------- slow video, vertical retace and pump --------------*/
   L_slow_blank:
   L_slow_blank_loop:
   asm in al, dx
   asm test al, 8
   asm jnz L_slow_blank_loop
   
   L_slow_blank1:
   asm in al, dx
   asm test al, 8
   asm jz L_slow_blank1
   
   /* write line */
   asm mov ah, bh
   L_slow_blank2:
   asm lodsb
   asm stosw
   asm loop L_slow_blank2
   
   asm jmp done
/*-------------- Fast video --------------*/   
   
   L_fast:
   asm lodsb
   asm stosw
   asm loop L_fast

   done:
   
   asm pop di
   asm pop ds
   asm pop si
   return (pos + n);
#endif
#endif  /* GO32_DPMI */
#endif  /* __os2__ */
}

static void write_attributes(unsigned short *s, int row, int len)
{
   unsigned char out[250], ch, attr;
   register unsigned char *p;
   register unsigned short sh, *pos;
   unsigned short *smax;
#if defined(__os2__) || defined(GO32_DPMI)
    short Col;
#endif
   
   Cursor_Row = row;
   Cursor_Col = 0;
#if !defined(__os2__) && !defined(GO32_DPMI)
     pos = (unsigned short *) (Video_Base +  (One60 * Cursor_Row));
#else
    Col = 0;
    pos = &Col;
#endif
   
   smax = s + len;
   p = out;
   while (s < smax)
     {
	sh = *s++;
	ch = sh & 0xFF;
	attr = sh >> 8;
	if (attr != Rev_Vid_Flag)
	  {
	     if (p != out)
	       {
		  pos = video_write(out, p, pos);
		  p = out;
	       }
	     SLtt_reverse_video(attr);
	  }
	*p++ = ch;
     }
   pos = video_write(out, p, pos);
}


   

void SLtt_smart_puts(unsigned short *new_string, unsigned short *dummy, int len, int row)
{
#ifndef __GO32__
   (void) dummy;
#endif
   Cursor_Row = row;
   Cursor_Col = 0;
   write_attributes(new_string, row, len);
}

void SLtt_reset_video(void)
{
   SLtt_goto_rc (SLtt_Screen_Rows - 1, 0);
   Attribute_Byte = 0x7;
   Rev_Vid_Flag = -1;
}


#if 0
void wide_width(void)
{
}

void narrow_width(void)
{
}
#endif

void SLtt_cls(void)
{
#ifdef __os2__
   char Cell [2];
#endif
   
   SLtt_normal_video();
#ifdef __os2__
   Cell [0] = ' ';
   Cell [1] = Attribute_Byte;
   VioScrollUp (0, 0, -1, -1, -1, Cell, 0);
   return;
#else
#ifdef __GO32__
   SLtt_reset_scroll_region ();
   SLtt_goto_rc (0, 0);
   SLtt_delete_nlines(SLtt_Screen_Rows);
   
#else
   asm mov dx, SLtt_Screen_Cols
   asm dec dx
   asm mov ax, SLtt_Screen_Rows
   asm dec ax
   asm mov dh, al
   asm xor cx, cx
   asm xor ax, ax
   asm mov ah, 7
   asm mov bh, byte ptr Attribute_Byte
   asm int 10h
#endif
#endif /* __os2__ */
}

/* This function is called assuming that cursor is in correct 
   position */
void SLtt_putchar(char ch)
{
#ifndef GO32_DPMI
   unsigned short p, *pp;
#endif
   if (Rev_Vid_Flag) SLtt_normal_video();
   wherexy();
   switch(ch)
     {
      case 7: SLtt_beep(); break;
      case 8: SLtt_goto_rc(Cursor_Row, Cursor_Col - 1); break;
      case 13: SLtt_goto_rc(Cursor_Row, 0); break;
	
      default: 
#ifdef __os2__
 	VioWrtCharStrAtt (&ch, 1, Cursor_Row, Cursor_Col,
 			  (BYTE*)&Attribute_Byte, 0);
#else
#ifdef GO32_DPMI
	go32_dpmi_viowrite_str(&ch, 1, Attribute_Byte, Cursor_Row, Cursor_Col);
#else
	pp = (unsigned short *) (Video_Base + (One60 * Cursor_Row));
	pp += Cursor_Col;
	p = (Attribute_Byte << 8) | (unsigned char) ch;
#ifndef __GO32__
	SNOW_CHECK;
#endif
	*pp = p;
#endif  /* GO32_DPMI */
#endif  /* __os2__ */
	SLtt_goto_rc(Cursor_Row, Cursor_Col + 1);
     }
}

static char *pc_colors [16] =
{
   "black", "blue", "green", "cyan", "red", "magenta", "brown", "lightgray",
   "gray", "brightblue", "brightgreen", "brightcyan", "brightred", 
   "brightmagenta", "yellow", "white" 
};

void SLtt_set_color (int obj, char *what, char *fg, char *bg)
{
   int i, f = -1, g = -1;
   
   if (!IsColor) return;
   if (obj == -1) 
     {
	(void) what;
	return;
     }
   
   
   i = 0; while (i < 16)
     {
	if (!strcmp(fg, pc_colors[i]))
	  {
	     f = i;
	     break;
	  }
	i++;
     }
   i = 0; while (i < 16)
     {
	if (!strcmp(bg, pc_colors[i]))
	  {
	     if (Blink_Killed == 0) g = i & 0x7; else g = i;
	     break;
	  }
	i++;
     }
   if ((f == -1) || (g == -1) || (f == g)) return;
   Ansi_Color_Map[obj].fg = f;
   Ansi_Color_Map[obj].bg = g;
}

void SLtt_get_terminfo ()
{
}


#ifdef __os2__
static void set_color_from_attribute(int attribute)
{
  char *fg = pc_colors[attribute & 0xf];
  char *bg = pc_colors[attribute >> 4];

  SLtt_set_color(JNORMAL_COLOR, NULL, fg, bg);
}
#endif

void SLtt_init_video (void)
{
#ifdef __GO32__
   union REGS in, out;
#else
   unsigned char *p;
#endif
   Cursor_Row = Cursor_Col = 0;
   /* One60 = SLtt_Screen_Cols < 80 ?  80: SLtt_Screen_Cols; */
   One60 = SLtt_Screen_Cols;
   One60 *= 2;
   
#ifdef __os2__
   IsColor = 1;			       /* is it though?? */
   {
     /* Enable high-intensity background colors */
     VIOINTENSITY RequestBlock;
     RequestBlock.cb = sizeof(RequestBlock);
     RequestBlock.type = 2; RequestBlock.fs = 1;
     VioSetState(&RequestBlock, 0);  /* nop if !fullscreen */
     Blink_Killed = 1;
   }

   if (Attribute_Byte == 0) 
     {
	/* find the attribute currently under the cursor */
	USHORT Length = 2, Row, Col;
	char CellStr [2];
	VioGetCurPos (&Row, &Col, 0);
	VioReadCellStr(CellStr, &Length, Row, Col, 0);
	Attribute_Byte = CellStr[1];
	set_color_from_attribute(Attribute_Byte);
     }
   
#else 
#ifndef __GO32__
   p = (unsigned char far *) 0x00400049L;

   Video_Status_Port = 0x3DA;
   
   if (*p == 7)
     {
	Video_Status_Port = 0x3BA;
	Video_Base = (unsigned char *) MK_FP(0xb000,0000);
     }
   else 
     {
	Video_Base = (unsigned char *) MK_FP(0xb800,0000);
	IsColor = 1;
     }
   

   /* test for video adapter type.  Of primary interest is whether there is
    * snow or not.  Assume snow if the card is color and not EGA or greater.
    */

   /* Use Ralf Brown test for EGA or greater */
   asm mov ah, 0x12
   asm mov bl, 0x10
   asm mov bh, 0xFF
   asm int 10h
   asm cmp bh, 0xFF
   asm je L1
   
   /* (V)EGA */
   asm xor bx, bx
   asm mov SLtt_Msdos_Cheap_Video, bx
   asm mov ax, Attribute_Byte
   asm cmp ax, bx
   asm jne L2
   asm mov ax, 0x17
   asm mov Attribute_Byte, ax
   asm jmp L2
   
   L1:
   /* Not (V)EGA */
   asm mov ah, 0x0F
   asm int 10h
   asm cmp al, 7
   asm je L3
   asm mov ax, 1
   asm mov SLtt_Msdos_Cheap_Video, ax
   L3:
   asm mov ax, Attribute_Byte
   asm cmp ax, 0
   asm jne L2
   asm mov ax, 0x07
   asm mov Attribute_Byte, ax
   
   L2:
#else
#ifndef GO32_DPMI
   Video_Base = (unsigned char *) ScreenPrimary;
#endif
   if (Attribute_Byte == 0) Attribute_Byte = 0x17;
   IsColor = 1;			       /* is it though?? */
#endif
#endif  /* __os2__ */
   
#ifdef msdos
   /* toggle the blink bit so we can use hi intensity background */
   if (IsColor && !SLtt_Msdos_Cheap_Video)
     {
	asm mov ax, 0x1003
	asm mov bx, 0
	asm int 0x10
	Blink_Killed = 1;
     }
#endif
#ifdef __GO32__
#ifdef GO32_DPMI
   SLtt_Term_Cannot_Insert = 1;
#endif
   if (IsColor)
     {
	in.x.ax = 0x1003; in.x.bx = 0;
	int86(0x10, &in, &out);
	Blink_Killed = 1;
     }
#endif
}

int SLtt_flush_output (void)
{
   fflush (stdout);
   return -1;
}
