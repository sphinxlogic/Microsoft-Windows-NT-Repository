/* generic.c */

/* Demo of BEOS Mesa rendering */

/*
 * See Mesa/include/GL/osmesa.h for documentation of the OSMesa functions.
 *
 * If you want to render BIG images you'll probably have to increase
 * MAX_WIDTH and MAX_HEIGHT in src/config.h.
 *
 * This program is in the public domain.
 *
 * BEOS output provided by Tinic Urou
 * 5uro@informatik.uni-hamburg.de
 */

#include <AppKit.h>
#include <InterfaceKit.h>
#include <KernelKit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "GL/osmesa.h"
#include "GL/glu.h"
#include "gltk.h"

#define FALSE 0
#define TRUE  1
#ifndef PI
#define PI    3.14159265358979323846
#endif

GLenum doubleBuffer, directRender;
int W = 400, H = 400;

char *imageFileName = "1.rgb";
TK_RGBImageRec *image;

int numComponents;

float *minFilter, *magFilter, *sWrapMode, *tWrapMode;
float decal[] = {GL_DECAL};
float modulate[] = {GL_MODULATE};
float repeat[] = {GL_REPEAT};
float clamp[] = {GL_CLAMP};
float nnearest[] = {GL_NEAREST};
float linear[] = {GL_LINEAR};
float nearest_mipmap_nearest[] = {GL_NEAREST_MIPMAP_NEAREST};
float nearest_mipmap_linear[] = {GL_NEAREST_MIPMAP_LINEAR};
float linear_mipmap_nearest[] = {GL_LINEAR_MIPMAP_NEAREST};
float linear_mipmap_linear[] = {GL_LINEAR_MIPMAP_LINEAR};
GLint sphereMap[] = {GL_SPHERE_MAP};

float xRotation = 0.0, yRotation = 0.0;
float zTranslate = -4.0;
GLenum autoRotate = (GLenum)TRUE;
GLenum deepestColor = (GLenum)TK_GREEN;
GLenum isLit = (GLenum)TRUE;
GLenum isFogged = (GLenum)FALSE;
float *textureEnvironment = modulate;

struct MipMap {
    int width, height;
    unsigned char *data;
};

int cube, cage, cylinder, torus, genericObject;

float c[6][4][4][3] = {
    {
	{
	    {
		1.0, 1.0, -1.0
	    }, 
	    {
		0.0, 1.0, -1.0
	    },
	    {
		0.0, 0.0, -1.0
	    },
	    {
		1.0, 0.0, -1.0
	    },
	},
	{
	    {
		0.0, 1.0, -1.0
	    },
	    {
		-1.0, 1.0, -1.0
	    }, 
	    {
		-1.0, 0.0, -1.0
	    }, 
	    {
		0.0, 0.0, -1.0
	    },
	},
	{
	    {
		0.0,  0.0, -1.0
	    },
	    {
		-1.0, 0.0, -1.0
	    },
	    {
		-1.0, -1.0, -1.0
	    },
	    {
		0.0, -1.0, -1.0
	    },
	},
	{
	    {
		1.0, 0.0, -1.0
	    },
	    {
		0.0, 0.0, -1.0
	    },
	    {
		0.0, -1.0, -1.0
	    },
	    {
		1.0, -1.0, -1.0
	    },
	},
    },
    {
	{
	    {
		1.0, 1.0, 1.0
	    },
	    {
		1.0, 1.0, 0.0
	    },
	    {
		1.0, 0.0, 0.0
	    },
	    {
		1.0, 0.0, 1.0
	    },
	},
	{
	    {
		1.0, 1.0, 0.0
	    },
	    {
		1.0, 1.0, -1.0
	    },
	    {
		1.0, 0.0, -1.0
	    },
	    {
		1.0, 0.0, 0.0
	    },
	},
	{
	    {
		1.0, 0.0, -1.0
	    },
	    {
		1.0, -1.0, -1.0
	    },
	    {
		1.0, -1.0, 0.0
	    },
	    {
		1.0, 0.0, 0.0
	    },
	},
	{
	    {
		1.0, 0.0, 0.0
	    },
	    {
		1.0, -1.0, 0.0
	    },
	    {
		1.0, -1.0, 1.0
	    },
	    {
		1.0, 0.0, 1.0
	    },
	},
    },
    {
	{
	    {
		-1.0, 1.0, 1.0
	    },
	    {
		0.0, 1.0, 1.0
	    },
	    {
		0.0, 0.0, 1.0
	    },
	    {
		-1.0, 0.0, 1.0
	    },
	},
	{
	    {
		0.0, 1.0, 1.0
	    },
	    {
		1.0, 1.0, 1.0
	    },
	    {
		1.0, 0.0, 1.0
	    },
	    {
		0.0, 0.0, 1.0
	    },
	},
	{
	    {
		1.0, 0.0, 1.0
	    },
	    {
		1.0, -1.0, 1.0
	    },
	    {
		0.0, -1.0, 1.0
	    },
	    {
		0.0, 0.0, 1.0
	    },
	},
	{
	    {
		0.0, -1.0, 1.0
	    },
	    {
		-1.0, -1.0, 1.0
	    },
	    {
		-1.0, 0.0, 1.0
	    },
	    {
		0.0, 0.0, 1.0
	    },
	},
    },
    {
	{
	    {
		-1.0, 1.0, -1.0
	    },
	    {
		-1.0, 1.0, 0.0
	    },
	    {
		-1.0, 0.0, 0.0
	    },
	    {
		-1.0, 0.0, -1.0
	    },
	}, 
	{
	    {
		-1.0, 1.0, 0.0
	    },
	    {
		-1.0, 1.0, 1.0
	    },
	    {
		-1.0, 0.0, 1.0
	    },
	    {
		-1.0, 0.0, 0.0
	    },
	}, 
	{
	    {
		-1.0, 0.0, 1.0
	    },
	    {
		-1.0, -1.0, 1.0
	    },
	    {
		-1.0, -1.0, 0.0
	    },
	    {
		-1.0, 0.0, 0.0
	    },
	}, 
	{
	    {
		-1.0, -1.0, 0.0
	    },
	    {
		-1.0, -1.0, -1.0
	    },
	    {
		-1.0, 0.0, -1.0
	    },
	    {
		-1.0, 0.0, 0.0
	    },
	}, 
    },
    {
	{
	    {
		-1.0, 1.0, 1.0
	    },
	    {
		-1.0, 1.0, 0.0
	    },
	    {
		0.0, 1.0, 0.0
	    },
	    {
		0.0, 1.0, 1.0
	    },
	},
	{
	    {
		-1.0, 1.0, 0.0
	    },
	    {
		-1.0, 1.0, -1.0
	    },
	    {
		0.0, 1.0, -1.0
	    },
	    {
		0.0, 1.0, 0.0
	    },
	},
	{
	    {
		0.0, 1.0, -1.0
	    },
	    {
		1.0, 1.0, -1.0
	    },
	    {
		1.0, 1.0, 0.0
	    },
	    {
		0.0, 1.0, 0.0
	    },
	},
	{
	    {
		1.0, 1.0, 0.0
	    },
	    {
		1.0, 1.0, 1.0
	    },
	    {
		0.0, 1.0, 1.0
	    },
	    {
		0.0, 1.0, 0.0
	    },
	},
    },
    {
	{
	    {
		-1.0, -1.0, -1.0
	    },
	    {
		-1.0, -1.0, 0.0
	    },
	    {
		0.0, -1.0, 0.0
	    },
	    {
		0.0, -1.0, -1.0
	    },
	},
	{
	    {
		-1.0, -1.0, 0.0
	    },
	    {
		-1.0, -1.0, 1.0
	    },
	    {
		0.0, -1.0, 1.0
	    },
	    {
		0.0, -1.0, 0.0
	    },
	},
	{
	    {
		0.0, -1.0, 1.0
	    },
	    {
		1.0, -1.0, 1.0
	    },
	    {
		1.0, -1.0, 0.0
	    },
	    {
		0.0, -1.0, 0.0
	    },
	},
	{
	    {
		1.0, -1.0, 0.0
	    },
	    {
		1.0, -1.0, -1.0
	    },
	    {
		0.0, -1.0, -1.0
	    },
	    {
		0.0, -1.0, 0.0
	    },
	},
    }
};

float n[6][3] = {
    {
	0.0, 0.0, -1.0
    },
    {
	1.0, 0.0, 0.0
    },
    {
	0.0, 0.0, 1.0
    },
    {
	-1.0, 0.0, 0.0
    },
    {
	0.0, 1.0, 0.0
    },
    {
	0.0, -1.0, 0.0
    }
};

GLfloat identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
};


void BuildCylinder(int numEdges)
{
    int i, top = 1.0, bottom = -1.0;
    float x[100], y[100], angle; 
    
    for (i = 0; i <= numEdges; i++) {
	angle = i * 2.0 * PI / numEdges;
	x[i] = cos(angle);   /* was cosf() */
	y[i] = sin(angle);   /* was sinf() */
    }

    glNewList(cylinder, GL_COMPILE);
    glBegin(GL_TRIANGLE_STRIP);
	for (i = 0; i <= numEdges; i++) {
	    glNormal3f(x[i], y[i], 0.0);
	    glVertex3f(x[i], y[i], bottom);
	    glVertex3f(x[i], y[i], top);
	}
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, top);
	for (i = 0; i <= numEdges; i++) {
	    glVertex3f(x[i], -y[i], top);
	}
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 0.0, bottom);
	for (i = 0; i <= numEdges; i++) {
	    glVertex3f(x[i], y[i], bottom);
	}
    glEnd();
    glEndList();
}

void BuildTorus(float rc, int numc, float rt, int numt)
{
    int i, j, k;
    double s, t;
    double x, y, z;
    double pi, twopi;

    pi = 3.14159265358979323846;
    twopi = 2.0 * pi;
 
    glNewList(torus, GL_COMPILE);
    for (i = 0; i < numc; i++) {
	glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= numt; j++) {
	    for (k = 0; k <= 1; k++) {
		s = (i + k) % numc + 0.5;
		t = j % numt;

		x = cos(t*twopi/numt) * cos(s*twopi/numc);
		y = sin(t*twopi/numt) * cos(s*twopi/numc);
		z = sin(s*twopi/numc);
		glNormal3f(x, y, z);

		x = (rt + rc * cos(s*twopi/numc)) * cos(t*twopi/numt);
		y = (rt + rc * cos(s*twopi/numc)) * sin(t*twopi/numt);
		z = rc * sin(s*twopi/numc);
		glVertex3f(x, y, z);
	    }
        }
	glEnd();
    }
    glEndList();
}

void BuildCage(void)
{
    int i, j;
    float inc;
    float right, left, top, bottom, front, back;

    front  = 0.0;
    back   = -8.0;

    left   = -4.0;
    bottom = -4.0;
    right  = 4.0;
    top    = 4.0; 

    inc = 2.0 * 4.0 * 0.1;

    glNewList(cage, GL_COMPILE);
    for (i = 0; i < 10; i++) {

	/*
	** Back
	*/
	glBegin(GL_LINES);
	    glVertex3f(left+i*inc, top,    back);
	    glVertex3f(left+i*inc, bottom, back);
	glEnd();
	glBegin(GL_LINES);
	    glVertex3f(right, bottom+i*inc, back);
	    glVertex3f(left,  bottom+i*inc, back);
	glEnd();

	/*
	** Front
	*/
	glBegin(GL_LINES);
	    glVertex3f(left+i*inc, top,    front);
	    glVertex3f(left+i*inc, bottom, front);
	glEnd();
	glBegin(GL_LINES);
	    glVertex3f(right, bottom+i*inc, front);
	    glVertex3f(left,  bottom+i*inc, front);
	glEnd();

	/*
	** Left
	*/
	glBegin(GL_LINES);
	    glVertex3f(left, bottom+i*inc, front);
	    glVertex3f(left, bottom+i*inc, back);
	glEnd();
	glBegin(GL_LINES);
	    glVertex3f(left, top,    back+i*inc);
	    glVertex3f(left, bottom, back+i*inc);
	glEnd();

	/*
	** Right
	*/
	glBegin(GL_LINES);
	    glVertex3f(right, top-i*inc, front);
	    glVertex3f(right, top-i*inc, back);
	glEnd();
	glBegin(GL_LINES);
	    glVertex3f(right, top,    back+i*inc);
	    glVertex3f(right, bottom, back+i*inc);
	glEnd();

	/*
	** Top
	*/
	glBegin(GL_LINES);
	    glVertex3f(left+i*inc, top, front);
	    glVertex3f(left+i*inc, top, back);
	glEnd();
	glBegin(GL_LINES);
	    glVertex3f(right, top, back+i*inc);
	    glVertex3f(left,  top, back+i*inc);
	glEnd();

	/*
	** Bottom
	*/
	glBegin(GL_LINES);
	    glVertex3f(right-i*inc, bottom, front);
	    glVertex3f(right-i*inc, bottom, back);
	glEnd();
	glBegin(GL_LINES);
	    glVertex3f(right, bottom, back+i*inc);
	    glVertex3f(left,  bottom, back+i*inc);
	glEnd();
    }
    glEndList();
}

void BuildCube(void)
{
    int i, j;

    glNewList(cube, GL_COMPILE);
    for (i = 0; i < 6; i++) {
	for (j = 0; j < 4; j++) {
	    glNormal3fv(n[i]); 
	    glBegin(GL_POLYGON);
		glVertex3fv(c[i][j][0]);
		glVertex3fv(c[i][j][1]);
		glVertex3fv(c[i][j][2]);
		glVertex3fv(c[i][j][3]);
	    glEnd();
	}
    }
    glEndList();
}

void BuildLists(void)
{

    cube = glGenLists(1);
    BuildCube();

    cage = glGenLists(2);
    BuildCage();

    cylinder = glGenLists(3);
    BuildCylinder(60);

    torus = glGenLists(4);
    BuildTorus(0.65, 20, .85, 65);

    genericObject = torus;
}

void SetDeepestColor(void)
{
    GLint redBits, greenBits, blueBits;

    glGetIntegerv(GL_RED_BITS, &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS, &blueBits);

    deepestColor = (GLenum)((redBits >= greenBits) ? TK_RED : TK_GREEN);
    deepestColor = (GLenum)((deepestColor >= blueBits) ? deepestColor : TK_BLUE); 
}

void SetDefaultSettings(void)
{

    magFilter = nnearest;
    minFilter = nnearest;
    sWrapMode = repeat;
    tWrapMode = repeat;
    textureEnvironment = modulate;
    autoRotate = (GLenum)TRUE;
}

unsigned char *AlphaPadImage(int bufSize, unsigned char *inData, int alpha)
{
    unsigned char *outData, *out_ptr, *in_ptr;
    int i;

    outData = (unsigned char *) malloc(bufSize * 4);
    out_ptr = outData;
    in_ptr = inData;

    for (i = 0; i < bufSize; i++) {
	*out_ptr++ = *in_ptr++;
	*out_ptr++ = *in_ptr++;
	*out_ptr++ = *in_ptr++;
	*out_ptr++ = alpha;
    }

    free (inData);
    return outData;
}

void Init(void)
{
    float ambient[] = {0.0, 0.0, 0.0, 1.0};
    float diffuse[] = {0.0, 1.0, 0.0, 1.0};
    float specular[] = {1.0, 1.0, 1.0, 1.0};
    float position[] = {2.0, 2.0,  0.0, 1.0};
    float fog_color[] = {0.0, 0.0, 0.0, 1.0};
    float mat_ambient[] = {0.0, 0.0, 0.0, 1.0};
    float mat_shininess[] = {90.0};
    float mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    float mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    float lmodel_ambient[] = {0.0, 0.0, 0.0, 1.0};
    float lmodel_twoside[] = {GL_TRUE};
    float lmodel_local_viewer[] = {GL_FALSE};

    SetDeepestColor();
    SetDefaultSettings();

    if (numComponents == 4) {
	image = tkRGBImageLoad(imageFileName);
	image->data = AlphaPadImage(image->sizeX*image->sizeY,
                                    image->data, 128);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, numComponents, 
			  image->sizeX, image->sizeY, 
			  GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    } else {
	image = tkRGBImageLoad(imageFileName);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, numComponents, 
			  image->sizeX, image->sizeY, 
			  GL_RGB, GL_UNSIGNED_BYTE, image->data);
    }
    
    glFogf(GL_FOG_DENSITY, 0.125);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 4.0);
    glFogf(GL_FOG_END, 9.0);
    glFogfv(GL_FOG_COLOR, fog_color);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, W, H);
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_TEXTURE_2D);
    glTexGeniv(GL_S, GL_TEXTURE_GEN_MODE, sphereMap);
    glTexGeniv(GL_T, GL_TEXTURE_GEN_MODE, sphereMap);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);

    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textureEnvironment);

    BuildLists();
}

void ReInit(void)
{

    if (genericObject == torus) {
	glEnable(GL_DEPTH_TEST);
    } else  {
	glDisable(GL_DEPTH_TEST);
    }
    if (isFogged) {
	textureEnvironment = modulate;
    }

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textureEnvironment);
}

void Draw(void)
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.2, 0.2, -0.2, 0.2, 0.15, 9.0);
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if (isFogged) {
	glEnable(GL_FOG);
	glColor3fv(tkRGBMap[deepestColor]);
    } else {
	glColor3fv(tkRGBMap[TK_WHITE]);
    }
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_TEXTURE_2D);
    glCallList(cage);

    glPushMatrix();
    glTranslatef(0.0, 0.0, zTranslate);
    glRotatef(xRotation, 1, 0, 0);
    glRotatef(yRotation, 0, 1, 0);

    if (isLit == TRUE) {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    }

    glEnable(GL_TEXTURE_2D);
    if (isFogged) {
	glDisable(GL_FOG);
    }
    glPolygonMode(GL_FRONT, GL_FILL);
    glColor3fv(tkRGBMap[deepestColor]);
    glCallList(genericObject);

    glPopMatrix();
    glFlush();

    if (autoRotate) {
	xRotation += .75;
	yRotation += .375;
    } 
    tkSwapBuffers();
}

void Reshape(int width, int height)
{

    W = width;
    H = height;
    ReInit();
    glViewport( 0, 0, width, height );  /*new*/
}

GLenum Args(int argc, char **argv)
{
    GLint i;

    doubleBuffer = GL_FALSE;
    directRender = GL_TRUE;
    numComponents = 4;

    for (i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-sb") == 0) {
	    doubleBuffer = GL_FALSE;
	} else if (strcmp(argv[i], "-db") == 0) {
	    doubleBuffer = GL_TRUE;
	} else if (strcmp(argv[i], "-dr") == 0) {
	    directRender = GL_TRUE;
	} else if (strcmp(argv[i], "-ir") == 0) {
	    directRender = GL_FALSE;
	} else if (strcmp(argv[i], "-f") == 0) {
	    if (i+1 >= argc || argv[i+1][0] == '-') {
		printf("-f (No file name).\n");
		return GL_FALSE;
	    } else {
		imageFileName = argv[++i];
	    }
	} else if (strcmp(argv[i], "-4") == 0) {
	    numComponents = 4;
	} else if (strcmp(argv[i], "-3") == 0) {
	    numComponents = 3;
	} else {
	    printf("%s (Bad option).\n", argv[i]);
	    return GL_FALSE;
	}
    }
    return GL_TRUE;
}

int 					gl_width=300,gl_height=300;

const 	ulong 			APP_SIGNATURE = '????';
const 	ulong			MSG_REDRAW = 1;

class 					MesaWindow;
class 					MesaView;
class					MesaApp;

// Lets make our life easy and make them global:

BBitmap 				*the_bitmap;
MesaView				*the_view;	
MesaWindow 			    *the_window;

class MesaView : public BView
{
	public:
	
	MesaView(BRect frame):BView(frame,"Mesa View",B_FOLLOW_NONE,B_WILL_DRAW)
	{
	};
	
	void KeyDown(ulong aKey)
	{
		switch(aKey)
		{
      case B_LEFT_ARROW:
	yRotation -= 0.5;
	autoRotate = (GLenum)FALSE;
	ReInit();
	break;
      case B_RIGHT_ARROW:
	yRotation += 0.5;
	autoRotate = (GLenum)FALSE;
	ReInit();
	break;
      case B_UP_ARROW:
	xRotation -= 0.5;
	autoRotate = (GLenum)FALSE;
	ReInit();
	break;
      case B_DOWN_ARROW:
	xRotation += 0.5;
	autoRotate = (GLenum)FALSE;
	ReInit();
	break;
      case 'a':
	autoRotate = (GLenum)!autoRotate;
	ReInit();
	break;
      case 'c':
	genericObject = (GLenum)((genericObject == cube) ? cylinder : cube);
	ReInit();
	break;
      case 'd':
	textureEnvironment = decal;
	ReInit();
	break;
      case 'm':
	textureEnvironment = modulate;
	ReInit();
	break;
      case 'l':
	isLit = (GLenum)!isLit;
	ReInit();
	break;
      case 'f':
	isFogged = (GLenum)!isFogged;
	ReInit();
	break;
      case 't':
	genericObject = (GLenum)torus;
	ReInit();
	break;
      case '0':
	magFilter = nnearest;
	ReInit();
	break;
      case '1':
	magFilter = linear;
	ReInit();
	break;
      case '2':
	minFilter = nnearest;
	ReInit();
	break;
      case '3':
	minFilter = linear;
	ReInit();
	break;
      case '4':
	minFilter = nearest_mipmap_nearest;
	ReInit();
	break;
      case '5':
	minFilter = nearest_mipmap_linear;
	ReInit();
	break;
      case '6':
	minFilter = linear_mipmap_nearest;
	ReInit();
	break;
      case '7':
	minFilter = linear_mipmap_linear;
	ReInit();
	break;
		}
	};
	
	void Draw(BRect frame)
	{
		DrawBitmap(the_bitmap,BPoint(0,0));
	};					
};

class MesaWindow : public BWindow 
{
	public:
	
	MesaWindow(int width, int height):BWindow(BRect(0,0,width-1,height-1),"MesaView",B_TITLED_WINDOW,B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
	{
		// Move window to right position
		MoveTo(80, 24);
		// Create bitmap view
		Lock();
		AddChild(the_view = new MesaView(BRect(0, 0, (gl_width)-1, (gl_height)-1)));
		the_view->MakeFocus();
		Unlock();
	};
	
	void MessageReceived(BMessage *msg)
	{
		switch(msg->what)
		{	
			case	MSG_REDRAW:
					Lock();
					the_view->DrawBitmap(the_bitmap,BPoint(0,0));
					Draw();
					Unlock();
					PostMessage(MSG_REDRAW);
					break;
			default:
					BWindow::MessageReceived(msg);
					break;
		}
	};
	
	bool QuitRequested(void)
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return TRUE;
	};
};

class MesaApp : public BApplication 
{
	OSMesaContext ctx;
	
	public:
	
	MesaApp():BApplication(APP_SIGNATURE)
	{
		the_window = NULL;
		the_bitmap = NULL;
	};
			
	void ReadyToRun(void)
	{
		// Allocate the bitmap		
		the_bitmap = new BBitmap(BRect(0, 0, gl_width-1, gl_height-1), B_RGB_32_BIT);

		uchar *bits = (uchar *)the_bitmap->Bits();

		memset(bits,0,the_bitmap->BytesPerRow()*gl_height);

		// Open window
		the_window = new MesaWindow((gl_width),(gl_height));
		the_window->Show();

		unsigned char *buffer;
		double start,end;

		// Create an RGBA-mode context 
		ctx = OSMesaCreateContext( GL_RGBA, NULL );

		// Bind the buffer to the context and make it current */
		OSMesaMakeCurrent( ctx, bits, GL_UNSIGNED_BYTE, gl_width, gl_height );
		OSMesaPixelStore( OSMESA_Y_UP, 0 );

    	Init();
    	Reshape(gl_width,gl_height);
		the_window->PostMessage(MSG_REDRAW);	
	};

	bool QuitRequested(void)
	{
		// Make sure that the window closes first
		if (BApplication::QuitRequested()) 
		{
			OSMesaDestroyContext( ctx );
			if (the_bitmap)	delete the_bitmap;
			return TRUE;
		}
		return FALSE;
	};
			
	void AboutRequested(void)
	{
		char str[256];
		sprintf(str, "MesaDemos, ported by Tinic Urou\n<5uro@informatik.uni-hamburg.de>\nFreely distributable.");
		BAlert *the_alert = new BAlert("", str, "OK");
		the_alert->Go();
	};
};

int main( int argc, char *argv[] )
{
	MesaApp *the_app;

	Args(argc,argv);

	the_app = new MesaApp();
	the_app->Run();
	delete the_app;

	return 0;
}
