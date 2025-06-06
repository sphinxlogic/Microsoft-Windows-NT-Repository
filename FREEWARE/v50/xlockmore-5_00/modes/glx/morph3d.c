/* -*- Mode: C; tab-width: 4 -*- */
/* morph3d --- Shows 3D morphing objects */

#if !defined( lint ) && !defined( SABER )
static const char sccsid[] = "@(#)morph3d.c	5.00 2000/11/01 xlockmore";

#endif

#undef DEBUG_CULL_FACE

/*-
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
 * The original code for this mode was written by Marcelo Fernandes Vianna
 * (me...) and was inspired on a WindowsNT(R)'s screen saver (Flower Box).
 * It was written from scratch and it was not based on any other source code.
 *
 * Porting it to xlock (the final objective of this code since the moment I
 * decided to create it) was possible by comparing the original Mesa's gear
 * demo with it's ported version to xlock, so thanks for Danny Sung (look at
 * gear.c) for his indirect help.
 *
 * Thanks goes also to Brian Paul for making it possible and inexpensive
 * to use OpenGL at home.
 *
 * If you are interested in the original version of this program (not a xlock
 * mode, please refer to the Mesa package (ftp iris.ssec.wisc.edu on /pub/Mesa)
 *
 * Since I'm not a native English speaker, my apologies for any grammatical
 * mistakes.
 *
 * My e-mail address is
 * m-vianna@usa.net
 *
 * Marcelo F. Vianna (Feb-13-1997)
 *
 * Revision History:
 * 27-Jul-97: Speed ups by Marcelo F. Vianna.
 * 08-May-97: Speed ups by Marcelo F. Vianna.
 *
 */

/*-
 * PURIFY 3.0a on SunOS4 reports an unitialized memory read on each of
 * the glCallList() functions below when using MesaGL 2.1.  This has
 * been fixed in MesaGL 2.2 and later releases.
 */

/*-
 * due to a Bug/feature in VMS X11/Intrinsic.h has to be placed before xlock.
 * otherwise caddr_t is not defined correctly
 */

#include <X11/Intrinsic.h>

#ifdef STANDALONE
#define PROGCLASS "Morph3d"
#define HACK_INIT init_morph3d
#define HACK_DRAW draw_morph3d
#define morph3d_opts xlockmore_opts
#define DEFAULTS "*delay: 1000 \n" \
 "*count: 0 \n"
#include "xlockmore.h"		/* from the xscreensaver distribution */
#else /* !STANDALONE */
#include "xlock.h"		/* from the xlockmore distribution */
#include "vis.h"
#endif /* !STANDALONE */

#ifdef MODE_moebius

ModeSpecOpt morph3d_opts =
{0, NULL, 0, NULL, NULL};

#ifdef USE_MODULES
ModStruct   morph3d_description =
{"morph3d", "init_morph3d", "draw_morph3d", "release_morph3d",
 "draw_morph3d", "change_morph3d", NULL, &morph3d_opts,
 1000, 0, 1, 1, 4, 1.0, "",
 "Shows GL morphing polyhedra", 0, NULL};

#endif

#define Scale4Window               0.3
#define Scale4Iconic               1.0

#define VectMul(X1,Y1,Z1,X2,Y2,Z2) (Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2)
#define sqr(A)                     ((A)*(A))

/* Increasing this values produces better image quality, the price is speed. */
#define tetradivisions             23
#define cubedivisions              20
#define octadivisions              21
#define dodecadivisions            10
#define icodivisions               15

#define tetraangle                 109.47122063449069174
#define cubeangle                  90.000000000000000000
#define octaangle                  109.47122063449069174
#define dodecaangle                63.434948822922009981
#define icoangle                   41.810314895778596167

#ifndef Pi
#define Pi                         M_PI
#endif
#define SQRT2                      1.4142135623730951455
#define SQRT3                      1.7320508075688771932
#define SQRT5                      2.2360679774997898051
#define SQRT6                      2.4494897427831778813
#define SQRT15                     3.8729833462074170214
#define cossec36_2                 0.8506508083520399322
#define cos72                      0.3090169943749474241
#define sin72                      0.9510565162951535721
#define cos36                      0.8090169943749474241
#define sin36                      0.5877852522924731292

/*************************************************************************/

typedef struct {
	GLint       WindH, WindW;
	GLfloat     step;
	GLfloat     seno;
	int         object;
	int         edgedivisions;
	int         VisibleSpikes;
	void        (*draw_object) (ModeInfo * mi);
	float       Magnitude;
	float      *MaterialColor[20];
	GLXContext *glx_context;
} morph3dstruct;

static float front_shininess[] =
{60.0};
static float front_specular[] =
{0.7, 0.7, 0.7, 1.0};
static float ambient[] =
{0.0, 0.0, 0.0, 1.0};
static float diffuse[] =
{1.0, 1.0, 1.0, 1.0};
static float position0[] =
{1.0, 1.0, 1.0, 0.0};
static float position1[] =
{-1.0, -1.0, 1.0, 0.0};
static float lmodel_ambient[] =
{0.5, 0.5, 0.5, 1.0};
static float lmodel_twoside[] =
{GL_TRUE};

static float MaterialRed[] =
{0.7, 0.0, 0.0, 1.0};
static float MaterialGreen[] =
{0.1, 0.5, 0.2, 1.0};
static float MaterialBlue[] =
{0.0, 0.0, 0.7, 1.0};
static float MaterialCyan[] =
{0.2, 0.5, 0.7, 1.0};
static float MaterialYellow[] =
{0.7, 0.7, 0.0, 1.0};
static float MaterialMagenta[] =
{0.6, 0.2, 0.5, 1.0};
static float MaterialWhite[] =
{0.7, 0.7, 0.7, 1.0};
static float MaterialGray[] =
{0.5, 0.5, 0.5, 1.0};

static morph3dstruct *morph3d = NULL;

#define TRIANGLE(Edge, Amp, Divisions, Z, VS)                                                                    \
{                                                                                                                \
  GLfloat   Xf,Yf,Xa,Yb,Xf2,Yf2,Yf_2,Yb2,Yb_2;                                                                   \
  GLfloat   Factor=0.0,Factor1,Factor2;                                                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;                                               \
  GLfloat   Ax,Ay;                                                                                               \
  int       Ri,Ti;                                                                                               \
  GLfloat   Vr=(Edge)*SQRT3/3;                                                                                   \
  GLfloat   AmpVr2=(Amp)/sqr(Vr);                                                                                \
  GLfloat   Zf=(Edge)*(Z);                                                                                       \
                                                                                                                 \
  Ax=(Edge)*(+0.5/(Divisions)), Ay=(Edge)*(-SQRT3/(2*Divisions));                                                \
                                                                                                                 \
  Yf=Vr+Ay; Yb=Yf+0.001;                                                                                         \
  for (Ri=1; Ri<=(Divisions); Ri++) {                                                                            \
    glBegin(GL_TRIANGLE_STRIP);                                                                                  \
    Xf=(float)Ri*Ax; Xa=Xf+0.001;                                                                                \
    Yf2=sqr(Yf); Yf_2=sqr(Yf-Ay);                                                                                \
    Yb2=sqr(Yb); Yb_2=sqr(Yb-Ay);                                                                                \
    for (Ti=0; Ti<Ri; Ti++) {                                                                                    \
      Factor=1-(((Xf2=sqr(Xf))+Yf2)*AmpVr2);                                                                     \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                          \
      Factor2=1-((Xf2+Yb2)*AmpVr2);                                                                              \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
                                                                                                                 \
      Xf-=Ax; Yf-=Ay; Xa-=Ax; Yb-=Ay;                                                                            \
                                                                                                                 \
      Factor=1-(((Xf2=sqr(Xf))+Yf_2)*AmpVr2);                                                                    \
      Factor1=1-((sqr(Xa)+Yf_2)*AmpVr2);                                                                         \
      Factor2=1-((Xf2+Yb_2)*AmpVr2);                                                                             \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
                                                                                                                 \
      Xf-=Ax; Yf+=Ay; Xa-=Ax; Yb+=Ay;                                                                            \
    }                                                                                                            \
    Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                             \
    Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                            \
    Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                            \
    VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                             \
    NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                      \
    NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                      \
    glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                               \
    glVertex3f(VertX, VertY, VertZ);                                                                             \
    Yf+=Ay; Yb+=Ay;                                                                                              \
    glEnd();                                                                                                     \
  }                                                                                                              \
  VS=(Factor<0);                                                                                                 \
}

#define SQUARE(Edge, Amp, Divisions, Z, VS)                                                                      \
{                                                                                                                \
  int       Xi,Yi;                                                                                               \
  GLfloat   Xf,Yf,Y,Xf2,Yf2,Y2,Xa,Xa2,Yb;                                                                        \
  GLfloat   Factor=0.0,Factor1,Factor2;                                                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;                                               \
  GLfloat   Zf=(Edge)*(Z);                                                                                       \
  GLfloat   AmpVr2=(Amp)/sqr((Edge)*SQRT2/2);                                                                    \
                                                                                                                 \
  for (Yi=0; Yi<(Divisions); Yi++) {                                                                             \
    Yf=-((Edge)/2.0) + ((float)Yi)/(Divisions)*(Edge);                                                           \
    Yf2=sqr(Yf);                                                                                                 \
    Y=Yf+1.0/(Divisions)*(Edge);                                                                                 \
    Y2=sqr(Y);                                                                                                   \
    glBegin(GL_QUAD_STRIP);                                                                                      \
    for (Xi=0; Xi<=(Divisions); Xi++) {                                                                          \
      Xf=-((Edge)/2.0) + ((float)Xi)/(Divisions)*(Edge);                                                         \
      Xf2=sqr(Xf);                                                                                               \
                                                                                                                 \
      Xa=Xf+0.001; Yb=Y+0.001;                                                                                   \
      Factor=1-((Xf2+Y2)*AmpVr2);                                                                                \
      Factor1=1-(((Xa2=sqr(Xa))+Y2)*AmpVr2);                                                                     \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                          \
      VertX=Factor*Xf;        VertY=Factor*Y;         VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Y-VertY;  NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
                                                                                                                 \
      Yb=Yf+0.001;                                                                                               \
      Factor=1-((Xf2+Yf2)*AmpVr2);                                                                               \
      Factor1=1-((Xa2+Yf2)*AmpVr2);                                                                              \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                          \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
    }                                                                                                            \
    glEnd();                                                                                                     \
  }                                                                                                              \
  VS=(Factor<0);                                                                                                 \
}

#define PENTAGON(Edge, Amp, Divisions, Z, VS)                                                                    \
{                                                                                                                \
  int       Ri,Ti,Fi;                                                                                            \
  GLfloat   Xf,Yf,Xa,Yb,Xf2,Yf2;                                                                                 \
  GLfloat   Factor=0.0,Factor1,Factor2;                                                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;                                               \
  GLfloat   Zf=(Edge)*(Z);                                                                                       \
  GLfloat   AmpVr2=(Amp)/sqr((Edge)*cossec36_2);                                                                 \
                                                                                                                 \
  static    GLfloat x[6],y[6];                                                                                   \
  static    int arrayninit=1;                                                                                    \
                                                                                                                 \
  if (arrayninit) {                                                                                              \
    for(Fi=0;Fi<6;Fi++) {                                                                                        \
      x[Fi]=-cos( Fi*2*Pi/5 + Pi/10 )/(Divisions)*cossec36_2*(Edge);                                             \
      y[Fi]=sin( Fi*2*Pi/5 + Pi/10 )/(Divisions)*cossec36_2*(Edge);                                              \
    }                                                                                                            \
    arrayninit=0;                                                                                                \
  }                                                                                                              \
                                                                                                                 \
  for (Ri=1; Ri<=(Divisions); Ri++) {                                                                            \
    for (Fi=0; Fi<5; Fi++) {                                                                                     \
      glBegin(GL_TRIANGLE_STRIP);                                                                                \
      for (Ti=0; Ti<Ri; Ti++) {                                                                                  \
        Xf=(float)(Ri-Ti)*x[Fi] + (float)Ti*x[Fi+1];                                                             \
        Yf=(float)(Ri-Ti)*y[Fi] + (float)Ti*y[Fi+1];                                                             \
        Xa=Xf+0.001; Yb=Yf+0.001;                                                                                \
	Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                         \
	Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                        \
	Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                        \
        VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                         \
        NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                  \
        NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                  \
        glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                           \
	glVertex3f(VertX, VertY, VertZ);                                                                         \
                                                                                                                 \
        Xf-=x[Fi]; Yf-=y[Fi]; Xa-=x[Fi]; Yb-=y[Fi];                                                              \
                                                                                                                 \
	Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                         \
	Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                        \
	Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                        \
        VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                         \
        NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                  \
        NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                  \
        glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                           \
	glVertex3f(VertX, VertY, VertZ);                                                                         \
                                                                                                                 \
      }                                                                                                          \
      Xf=(float)Ri*x[Fi+1];                                                                                      \
      Yf=(float)Ri*y[Fi+1];                                                                                      \
      Xa=Xf+0.001; Yb=Yf+0.001;                                                                                  \
      Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                           \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                          \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                          \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
      glEnd();                                                                                                   \
    }                                                                                                            \
  }                                                                                                              \
  VS=(Factor<0);                                                                                             \
}

static void
draw_tetra(ModeInfo * mi)
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	if ((list = glGenLists(1)) == 0)
		return;
	glNewList(list, GL_COMPILE_AND_EXECUTE);
	if (glGetError() != GL_NO_ERROR) {
		glDeleteLists(list, 1);
		return;
	}
	TRIANGLE(2, mp->seno, mp->edgedivisions, 0.5 / SQRT6, mp->VisibleSpikes);
	glEndList();

	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-tetraangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + tetraangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + tetraangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_cube(ModeInfo * mi)
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];


	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	if ((list = glGenLists(1)) == 0)
		return;
	glNewList(list, GL_COMPILE_AND_EXECUTE);
	if (glGetError() != GL_NO_ERROR) {
		glDeleteLists(list, 1);
		return;
	}
	SQUARE(2, mp->seno, mp->edgedivisions, 0.5, mp->VisibleSpikes)
		glEndList();

	glRotatef(cubeangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glRotatef(cubeangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glRotatef(cubeangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glRotatef(cubeangle, 0, 1, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glRotatef(2 * cubeangle, 0, 1, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_octa(ModeInfo * mi)
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	if ((list = glGenLists(1)) == 0)
		return;
	glNewList(list, GL_COMPILE_AND_EXECUTE);
	if (glGetError() != GL_NO_ERROR) {
		glDeleteLists(list, 1);
		return;
	}
	TRIANGLE(2, mp->seno, mp->edgedivisions, 1 / SQRT6, mp->VisibleSpikes);
	glEndList();

	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-180 + octaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-180 + octaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-octaangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_dodeca(ModeInfo * mi)
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

#define TAU ((SQRT5+1)/2)

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	if ((list = glGenLists(1)) == 0)
		return;
	glNewList(list, GL_COMPILE_AND_EXECUTE);
	if (glGetError() != GL_NO_ERROR) {
		glDeleteLists(list, 1);
		return;
	}
	PENTAGON(1, mp->seno, mp->edgedivisions, sqr(TAU) * sqrt((TAU + 2) / 5) / 2, mp->VisibleSpikes);
	glEndList();

	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glPushMatrix();
	glRotatef(-dodecaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, -sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(dodecaangle, cos36, -sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glPopMatrix();
	glRotatef(dodecaangle, cos36, sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	glCallList(list);
	glRotatef(180, 0, 0, 1);
	glPushMatrix();
	glRotatef(-dodecaangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[8]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-dodecaangle, cos72, -sin72, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[9]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(dodecaangle, cos36, -sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[10]);
	glCallList(list);
	glPopMatrix();
	glRotatef(dodecaangle, cos36, sin36, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[11]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
draw_icosa(ModeInfo * mi)
{
	GLuint      list;

	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	if ((list = glGenLists(1)) == 0)
		return;
	glNewList(list, GL_COMPILE_AND_EXECUTE);
	if (glGetError() != GL_NO_ERROR) {
		glDeleteLists(list, 1);
		return;
	}
	TRIANGLE(1.5, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glEndList();

	glPushMatrix();

	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[8]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[9]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[10]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[11]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[12]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[13]);
	glCallList(list);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[14]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[15]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[16]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[17]);
	glCallList(list);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(-180 + icoangle, 0.5, -SQRT3 / 2, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[18]);
	glCallList(list);
	glPopMatrix();
	glRotatef(180, 0, 0, 1);
	glRotatef(-icoangle, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[19]);
	glCallList(list);

	glDeleteLists(list, 1);
}

static void
reshape(ModeInfo * mi, int width, int height)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glViewport(0, 0, mp->WindW = (GLint) width, mp->WindH = (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

static void
pinit(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);

	switch (mp->object) {
		case 2:
			mp->draw_object = draw_cube;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialCyan;
			mp->MaterialColor[3] = MaterialMagenta;
			mp->MaterialColor[4] = MaterialYellow;
			mp->MaterialColor[5] = MaterialBlue;
			mp->edgedivisions = cubedivisions;
			mp->Magnitude = 2.0;
			break;
		case 3:
			mp->draw_object = draw_octa;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialWhite;
			mp->MaterialColor[4] = MaterialCyan;
			mp->MaterialColor[5] = MaterialMagenta;
			mp->MaterialColor[6] = MaterialGray;
			mp->MaterialColor[7] = MaterialYellow;
			mp->edgedivisions = octadivisions;
			mp->Magnitude = 2.5;
			break;
		case 4:
			mp->draw_object = draw_dodeca;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialCyan;
			mp->MaterialColor[3] = MaterialBlue;
			mp->MaterialColor[4] = MaterialMagenta;
			mp->MaterialColor[5] = MaterialYellow;
			mp->MaterialColor[6] = MaterialGreen;
			mp->MaterialColor[7] = MaterialCyan;
			mp->MaterialColor[8] = MaterialRed;
			mp->MaterialColor[9] = MaterialMagenta;
			mp->MaterialColor[10] = MaterialBlue;
			mp->MaterialColor[11] = MaterialYellow;
			mp->edgedivisions = dodecadivisions;
			mp->Magnitude = 2.0;
			break;
		case 5:
			mp->draw_object = draw_icosa;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialCyan;
			mp->MaterialColor[4] = MaterialYellow;
			mp->MaterialColor[5] = MaterialMagenta;
			mp->MaterialColor[6] = MaterialRed;
			mp->MaterialColor[7] = MaterialGreen;
			mp->MaterialColor[8] = MaterialBlue;
			mp->MaterialColor[9] = MaterialWhite;
			mp->MaterialColor[10] = MaterialCyan;
			mp->MaterialColor[11] = MaterialYellow;
			mp->MaterialColor[12] = MaterialMagenta;
			mp->MaterialColor[13] = MaterialRed;
			mp->MaterialColor[14] = MaterialGreen;
			mp->MaterialColor[15] = MaterialBlue;
			mp->MaterialColor[16] = MaterialCyan;
			mp->MaterialColor[17] = MaterialYellow;
			mp->MaterialColor[18] = MaterialMagenta;
			mp->MaterialColor[19] = MaterialGray;
			mp->edgedivisions = icodivisions;
			mp->Magnitude = 2.5;
			break;
		default:
			mp->draw_object = draw_tetra;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialWhite;
			mp->edgedivisions = tetradivisions;
			mp->Magnitude = 2.5;
			break;
	}
	if (MI_IS_MONO(mi)) {
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialGray;
	}
}

void
init_morph3d(ModeInfo * mi)
{
	morph3dstruct *mp;

	if (morph3d == NULL) {
		if ((morph3d = (morph3dstruct *) calloc(MI_NUM_SCREENS(mi),
					    sizeof (morph3dstruct))) == NULL)
			return;
	}
	mp = &morph3d[MI_SCREEN(mi)];
	mp->step = NRAND(90);
	mp->VisibleSpikes = 1;

	if ((mp->glx_context = init_GL(mi)) != NULL) {

		reshape(mi, MI_WIDTH(mi), MI_HEIGHT(mi));
		glDrawBuffer(GL_BACK);
		mp->object = MI_COUNT(mi);
		if (mp->object <= 0 || mp->object > 5)
			mp->object = NRAND(5) + 1;
		pinit(mi);
	} else {
		MI_CLEARWINDOW(mi);
	}
}

void
draw_morph3d(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	morph3dstruct *mp;

	if (morph3d == NULL)
		return;
	mp = &morph3d[MI_SCREEN(mi)];

	MI_IS_DRAWN(mi) = True;

	if (!mp->glx_context)
		return;

	glXMakeCurrent(display, window, *(mp->glx_context));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0, 0.0, -10.0);

	if (!MI_IS_ICONIC(mi)) {
		glScalef(Scale4Window * mp->WindH / mp->WindW, Scale4Window, Scale4Window);
		glTranslatef(2.5 * mp->WindW / mp->WindH * sin(mp->step * 1.11), 2.5 * cos(mp->step * 1.25 * 1.11), 0);
	} else {
		glScalef(Scale4Iconic * mp->WindH / mp->WindW, Scale4Iconic, Scale4Iconic);
	}

	glRotatef(mp->step * 100, 1, 0, 0);
	glRotatef(mp->step * 95, 0, 1, 0);
	glRotatef(mp->step * 90, 0, 0, 1);

	mp->seno = (sin(mp->step) + 1.0 / 3.0) * (4.0 / 5.0) * mp->Magnitude;

	if (mp->VisibleSpikes) {
#ifdef DEBUG_CULL_FACE
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialGray;
#endif
		glDisable(GL_CULL_FACE);
	} else {
#ifdef DEBUG_CULL_FACE
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialWhite;
#endif
		glEnable(GL_CULL_FACE);
	}

	mp->draw_object(mi);

	glPopMatrix();

	glFlush();

	glXSwapBuffers(display, window);

	mp->step += 0.05;
}

void
change_morph3d(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	if (!mp->glx_context)
		return;

	mp->object = (mp->object) % 5 + 1;
	pinit(mi);
}

void
release_morph3d(ModeInfo * mi)
{
	if (morph3d != NULL) {
		(void) free((void *) morph3d);
		morph3d = NULL;
	}
	FreeAllGL(mi);
}

#endif
