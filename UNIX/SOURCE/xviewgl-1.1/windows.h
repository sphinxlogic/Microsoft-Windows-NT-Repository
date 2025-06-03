#ifndef WINDOWS_INCLUDED
#define WINDOWS_INCLUDED
#include "patchlevel.h"

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "effects.h"
#include "video.h"

#define NPAGES 2

typedef
    struct {
	int	    xloc, yloc;
        unsigned long int Width, Height;
        unsigned int maxcolor;
        unsigned int maxcells;
        int         cscalemax;
        int         pixwid;
        int         pixht;
	int	    xcmult;
	int	    ycmult;
	int	    xoffs;
	int	    yoffs;
        Display     *theDisp;
        Colormap    theCmap;
        GC          xorGC;
        GC          clearbitsGC;
        GC          overlayGC;
        GC          clearGC;
        GC          copyGC;
        GC          createGC;
        GC          tileGC;
        GC          drawGC;
        GC          edgeGC;
        int         theScreen;
        int         theDepth;
	unsigned long pixvals[MAXCOLOR];
	unsigned long planes;
	unsigned long fcol, bcol;
        Visual      *theVisual;
        Window      RootW;
        Window      theWindow;
	Pixmap	    thePix;
	Pixmap	    winPix;
	Pixmap	    page[NPAGES];
	byte	    current_mode;
    } WINDOW;

typedef struct CMap_struct { /* Sparse colormap */
    XColor clist[MAXCOLOR];
    int cindx[MAXCOLOR];
    int ncolors;
    int last_upd;
} *CMap;

extern WINDOW MainWindow;
extern CMap current_cmap;
extern BOOLEAN edge_mode;
extern BOOLEAN transparent_mode;
extern BOOLEAN is_transparent[MAXCOLOR];
extern unsigned long int pixarr[MAXCOLOR];

#define index_to_pixel(i) (pixarr[i])

#define MAXPIXEL 65535
#define NUMFONTBUFS 32
typedef struct {
    unsigned charcnt,width,height,firstch,style,bytes_per_char,
	cgap,sgap,def_cgap,def_sgap;
    struct {
	int firstx, lastx, width;
    } *cwidths;
    XImage *img;
    BOOLEAN active;
} MyFont;
extern MyFont fonts[];
extern MyFont *curfont;

#define FSTYLE_BOLD_UP			1
#define FSTYLE_BOLD_RIGHT		2
#define FSTYLE_SHADOW_UP_RIGHT		3
#define FSTYLE_SHADOW_UP_LEFT		4
#define FSTYLE_SHADOW_2UP_2RIGHT	5
#define FSTYLE_SHADOW_2UP_2LEFT		6

#define TO_WINDOW TRUE
#define TO_DRAWING FALSE
#ifdef CLIP_ALL_BUT_CFADE
#define put_preg(r,x,y,e,s,d,em) put_reg(r,x,y,e,s,FALSE,TRUE,d,em)
#define put_creg put_preg
#define put_transient(r,x,y,e,s,d,em) put_reg(r,x,y,e,s,TRUE,TRUE,d,em)
#else
#define put_preg(r,x,y,e,s,d,em) put_reg(r,x,y,e,s,FALSE,TRUE,d,em)
#define put_creg(r,x,y,e,s,d,em) put_reg(r,x,y,e,s,FALSE,FALSE,d,em)
#define put_transient(r,x,y,e,s,d,em) put_reg(r,x,y,e,s,TRUE,FALSE,d,em)
#endif
#define save_bg(e) TRUE
#define screen_x(x) ((((x)*MainWindow.xcmult)+MainWindow.xoffs)*MainWindow.pixwid)
#define screen_y(y,ht) (MainWindow.Height-((((y)*MainWindow.ycmult)+MainWindow.yoffs)*MainWindow.pixht)-(ht))

#include "registers.h"
BOOLEAN convert_gif_for_screen(struct PIC_register *r, byte *p, char *err_msg);
BOOLEAN convert_img_for_screen(struct PIC_register *r, byte *dp, int trancolor,
			       char *err_msg);
BOOLEAN create_font(MyFont *font, byte *p, char *err_msg);
BOOLEAN set_font_default(MyFont *font, char *err_msg);
BOOLEAN create_preg(struct PIC_register *r, int width, int height,
		    char *err_msg);
BOOLEAN cycle_colors(int start, int end, int times, int delay);
BOOLEAN get_reg(struct PIC_register *r, int x1, int y1, int x2, int y2,
		BOOLEAN get_cmap, char *err_msg);
BOOLEAN init_window(int *argc, char **argv);
BOOLEAN pan_preg(struct PIC_register *r, int x1, int y1, int x2, int y2,
		 char *err_msg);
BOOLEAN put_partial(struct PIC_register *r, int x, int y, BOOLEAN rel);
BOOLEAN put_reg(struct PIC_register *r, int x, int y, int effect, int speed,
		BOOLEAN transient, BOOLEAN clip, BOOLEAN to_window,
		char *err_msg);
BOOLEAN set_clipping_region(int x1, int y1, int x2, int y2, char *err_msg);
BOOLEAN set_disp_page(int visible, int drawing, char *err_msg);
BOOLEAN set_drawing_reg(int reg, char *err_msg);
BOOLEAN setup_creg_for_display(struct PIC_register *r);
BOOLEAN setup_preg_for_display(struct PIC_register *r);
BOOLEAN setup_for_trans_mode(struct PIC_register *r);
BOOLEAN spread_colors(CMap c1, CMap c2, int rstep, int gstep, int bstep);
BOOLEAN start_window(struct screen_info *vid, byte mode, char *err_msg);
BOOLEAN tile_window(struct PIC_register *r);
CMap convert_clist_to_cmap(byte *clist, int reg, unsigned cmax);
int evt_handler(int dummy);
int get_keystroke(int timeout);
void activate_current_cmap(int maxv);
void beep();
void cleanup();
void clear_creg(struct PIC_register *r);
void clear_preg(struct PIC_register *r);
void clear_window();
void copy_cmap(CMap dst, CMap src);
void copy_pix(Pixmap src, Pixmap dst);
void dispose_font(MyFont *font);
void drawbox(int x1, int y1, int x2, int y2, int width);
void drawellipse(int cx, int cy, int rx, int ry);
void drawline(int x1, int y1, int x2, int y2);
void drawpoint(int x, int y);
void flush_windows();
void put_char(char c, int x, int y);
void redraw_screen();
void restore_bg(int x1, int y1, struct PIC_register *was, int x2, int y2,
		struct PIC_register *is);
void set_current_cmap_entry(int cnum, unsigned int red, unsigned int green,
			    unsigned int blue);
void set_current_drawcolor(int color);
void set_current_drawcolor2(int color);
void set_edge_color(int color);
void update_current_cmap(int maxv);
void synchronize_display();

#endif
