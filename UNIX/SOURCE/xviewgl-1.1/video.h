#ifndef VIDEO_INCLUDED
#define VIDEO_INCLUDED
#include "patchlevel.h"

#define VIDTYPES 24

extern struct screen_info {
    char key;
    int width, height, maxcolor, cmax, xmult, ymult, xcmult, ycmult;
    void (*init)();
} video_modes[];

struct my_color {
    unsigned long int red, green, blue;
};

#define MAXCOLOR 256
#define MAXEGACOLOR 64
#define MAXCGACOLOR 64
#define MAXCGAPALETTE 1
#define CGAPALETTESIZE 4
extern struct my_color EGA_color[], CGA_color[];
extern int CGA_palette[][CGAPALETTESIZE];
#endif
