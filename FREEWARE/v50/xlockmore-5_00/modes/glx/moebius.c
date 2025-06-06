/* -*- Mode: C; tab-width: 4 -*- */
/* moebius --- Moebius Strip II, an Escher-like GL scene with ants. */

#if !defined( lint ) && !defined( SABER )
static const char sccsid[] = "@(#)moebius.c	5.00 2000/11/01 xlockmore";

#endif

#undef DEBUG_LISTS

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
 * The RotateAroundU() routine was adapted from the book
 *    "Computer Graphics Principles and Practice
 *     Foley - vanDam - Feiner - Hughes
 *     Second Edition" Pag. 227, exercise 5.15.
 *
 * This mode shows some interesting scenes that are impossible OR very
 * wierd to build in the real universe. Much of the scenes are inspirated
 * on Mauritz Cornelis Escher's works which derivated the mode's name.
 * M.C. Escher (1898-1972) was a dutch artist and many people prefer to
 * say he was a mathematician.
 *
 * Thanks goes to Brian Paul for making it possible and inexpensive to use
 * OpenGL at home.
 *
 * Since I'm not a native English speaker, my apologies for any grammatical
 * mistakes.
 *
 * My e-mail address is
 * m-vianna@usa.net
 *
 * Marcelo F. Vianna (Jun-01-1997)
 *
 * Revision History:
 * 01-Nov-2000: Allocation checks
 * 01-Jan-1998: Mode separated from escher and renamed
 * 08-Jun-1997: New scene implemented: "Impossible Cage" based in a M.C.
 *              Escher's painting with the same name (quite similar). The
 *              first GL mode to use texture mapping.
 *              The "Impossible Cage" scene doesn't use DEPTH BUFFER, the
 *              wood planks are drawn consistently using GL_CULL_FACE, and
 *              the painter's algorithm is used to sort the planks.
 *              Marcelo F. Vianna.
 * 07-Jun-1997: Speed ups in Moebius Strip using GL_CULL_FACE.
 *              Marcelo F. Vianna.
 * 03-Jun-1997: Initial Release (Only one scene: "Moebius Strip")
 *              The Moebius Strip scene was inspirated in a M.C. Escher's
 *              painting named Moebius Strip II in wich ants walk across a
 *              Moebius Strip path, sometimes meeting each other and sometimes
 *              being in "opposite faces" (note that the moebius strip has
 *              only one face and one edge).
 *              Marcelo F. Vianna.
 */

/*-
 * Texture mapping is only available on RGBA contexts, Mono and color index
 * visuals DO NOT support texture mapping in OpenGL.
 *
 * BUT Mesa do implements RGBA contexts in pseudo color visuals, so texture
 * mapping shuld work on PseudoColor, DirectColor, TrueColor using Mesa. Mono
 * is not officially supported for both OpenGL and Mesa, but seems to not crash
 * Mesa.
 *
 * In real OpenGL, PseudoColor DO NOT support texture map (as far as I know).
 */

#include <X11/Intrinsic.h>

#ifdef STANDALONE
#define PROGCLASS "Moebius"
#define HACK_INIT init_moebius
#define HACK_DRAW draw_moebius
#define moebius_opts xlockmore_opts
#define DEFAULTS "*delay: 1000 \n"
#include "xlockmore.h"		/* from the xscreensaver distribution */
#else /* !STANDALONE */
#include "xlock.h"		/* from the xlockmore distribution */
#include "vis.h"
#endif /* !STANDALONE */

#ifdef MODE_moebius


#include <GL/glu.h>
#include "e_textures.h"

#define DEF_SOLIDMOEBIUS  "False"
#define DEF_NOANTS  "False"

static int  solidmoebius;
static int  noants;

static XrmOptionDescRec opts[] =
{
  {(char *) "-solidmoebius", (char *) ".moebius.solidmoebius", XrmoptionNoArg, (caddr_t) "on"},
 {(char *) "+solidmoebius", (char *) ".moebius.solidmoebius", XrmoptionNoArg, (caddr_t) "off"},
	{(char *) "-noants", (char *) ".moebius.noants", XrmoptionNoArg, (caddr_t) "on"},
	{(char *) "+noants", (char *) ".moebius.noants", XrmoptionNoArg, (caddr_t) "off"}
};
static argtype vars[] =
{
	{(caddr_t *) & solidmoebius, (char *) "solidmoebius", (char *) "Solidmoebius", (char *) DEF_SOLIDMOEBIUS, t_Bool},
	{(caddr_t *) & noants, (char *) "noants", (char *) "Noants", (char *) DEF_NOANTS, t_Bool}
};
static OptionStruct desc[] =
{
	{(char *) "-/+solidmoebius", (char *) "select between a SOLID or a NET Moebius Strip"},
	{(char *) "-/+noants", (char *) "turn on/off walking ants"}
};

ModeSpecOpt moebius_opts =
{sizeof opts / sizeof opts[0], opts, sizeof vars / sizeof vars[0], vars, desc};

#ifdef USE_MODULES
ModStruct   moebius_description =
{"moebius", "init_moebius", "draw_moebius", "release_moebius",
 "draw_moebius", "change_moebius", NULL, &moebius_opts,
 1000, 1, 1, 1, 4, 1.0, "",
 "Shows Moebius Strip II, an Escher-like GL scene with ants", 0, NULL};

#endif

#define Scale4Window               0.3
#define Scale4Iconic               0.4

#define sqr(A)                     ((A)*(A))

#ifndef Pi
#define Pi                         M_PI
#endif

#define ObjMoebiusStrip 0
#define ObjAntBody      1
#define MaxObj          2

/*************************************************************************/

typedef struct {
	GLint       WindH, WindW;
	GLfloat     step;
	GLfloat     ant_position;
	Bool        AreObjectsDefined[MaxObj];
	GLXContext *glx_context;
} moebiusstruct;

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
{0.2, 0.2, 0.2, 1.0};
static float MaterialGray5[] =
{0.5, 0.5, 0.5, 1.0};
static float MaterialGray6[] =
{0.6, 0.6, 0.6, 1.0};
static float MaterialGray8[] =
{0.8, 0.8, 0.8, 1.0};

static moebiusstruct *moebius = NULL;
static GLuint objects = 0;

static Bool
mySphere(float radius)
{
	GLUquadricObj *quadObj;

	if ((quadObj = gluNewQuadric()) == 0)
		return False;
	gluQuadricDrawStyle(quadObj, (GLenum) GLU_FILL);
	gluSphere(quadObj, radius, 16, 16);
	gluDeleteQuadric(quadObj);
	return True;
}

static Bool
myCone(float radius)
{
	GLUquadricObj *quadObj;

	if ((quadObj = gluNewQuadric()) == 0)
		return False;
	gluQuadricDrawStyle(quadObj, (GLenum) GLU_FILL);
	gluCylinder(quadObj, radius, 0, radius * 3, 8, 1);
	gluDeleteQuadric(quadObj);
	return True;
}

static Bool
draw_moebius_ant(moebiusstruct * mp, float *Material, int mono)
{
	static float ant_step = 0;
	float       cos1 = cos(ant_step);
	float       cos2 = cos(ant_step + 2 * Pi / 3);
	float       cos3 = cos(ant_step + 4 * Pi / 3);
	float       sin1 = sin(ant_step);
	float       sin2 = sin(ant_step + 2 * Pi / 3);
	float       sin3 = sin(ant_step + 4 * Pi / 3);

	if (mono)
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray5);
	else
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Material);
	if (!mp->AreObjectsDefined[ObjAntBody]) {
		glNewList(objects + ObjAntBody, GL_COMPILE_AND_EXECUTE);
		if (glGetError() != GL_NO_ERROR) {
			return False;
		}
		glEnable(GL_CULL_FACE);
		glPushMatrix();
		glScalef(1, 1.3, 1);
		if (!mySphere(0.18))
			return False;
		glScalef(1, 1 / 1.3, 1);
		glTranslatef(0.00, 0.30, 0.00);
		if (!mySphere(0.2))
			return False;

		glTranslatef(-0.05, 0.17, 0.05);
		glRotatef(-90, 1, 0, 0);
		glRotatef(-25, 0, 1, 0);
		if (!myCone(0.05))
			return False;
		glTranslatef(0.00, 0.10, 0.00);
		if (!myCone(0.05))
			return False;
		glRotatef(25, 0, 1, 0);
		glRotatef(90, 1, 0, 0);

		glScalef(1, 1.3, 1);
		glTranslatef(0.15, -0.65, 0.05);
		if (!mySphere(0.25))
			return False;
		glScalef(1, 1 / 1.3, 1);
		glPopMatrix();
		glDisable(GL_CULL_FACE);
		glEndList();
		mp->AreObjectsDefined[ObjAntBody] = True;
#ifdef DEBUG_LISTS
		(void) printf("Ant drawn SLOWLY\n");
#endif
	} else {
		glCallList(objects + ObjAntBody);
#ifdef DEBUG_LISTS
		(void) printf("Ant drawn quickly\n");
#endif
	}

	glDisable(GL_LIGHTING);
	/* ANTENNAS */
	glBegin(GL_LINES);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, 0.30, 0.00);
	glColor3fv(MaterialGray);
	glVertex3f(0.40, 0.70, 0.40);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, 0.30, 0.00);
	glColor3fv(MaterialGray);
	glVertex3f(0.40, 0.70, -0.40);
	glEnd();
	glBegin(GL_POINTS);
	if (mono)
		glColor3fv(MaterialGray6);
	else
		glColor3fv(MaterialRed);
	glVertex3f(0.40, 0.70, 0.40);
	glVertex3f(0.40, 0.70, -0.40);
	glEnd();

	/* LEFT-FRONT ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, 0.05, 0.18);
	glVertex3f(0.35 + 0.05 * cos1, 0.15, 0.25);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20 + 0.05 * cos1, 0.25 + 0.1 * sin1, 0.45);
	glEnd();

	/* LEFT-CENTER ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, 0.00, 0.18);
	glVertex3f(0.35 + 0.05 * cos2, 0.00, 0.25);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20 + 0.05 * cos2, 0.00 + 0.1 * sin2, 0.45);
	glEnd();

	/* LEFT-BACK ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, -0.05, 0.18);
	glVertex3f(0.35 + 0.05 * cos3, -0.15, 0.25);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20 + 0.05 * cos3, -0.25 + 0.1 * sin3, 0.45);
	glEnd();

	/* RIGHT-FRONT ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, 0.05, -0.18);
	glVertex3f(0.35 - 0.05 * sin1, 0.15, -0.25);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20 - 0.05 * sin1, 0.25 + 0.1 * cos1, -0.45);
	glEnd();

	/* RIGHT-CENTER ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, 0.00, -0.18);
	glVertex3f(0.35 - 0.05 * sin2, 0.00, -0.25);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20 - 0.05 * sin2, 0.00 + 0.1 * cos2, -0.45);
	glEnd();

	/* RIGHT-BACK ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00, -0.05, -0.18);
	glVertex3f(0.35 - 0.05 * sin3, -0.15, -0.25);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20 - 0.05 * sin3, -0.25 + 0.1 * cos3, -0.45);
	glEnd();

	glBegin(GL_POINTS);
	if (mono)
		glColor3fv(MaterialGray8);
	else
		glColor3fv(MaterialMagenta);
	glVertex3f(-0.20 + 0.05 * cos1, 0.25 + 0.1 * sin1, 0.45);
	glVertex3f(-0.20 + 0.05 * cos2, 0.00 + 0.1 * sin2, 0.45);
	glVertex3f(-0.20 + 0.05 * cos3, -0.25 + 0.1 * sin3, 0.45);
	glVertex3f(-0.20 - 0.05 * sin1, 0.25 + 0.1 * cos1, -0.45);
	glVertex3f(-0.20 - 0.05 * sin2, 0.00 + 0.1 * cos2, -0.45);
	glVertex3f(-0.20 - 0.05 * sin3, -0.25 + 0.1 * cos3, -0.45);
	glEnd();

	glEnable(GL_LIGHTING);

	ant_step += 0.3;
	return True;
}

static void
RotateAaroundU(float Ax, float Ay, float Az,
	       float Ux, float Uy, float Uz,
	       float *Cx, float *Cy, float *Cz,
	       float Theta)
{
	float       cosO = cos(Theta);
	float       sinO = sin(Theta);
	float       one_cosO = 1 - cosO;
	float       Ux2 = sqr(Ux);
	float       Uy2 = sqr(Uy);
	float       Uz2 = sqr(Uz);
	float       UxUy = Ux * Uy;
	float       UxUz = Ux * Uz;
	float       UyUz = Uy * Uz;

	*Cx = (Ux2 + cosO * (1 - Ux2)) * Ax + (UxUy * one_cosO - Uz * sinO) * Ay + (UxUz * one_cosO + Uy * sinO) * Az;
	*Cy = (UxUy * one_cosO + Uz * sinO) * Ax + (Uy2 + cosO * (1 - Uy2)) * Ay + (UyUz * one_cosO - Ux * sinO) * Az;
	*Cz = (UxUz * one_cosO - Uy * sinO) * Ax + (UyUz * one_cosO + Ux * sinO) * Ay + (Uz2 + cosO * (1 - Uz2)) * Az;
}

#define MoebiusDivisions 40
#define MoebiusTransversals 4
static Bool
draw_moebius_strip(ModeInfo * mi)
{
	GLfloat     Phi, Theta;
	GLfloat     cPhi, sPhi;
	moebiusstruct *mp = &moebius[MI_SCREEN(mi)];
	int         i, j;
	int         mono = MI_IS_MONO(mi);

	float       Cx, Cy, Cz;

	if (!mp->AreObjectsDefined[ObjMoebiusStrip]) {
		glNewList(objects + ObjMoebiusStrip, GL_COMPILE_AND_EXECUTE);
		if (glGetError() != GL_NO_ERROR) {
			return False;
		}
		if (solidmoebius) {
			glBegin(GL_QUAD_STRIP);
			Phi = 0;
			i = 0;
			while (i < (MoebiusDivisions * 2 + 1)) {
				Theta = Phi / 2;
				cPhi = cos(Phi);
				sPhi = sin(Phi);

				i++;
				if (mono)
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialWhite);
				else if (i % 2)
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialRed);
				else
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray);

				RotateAaroundU(cPhi, sPhi, 0, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
				glNormal3f(Cx, Cy, Cz);
				RotateAaroundU(0, 0, 1, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
				glVertex3f(cPhi * 3 + Cx, sPhi * 3 + Cy, +Cz);
				glVertex3f(cPhi * 3 - Cx, sPhi * 3 - Cy, -Cz);

				Phi += Pi / MoebiusDivisions;
			}
			glEnd();
		} else {
			for (j = -MoebiusTransversals; j < MoebiusTransversals; j++) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glBegin(GL_QUAD_STRIP);
				Phi = 0;
				i = 0;
				while (i < (MoebiusDivisions * 2 + 1)) {
					Theta = Phi / 2;
					cPhi = cos(Phi);
					sPhi = sin(Phi);

					RotateAaroundU(cPhi, sPhi, 0, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
					glNormal3f(Cx, Cy, Cz);
					RotateAaroundU(0, 0, 1, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
					j++;
					if (j == MoebiusTransversals || mono)
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialWhite);
					else if (i % 2)
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialRed);
					else
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray);
					glVertex3f(cPhi * 3 + Cx / MoebiusTransversals * j, sPhi * 3 + Cy / MoebiusTransversals * j, +Cz / MoebiusTransversals * j);
					j--;
					if (j == -MoebiusTransversals || mono)
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialWhite);
					else if (i % 2)
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialRed);
					else
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray);
					glVertex3f(cPhi * 3 + Cx / MoebiusTransversals * j, sPhi * 3 + Cy / MoebiusTransversals * j, +Cz / MoebiusTransversals * j);

					Phi += Pi / MoebiusDivisions;
					i++;
				}
				glEnd();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glEndList();
		mp->AreObjectsDefined[ObjMoebiusStrip] = True;
#ifdef DEBUG_LISTS
		(void) printf("Strip drawn SLOWLY\n");
#endif
	} else {
		glCallList(objects + ObjMoebiusStrip);
#ifdef DEBUG_LISTS
		(void) printf("Strip drawn quickly\n");
#endif
	}

	if (!noants) {
		/* DRAW BLUE ANT */
		glPushMatrix();
		glRotatef(mp->ant_position + 180, 0, 0, 1);
		glTranslatef(3, 0, 0);
		glRotatef(mp->ant_position / 2 + 90, 0, 1, 0);
		glTranslatef(0.28, 0, -0.45);
		if (!draw_moebius_ant(mp, MaterialYellow, mono))
			return False;
		glPopMatrix();

		/* DRAW YELLOW ANT */
		glPushMatrix();
		glRotatef(mp->ant_position, 0, 0, 1);
		glTranslatef(3, 0, 0);
		glRotatef(mp->ant_position / 2, 0, 1, 0);
		glTranslatef(0.28, 0, -0.45);
		if (!draw_moebius_ant(mp, MaterialBlue, mono))
			return False;
		glPopMatrix();

		/* DRAW GREEN ANT */
		glPushMatrix();
		glRotatef(-mp->ant_position, 0, 0, 1);
		glTranslatef(3, 0, 0);
		glRotatef(-mp->ant_position / 2, 0, 1, 0);
		glTranslatef(0.28, 0, 0.45);
		glRotatef(180, 1, 0, 0);
		if (!draw_moebius_ant(mp, MaterialGreen, mono))
			return False;
		glPopMatrix();

		/* DRAW CYAN ANT */
		glPushMatrix();
		glRotatef(-mp->ant_position + 180, 0, 0, 1);
		glTranslatef(3, 0, 0);
		glRotatef(-mp->ant_position / 2 + 90, 0, 1, 0);
		glTranslatef(0.28, 0, 0.45);
		glRotatef(180, 1, 0, 0);
		if (!draw_moebius_ant(mp, MaterialCyan, mono))
			return False;
		glPopMatrix();
	}
	mp->ant_position += 1;
	return True;
}
#undef MoebiusDivisions
#undef MoebiusTransversals

static void
reshape(ModeInfo * mi, int width, int height)
{
	moebiusstruct *mp = &moebius[MI_SCREEN(mi)];
	int i;

	glViewport(0, 0, mp->WindW = (GLint) width, mp->WindH = (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
	i = width / 512 + 1;
	glLineWidth(i);
	glPointSize(i);
	mp->AreObjectsDefined[ObjMoebiusStrip] = False;
	mp->AreObjectsDefined[ObjAntBody] = False;
}

static void
pinit(void)
{
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 1.0);

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
	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	/* moebius */
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, WoodTextureWidth, WoodTextureHeight,
			  GL_RGB, GL_UNSIGNED_BYTE, WoodTextureData);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);
}

void
init_moebius(ModeInfo * mi)
{
	moebiusstruct *mp;

	if (moebius == NULL) {
		if ((moebius = (moebiusstruct *) calloc(MI_NUM_SCREENS(mi),
					    sizeof (moebiusstruct))) == NULL)
			return;
	}
	mp = &moebius[MI_SCREEN(mi)];
	mp->step = NRAND(90);
	mp->ant_position = NRAND(90);

	if ((mp->glx_context = init_GL(mi)) != NULL) {
		reshape(mi, MI_WIDTH(mi), MI_HEIGHT(mi));
		glDrawBuffer(GL_BACK);
		if (!glIsList(objects))
			if ((objects = glGenLists(MaxObj)) == 0) {
				MI_CLEARWINDOW(mi);
				release_cage(mi);
				return;
			}
		pinit();
	} else {
		MI_CLEARWINDOW(mi);
	}
}

void
draw_moebius(ModeInfo * mi)
{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	moebiusstruct *mp;

	if (moebius == NULL)
		return;
	mp = &moebius[MI_SCREEN(mi)];

	MI_IS_DRAWN(mi) = True;

	if (!mp->glx_context)
		return;

	glXMakeCurrent(display, window, *(mp->glx_context));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0, 0.0, -10.0);

	if (!MI_IS_ICONIC(mi)) {
		glScalef(Scale4Window * mp->WindH / mp->WindW, Scale4Window, Scale4Window);
	} else {
		glScalef(Scale4Iconic * mp->WindH / mp->WindW, Scale4Iconic, Scale4Iconic);
	}

	/* moebius */
	glRotatef(mp->step * 100, 1, 0, 0);
	glRotatef(mp->step * 95, 0, 1, 0);
	glRotatef(mp->step * 90, 0, 0, 1);
	if (!draw_moebius_strip(mi)) {
		release_moebius(mi);
		return;
	}

	glPopMatrix();

	glFlush();

	glXSwapBuffers(display, window);

	mp->step += 0.025;
}

void
change_moebius(ModeInfo * mi)
{
	moebiusstruct *mp = &moebius[MI_SCREEN(mi)];

	if (!mp->glx_context)
		return;

	glXMakeCurrent(MI_DISPLAY(mi), MI_WINDOW(mi), *(mp->glx_context));
	pinit();
}

void
release_moebius(ModeInfo * mi)
{
	if (moebius != NULL) {
		int screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			moebiusstruct *mp = &moebius[screen];

			if (mp->glx_context) {
				if (glIsList(objects)) {
					glDeleteLists(objects, MaxObj);
					objects = 0;
				}
			}
		}
		(void) free((void *) moebius);
		moebius = NULL;
	}
	FreeAllGL(mi);
}

#endif
