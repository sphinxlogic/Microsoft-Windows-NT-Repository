/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#ifndef MIN
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(a)		((a > 0) ? (a) : 0 - (a))
#endif

#ifdef __STDC__
#define CONCAT(a,b)	a##b
#else
#define CONCAT(a,b)	a/**/b
#endif

#ifdef AIXV3
#ifdef NULL
#undef NULL
#endif /* NULL */
#define NULL 0
#endif /* AIXV3 */

typedef void	(*PaintMenuCallback)();

typedef struct {
	char			*name;
	PaintMenuCallback	callback;
	void			*data;
	int			flags;
} PaintMenuItem;

#define MF_NONE		0x00
#define MF_CHECK	0x01
#define MF_CHECKON	0x01|0x02

typedef struct {
	char		*name;
	int		nitems;
	PaintMenuItem	*items;
} PaintMenuBar;

extern struct Global_s {
	GC		gcx;
	struct {
		Colormap	cmap;
		int		width, height;
		Pixmap		pix, mask;
	} region;
	XtAppContext	appContext;
} Global;

typedef	void		*(*OperationFunc)(Widget, ...);

typedef OperationFunc	Operation_t;

extern Widget 		MenuBarCreate(Widget, int, PaintMenuBar[]);

#ifdef DEFINE_GLOBAL
#define EXTERN(var, val)	var = val ;
#else
#define EXTERN(var, val)	extern var ;
#endif

#define FT_XBM	1
#define FT_PBM	2

EXTERN(Operation_t *CurrentOp, NULL)
EXTERN(Pixmap CheckBitmap,None)
#ifdef DEFINE_GLOBAL
struct Global_s Global;
#endif

extern void 	BrushSelect(Widget);
extern void 	FontSelect(Widget, Widget);
extern void	GraphicCreate(Widget, int);
extern void	ToggleSetPattern(Widget, Widget);
extern void	SizeSelect(Widget, Widget, void (*)(Widget,int,int,int));
extern void	GetFileName(Widget, int, char *, XtCallbackProc);

extern void	StdSaveFile(Widget, XtPointer, XtPointer);
extern void	StdSaveRegionFile(Widget, XtPointer, XtPointer);

#define XYtoRECT(x1,y1,x2,y2,rect)					\
			(rect)->x = MIN(x1,x2); 			\
			(rect)->y = MIN(y1,y2);				\
			(rect)->width = MAX(x1,x2) - (rect)->x + 1;	\
			(rect)->height = MAX(y1,y2) - (rect)->y + 1;
