/* bounce.c */


/*
 * Bouncing ball demo.
 */


#include <math.h>
#include <stdio.h>
#include "tk.h"



#define COS(X)   cos( (X) * 3.14159/180.0 )
#define SIN(X)   sin( (X) * 3.14159/180.0 )


GLuint Ball;
GLenum Mode;
GLfloat Zrot = 0.0, Zstep = 6.0;
GLfloat Xpos = 0.0, Ypos = 1.0;
GLfloat Xvel = 0.2, Yvel = 0.0;
GLfloat Xmin=-4.0, Xmax=4.0;
GLfloat Ymin=-4.0, Ymax=4.0;
GLfloat G = -0.1;



static GLuint make_ball( void )
{
   GLuint list;
   GLfloat a, b;
   GLfloat da = 18.0, db = 18.0;
   GLfloat radius = 1.0;
   GLuint color;
   GLfloat x, y, z;

   list = glGenLists( 1 );

   glNewList( list, GL_COMPILE );

   color = 0;
   for (a=-90.0;a+da<=90.0;a+=da) {

      glBegin( GL_QUAD_STRIP );
      for (b=0.0;b<=360.0;b+=db) {

	 if (color) {
	    TK_SETCOLOR( Mode, TK_RED );
	 }
	 else {
	    TK_SETCOLOR( Mode, TK_WHITE );
	 }

	 x = COS(b) * COS(a);
	 y = SIN(b) * COS(a);
	 z = SIN(a);
	 glVertex3f( x, y, z );

	 x = radius * COS(b) * COS(a+da);
	 y = radius * SIN(b) * COS(a+da);
	 z = radius * SIN(a+da);
	 glVertex3f( x, y, z );

	 color = 1-color;
      }
      glEnd();

   }

   glEndList();

   return list;
}



static void reshape( int width, int height )
{
   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho( -6.0, 6.0, -6.0, 6.0, -6.0, 6.0 );
   glMatrixMode(GL_MODELVIEW);
}


static GLenum key(int k, GLenum mask)
{
   switch (k) {
      case TK_ESCAPE:
	 tkQuit();
   }
   return GL_FALSE;
}


static void idle( void )
{
   static float vel0 = -100.0;

   Zrot += Zstep;

   Xpos += Xvel;
   if (Xpos>=Xmax) {
      Xpos = Xmax;
      Xvel = -Xvel;
      Zstep = -Zstep;
   }
   if (Xpos<=Xmin) {
      Xpos = Xmin;
      Xvel = -Xvel;
      Zstep = -Zstep;
   }

   Ypos += Yvel;
   Yvel += G;
   if (Ypos<Ymin) {
      Ypos = Ymin;
      if (vel0==-100.0)  vel0 = fabs(Yvel);
      Yvel = vel0;
   }
#ifdef __WIN32__
  InvalidateRect(tkGetHWND(),NULL,0);
#endif
}



static void draw( void )
{
   glClear( GL_COLOR_BUFFER_BIT );

   glPushMatrix();
   glTranslatef( Xpos, Ypos, 0.0 );
   glScalef( 2.0, 2.0, 2.0 );
   glRotatef( 8.0, 0.0, 0.0, 1.0 );
   glRotatef( 90.0, 1.0, 0.0, 0.0 );
   glRotatef( Zrot, 0.0, 0.0, 1.0 );


   glCallList( Ball );

   glPopMatrix();

   glFlush();
   tkSwapBuffers();
}


main( int argc, char *argv[] )
{

   tkInitPosition(0, 0, 300, 300);

   Mode = (GLenum) TK_INDEX;
   Mode |= TK_DOUBLE;
   Mode |= TK_DIRECT;
   tkInitDisplayMode(Mode);

   if (tkInitWindow("Bounce") == GL_FALSE) {
      tkQuit();
   }

   Ball = make_ball();
   glCullFace( GL_BACK );
   glEnable( GL_CULL_FACE );
   glDisable( GL_DITHER );
   glShadeModel( GL_FLAT );

   tkExposeFunc( reshape );
   tkReshapeFunc( reshape );
   tkKeyDownFunc( key );
   tkIdleFunc( idle );
   tkDisplayFunc( draw );
   tkExec();
}
