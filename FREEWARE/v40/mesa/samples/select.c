/*
 * Copyright (c) 1991, 1992, 1993 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the name of
 * Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF
 * ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "gltk.h"


#define MAXOBJS 10000
#define MAXSELECT 100
#define MAXFEED 300
#define	SOLID 1
#define	LINE 2
#define	POINT 3


GLenum directRender;
GLint windW, windH;

GLuint selectBuf[MAXSELECT];
GLfloat feedBuf[MAXFEED];
GLint vp[4];
float zRotation = 90.0;
float zoom = 1.0;
GLint objectCount;
GLint numObjects;
struct object {
    float v1[2];
    float v2[2];
    float v3[2];
    float color[3];
} objects[MAXOBJS];
GLenum linePoly = GL_FALSE;


static void InitObjects(GLint num)
{
    GLint i;
    float x, y;

    if (num > MAXOBJS) {
	num = MAXOBJS;
    }
    if (num < 1) {
	num = 1;
    }
    objectCount = num;

    srand((unsigned int)time(NULL));
    for (i = 0; i < num; i++) {
	x = (rand() % 300) - 150;
	y = (rand() % 300) - 150;

	objects[i].v1[0] = x + (rand() % 50) - 25;
	objects[i].v2[0] = x + (rand() % 50) - 25;
	objects[i].v3[0] = x + (rand() % 50) - 25;
	objects[i].v1[1] = y + (rand() % 50) - 25;
	objects[i].v2[1] = y + (rand() % 50) - 25;
	objects[i].v3[1] = y + (rand() % 50) - 25;
	objects[i].color[0] = ((rand() % 100) + 50) / 150.0;
	objects[i].color[1] = ((rand() % 100) + 50) / 150.0;
	objects[i].color[2] = ((rand() % 100) + 50) / 150.0;
    }
}

static void Init(void)
{

    numObjects = 10;
    InitObjects(numObjects);
    glGetIntegerv(GL_VIEWPORT, vp);
}

static void Reshape(int width, int height)
{

    windW = (GLint)width;
    windH = (GLint)height;
}

static void Render(GLenum mode)
{
    GLint i;

    for (i = 0; i < objectCount; i++) {
	if (mode == GL_SELECT) {
	    glLoadName(i);
	}
	glColor3fv(objects[i].color);
	glBegin(GL_POLYGON);
	    glVertex2fv(objects[i].v1);
	    glVertex2fv(objects[i].v2);
	    glVertex2fv(objects[i].v3);
	glEnd();
    }
}

static GLint DoSelect(GLint x, GLint y)
{
    GLint hits;

    glSelectBuffer(MAXSELECT, selectBuf);
    (void)glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(~0);

    glPushMatrix();

    glViewport(0, 0, windW, windH);
    glGetIntegerv(GL_VIEWPORT, vp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix(x, windH-y, 4, 4, vp);
    gluOrtho2D(-175, 175, -175, 175);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glScalef(zoom, zoom, zoom);
    glRotatef(zRotation, 0, 0, 1);

    Render(GL_SELECT);

    glPopMatrix();
    
    hits = glRenderMode(GL_RENDER); 
    if (hits <= 0) {
	return -1;
    }

    return selectBuf[(hits-1)*4+3];
}

static void RecolorTri(GLint h)
{

    objects[h].color[0] = ((rand() % 100) + 50) / 150.0;
    objects[h].color[1] = ((rand() % 100) + 50) / 150.0;
    objects[h].color[2] = ((rand() % 100) + 50) / 150.0;
}

static void DeleteTri(GLint h)
{

    objects[h] = objects[objectCount-1];
    objectCount--;
}

static void GrowTri(GLint h)
{
    float v[2];
    float *oldV;
    GLint i;

    v[0] = objects[h].v1[0] + objects[h].v2[0] + objects[h].v3[0];
    v[1] = objects[h].v1[1] + objects[h].v2[1] + objects[h].v3[1];
    v[0] /= 3;
    v[1] /= 3;

    for (i = 0; i < 3; i++) {
	switch (i) {
	  case 0:
	    oldV = objects[h].v1;
	    break;
	  case 1:
	    oldV = objects[h].v2;
	    break;
	  case 2:
	    oldV = objects[h].v3;
	    break;
	}
	oldV[0] = 1.5 * (oldV[0] - v[0]) + v[0];
	oldV[1] = 1.5 * (oldV[1] - v[1]) + v[1];
    }
}

static GLenum Mouse(int mouseX, int mouseY, GLenum button)
{
    GLint hit;

    hit = DoSelect((GLint)mouseX, (GLint)mouseY);
    if (hit != -1) {
	if (button & TK_LEFTBUTTON) {
	    RecolorTri(hit);
	}
	if (button & TK_MIDDLEBUTTON) {
	    GrowTri(hit);
	}
	if (button & TK_RIGHTBUTTON) {
	    DeleteTri(hit);
	}
	return GL_TRUE;
    }
    return GL_FALSE;
}

static void Draw(void)
{

    glPushMatrix();

    glViewport(0, 0, windW, windH);
    glGetIntegerv(GL_VIEWPORT, vp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-175, 175, -175, 175);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glScalef(zoom, zoom, zoom);
    glRotatef(zRotation, 0, 0, 1);

    Render(GL_RENDER);

    glPopMatrix();

    glFlush();
}

static void DrawZoom(GLint x, GLint y)
{

    glPushMatrix();

    glViewport(0, 0, windW, windH);
    glGetIntegerv(GL_VIEWPORT, vp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix(x, windH-y, 4, 4, vp);
    gluOrtho2D(-175, 175, -175, 175);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glScalef(zoom, zoom, zoom);
    glRotatef(zRotation, 0, 0, 1);

    Render(GL_RENDER);

    glPopMatrix();
}

static void DumpFeedbackVert(GLint *i, GLint n)
{
    GLint index;

    index = *i;
    if (index+7 > n) {
	*i = n;
	printf("  ???\n");
	return;
    }
    printf("  (%g %g %g), color = (%4.2f %4.2f %4.2f)\n",
	   feedBuf[index],
	   feedBuf[index+1],
	   feedBuf[index+2],
	   feedBuf[index+3],
	   feedBuf[index+4],
	   feedBuf[index+5]);
    index += 7;
    *i = index;
}

static void DrawFeedback(GLint n)
{
    GLint i;
    GLint verts;

    printf("Feedback results (%d floats):\n", n);
    for (i = 0; i < n; i++) {
	switch ((GLint)feedBuf[i]) {
	  case GL_POLYGON_TOKEN:
	    printf("Polygon");
	    i++;
	    if (i < n) {
		verts = (GLint)feedBuf[i];
		i++;
		printf(": %d vertices", verts);
	    } else {
		verts = 0;
	    }
	    printf("\n");
	    while (verts) {
		DumpFeedbackVert(&i, n);
		verts--;
	    }
	    i--;
	    break;
	  case GL_LINE_TOKEN:
	    printf("Line:\n");
	    i++;
	    DumpFeedbackVert(&i, n);
	    DumpFeedbackVert(&i, n);
	    i--;
	    break;
	  case GL_LINE_RESET_TOKEN:
	    printf("Line Reset:\n");
	    i++;
	    DumpFeedbackVert(&i, n);
	    DumpFeedbackVert(&i, n);
	    i--;
	    break;
	  default:
	    printf("%9.2f\n", feedBuf[i]);
	    break;
	}
    }
    if (i == MAXFEED) {
	printf("...\n");
    }
    printf("\n");
}

static void DoFeedback(void) 
{
    GLint x;

    glFeedbackBuffer(MAXFEED, GL_3D_COLOR, feedBuf);
    (void)glRenderMode(GL_FEEDBACK);

    glPushMatrix();

    glViewport(0, 0, windW, windH);
    glGetIntegerv(GL_VIEWPORT, vp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-175, 175, -175, 175);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glScalef(zoom, zoom, zoom);
    glRotatef(zRotation, 0, 0, 1);

    Render(GL_FEEDBACK);

    glPopMatrix();
    
    x = glRenderMode(GL_RENDER); 
    if (x == -1) {
	x = MAXFEED;
    }

    DrawFeedback((GLint)x);
}

static GLenum Key(int key, GLenum mask)
{
    int mouseX, mouseY;

    switch (key) {
      case TK_ESCAPE:
	tkQuit();
      case TK_LEFT:
	zRotation += 0.5;
	break;
      case TK_RIGHT:
	zRotation -= 0.5;
	break;
      case TK_Z:
	zoom /= 0.75;
	break;
      case TK_z:
	zoom *= 0.75;
	break;
      case TK_f:
	DoFeedback();
	break;
      case TK_d:
	tkGetMouseLoc(&mouseX, &mouseY);
	DrawZoom((GLint)mouseX, (GLint)mouseY);
	break;
      case TK_l:
	linePoly = !linePoly;
	if (linePoly) {
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	break;
      default:
	return GL_FALSE;
    }
    return GL_TRUE;
}

static GLenum Args(int argc, char **argv)
{
    GLint i;

    directRender = GL_TRUE;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-dr") == 0) {
	    directRender = GL_TRUE;
	} else if (strcmp(argv[i], "-ir") == 0) {
	    directRender = GL_FALSE;
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}

void main(int argc, char **argv)
{
    GLenum type;

    if (Args(argc, argv) == GL_FALSE) {
	tkQuit();
    }

    windW = 300;
    windH = 300;
    tkInitPosition(0, 0, windW, windH);

    type = TK_RGB | TK_SINGLE;
    type |= (directRender) ? TK_DIRECT : TK_INDIRECT;
    tkInitDisplayMode(type);

    if (tkInitWindow("Select Test") == GL_FALSE) {
	tkQuit();
    }

    Init();

    tkExposeFunc(Reshape);
    tkReshapeFunc(Reshape);
    tkKeyDownFunc(Key);
    tkMouseDownFunc(Mouse);
    tkDisplayFunc(Draw);
    tkExec();
}
