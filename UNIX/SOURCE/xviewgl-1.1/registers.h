#include "patchlevel.h"
#include "windows.h"
#ifndef REGISTERS_H_INCLUDED
#define REGISTERS_H_INCLUDED
#define MAXPICREG 16
#define MAXCLPREG 128

struct PIC_register {
    Pixmap p, clippix;
    GC trans_GC;
    BOOLEAN is_trans;
    BOOLEAN force_trans;
    XImage *img;
    CMap cmap;
    BOOLEAN hascmap;
    int depth;
    int xoffs, yoffs;
    unsigned long int width, height, maxpval;
    BOOLEAN haspixmap;
};

extern struct CMap_struct CMaps[];
extern struct PIC_register preg[];
extern struct PIC_register creg[];
#endif
