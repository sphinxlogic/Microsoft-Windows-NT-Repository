/* -*- Mode: C; tab-width: 4 -*- */
/* fire --- 3D fire-like image */

#if !defined( lint ) && !defined( SABER )
static const char sccsid[] = "@(#)fire.c	5.00 2000/11/28 xlockmore";
#endif

/* Copyright (c) E. Lassauge, 2000. */

/*
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * The original code for this mode was written by 
 * David Bucciarelli (tech.hmw@plus.it) and could be found in the demo package 
 * of Mesa (Mesa-3.2/3Dfx/demos/).
 *
 * Eric Lassauge  (October-10-2000) <lassauge@mail.dotcom.fr>
 * 				    http://lassauge.free.fr/linux.html
 *
 * REVISION HISTORY:
 *
 * E.Lassauge - 28-Nov-2000:
 *      - modified release part to add freeing of GL objects
 *
 * E.Lassauge - 14-Nov-2000:
 *      - use new common xpm_to_ximage function
 *
 * E.Lassauge - 25-Oct-2000:
 *	- add the trees (with a new resource '-trees')
 *      - corrected handling of color (textured vs untextured)
 *      - corrected handling of endiannes for the xpm files
 *      - inverted ground pixmap file
 *      - use malloc-ed tree array
 *
 * TSchmidt - 23-Oct-2000:
 *	- added size option like used in sproingies mode
 *
 * E.Lassauge - 13-Oct-2000:
 *	- when trackmouse and window is iconified (login screen): stop tracking
 *	- add pure GLX handling of framerate display (erased GLUT stuff)
 *	- made count a per screen variable and update it only if framemode
 *	- changes for no_texture an wireframe modes
 *	- change no_texture color for the ground
 *	- add freeing of texture image
 *	- misc comments and little tweakings
 *
 * TODO:
 *      - perhaps use a user supplied xpm for ground image (or a whatever image
 *        file using ImageMagick ?)
 *	- random number of trees ? change trees at change_fire ?
 *	- fix wireframe mode: it's too CPU intensive.
 *	- look how we can get the Wheel events (Button4&5).
 *	- perhaps use an observer programmed movement:
 *        watch out for the trees !!!
 */

#ifdef STANDALONE
#define PROGCLASS "Fire"
#define HACK_INIT init_fire
#define HACK_DRAW draw_fire
#define fire xlockmore_opts
#define DEFAULTS "*delay: 10000 \n" \
 "*count: 800 \n" \
 "*size: 0 \n" \
 "*trees: 5 \n" \
 "*trackmouse: False \n" \
 "*wireframe:  False \n"	/* still needs some work */

#include "xlockmore.h"		/* from the xscreensaver distribution */
#else				/* !STANDALONE */
#include "xlock.h"		/* from the xlockmore distribution */
#include "vis.h"
#endif				/* !STANDALONE */
#include "iostuff.h"		/* getFont() */

#ifdef MODE_fire

#define MINSIZE 32

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "xpm-ximage.h"

#ifdef STANDALONE
#include "../images/ground.xpm"
#include "../images/tree.xpm"
#else /* !STANDALONE */
#include "pixmaps/ground.xpm"
#include "pixmaps/tree.xpm"
#endif /* !STANDALONE */

/* vector utility macros */
#define vinit(a,i,j,k) {\
  (a)[0]=i;\
  (a)[1]=j;\
  (a)[2]=k;\
}

#define vinit4(a,i,j,k,w) {\
  (a)[0]=i;\
  (a)[1]=j;\
  (a)[2]=k;\
  (a)[3]=w;\
}

#define vadds(a,dt,b) {\
  (a)[0]+=(dt)*(b)[0];\
  (a)[1]+=(dt)*(b)[1];\
  (a)[2]+=(dt)*(b)[2];\
}

#define vequ(a,b) {\
  (a)[0]=(b)[0];\
  (a)[1]=(b)[1];\
  (a)[2]=(b)[2];\
}

#define vinter(a,dt,b,c) {\
  (a)[0]=(dt)*(b)[0]+(1.0-dt)*(c)[0];\
  (a)[1]=(dt)*(b)[1]+(1.0-dt)*(c)[1];\
  (a)[2]=(dt)*(b)[2]+(1.0-dt)*(c)[2];\
}

#define clamp(a)        ((a) < 0.0 ? 0.0 : ((a) < 1.0 ? (a) : 1.0))

#define vclamp(v) {\
  (v)[0]=clamp((v)[0]);\
  (v)[1]=clamp((v)[1]);\
  (v)[2]=clamp((v)[2]);\
}

/* Manage option vars */
#define DEF_TEXTURE	"True"
#define DEF_FOG		"False"
#define DEF_SHADOWS	"True"
#define DEF_FRAMERATE	"False"
#define DEF_TRACKMOUSE  "False"
#define DEF_TREES	"5"
#define MAX_TREES	20
static Bool do_texture;
static Bool do_fog;
static Bool do_framerate;
static Bool do_shadows;
static Bool do_trackmouse;
static int  num_trees;
static XFontStruct *mode_font = None;

static XrmOptionDescRec opts[] = {
    {(char *) "-texture", (char *) ".fire.texture", XrmoptionNoArg, (caddr_t) "on"},
    {(char *) "+texture", (char *) ".fire.texture", XrmoptionNoArg, (caddr_t) "off"},
    {(char *) "-fog", (char *) ".fire.fog", XrmoptionNoArg, (caddr_t) "on"},
    {(char *) "+fog", (char *) ".fire.fog", XrmoptionNoArg, (caddr_t) "off"},
    {(char *) "-framerate", (char *) ".fire.framerate", XrmoptionNoArg, (caddr_t) "on"},
    {(char *) "+framerate", (char *) ".fire.framerate", XrmoptionNoArg, (caddr_t) "off"},
    {(char *) "-shadows", (char *) ".fire.shadows", XrmoptionNoArg, (caddr_t) "on"},
    {(char *) "+shadows", (char *) ".fire.shadows", XrmoptionNoArg, (caddr_t) "off"},
    {(char *) "-trackmouse", (char *) ".fire.trackmouse", XrmoptionNoArg, (caddr_t) "on"},
    {(char *) "+trackmouse", (char *) ".fire.trackmouse", XrmoptionNoArg, (caddr_t) "off"},
    {(char *) "-trees", (char *) ".fire.trees", XrmoptionSepArg, (caddr_t) NULL},

};

static argtype vars[] = {
    {(caddr_t *) & do_texture, (char *) "texture", (char *) "Texture", (char *) DEF_TEXTURE, t_Bool},
    {(caddr_t *) & do_fog, (char *) "fog", (char *) "Fog", (char *) DEF_FOG, t_Bool},
    {(caddr_t *) & do_framerate, (char *) "framerate", (char *) "Framerate", (char *) DEF_FRAMERATE, t_Bool},
    {(caddr_t *) & do_shadows, (char *) "shadows", (char *) "Shadows", (char *) DEF_SHADOWS, t_Bool},
    {(caddr_t *) & do_trackmouse, (char *) "trackmouse", (char *) "TrackMouse", (char *) DEF_TRACKMOUSE, t_Bool},
    {(caddr_t *) & num_trees, (char *) "trees", (char *) "Trees", (char *) DEF_TREES, t_Int},
};

static OptionStruct desc[] = {
    {(char *) "-/+texture", (char *) "turn on/off texturing"},
    {(char *) "-/+fog", (char *) "turn on/off fog"},
    {(char *) "-/+framerate", (char *) "turn on/off framerate display"},
    {(char *) "-/+shadows", (char *) "turn on/off shadows"},
    {(char *) "-/+trackmouse", (char *) "turn on/off the tracking of the mouse"},
    {(char *) "-trees num", (char *) "number of trees (0 disables)"},
};

ModeSpecOpt fire_opts =
 { sizeof opts / sizeof opts[0], opts, sizeof vars / sizeof vars[0], vars, desc };

#ifdef USE_MODULES
ModStruct fire_description =
    { "fire", "init_fire", "draw_fire", "release_fire",
    "draw_fire", "change_fire", NULL, &fire_opts,
    10000, 800, 1, 400, 64, 1.0, "",
    "Shows a 3D fire-like image", 0, NULL
};
#endif /* USE_MODULES */

/* misc defines */
#define TREEINR		2.5	/* tree min distance */
#define TREEOUTR	8.0	/* tree max distance */
#define FRAME 		50	/* frame count interval */
#define DIMP 		20.0	/* dimension of ground */
#define DIMTP 		16.0	/* dimension of ground texture */

#define RIDCOL 		0.4	/* factor for color blending */

#define AGRAV 		-9.8	/* gravity */

/* particle struct */
typedef struct {
    int age;
    float p[3][3];
    float v[3];
    float c[3][4];
} part;

/* colors */
static float black[3]    = { 0.0, 0.0, 0.0 }; /* shadow color */
static float partcol1[3] = { 1.0, 0.2, 0.0 }; /* initial color: red-ish */
static float partcol2[3] = { 1.0, 1.0, 0.0 }; /* blending color: yellow-ish */
static float fogcolor[4] = { 0.9, 0.9, 1.0, 1.0 };

/* ground */
static float q[4][3] = {
    {-DIMP, 0.0, -DIMP},
    {DIMP, 0.0, -DIMP},
    {DIMP, 0.0, DIMP},
    {-DIMP, 0.0, DIMP}
};

/* ground texture */
static float qt[4][2] = {
    {-DIMTP, -DIMTP},
    {DIMTP, -DIMTP},
    {DIMTP, DIMTP},
    {-DIMTP, DIMTP}
};

/* default values for observer */
static const float DEF_OBS[3] = { 2.0, 1.0, 0.0 };
#define DEV_V		0.0
#define DEF_ALPHA	-90.0
#define DEF_BETA	90.0

/* tree struct */
typedef struct {
    float x,y,z;
} treestruct;

/* the mode struct, contains all per screen variables */
typedef struct {
    GLint WIDTH, HEIGHT;	/* display dimensions */
    GLXContext *glx_context;

    int np;			/* number of particles : set it through 'count' resource */
    float eject_r;		/* emission radius */
    float dt, maxage, eject_vy, eject_vl;
    float ridtri;		/* particle size */
    Bool shadows;		/* misc booleans: set them through specific resources */
    Bool fog;
    Bool framerate;

    part *p;			/* particle array */

    XImage *gtexture;		/* ground texture image bits */
    XImage *ttexture;		/* tree texture image bits */
    GLuint groundid;		/* ground texture id: GL world */
    GLuint treeid;		/* tree texture id: GL world */
    GLuint fontbase;		/* fontbase id: GL world */

    int   num_trees;		/* number of trees: set it through 'trees' resource */
    treestruct *treepos;	/* trees positions: float treepos[num_trees][3] */

    float obs[3];		/* observer coordinates */
    float dir[3];		/* view direction */
    float v;			/* observer velocity */
    float alpha;		/* observer angles */
    float beta;

    clock_t told;		/* frame timetag */
    int frcount;		/* frame counter */
    char frbuf[80];		/* frame value string */
} firestruct;

/* array of firestruct indexed by screen number */
static firestruct *fire = NULL;

/*
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 *    Misc funcs.
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 */

/* utility function for the framerate display */
static float gettime(firestruct * fs)
{
    clock_t tnew, ris;

    tnew = clock();

    ris = tnew - fs->told;

    fs->told = tnew;

    return (ris / (float) CLOCKS_PER_SEC);
}

/* my RAND */
static float vrnd(void)
{
    return ((float) LRAND() / (float) MAXRAND);
}

/* initialise new particle */
static void setnewpart(firestruct * fs, part * p)
{
    float a, vi[3], *c;

    p->age = 0;

    a = vrnd() * M_PI * 2.0;

    vinit(vi, sin(a) * fs->eject_r * vrnd(), 0.15, cos(a) * fs->eject_r * vrnd());
    vinit(p->p[0], vi[0] + vrnd() * fs->ridtri, vi[1] + vrnd() * fs->ridtri, vi[2] + vrnd() * fs->ridtri);
    vinit(p->p[1], vi[0] + vrnd() * fs->ridtri, vi[1] + vrnd() * fs->ridtri, vi[2] + vrnd() * fs->ridtri);
    vinit(p->p[2], vi[0] + vrnd() * fs->ridtri, vi[1] + vrnd() * fs->ridtri, vi[2] + vrnd() * fs->ridtri);

    vinit(p->v, vi[0] * fs->eject_vl / (fs->eject_r / 2),
	  vrnd() * fs->eject_vy + fs->eject_vy / 2,
	  vi[2] * fs->eject_vl / (fs->eject_r / 2));

    c = partcol1;

    vinit4(p->c[0], c[0] * ((1.0 - RIDCOL) + vrnd() * RIDCOL),
	   c[1] * ((1.0 - RIDCOL) + vrnd() * RIDCOL),
	   c[2] * ((1.0 - RIDCOL) + vrnd() * RIDCOL), 1.0);
    vinit4(p->c[1], c[0] * ((1.0 - RIDCOL) + vrnd() * RIDCOL),
	   c[1] * ((1.0 - RIDCOL) + vrnd() * RIDCOL),
	   c[2] * ((1.0 - RIDCOL) + vrnd() * RIDCOL), 1.0);
    vinit4(p->c[2], c[0] * ((1.0 - RIDCOL) + vrnd() * RIDCOL),
	   c[1] * ((1.0 - RIDCOL) + vrnd() * RIDCOL),
	   c[2] * ((1.0 - RIDCOL) + vrnd() * RIDCOL), 1.0);
}

/* set particle values */
static void setpart(firestruct * fs, part * p)
{
    float fact;

    if (p->p[0][1] < 0.1) {
	setnewpart(fs, p);
	return;
    }

    p->v[1] += AGRAV * fs->dt;

    vadds(p->p[0], fs->dt, p->v);
    vadds(p->p[1], fs->dt, p->v);
    vadds(p->p[2], fs->dt, p->v);

    p->age++;

    if ((p->age) > fs->maxage) {
	vequ(p->c[0], partcol2);
	vequ(p->c[1], partcol2);
	vequ(p->c[2], partcol2);
    } else {
	fact = 1.0 / fs->maxage;
	vadds(p->c[0], fact, partcol2);
	vclamp(p->c[0]);
	p->c[0][3] = fact * (fs->maxage - p->age);

	vadds(p->c[1], fact, partcol2);
	vclamp(p->c[1]);
	p->c[1][3] = fact * (fs->maxage - p->age);

	vadds(p->c[2], fact, partcol2);
	vclamp(p->c[2]);
	p->c[2][3] = fact * (fs->maxage - p->age);
    }
}

/* draw a tree */
static void drawtree(float x, float y, float z)
{
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex3f(x-1.5,y+0.0,z);

    glTexCoord2f(1.0,0.0);
    glVertex3f(x+1.5,y+0.0,z);

    glTexCoord2f(1.0,1.0);
    glVertex3f(x+1.5,y+3.0,z);

    glTexCoord2f(0.0,1.0);
    glVertex3f(x-1.5,y+3.0,z);


    glTexCoord2f(0.0,0.0);
    glVertex3f(x,y+0.0,z-1.5);

    glTexCoord2f(1.0,0.0);
    glVertex3f(x,y+0.0,z+1.5);

    glTexCoord2f(1.0,1.0);
    glVertex3f(x,y+3.0,z+1.5);

    glTexCoord2f(0.0,1.0);
    glVertex3f(x,y+3.0,z-1.5);

    glEnd();

}

/* calculate observer position : modified only if trackmouse is used */
static void calcposobs(firestruct * fs)
{
    fs->dir[0] = sin(fs->alpha * M_PI / 180.0);
    fs->dir[2] =
	cos(fs->alpha * M_PI / 180.0) * sin(fs->beta * M_PI / 180.0);
    fs->dir[1] = cos(fs->beta * M_PI / 180.0);

    fs->obs[0] += fs->v * fs->dir[0];
    fs->obs[1] += fs->v * fs->dir[1];
    fs->obs[2] += fs->v * fs->dir[2];
}

/* utility function for string printing for framerate */
static void printstring(GLuint fontbase, char *string)
{
    int len;

    len = (int) strlen(string);
    glListBase(fontbase);
    glCallLists(len, GL_UNSIGNED_BYTE, (GLubyte *) string);
}

/* track the mouse in a joystick manner : not perfect but it works */
static void trackmouse(ModeInfo * mi)
{
    firestruct *fs = &fire[MI_SCREEN(mi)];
    /* we keep static values (not per screen) for the mouse stuff: in general you have only one mouse :-> */
    static int max[2] = { 0, 0 };
    static int min[2] = { 0x7fffffff, 0x7fffffff }, center[2];
    Window r, c;
    int rx, ry, cx, cy;
    unsigned int m;

    (void) XQueryPointer(MI_DISPLAY(mi), MI_WINDOW(mi),
			 &r, &c, &rx, &ry, &cx, &cy, &m);

    if (max[0] < cx)
	max[0] = cx;
    if (min[0] > cx)
	min[0] = cx;
    center[0] = (max[0] + min[0]) / 2;

    if (max[1] < cy)
	max[1] = cy;
    if (min[1] > cy)
	min[1] = cy;
    center[1] = (max[1] + min[1]) / 2;

    if (fabs(center[0] - (float) cx) > 0.1 * (max[0] - min[0]))
	fs->alpha += 2.5 * (center[0] - (float) cx) / (max[0] - min[0]);
    if (fabs(center[1] - (float) cy) > 0.1 * (max[1] - min[1]))
	fs->beta += 2.5 * (center[1] - (float) cy) / (max[1] - min[1]);

    /* oops: can't get those buttons */
    if (m & Button4Mask)
	fs->v += 0.01;
    if (m & Button5Mask)
	fs->v -= 0.01;

}

/* initialise textures */
static void inittextures(ModeInfo * mi)
{
    firestruct *fs = &fire[MI_SCREEN(mi)];
    if (do_texture) {

	glGenTextures(1, &fs->groundid);
#ifdef HAVE_GLBINDTEXTURE
	glBindTexture(GL_TEXTURE_2D, fs->groundid);
#endif /* HAVE_GLBINDTEXTURE */

        if ((fs->gtexture = xpm_to_ximage(MI_DISPLAY(mi), MI_VISUAL(mi),
			 MI_COLORMAP(mi), ground)) == None) {
	    (void) fprintf(stderr, "Error reading the ground texture.\n");
	    glDeleteTextures(1, &fs->groundid);
            do_texture = False;
	    fs->groundid = 0;
	    fs->treeid   = 0;
	    return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		     fs->gtexture->width, fs->gtexture->height, 0,
		     GL_RGBA, GL_UNSIGNED_BYTE, fs->gtexture->data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

        if (fs->num_trees)
	{
	    glGenTextures(1, &fs->treeid);
#ifdef HAVE_GLBINDTEXTURE
	    glBindTexture(GL_TEXTURE_2D,fs->treeid);
#endif /* HAVE_GLBINDTEXTURE */
            if ((fs->ttexture = xpm_to_ximage(MI_DISPLAY(mi), MI_VISUAL(mi),
			 MI_COLORMAP(mi), tree)) == None) {
	      (void)fprintf(stderr,"Error reading tree texture.\n");
	      glDeleteTextures(1, &fs->treeid);
	      fs->treeid    = 0;
              fs->num_trees = 0; 
	      return;
	    }

	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		     	fs->ttexture->width, fs->ttexture->height, 0,
		     	GL_RGBA, GL_UNSIGNED_BYTE, fs->ttexture->data);

	    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}
    }
    else
    {
	fs->groundid = 0;	/* default textures */
	fs->treeid   = 0;
    }
}

/* init tree array and positions */
static Bool inittree(ModeInfo * mi)
{
    firestruct *fs = &fire[MI_SCREEN(mi)];
    int i;
    float dist;

    /* allocate treepos array */
    if ((fs->treepos = (treestruct *) malloc(fs->num_trees *
					sizeof(treestruct))) == NULL) {
		return False;
    }
    /* initialise positions */
    for(i=0;i<fs->num_trees;i++)
    	do {
      	    fs->treepos[i].x =vrnd()*TREEOUTR*2.0-TREEOUTR;
      	    fs->treepos[i].y =0.0;
      	    fs->treepos[i].z =vrnd()*TREEOUTR*2.0-TREEOUTR;
      	    dist=sqrt(fs->treepos[i].x *fs->treepos[i].x +fs->treepos[i].z *fs->treepos[i].z );
        } while((dist<TREEINR) || (dist>TREEOUTR));
	return True;
}

/*
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 *    GL funcs.
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 */

static void Reshape(ModeInfo * mi)
{

    firestruct *fs = &fire[MI_SCREEN(mi)];
    int size = MI_SIZE(mi);

    /* Viewport is specified size if size >= MINSIZE && size < screensize */
    if (size <= 1) {
        fs->WIDTH = MI_WIDTH(mi);
        fs->HEIGHT = MI_HEIGHT(mi);
    } else if (size < MINSIZE) {
        fs->WIDTH = MINSIZE;
        fs->HEIGHT = MINSIZE;
    } else {
        fs->WIDTH = (size > MI_WIDTH(mi)) ? MI_WIDTH(mi) : size;
        fs->HEIGHT = (size > MI_HEIGHT(mi)) ? MI_HEIGHT(mi) : size;
    }
    glViewport((MI_WIDTH(mi) - fs->WIDTH) / 2, (MI_HEIGHT(mi) - fs->HEIGHT) / 2, fs->WIDTH, fs->HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, fs->WIDTH / (float) fs->HEIGHT, 0.1, 30.0);

    glMatrixMode(GL_MODELVIEW);

}

static void DrawFire(ModeInfo * mi)
{
    float fr;
    int j;
    firestruct *fs = &fire[MI_SCREEN(mi)];
    Bool wire = MI_IS_WIREFRAME(mi);

    if (do_trackmouse && !MI_IS_ICONIC(mi))
	trackmouse(mi);
    glEnable(GL_DEPTH_TEST);

    if (fs->fog)
	glEnable(GL_FOG);
    else
	glDisable(GL_FOG);

    glDepthMask(GL_TRUE);
    glClearColor(0.5, 0.5, 0.8, 1.0);	/* sky in the distance */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    calcposobs(fs);
    gluLookAt(fs->obs[0], fs->obs[1], fs->obs[2],
	      fs->obs[0] + fs->dir[0], fs->obs[1] + fs->dir[1],
	      fs->obs[2] + fs->dir[2], 0.0, 1.0, 0.0);


    glEnable(GL_TEXTURE_2D);

    /* draw ground using the computed texture */
    if (do_texture) {
	glColor4f(1.0,1.0,1.0,1.0);	/* white to get texture in it's true color */
#ifdef HAVE_GLBINDTEXTURE
	glBindTexture(GL_TEXTURE_2D, fs->groundid);
#endif /* HAVE_GLBINDTEXTURE */
    }
    else
        glColor4f(0.54, 0.27, 0.07, 1.0);	/* untextured ground color */
    glBegin(GL_QUADS);
    glTexCoord2fv(qt[0]);
    glVertex3fv(q[0]);
    glTexCoord2fv(qt[1]);
    glVertex3fv(q[1]);
    glTexCoord2fv(qt[2]);
    glVertex3fv(q[2]);
    glTexCoord2fv(qt[3]);
    glVertex3fv(q[3]);
    glEnd();

    glAlphaFunc(GL_GEQUAL, 0.9);
    if (fs->num_trees)
    {
	/* here do_texture IS True - and color used is white */
	glEnable(GL_ALPHA_TEST);
#ifdef HAVE_GLBINDTEXTURE
	glBindTexture(GL_TEXTURE_2D,fs->treeid);
#endif /* HAVE_GLBINDTEXTURE */
	for(j=0;j<fs->num_trees;j++)
	    drawtree(fs->treepos[j].x ,fs->treepos[j].y ,fs->treepos[j].z );
    	glDisable(GL_ALPHA_TEST);
    }
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);

    if (fs->shadows) {
	/* draw shadows with black color */
	glBegin(wire ? GL_LINE_STRIP : GL_TRIANGLES);
	for (j = 0; j < fs->np; j++) {
	    glColor4f(black[0], black[1], black[2], fs->p[j].c[0][3]);
	    glVertex3f(fs->p[j].p[0][0], 0.1, fs->p[j].p[0][2]);

	    glColor4f(black[0], black[1], black[2], fs->p[j].c[1][3]);
	    glVertex3f(fs->p[j].p[1][0], 0.1, fs->p[j].p[1][2]);

	    glColor4f(black[0], black[1], black[2], fs->p[j].c[2][3]);
	    glVertex3f(fs->p[j].p[2][0], 0.1, fs->p[j].p[2][2]);
	}
	glEnd();
    }

    glBegin(wire ? GL_LINE_STRIP : GL_TRIANGLES);
    for (j = 0; j < fs->np; j++) {
	/* draw particles: colors are computed in setpart */
	glColor4fv(fs->p[j].c[0]);
	glVertex3fv(fs->p[j].p[0]);

	glColor4fv(fs->p[j].c[1]);
	glVertex3fv(fs->p[j].p[1]);

	glColor4fv(fs->p[j].c[2]);
	glVertex3fv(fs->p[j].p[2]);

	setpart(fs, &fs->p[j]);
    }
    glEnd();

    /* manage framerate value */
    if (fs->framerate && fs->frcount >= FRAME) {
        fs->frcount = 0;
	fr = gettime(fs);
	(void) sprintf(fs->frbuf, "Frame rate: %f", FRAME / fr);
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);

    /* manage framerate display */
    if (fs->framerate) {
    	glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
    	glOrtho(-0.5, 639.5, -0.5, 479.5, -1.0, 1.0);
    	glMatrixMode(GL_MODELVIEW);
    	glLoadIdentity();

	/* draw framerate (red text) */
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2i(10, 10);
	printstring(fs->fontbase, fs->frbuf);
	fs->frcount++;
    }
    glPopMatrix();
}


static Bool Init(ModeInfo * mi)
{
    int i;
    firestruct *fs = &fire[MI_SCREEN(mi)];

    /* Default settings */

    fs->eject_r = 0.1 + NRAND(10) * 0.03;
    fs->dt = 0.015;
    fs->eject_vy = 4;
    fs->eject_vl = 1;
    fs->ridtri = 0.1 + NRAND(10) * 0.005;
    fs->maxage = 1.0 / fs->dt;
    fs->frcount = 0;
    vinit(fs->obs, DEF_OBS[0], DEF_OBS[1], DEF_OBS[2]);
    fs->v = 0.0;
    fs->alpha = DEF_ALPHA;
    fs->beta = DEF_BETA;

    if (do_texture)
    	inittextures(mi);
    else
    {
	fs->ttexture = (XImage*) NULL;
	fs->gtexture = (XImage*) NULL;
    }

    /* initialise font stuff */
    if (fs->framerate)
    {
	Font id;
	unsigned int first, last;

	if (mode_font == None)
	    mode_font = getFont(MI_DISPLAY(mi)); 
	if (mode_font == None) {
	    fs->framerate = False;
	} else {
	    id    = mode_font->fid;
	    first = mode_font->min_char_or_byte2;
	    last  = mode_font->max_char_or_byte2;

	    fs->fontbase = glGenLists((GLuint) last + 1);
	    if (!fs->fontbase) {
		(void) fprintf(stderr, "%s: unable to allocate font lists.\n",MI_NAME(mi));
		fs->framerate = False;
	    }
	    else
	    {
		glXUseXFont(id, first, last - first + 1, fs->fontbase + first);
	    }
	}
    }

    if (MI_IS_DEBUG(mi)) {
	(void) fprintf(stderr,
		       "%s:\n\tnum_part=%d\n\ttrees=%d\n\tfog=%s\n\tframerate=%s\n\tshadows=%s\n\teject_r=%.3f\n\tridtri=%.3f\n",
		       MI_NAME(mi),
		       fs->np,
		       fs->num_trees,
		       fs->fog ? "on" : "off",
		       fs->framerate ? "on" : "off",
		       fs->shadows ? "on" : "off",
		       fs->eject_r, fs->ridtri);
    }

    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    /* makes particles blend with background */
    if (!MI_IS_WIREFRAME(mi))
    {
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /* fog stuff */
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogcolor);
    glFogf(GL_FOG_DENSITY, 0.03);
    glHint(GL_FOG_HINT, GL_NICEST);

    /* initialise particles and trees */
    for (i = 0; i < fs->np; i++) {
	setnewpart(fs, &(fs->p[i]));
    }

    if (fs->num_trees)
	if (!inittree(mi)) {
		return False;
	}
    return True;
}

/*
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 *    Xlock hooks.
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 */


static void
free_fire(firestruct *fs)
{
	if (mode_font != None && fs->fontbase != None) {
		glDeleteLists(fs->fontbase, mode_font->max_char_or_byte2 -
			mode_font->min_char_or_byte2 + 1);
		fs->fontbase = None;
	}
	if (fs->p != NULL) {
		(void) free((void *) fs->p);
		fs->p = NULL;
	}
	if (fs->treepos != NULL) {
		(void) free((void *) fs->treepos);
		fs->treepos = NULL;
	}
	if (fs->ttexture != None) {
		glDeleteTextures(1, &fs->treeid);
		XDestroyImage(fs->ttexture);
		fs->ttexture = None;
	}
	if (fs->gtexture != None) {
		glDeleteTextures(1, &fs->groundid);
		XDestroyImage(fs->gtexture);
		fs->gtexture = None;
	}
}

/*
 *-----------------------------------------------------------------------------
 *    Initialize fire.  Called each time the window changes.
 *-----------------------------------------------------------------------------
 */

void
init_fire(ModeInfo * mi)
{
    firestruct *fs;

    if (fire == NULL) {
	if ((fire = (firestruct *) calloc(MI_NUM_SCREENS(mi),
					  sizeof(firestruct))) == NULL)
	    return;
    }
    fs = &fire[MI_SCREEN(mi)];
    fs->np = MI_COUNT(mi);
    fs->fog = do_fog;
    fs->shadows = do_shadows;
    fs->framerate = do_framerate;
    if (fs->p == NULL) {
	if ((fs->p = (part *) calloc(fs->np, sizeof(part))) == NULL) {
	    free_fire(fs);
	    return;
	}
    }
    if (do_texture)
    	fs->num_trees = (num_trees<MAX_TREES)?num_trees:MAX_TREES;
    else
    	fs->num_trees = 0;

    if ((fs->glx_context = init_GL(mi)) != NULL) {

	Reshape(mi);
	glDrawBuffer(GL_BACK);
	if (!Init(mi)) {
		free_fire(fs);
		return;
	}
    } else {
	MI_CLEARWINDOW(mi);
    }
}

/*
 *-----------------------------------------------------------------------------
 *    Called by the mainline code periodically to update the display.
 *-----------------------------------------------------------------------------
 */
void draw_fire(ModeInfo * mi)
{
    firestruct *fs = &fire[MI_SCREEN(mi)];

    Display *display = MI_DISPLAY(mi);
    Window window = MI_WINDOW(mi);

    MI_IS_DRAWN(mi) = True;

    if (!fs->glx_context)
	return;

    glXMakeCurrent(display, window, *(fs->glx_context));
    DrawFire(mi);
    Reshape(mi);

    glFinish();
    glXSwapBuffers(display, window);
}


/*
 *-----------------------------------------------------------------------------
 *    The display is being taken away from us.  Free up malloc'ed
 *      memory and X resources that we've alloc'ed.  Only called
 *      once, we must zap everything for every screen.
 *-----------------------------------------------------------------------------
 */

void release_fire(ModeInfo * mi)
{
    if (fire != NULL) {
    int screen;
	for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++)
		free_fire(&fire[screen]);
	(void) free((void *) fire);
	fire = NULL;
    }
    if (mode_font != None)
    {
	/* only free-ed when there are no more screens used */
	XFreeFont(MI_DISPLAY(mi), mode_font);
	mode_font = None;
    }
    FreeAllGL(mi);
}

void change_fire(ModeInfo * mi)
{
    firestruct *fs = &fire[MI_SCREEN(mi)];

    if (!fs->glx_context)
	return;

    glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(fs->glx_context));

    /* if available, randomly change some values */
    if (do_fog)
	fs->fog = LRAND() & 1;
    if (do_shadows)
	fs->shadows = LRAND() & 1;
    /* reset observer position */
    vinit(fs->obs, DEF_OBS[0], DEF_OBS[1], DEF_OBS[2]);
    fs->v = 0.0;
    /* particle randomisation */
    fs->eject_r = 0.1 + NRAND(10) * 0.03;
    fs->ridtri = 0.1 + NRAND(10) * 0.005;

    if (MI_IS_DEBUG(mi)) {
	(void) fprintf(stderr,
		       "%s:\n\tnum_part=%d\n\ttrees=%d\n\tfog=%s\n\tframerate=%s\n\tshadows=%s\n\teject_r=%.3f\n\tridtri=%.3f\n",
		       MI_NAME(mi),
		       fs->np,
		       fs->num_trees,
		       fs->fog ? "on" : "off",
		       fs->framerate ? "on" : "off",
		       fs->shadows ? "on" : "off",
		       fs->eject_r, fs->ridtri);
    }
}
#endif /* MODE_fire */
