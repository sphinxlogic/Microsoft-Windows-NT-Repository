#undef False
#undef True
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#undef index
#ifdef hpux
#undef SYSV
#endif

/*
  Colormap declarations.
*/
#define UndefinedColormap  0
#define PrivateColormap  1
#define SharedColormap  2
/*
  Define declarations.
*/
#define MaxNumberFonts  9
#define MaxNumberPens  9
#define WindowBorderWidth  2
#define XSetWindowExtents(window,text,border)  \
  window.width=XTextWidth(window.font_info,text,strlen(text))+  \
    (int) border*window.font_info->max_bounds.width;  \
  window.height=window.font_info->ascent+window.font_info->descent+4;  \
  XResizeWindow(display,window.id,window.width,window.height);
#define XStandardPixel(map,color,shift)  (unsigned long) (map->base_pixel+  \
  ((color.red*map->red_max+(1 << (shift-1))) >> shift)*map->red_mult+  \
  ((color.green*map->green_max+(1 << (shift-1))) >> shift)*map->green_mult+  \
  ((color.blue*map->blue_max+(1 << (shift-1))) >> shift)*map->blue_mult)

/*
  Typedef declarations.
*/
typedef struct _DiversityPacket
{
  unsigned char
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned long
    count;
} DiversityPacket;

typedef struct _XAnnotateInfo
{
  int
    x,
    y;

  unsigned int
    width,
    height;

  double
    degrees;

  XFontStruct
    *font_info;

  char
    *text,
    geometry[2048];

  struct _XAnnotateInfo
    *previous,
    *next;
} XAnnotateInfo;

typedef struct _XPixelInfo
{
  unsigned int
    colors;

  unsigned long
    *pixels;

  XColor
    background_color,
    border_color,
    foreground_color,
    pen_color[MaxNumberPens],
    annotate_color;

  unsigned short
    background_index,
    annotate_index;
} XPixelInfo;

typedef struct _XResourceInfo
{
  unsigned int
    backdrop;

  char
    *background_color,
    *border_color;

  unsigned int
    border_width,
    colormap,
    colorspace,
    debug,
    delay,
    dither;

  char
    *font,
    *font_name[MaxNumberFonts],
    *foreground_color;

  int
    gravity;

  char
    *icon_geometry;

  unsigned int
    iconic;

  char
    *image_geometry;

  unsigned int
    magnify;

  char
    *map_type;

  unsigned int
    monochrome;

  char
    *name;

  unsigned int
    number_colors;

  char
    *pen_color[MaxNumberPens],
    *print_filename,
    *server_name,
    *title;

  unsigned int
    tree_depth;

  char
    *visual_type,
    *write_filename;
} XResourceInfo;

typedef struct _XWindowInfo
{
  Window
    id;

  int
    screen;

  unsigned int
    depth;

  XVisualInfo
    *visual_info;

  XStandardColormap
    *map_info;

  XPixelInfo
    *pixel_info;

  XFontStruct
    *font_info;

  GC
    graphic_context,
    highlight_context;

  Cursor
    cursor,
    busy_cursor;

  char
    *name,
    *geometry,
    *icon_name,
    *icon_geometry,
    *clip_geometry;

  unsigned long
    flags;

  int
    x,
    y;

  unsigned int
    width,
    height,
    min_width,
    min_height,
    width_inc,
    height_inc,
    border_width,
    immutable;

  XImage
    *ximage;

  Pixmap
    pixmap,
    *pixmaps;

  int
    mask;

  XSetWindowAttributes
    attributes;
} XWindowInfo;

typedef struct _XWindows
{
  XWindowInfo
    superclass,
    backdrop,
    icon,
    info,
    image,
    magnify,
    pan,
    popup;
} XWindows;

/*
  X utilities routines.
*/
extern char 
  *XGetResource _Declare((XrmDatabase,char *,char *,char *)),
  *XVisualClassName _Declare((XVisualInfo *));

extern Cursor 
  XMakeInvisibleCursor _Declare((Display *,Window));

extern Image
  *ReadTEXTImage _Declare((char *,char *,char *,char *)),
  *ReadXImage _Declare((char *,char *,unsigned int,unsigned int,unsigned int)),
  *ReadXCImage _Declare((char *,char *,char *));

extern int
  Latin1Compare _Declare((char *,char *)),
  XError _Declare((Display *,XErrorEvent *));

extern unsigned int 
  IsTrue _Declare((char *)),
  XAnnotateImage _Declare((Display *,XWindowInfo *,XAnnotateInfo *,
    unsigned int,Image *)),
  XMakeImage _Declare((Display *,XResourceInfo *,XWindowInfo *,Image *,
    unsigned int,unsigned int)),
  XMakePixmap _Declare((Display *,XResourceInfo *,XWindowInfo *)),
  XPopupMenu _Declare((Display *,XWindowInfo *,int,int,char *,char **,
    unsigned int,char *));

extern XVisualInfo 
  *XBestVisualInfo _Declare((Display *,char *,char *,XStandardColormap *));

extern void 
  XBestIconSize _Declare((Display *,XWindowInfo *,Image *)),
  XFreeStandardColormap _Declare((Display *,XVisualInfo *,XPixelInfo *,
    XStandardColormap *)),
  XGetAnnotateInfo _Declare((XAnnotateInfo *)),
  XGetPixelInfo _Declare((Display *,XVisualInfo *,XStandardColormap *,
    XResourceInfo *,Image *,XPixelInfo *)),
  XGetResourceInfo _Declare((XrmDatabase,char *,XResourceInfo *)),
  XGetWindowInfo _Declare((XWindowInfo *,XWindowInfo *)),
  XMakeStandardColormap _Declare((Display *,XVisualInfo *,XResourceInfo *,
    XPixelInfo *,Image *,XStandardColormap *)),
  XMakeWindow _Declare((Display *,Window,char **,int,XClassHint *,XWMHints *,
    Atom,XWindowInfo *)),
  XPopupAlert _Declare((Display *,XWindowInfo *,char *,char *)),
  XPopupQuery _Declare((Display *,XWindowInfo *,char *,char *)),
  XRefreshWindow _Declare((Display *,XWindowInfo *,XEvent *));

extern Window 
  XWindowByID _Declare((Display *,Window,unsigned long)),
  XWindowByName _Declare((Display *,Window,char *));

extern XFontStruct 
  *XBestFont _Declare((Display *,XResourceInfo *,char *,unsigned int));

/*
  Invoke pre-X11R5 ICCCM routines if XlibSpecificationRelease is not defined.
*/
#ifndef XlibSpecificationRelease
#define PRE_R5_ICCCM
#endif
/*
  Invoke pre-X11R4 ICCCM routines if PWinGravity is not defined.
*/
#ifndef PWinGravity
#define PRE_R4_ICCCM
#endif
#include "PreRvIcccm.h"
