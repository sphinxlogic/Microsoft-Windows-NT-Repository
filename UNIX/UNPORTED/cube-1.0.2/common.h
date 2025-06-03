/**********************************************************************************
 * common.h: defs for all .c's that make up cube
 *The following references were used:
   "The X Window System Programming And Applications with Xt
   OSF/MOTIF EDITION"
   by Douglas A Young 
   Prentice-Hall, 1990.
   ISBN 0-13-642786-3

   "Mastering Rubik's Cube"
   by Don Taylor
   An Owl Book; Holt, Rinehart and Winston, New York, 1980
   ISBN 0-03-059941-5

-------------------------------------------------------------------
Copyright (C) 1993 by Pierre A. Fleurant
Permission is granted to copy and distribute this program
without charge, provided this copyright notice is included
in the copy.
This Software is distributed on an as-is basis. There will be
ABSOLUTELY NO WARRANTY for any part of this software to work
correct. In no case will the author be liable to you for damages
caused by the usage of this software.
-------------------------------------------------------------------
 */

typedef struct {
  GC           gc_line;
  int          init_cubik_color;
  int          init_cubik_number;
  Boolean      init_cubik_mirror;   /* subplane numbers are in a mirror image */
  int          foreground, background;
} graphics_data;

typedef struct _widgetData{
  Widget        toplevel;
  Widget        canvas;
  Widget        popup;
  graphics_data *graph_pointer;
  GC            subface_gc[9][6];
  int           subface_side[9][6]; /* used by init_subplane_pattern, this corresponds to the new inited color */
                                    /* this is used instead of using xgcvalues */
  /* pixmap data; cube is drawn onto ->pix then XCopied to canvas */
  GC            gc;
  Pixmap        pix;
  Dimension     width, height;
  Region        init_cubik_region[9];
  struct _widgetData *subwdata;
}widget_data;


#define Front(x)  wdata->subface_side[x][0]
#define Left(x)   wdata->subface_side[x][1]
#define Right(x)  wdata->subface_side[x][2]
#define Back(x)   wdata->subface_side[x][3]
#define Top(x)    wdata->subface_side[x][4]
#define Bottom(x) wdata->subface_side[x][5]

#define FRONT  0
#define LEFT   1
#define RIGHT  2
#define BACK   3
#define TOP    4
#define BOTTOM 5
