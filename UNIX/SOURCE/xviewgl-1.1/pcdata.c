#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include "my_types.h"
#include "windows.h"
#include "video.h"

static void cga4()
{
int i;

    set_current_cmap_entry(0,
	    CGA_color[0].red, CGA_color[0].green,
	    CGA_color[0].blue);
    for (i=0;i<3;i++)
	set_current_cmap_entry(i+1,
		CGA_color[CGA_palette[0][i]].red,
		CGA_color[CGA_palette[0][i]].green,
		CGA_color[CGA_palette[0][i]].blue);
    set_current_drawcolor(1);
    set_current_drawcolor2(0);
}

static void cga2()
{
    set_current_cmap_entry(0,
	    CGA_color[0].red, CGA_color[0].green,
	    CGA_color[0].blue);
    set_current_cmap_entry(1,
	    CGA_color[7].red, CGA_color[7].green,
	    CGA_color[7].blue);
    set_current_drawcolor(1);
    set_current_drawcolor2(0);
}

static void ega()
{
int i;
    cga4();
    for (i=4;i<16;i++)
	set_current_cmap_entry(i, CGA_color[i].red, CGA_color[i].green,
	    CGA_color[i].blue);
}

static void vga()
{
    ega();
}

/* cmax, the maximum value of a color component, is only used in VGA modes,
   so it is specified as 0 for all other modes. */
#undef NULL
#define NULL (0)
struct screen_info video_modes[VIDTYPES] = {
      /* Description
	    key  width height colors cmax xmult  ymult xcmult ycmult init */
    { /* 40 Column text */
	    '0',  320,   200,    16, 256,    2,     2,    16,     8, cga2 },
    { /* 80 Column text */
	    '1',  640,   200,    16, 256,    1,     2,     8,     8, cga2 },
    { /* 80 Column text */
	    '2',  640,   200,     2, 256,    1,     2,     8,     8, cga2 },
    { /* IBM CGA        */
	    'a',  320,   200,     4, 256,    2,     2,     1,     1, cga4 },
    { /* IBM PCjr/STB   */
	    'b',  320,   200,    16, 256,    2,     2,     1,     1, ega  },
    { /* IBM CGA        */
	    'c',  640,   200,     2, 256,    1,     2,     1,     1, cga2 },
    { /* IBM EGA        */
	    'd',  640,   200,    16, 256,    1,     2,     1,     1, ega  },
    { /* IBM EGA mono   */
	    'e',  640,   350,     2, 256,    1,     2,     1,     1, cga2 },
    { /* IBM EGA        */
	    'f',  640,   350,     4, 256,    1,     2,     1,     1, ega  },
    { /* IBM EGA        */
	    'g',  640,   350,    16, 256,    1,     2,     1,     1, ega  },
    { /* Hercules mono  */
	    'h',  720,   348,     2, 256,    1,     2,     1,     1, cga2 },
    { /* Plantronics CGP*/
	    'i',  320,   200,    16, 256,    2,     2,     1,     1, cga2 },
    { /* IBM EGA        */
	    'j',  320,   200,    16, 256,    2,     2,     1,     1, ega  },
    { /* AT&T/Tosh 3100 */
	    'k',  640,   400,     2, 256,    1,     1,     1,     1, cga2 },
    { /* IBM VGA        */
	    'l',  320,   200,   256,  64,    2,     2,     1,     1, vga  },
    { /* EGA plus       */
	    'm',  640,   480,    16,  64,    1,     1,     1,     1, ega  },
    { /* Herc InColor   */
	    'n',  720,   348,    16,  64,    1,     1,     1,     1, ega  },
    { /* VGA (?)        */
	    'o',  640,   480,     2,  64,    1,     1,     1,     1, cga2 },
    { /* VGA (?)        */
	    'p',  800,   600,     2,  64,    1,     1,     1,     1, cga2 },
    { /* VGA (?)        */
	    'q',  800,   600,    16,  64,    1,     1,     1,     1, ega  },
    { /* VGA (?)        */
	    'r',  640,   400,   256, 256,    1,     1,     1,     1, vga  },
    { /* VGA (?)        */
	    's',  640,   480,   256, 256,    1,     1,     1,     1, vga  },
    { /* VGA (?)        */
	    't',  800,   600,   256, 256,    1,     1,     1,     1, vga  },
    { /* VGA (?)        */
	    'w',  360,   480,   256, 256,    1,     1,     1,     1, vga  }
};

struct my_color EGA_color[MAXEGACOLOR];
struct my_color CGA_color[MAXCGACOLOR] = {
    /* Black */
    {0, 0, 0},
    /* Blue */
    {0, 0, MAXPIXEL/2},
    /* Green */
    {0, MAXPIXEL/2, 0},
    /* Cyan */
    {0, MAXPIXEL/2, MAXPIXEL/2},
    /* Red */
    {MAXPIXEL/2, 0, 0},
    /* Magenta */
    {MAXPIXEL/2, 0, MAXPIXEL/2},
    /* Brown */
    {MAXPIXEL/2, MAXPIXEL/2, 0},
    /* White */
    {MAXPIXEL/2, MAXPIXEL/2, MAXPIXEL/2},
    /* Grey */
    {MAXPIXEL/4, MAXPIXEL/4, MAXPIXEL/4},
    /* Bright Blue */
    {0, 0, MAXPIXEL},
    /* Bright Green */
    {0, MAXPIXEL, 0},
    /* Bright Cyan */
    {0, MAXPIXEL, MAXPIXEL},
    /* Bright Red */
    {MAXPIXEL, 0, 0},
    /* Bright Magenta */
    {MAXPIXEL, 0, MAXPIXEL},
    /* Bright Brown */
    {MAXPIXEL, MAXPIXEL, 0},
    /* Bright White */
    {MAXPIXEL, MAXPIXEL, MAXPIXEL}
};
int CGA_palette[][CGAPALETTESIZE] = {
    {2,4,6},
    {3,5,15},
    {3,4,15},
    {10,12,14},
    {11,13,15},
    {11,12,15}
};

#define EGA_BPP 2
#define EGA_MAXRGB (1<<EGA_BPP)
#define EGA_SCALE (MAXPIXEL/(EGA_MAXRGB-1))
static unsigned short egac[4] = {0x0000, 0xaaaa, 0x5555, 0xffff};
init_EGA_colors() {
int r, g, b, i;
    i = 0;
    for (r = 0; r < EGA_MAXRGB; r++) {
	for (g = 0; g < EGA_MAXRGB; g++) {
	    for (b = 0; b < EGA_MAXRGB; b++) {
		i = ((r&1)<<2)|((r&2)<<4)|((g&1)<<1)|((g&2)<<3)|
		    ((b&1))|((b&2)<<2);
		EGA_color[i].red = egac[r];
		EGA_color[i].green = egac[g];
		EGA_color[i].blue = egac[b];
	    }
	}
    }
}
