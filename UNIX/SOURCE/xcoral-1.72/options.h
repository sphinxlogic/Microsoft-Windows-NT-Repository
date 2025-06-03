/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <X11/Xlib.h>

/*
 *	Les options utilisables par le programme.
 *	La geometrie, les fontes, les couleurs etc.
 */
typedef struct {
	int 		x, y, width, height;
	unsigned long 	fg, bg;
	unsigned long	text_top_shadow, text_bottom_shadow;
	XFontStruct 	*text_font;
	unsigned long 	mfg, mbg;
	unsigned long	menu_top_shadow, menu_bottom_shadow;
	XFontStruct	*menu_font;
	unsigned long 	dfg, dbg;
	unsigned long	dialog_top_shadow, dialog_bottom_shadow;
	unsigned long 	bd;
	unsigned int 	bw;
   	int 		verb;
	char filename [512];
} Options;

/*
 *	Public
 */
extern void		ParseOpenDisp ( /* argc, argv */ );
extern void		GetUserDatabase ();
extern void		MergeOptions ();
extern XFontStruct	*GetOpFont ( /* type */ );
extern unsigned long 	GetOpColor ( /* type */ );
extern char 		*GetOpFilename ();
extern unsigned int 	GetOpBW ();
extern unsigned long	GetOpBD ();
extern int 		GetOpGeo ( /* type */ );
extern unsigned long 	PixelValue ( /* display, str, result */ );

#define OP_TEXT_FG	1
#define OP_TEXT_BG	2
#define OP_TEXT_TS	3
#define OP_TEXT_BS	4
#define OP_TEXT_FONT	5

#define OP_MENU_FG	6
#define OP_MENU_BG	7
#define OP_MENU_TS	8
#define OP_MENU_BS	9
#define OP_MENU_FONT	10

#define OP_DIAL_FG	11
#define OP_DIAL_BG	12
#define OP_DIAL_TS	13
#define OP_DIAL_BS	14
#define OP_DIAL_FONT	15

#define OP_X		16
#define OP_Y		17
#define OP_WIDTH	18
#define OP_HEIGHT	19

#endif  /* _OPTIONS_H_ */

