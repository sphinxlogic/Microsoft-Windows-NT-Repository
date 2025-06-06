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
/*  stencil.c
 *  This program draws two rotated tori in a window.  
 *  A diamond in the center of the window masks out part 
 *  of the scene.  Within this mask, a different model 
 *  (a sphere) is drawn in a different color.
 */
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include "glaux.h"

#define YELLOWMAT   1
#define BLUEMAT 2

void myinit (void) 
{
    GLfloat yellow_diffuse[] = { 0.7, 0.7, 0.0, 1.0 };
    GLfloat yellow_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    GLfloat blue_diffuse[] = { 0.1, 0.1, 0.7, 1.0 };
    GLfloat blue_specular[] = { 0.1, 1.0, 1.0, 1.0 };

    GLfloat position_one[] = { 1.0, 1.0, 1.0, 0.0 };

    glNewList(YELLOWMAT, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
    glEndList();

    glNewList(BLUEMAT, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 45.0);
    glEndList();

    glLightfv(GL_LIGHT0, GL_POSITION, position_one);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glClearStencil(0x0);
    glEnable(GL_STENCIL_TEST);

}

/*  Draw a sphere in a diamond-shaped section in the
 *  middle of a window with 2 tori.
 */
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/* draw blue sphere where the stencil is 1 */
    glStencilFunc (GL_EQUAL, 0x1, 0x1);
    glCallList (BLUEMAT);
    auxSolidSphere (0.5);

/* draw the tori where the stencil is not 1 */
    glStencilFunc (GL_NOTEQUAL, 0x1, 0x1);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
    glPushMatrix();
	glRotatef (45.0, 0.0, 0.0, 1.0);
	glRotatef (45.0, 0.0, 1.0, 0.0);
	glCallList (YELLOWMAT);
	auxSolidTorus (0.275, 0.85);
	glPushMatrix();
	    glRotatef (90.0, 1.0, 0.0, 0.0);
	    auxSolidTorus (0.275, 0.85);
	glPopMatrix();
    glPopMatrix();

}

/*  Whenever the window is reshaped, redefine the 
 *  coordinate system and redraw the stencil area.
 */
void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glClear(GL_STENCIL_BUFFER_BIT);
/* create a diamond shaped stencil area */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-3.0, 3.0, -3.0, 3.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glStencilFunc (GL_ALWAYS, 0x1, 0x1);
    glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glBegin(GL_QUADS);
	glVertex3f (-1.0, 0.0, 0.0);
	glVertex3f (0.0, 1.0, 0.0);
	glVertex3f (1.0, 0.0, 0.0);
	glVertex3f (0.0, -1.0, 0.0);
    glEnd();
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) w/(GLfloat) h, 3.0, 7.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
    auxInitDisplayMode (AUX_SINGLE | AUX_RGB 
	| AUX_DEPTH | AUX_STENCIL);
    auxInitPosition (0, 0, 400, 400);
    if (!auxInitWindow (argv[0]))
       auxQuit();
    myinit ();
    auxReshapeFunc (myReshape);
    auxMainLoop(display);
    return 0;
}
