/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif



/*
** Window Types
*/

#define TK_RGB		0
#define TK_INDEX	1
#define TK_SINGLE	0
#define TK_DOUBLE	2
#define TK_DIRECT	0
#define TK_INDIRECT	4
#define TK_ACCUM	8
#define TK_ALPHA	16
#define TK_DEPTH	32
#define TK_STENCIL	64

/* 
** Window Masks
*/

#define TK_IS_RGB(x)		(((x) & TK_INDEX) == 0)
#define TK_IS_INDEX(x)		(((x) & TK_INDEX) != 0)
#define TK_IS_SINGLE(x)		(((x) & TK_DOUBLE) == 0)
#define TK_IS_DOUBLE(x)		(((x) & TK_DOUBLE) != 0)
#define TK_HAS_ACCUM(x)		(((x) & TK_ACCUM) != 0)
#define TK_HAS_ALPHA(x)		(((x) & TK_ALPHA) != 0)
#define TK_HAS_DEPTH(x)		(((x) & TK_DEPTH) != 0)
#define TK_HAS_STENCIL(x)	(((x) & TK_STENCIL) != 0)
#define TK_IS_DIRECT(x)		(((x) & TK_INDIRECT) != 0)
#define TK_IS_INDIRECT(x)	(((x) & TK_INDIRECT) == 0)

/*
** Event Status
*/

#define	TK_LEFTBUTTON		1
#define	TK_RIGHTBUTTON		2
#define	TK_MIDDLEBUTTON		4
#define	TK_SHIFT		1
#define	TK_CONTROL		2

/* 
** Key Codes
*/

#define TK_SHIFT_L		0x01
#define TK_SHIFT_R		0x02
#define TK_CONTROL_L		0x03
#define TK_CONTROL_R		0x04



#define TK_RETURN		0x0D
#define TK_ESCAPE		0x1B
#define TK_SPACE		0x20
#define TK_LEFT			0x25
#define TK_UP			0x26
#define TK_RIGHT		0x27
#define TK_DOWN			0x28
#define TK_A			'A'
#define TK_B			'B'
#define TK_C			'C'
#define TK_D			'D'
#define TK_E			'E'
#define TK_F			'F'
#define TK_G			'G'
#define TK_H			'H'
#define TK_I			'I'
#define TK_J			'J'
#define TK_K			'K'
#define TK_L			'L'
#define TK_M			'M'
#define TK_N			'N'
#define TK_O			'O'
#define TK_P			'P'
#define TK_Q			'Q'
#define TK_R			'R'
#define TK_S			'S'
#define TK_T			'T'
#define TK_U			'U'
#define TK_V			'V'
#define TK_W			'W'
#define TK_X			'X'
#define TK_Y			'Y'
#define TK_Z			'Z'
#define TK_a			'a'
#define TK_b			'b'
#define TK_c			'c'
#define TK_d			'd'
#define TK_e			'e'
#define TK_f			'f'
#define TK_g			'g'
#define TK_h			'h'
#define TK_i			'i'
#define TK_j			'j'
#define TK_k			'k'
#define TK_l			'l'
#define TK_m			'm'
#define TK_n			'n'
#define TK_o			'o'
#define TK_p			'p'
#define TK_q			'q'
#define TK_r			'r'
#define TK_s			's'
#define TK_t			't'
#define TK_u			'u'
#define TK_v			'v'
#define TK_w			'w'
#define TK_x			'x'
#define TK_y			'y'
#define TK_z			'z'
#define TK_0			'0'
#define TK_1			'1'
#define TK_2			'2'
#define TK_3			'3'
#define TK_4			'4'
#define TK_5			'5'
#define TK_6			'6'
#define TK_7			'7'
#define TK_8			'8'
#define TK_9			'9'

/*
** Color Macros
*/

enum {
    TK_BLACK = 0,
    TK_RED,
    TK_GREEN,
    TK_YELLOW,
    TK_BLUE,
    TK_MAGENTA,
    TK_CYAN,
    TK_WHITE
};

extern float tkRGBMap[8][3];

#define TK_SETCOLOR(x, y) (TK_IS_RGB((x)) ? \
		           glColor3fv(tkRGBMap[(y)]) : glIndexf((y)))

/*
** RGB Image Structure
*/

typedef struct _TK_RGBImageRec {
    GLint sizeX, sizeY;
    unsigned char *data;
} TK_RGBImageRec;

/*
** Prototypes
*/

extern void tkInitDisplayMode(GLenum);
extern void tkInitPosition(int, int, int, int);
extern GLenum tkInitWindow(char *);
extern void tkCloseWindow(void);
extern void tkQuit(void);

extern void tkExec(void);
extern void tkExposeFunc(void (*)(int, int));
extern void tkReshapeFunc(void (*)(int, int));
extern void tkDisplayFunc(void (*)(void));
extern void tkKeyDownFunc(GLenum (*)(int, GLenum));
extern void tkMouseDownFunc(GLenum (*)(int, int, GLenum));
extern void tkMouseUpFunc(GLenum (*)(int, int, GLenum));
extern void tkMouseMoveFunc(GLenum (*)(int, int, GLenum));
extern void tkIdleFunc(void (*)(void));

extern void tkSwapBuffers(void);

extern int tkGetColorMapSize(void);
extern void tkGetMouseLoc(int *, int *);
extern Display *tkGetXDisplay(void);
extern Window tkGetXWindow(void);

extern void tkSetOneColor(int, float, float, float);
extern void tkSetFogRamp(int, int);
extern void tkSetGreyRamp(void);
extern void tkSetRGBMap(int, float *);

extern TK_RGBImageRec *tkRGBImageLoad(char *);

extern GLenum tkCreateStrokeFont(GLuint);
extern GLenum tkCreateOutlineFont(GLuint);
extern GLenum tkCreateFilledFont(GLuint);
extern GLenum tkCreateBitmapFont(GLuint);
extern void tkDrawStr(GLuint, char *);

#ifdef __cplusplus
}
#endif
