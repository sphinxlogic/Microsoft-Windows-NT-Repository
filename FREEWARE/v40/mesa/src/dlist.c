/* $Id: dlist.c,v 1.12 1997/02/27 19:58:08 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  2.2
 * Copyright (C) 1995-1997  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log: dlist.c,v $
 * Revision 1.12  1997/02/27 19:58:08  brianp
 * call gl_problem() instead of gl_warning()
 *
 * Revision 1.11  1997/02/09 18:50:18  brianp
 * added GL_EXT_texture3D support
 *
 * Revision 1.10  1997/01/29 18:51:30  brianp
 * small MEMCPY call change for Acorn compiler, per Graham Jones
 *
 * Revision 1.9  1997/01/09 21:25:28  brianp
 * set reference count to one for texture images in display lists
 *
 * Revision 1.8  1996/12/11 20:19:11  brianp
 * abort display list execution if invalid opcode found
 *
 * Revision 1.7  1996/12/09 21:39:23  brianp
 * compare list to MAX_DISPLAYLISTS in gl_IsList()
 *
 * Revision 1.6  1996/12/04 22:14:27  brianp
 * improved the mesa_print_display_list() debug function
 *
 * Revision 1.5  1996/11/07 04:12:45  brianp
 * texture images are now gl_image structs, not gl_texture_image structs
 *
 * Revision 1.4  1996/10/16 00:59:12  brianp
 * fixed bug in gl_save_Lightfv()
 * execution of OPCODE_EDGE_FLAG used enum value instead of boolean
 *
 * Revision 1.3  1996/09/27 01:26:08  brianp
 * removed unused variables
 *
 * Revision 1.2  1996/09/19 00:54:43  brianp
 * fixed bug in gl_save_Rotatef() when in GL_COMPILE_AND_EXECUTE mode
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "accum.h"
#include "alpha.h"
#include "attrib.h"
#include "bitmap.h"
#include "blend.h"
#include "clip.h"
#include "context.h"
#include "copypix.h"
#include "depth.h"
#include "draw.h"
#include "drawpix.h"
#include "enable.h"
#include "eval.h"
#include "feedback.h"
#include "fog.h"
#include "image.h"
#include "light.h"
#include "lines.h"
#include "dlist.h"
#include "logic.h"
#include "macros.h"
#include "masking.h"
#include "matrix.h"
#include "misc.h"
#include "pixel.h"
#include "points.h"
#include "polygon.h"
#include "scissor.h"
#include "stencil.h"
#include "texobj.h"
#include "teximage.h"
#include "texture.h"
#include "types.h"
#include "vb.h"
#include "vertex.h"
#include "winpos.h"



#if 0
#define EXEC(FUNC)  (*ctx->Exec.FUNC)
#else
#define EXEC(FUNC)  gl_##FUNC
#endif




/*
Functions which aren't compiled but executed immediately:
	glIsList
	glGenLists
	glDeleteLists
	glEndList
	glFeedbackBuffer
	glSelectBuffer
	glRenderMode
	glReadPixels
	glPixelStore
	glFlush
	glFinish
	glIsEnabled
	glGet*

Functions which cause errors if called while compiling a display list:
	glNewList
*/



/*
 * Display list instructions are stored as sequences of "nodes".  Nodes
 * are allocated in blocks.  Each block has BLOCK_SIZE nodes.  Blocks
 * are linked together with a pointer.
 */


/* How many nodes to allocate at a time: */
#define BLOCK_SIZE 500


/*
 * Display list opcodes.
 *
 * The fact that these identifiers are assigned consecutive
 * integer values starting at 0 is very important, see InstSize array usage)
 */
typedef enum {
	OPCODE_ACCUM,
	OPCODE_ALPHA_FUNC,
        OPCODE_BEGIN,
        OPCODE_BIND_TEXTURE,
	OPCODE_BITMAP,
	OPCODE_BLEND_COLOR,
	OPCODE_BLEND_EQUATION,
	OPCODE_BLEND_FUNC,
        OPCODE_CALL_LIST,
        OPCODE_CALL_LIST_OFFSET,
	OPCODE_CLEAR,
	OPCODE_CLEAR_ACCUM,
	OPCODE_CLEAR_COLOR,
	OPCODE_CLEAR_DEPTH,
	OPCODE_CLEAR_INDEX,
	OPCODE_CLEAR_STENCIL,
        OPCODE_CLIP_PLANE,
	OPCODE_COLOR_4F,
	OPCODE_COLOR_4UB,
	OPCODE_COLOR_MASK,
	OPCODE_COLOR_MATERIAL,
	OPCODE_COPY_PIXELS,
        OPCODE_COPY_TEX_IMAGE1D,
        OPCODE_COPY_TEX_IMAGE2D,
        OPCODE_COPY_TEX_IMAGE3D,
        OPCODE_COPY_TEX_SUB_IMAGE1D,
        OPCODE_COPY_TEX_SUB_IMAGE2D,
        OPCODE_COPY_TEX_SUB_IMAGE3D,
	OPCODE_CULL_FACE,
	OPCODE_DEPTH_FUNC,
	OPCODE_DEPTH_MASK,
	OPCODE_DEPTH_RANGE,
	OPCODE_DISABLE,
	OPCODE_DRAW_BUFFER,
	OPCODE_DRAW_PIXELS,
        OPCODE_EDGE_FLAG,
	OPCODE_ENABLE,
        OPCODE_END,
	OPCODE_EVALCOORD1,
	OPCODE_EVALCOORD2,
	OPCODE_EVALMESH1,
	OPCODE_EVALMESH2,
	OPCODE_EVALPOINT1,
	OPCODE_EVALPOINT2,
	OPCODE_FOG,
	OPCODE_FRONT_FACE,
	OPCODE_FRUSTUM,
	OPCODE_HINT,
	OPCODE_INDEX,
	OPCODE_INDEX_MASK,
	OPCODE_INIT_NAMES,
	OPCODE_LIGHT,
	OPCODE_LIGHT_MODEL,
	OPCODE_LINE_STIPPLE,
	OPCODE_LINE_WIDTH,
	OPCODE_LIST_BASE,
	OPCODE_LOAD_MATRIX,
	OPCODE_LOAD_NAME,
	OPCODE_LOGIC_OP,
	OPCODE_MAP1,
	OPCODE_MAP2,
	OPCODE_MAPGRID1,
	OPCODE_MAPGRID2,
	OPCODE_MATERIAL,
	OPCODE_MATRIX_MODE,
	OPCODE_MULT_MATRIX,
        OPCODE_NORMAL,
	OPCODE_PASSTHROUGH,
	OPCODE_PIXEL_MAP,
	OPCODE_PIXEL_TRANSFER,
	OPCODE_PIXEL_ZOOM,
	OPCODE_POINTSIZE,
	OPCODE_POLYGON_MODE,
        OPCODE_POLYGON_STIPPLE,
	OPCODE_POLYGON_OFFSET,
	OPCODE_POP_ATTRIB,
	OPCODE_POP_MATRIX,
	OPCODE_POP_NAME,
	OPCODE_PRIORITIZE_TEXTURE,
	OPCODE_PUSH_ATTRIB,
	OPCODE_PUSH_MATRIX,
	OPCODE_PUSH_NAME,
	OPCODE_RASTER_POS,
	OPCODE_READ_BUFFER,
        OPCODE_SCALE,
	OPCODE_SCISSOR,
	OPCODE_SHADE_MODEL,
	OPCODE_STENCIL_FUNC,
	OPCODE_STENCIL_MASK,
	OPCODE_STENCIL_OP,
	OPCODE_TEXCOORD,
        OPCODE_TEXENV,
        OPCODE_TEXGEN,
        OPCODE_TEXPARAMETER,
	OPCODE_TEX_IMAGE1D,
	OPCODE_TEX_IMAGE2D,
	OPCODE_TEX_IMAGE3D,
	OPCODE_TEX_SUB_IMAGE1D,
	OPCODE_TEX_SUB_IMAGE2D,
	OPCODE_TEX_SUB_IMAGE3D,
        OPCODE_TRANSLATE,
        OPCODE_VERTEX,
	OPCODE_VIEWPORT,
	OPCODE_WINDOW_POS,
	/* The following two are meta instructions */
	OPCODE_CONTINUE,
	OPCODE_END_OF_LIST
} OpCode;


/*
 * Each instruction in the display list is stored as a sequence of
 * contiguous nodes in memory.
 * Each node is the union of a variety of datatypes.
 */
typedef union node {
	OpCode		opcode;
	GLboolean	b;
	GLbitfield	bf;
	GLubyte		ub;
	GLshort		s;
	GLushort	us;
	GLint		i;
	GLuint		ui;
	GLenum		e;
	GLfloat		f;
	GLvoid		*data;
	void		*next;	/* If prev node's opcode==OPCODE_CONTINUE */
} Node;



/* Number of nodes of storage needed for each instruction: */
static GLuint InstSize[ OPCODE_END_OF_LIST+1 ];


/* Used while a display list is under construction: */
static Node *CurrentListPtr;	/* Head of list being compiled */
static GLuint CurrentListNum;	/* Number of the list being compiled */
static Node *CurrentBlock;	/* Pointer to current block of nodes */
static GLuint CurrentPos;	/* Index into current block of nodes */




/**********************************************************************/
/*****                           Private                          *****/
/**********************************************************************/


/*
 * Allocate space for a display list instruction.
 * Input:  opcode - type of instruction
 *         argcount - number of arguments following the instruction
 * Return: pointer to first node in the instruction
 */
static Node *alloc_instruction( GLcontext *ctx, OpCode opcode, GLint argcount )
{
   Node *n, *newblock;
   GLuint count = InstSize[opcode];

   assert( count == argcount+1 );

   if (CurrentPos + count + 2 > BLOCK_SIZE) {
      /* This block is full.  Allocate a new block and chain to it */
      n = CurrentBlock + CurrentPos;
      n[0].opcode = OPCODE_CONTINUE;
      newblock = (Node *) malloc( sizeof(Node) * BLOCK_SIZE );
      if (!newblock) {
         gl_error( ctx, GL_OUT_OF_MEMORY, "Building display list" );
         return NULL;
      }
      n[1].next = (Node *) newblock;
      CurrentBlock = newblock;
      CurrentPos = 0;
   }

   n = CurrentBlock + CurrentPos;
   CurrentPos += count;

   n[0].opcode = opcode;

   return n;
}



/*
 * Make an empty display list.  This is used by glGenLists() to
 * reserver display list IDs.
 */
static Node *make_empty_list( void )
{
   Node *n = (Node *) malloc( sizeof(Node) );
   n[0].opcode = OPCODE_END_OF_LIST;
   return n;
}



/*
 * Destroy all nodes in a display list.
 * Input:  list - list number in [0..]
 */
static void destroy_list( GLcontext *ctx, GLuint list )
{
   Node *n, *block;
   GLboolean done;

   block = n = ctx->Shared->List[list];

   done = GL_FALSE;
   while (!done) {
      switch (n[0].opcode) {
	 /* special cases first */
	 case OPCODE_MAP1:
	    gl_free_control_points( ctx, n[1].e, (GLfloat *) n[6].data );
	    n += InstSize[n[0].opcode];
	    break;
	 case OPCODE_MAP2:
	    gl_free_control_points( ctx, n[1].e, (GLfloat *) n[10].data );
	    n += InstSize[n[0].opcode];
	    break;
	 case OPCODE_DRAW_PIXELS:
	    free( n[5].data );
	    n += InstSize[n[0].opcode];
	    break;
	 case OPCODE_BITMAP:
	    gl_free_image( (struct gl_image *) n[7].data );
	    n += InstSize[n[0].opcode];
	    break;
         case OPCODE_POLYGON_STIPPLE:
            free( n[1].data );
	    n += InstSize[n[0].opcode];
            break;
	 case OPCODE_TEX_IMAGE1D:
            gl_free_image( (struct gl_image *) n[8].data );
            n += InstSize[n[0].opcode];
	    break;
	 case OPCODE_TEX_IMAGE2D:
            gl_free_image( (struct gl_image *) n[9].data );
            n += InstSize[n[0].opcode];
	    break;
         case OPCODE_TEX_SUB_IMAGE1D:
            {
               struct gl_image *image;
               image = (struct gl_image *) n[7].data;
               gl_free_image( image );
            }
            break;
         case OPCODE_TEX_SUB_IMAGE2D:
            {
               struct gl_image *image;
               image = (struct gl_image *) n[9].data;
               gl_free_image( image );
            }
            break;
	 case OPCODE_CONTINUE:
	    n = (Node *) n[1].next;
	    free( block );
	    block = n;
	    break;
	 case OPCODE_END_OF_LIST:
	    free( block );
	    done = GL_TRUE;
	    break;
	 default:
	    /* Most frequent case */
	    n += InstSize[n[0].opcode];
	    break;
      }
   }

   ctx->Shared->List[list] = NULL;
}



/*
 * Translate the nth element of list from type to GLuint.
 */
static GLuint translate_id( GLsizei n, GLenum type, const GLvoid *list )
{
   GLbyte *bptr;
   GLubyte *ubptr;
   GLshort *sptr;
   GLushort *usptr;
   GLint *iptr;
   GLuint *uiptr;
   GLfloat *fptr;

   switch (type) {
      case GL_BYTE:
         bptr = (GLbyte *) list;
         return (GLuint) *(bptr+n);
      case GL_UNSIGNED_BYTE:
         ubptr = (GLubyte *) list;
         return (GLuint) *(ubptr+n);
      case GL_SHORT:
         sptr = (GLshort *) list;
         return (GLuint) *(sptr+n);
      case GL_UNSIGNED_SHORT:
         usptr = (GLushort *) list;
         return (GLuint) *(usptr+n);
      case GL_INT:
         iptr = (GLint *) list;
         return (GLuint) *(iptr+n);
      case GL_UNSIGNED_INT:
         uiptr = (GLuint *) list;
         return (GLuint) *(uiptr+n);
      case GL_FLOAT:
         fptr = (GLfloat *) list;
         return (GLuint) *(fptr+n);
      case GL_2_BYTES:
         ubptr = ((GLubyte *) list) + 2*n;
         return (GLuint) *ubptr * 256 + (GLuint) *(ubptr+1);
      case GL_3_BYTES:
         ubptr = ((GLubyte *) list) + 3*n;
         return (GLuint) *ubptr * 65536
              + (GLuint) *(ubptr+1) * 256
              + (GLuint) *(ubptr+2);
      case GL_4_BYTES:
         ubptr = ((GLubyte *) list) + 4*n;
         return (GLuint) *ubptr * 16777216
              + (GLuint) *(ubptr+1) * 65536
              + (GLuint) *(ubptr+2) * 256
              + (GLuint) *(ubptr+3);
      default:
         return 0;
   }
}




/**********************************************************************/
/*****                        Public                              *****/
/**********************************************************************/

void gl_init_lists( void )
{
   static int init_flag = 0;

   if (init_flag==0) {
      CurrentListPtr = CurrentBlock = NULL;
      CurrentListNum = 0;

      InstSize[OPCODE_ACCUM] = 3;
      InstSize[OPCODE_ALPHA_FUNC] = 3;
      InstSize[OPCODE_BEGIN] = 2;
      InstSize[OPCODE_BIND_TEXTURE] = 3;
      InstSize[OPCODE_BITMAP] = 8;
      InstSize[OPCODE_BLEND_COLOR] = 5;
      InstSize[OPCODE_BLEND_EQUATION] = 2;
      InstSize[OPCODE_BLEND_FUNC] = 3;
      InstSize[OPCODE_CALL_LIST] = 2;
      InstSize[OPCODE_CALL_LIST_OFFSET] = 2;
      InstSize[OPCODE_CLEAR] = 2;
      InstSize[OPCODE_CLEAR_ACCUM] = 5;
      InstSize[OPCODE_CLEAR_COLOR] = 5;
      InstSize[OPCODE_CLEAR_DEPTH] = 2;
      InstSize[OPCODE_CLEAR_INDEX] = 2;
      InstSize[OPCODE_CLEAR_STENCIL] = 2;
      InstSize[OPCODE_CLIP_PLANE] = 6;
      InstSize[OPCODE_COLOR_4F] = 5;
      InstSize[OPCODE_COLOR_4UB] = 5;
      InstSize[OPCODE_COLOR_MASK] = 5;
      InstSize[OPCODE_COLOR_MATERIAL] = 3;
      InstSize[OPCODE_COPY_PIXELS] = 6;
      InstSize[OPCODE_COPY_TEX_IMAGE1D] = 8;
      InstSize[OPCODE_COPY_TEX_IMAGE2D] = 9;
      InstSize[OPCODE_COPY_TEX_SUB_IMAGE1D] = 7;
      InstSize[OPCODE_COPY_TEX_SUB_IMAGE2D] = 9;
      InstSize[OPCODE_COPY_TEX_SUB_IMAGE3D] = 10;
      InstSize[OPCODE_CULL_FACE] = 2;
      InstSize[OPCODE_DEPTH_FUNC] = 2;
      InstSize[OPCODE_DEPTH_MASK] = 2;
      InstSize[OPCODE_DEPTH_RANGE] = 3;
      InstSize[OPCODE_DISABLE] = 2;
      InstSize[OPCODE_DRAW_BUFFER] = 2;
      InstSize[OPCODE_DRAW_PIXELS] = 6;
      InstSize[OPCODE_ENABLE] = 2;
      InstSize[OPCODE_EDGE_FLAG] = 2;
      InstSize[OPCODE_END] = 1;
      InstSize[OPCODE_EVALCOORD1] = 2;
      InstSize[OPCODE_EVALCOORD2] = 3;
      InstSize[OPCODE_EVALMESH1] = 4;
      InstSize[OPCODE_EVALMESH2] = 6;
      InstSize[OPCODE_EVALPOINT1] = 2;
      InstSize[OPCODE_EVALPOINT2] = 3;
      InstSize[OPCODE_FOG] = 6;
      InstSize[OPCODE_FRONT_FACE] = 2;
      InstSize[OPCODE_FRUSTUM] = 7;
      InstSize[OPCODE_HINT] = 3;
      InstSize[OPCODE_INDEX] = 2;
      InstSize[OPCODE_INDEX_MASK] = 2;
      InstSize[OPCODE_INIT_NAMES] = 1;
      InstSize[OPCODE_LIGHT] = 7;
      InstSize[OPCODE_LIGHT_MODEL] = 6;
      InstSize[OPCODE_LINE_STIPPLE] = 3;
      InstSize[OPCODE_LINE_WIDTH] = 2;
      InstSize[OPCODE_LIST_BASE] = 2;
      InstSize[OPCODE_LOAD_MATRIX] = 17;
      InstSize[OPCODE_LOAD_NAME] = 2;
      InstSize[OPCODE_LOGIC_OP] = 2;
      InstSize[OPCODE_MAP1] = 7;
      InstSize[OPCODE_MAP2] = 11;
      InstSize[OPCODE_MAPGRID1] = 4;
      InstSize[OPCODE_MAPGRID2] = 7;
      InstSize[OPCODE_MATERIAL] = 7;
      InstSize[OPCODE_MATRIX_MODE] = 2;
      InstSize[OPCODE_MULT_MATRIX] = 17;
      InstSize[OPCODE_NORMAL] = 4;
      InstSize[OPCODE_PASSTHROUGH] = 2;
      InstSize[OPCODE_PIXEL_MAP] = 4;
      InstSize[OPCODE_PIXEL_TRANSFER] = 3;
      InstSize[OPCODE_PIXEL_ZOOM] = 3;
      InstSize[OPCODE_POINTSIZE] = 2;
      InstSize[OPCODE_POLYGON_MODE] = 3;
      InstSize[OPCODE_POLYGON_STIPPLE] = 2;
      InstSize[OPCODE_POLYGON_OFFSET] = 3;
      InstSize[OPCODE_POP_ATTRIB] = 1;
      InstSize[OPCODE_POP_MATRIX] = 1;
      InstSize[OPCODE_POP_NAME] = 1;
      InstSize[OPCODE_PRIORITIZE_TEXTURE] = 3;
      InstSize[OPCODE_PUSH_ATTRIB] = 2;
      InstSize[OPCODE_PUSH_MATRIX] = 1;
      InstSize[OPCODE_PUSH_NAME] = 2;
      InstSize[OPCODE_RASTER_POS] = 5;
      InstSize[OPCODE_READ_BUFFER] = 2;
      InstSize[OPCODE_SCALE] = 4;
      InstSize[OPCODE_SCISSOR] = 5;
      InstSize[OPCODE_STENCIL_FUNC] = 4;
      InstSize[OPCODE_STENCIL_MASK] = 2;
      InstSize[OPCODE_STENCIL_OP] = 4;
      InstSize[OPCODE_SHADE_MODEL] = 2;
      InstSize[OPCODE_TEXCOORD] = 5;
      InstSize[OPCODE_TEXENV] = 7;
      InstSize[OPCODE_TEXGEN] = 7;
      InstSize[OPCODE_TEXPARAMETER] = 7;
      InstSize[OPCODE_TEX_IMAGE1D] = 9;
      InstSize[OPCODE_TEX_IMAGE2D] = 10;
      InstSize[OPCODE_TEX_IMAGE3D] = 11;
      InstSize[OPCODE_TEX_SUB_IMAGE1D] = 8;
      InstSize[OPCODE_TEX_SUB_IMAGE2D] = 10;
      InstSize[OPCODE_TEX_SUB_IMAGE3D] = 12;
      InstSize[OPCODE_TRANSLATE] = 4;
      InstSize[OPCODE_VERTEX] = 5;
      InstSize[OPCODE_VIEWPORT] = 5;
      InstSize[OPCODE_WINDOW_POS] = 5;
      InstSize[OPCODE_CONTINUE] = 2;
      InstSize[OPCODE_END_OF_LIST] = 1;
   }
   init_flag = 1;
}



/*
 * Return the name of the display list currently being compiled.  This
 * function is only called by glGet().
 */
GLint gl_list_index( void )
{
   return CurrentListNum;
}



/*
 * Display List compilation functions
 */


void gl_save_Accum( GLcontext *ctx, GLenum op, GLfloat value )
{
   Node *n = alloc_instruction( ctx, OPCODE_ACCUM, 2 );
   if (n) {
      n[1].e = op;
      n[2].f = value;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Accum)( ctx, op, value );
   }
}


void gl_save_AlphaFunc( GLcontext *ctx, GLenum func, GLclampf ref )
{
   Node *n = alloc_instruction( ctx, OPCODE_ALPHA_FUNC, 2 );
   if (n) {
      n[1].e = func;
      n[2].f = (GLfloat) ref;
   }
   if (ctx->ExecuteFlag) {
      EXEC(AlphaFunc)( ctx, func, ref );
   }
}


void gl_save_Begin( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_BEGIN, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Begin)( ctx, mode );
   }
}


void gl_save_BindTexture( GLcontext *ctx, GLenum target, GLuint texture )
{
   Node *n = alloc_instruction( ctx, OPCODE_BIND_TEXTURE, 2 );
   if (n) {
      n[1].e = target;
      n[2].ui = texture;
   }
   if (ctx->ExecuteFlag) {
      EXEC(BindTexture)( ctx, target, texture );
   }
}


void gl_save_Bitmap( GLcontext *ctx,
                     GLsizei width, GLsizei height,
		     GLfloat xorig, GLfloat yorig,
		     GLfloat xmove, GLfloat ymove,
		     const struct gl_image *bitmap )
{
   Node *n = alloc_instruction( ctx, OPCODE_BITMAP, 7 );
   if (n) {
      n[1].i = (GLint) width;
      n[2].i = (GLint) height;
      n[3].f = xorig;
      n[4].f = yorig;
      n[5].f = xmove;
      n[6].f = ymove;
      n[7].data = (void *) bitmap;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Bitmap)( ctx, width, height,
                    xorig, yorig, xmove, ymove, bitmap );
   }
}


void gl_save_BlendEquation( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_BLEND_EQUATION, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(BlendEquation)( ctx, mode );
   }
}


void gl_save_BlendFunc( GLcontext *ctx, GLenum sfactor, GLenum dfactor )
{
   Node *n = alloc_instruction( ctx, OPCODE_BLEND_FUNC, 2 );
   if (n) {
      n[1].e = sfactor;
      n[2].e = dfactor;
   }
   if (ctx->ExecuteFlag) {
      EXEC(BlendFunc)( ctx, sfactor, dfactor );
   }
}


void gl_save_BlendColor( GLcontext *ctx, GLfloat red, GLfloat green,
                         GLfloat blue, GLfloat alpha )
{
   Node *n = alloc_instruction( ctx, OPCODE_BLEND_COLOR, 4 );
   if (n) {
      n[1].f = red;
      n[2].f = green;
      n[3].f = blue;
      n[4].f = alpha;
   }
   if (ctx->ExecuteFlag) {
      EXEC(BlendColor)( ctx, red, green, blue, alpha );
   }
}


void gl_save_CallList( GLcontext *ctx, GLuint list )
{
   Node *n = alloc_instruction( ctx, OPCODE_CALL_LIST, 1 );
   if (n) {
      n[1].ui = list;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CallList)( ctx, list );
   }
}


void gl_save_CallLists( GLcontext *ctx,
                        GLsizei n, GLenum type, const GLvoid *lists )
{
   GLuint i;

   for (i=0;i<n;i++) {
      GLuint list = translate_id( i, type, lists );
      Node *n = alloc_instruction( ctx, OPCODE_CALL_LIST_OFFSET, 1 );
      if (n) {
         n[1].ui = list;
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(CallLists)( ctx, n, type, lists );
   }
}


void gl_save_Clear( GLcontext *ctx, GLbitfield mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLEAR, 1 );
   if (n) {
      n[1].bf = mask;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Clear)( ctx, mask );
   }
}


void gl_save_ClearAccum( GLcontext *ctx, GLfloat red, GLfloat green,
			 GLfloat blue, GLfloat alpha )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLEAR_ACCUM, 4 );
   if (n) {
      n[1].f = red;
      n[2].f = green;
      n[3].f = blue;
      n[4].f = alpha;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ClearAccum)( ctx, red, green, blue, alpha );
   }
}


void gl_save_ClearColor( GLcontext *ctx, GLclampf red, GLclampf green,
			 GLclampf blue, GLclampf alpha )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLEAR_COLOR, 4 );
   if (n) {
      n[1].f = red;
      n[2].f = green;
      n[3].f = blue;
      n[4].f = alpha;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ClearColor)( ctx, red, green, blue, alpha );
   }
}


void gl_save_ClearDepth( GLcontext *ctx, GLclampd depth )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLEAR_DEPTH, 1 );
   if (n) {
      n[1].f = (GLfloat) depth;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ClearDepth)( ctx, depth );
   }
}


void gl_save_ClearIndex( GLcontext *ctx, GLfloat c )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLEAR_INDEX, 1 );
   if (n) {
      n[1].f = c;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ClearIndex)( ctx, c );
   }
}


void gl_save_ClearStencil( GLcontext *ctx, GLint s )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLEAR_STENCIL, 1 );
   if (n) {
      n[1].i = s;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ClearStencil)( ctx, s );
   }
}


void gl_save_ClipPlane( GLcontext *ctx, GLenum plane, const GLfloat *equ )
{
   Node *n = alloc_instruction( ctx, OPCODE_CLIP_PLANE, 5 );
   if (n) {
      n[1].e = plane;
      n[2].f = equ[0];
      n[3].f = equ[1];
      n[4].f = equ[2];
      n[5].f = equ[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(ClipPlane)( ctx, plane, equ );
   }
}


void gl_save_Color4f( GLcontext *ctx, GLfloat r, GLfloat g,
                                      GLfloat b, GLfloat a )
{
   Node *n = alloc_instruction( ctx, OPCODE_COLOR_4F, 4 );
   if (n) {
      n[1].f = r;
      n[2].f = g;
      n[3].f = b;
      n[4].f = a;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Color4f)( ctx, r, g, b, a );
   }
}


void gl_save_Color4ub( GLcontext *ctx, GLubyte r, GLubyte g,
                                       GLubyte b, GLubyte a )
{
   Node *n = alloc_instruction( ctx, OPCODE_COLOR_4UB, 4 );
   if (n) {
      n[1].ub = r;
      n[2].ub = g;
      n[3].ub = b;
      n[4].ub = a;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Color4ub)( ctx, r, g, b, a );
   }
}


void gl_save_ColorMask( GLcontext *ctx, GLboolean red, GLboolean green,
                        GLboolean blue, GLboolean alpha )
{
   Node *n = alloc_instruction( ctx, OPCODE_COLOR_MASK, 4 );
   if (n) {
      n[1].b = red;
      n[2].b = green;
      n[3].b = blue;
      n[4].b = alpha;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ColorMask)( ctx, red, green, blue, alpha );
   }
}


void gl_save_ColorMaterial( GLcontext *ctx, GLenum face, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_COLOR_MATERIAL, 2 );
   if (n) {
      n[1].e = face;
      n[2].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ColorMaterial)( ctx, face, mode );
   }
}


void gl_save_CopyPixels( GLcontext *ctx, GLint x, GLint y,
			 GLsizei width, GLsizei height, GLenum type )
{
   Node *n = alloc_instruction( ctx, OPCODE_COPY_PIXELS, 5 );
   if (n) {
      n[1].i = x;
      n[2].i = y;
      n[3].i = (GLint) width;
      n[4].i = (GLint) height;
      n[5].e = type;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CopyPixels)( ctx, x, y, width, height, type );
   }
}



void gl_save_CopyTexImage1D( GLcontext *ctx,
                             GLenum target, GLint level,
                             GLenum internalformat,
                             GLint x, GLint y, GLsizei width,
                             GLint border )
{
   Node *n = alloc_instruction( ctx, OPCODE_COPY_TEX_IMAGE1D, 7 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].e = internalformat;
      n[4].i = x;
      n[5].i = y;
      n[6].i = width;
      n[7].i = border;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CopyTexImage1D)( ctx, target, level, internalformat,
                            x, y, width, border );
   }
}


void gl_save_CopyTexImage2D( GLcontext *ctx,
                             GLenum target, GLint level,
                             GLenum internalformat,
                             GLint x, GLint y, GLsizei width,
                             GLsizei height, GLint border )
{
   Node *n = alloc_instruction( ctx, OPCODE_COPY_TEX_IMAGE2D, 8 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].e = internalformat;
      n[4].i = x;
      n[5].i = y;
      n[6].i = width;
      n[7].i = height;
      n[8].i = border;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CopyTexImage2D)( ctx, target, level, internalformat,
                            x, y, width, height, border );
   }
}



void gl_save_CopyTexSubImage1D( GLcontext *ctx,
                                GLenum target, GLint level,
                                GLint xoffset, GLint x, GLint y,
                                GLsizei width )
{
   Node *n = alloc_instruction( ctx, OPCODE_COPY_TEX_SUB_IMAGE1D, 6 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = xoffset;
      n[4].i = x;
      n[5].i = y;
      n[6].i = width;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CopyTexSubImage1D)( ctx, target, level, xoffset, x, y, width );
   }
}


void gl_save_CopyTexSubImage2D( GLcontext *ctx,
                                GLenum target, GLint level,
                                GLint xoffset, GLint yoffset,
                                GLint x, GLint y,
                                GLsizei width, GLint height )
{
   Node *n = alloc_instruction( ctx, OPCODE_COPY_TEX_SUB_IMAGE2D, 8 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = xoffset;
      n[4].i = yoffset;
      n[5].i = x;
      n[6].i = y;
      n[7].i = width;
      n[8].i = height;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CopyTexSubImage2D)( ctx, target, level, xoffset, yoffset,
                               x, y, width, height );
   }
}


void gl_save_CopyTexSubImage3DEXT( GLcontext *ctx,
                                   GLenum target, GLint level,
                                   GLint xoffset, GLint yoffset, GLint zoffset,
                                   GLint x, GLint y,
                                   GLsizei width, GLint height )
{
   Node *n = alloc_instruction( ctx, OPCODE_COPY_TEX_SUB_IMAGE3D, 9 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = xoffset;
      n[4].i = yoffset;
      n[5].i = zoffset;
      n[6].i = x;
      n[7].i = y;
      n[8].i = width;
      n[9].i = height;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CopyTexSubImage3DEXT)( ctx, target, level, xoffset, yoffset, zoffset,
                               x, y, width, height );
   }
}


void gl_save_CullFace( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_CULL_FACE, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(CullFace)( ctx, mode );
   }
}


void gl_save_DepthFunc( GLcontext *ctx, GLenum func )
{
   Node *n = alloc_instruction( ctx, OPCODE_DEPTH_FUNC, 1 );
   if (n) {
      n[1].e = func;
   }
   if (ctx->ExecuteFlag) {
      EXEC(DepthFunc)( ctx, func );
   }
}


void gl_save_DepthMask( GLcontext *ctx, GLboolean mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_DEPTH_MASK, 1 );
   if (n) {
      n[1].b = mask;
   }
   if (ctx->ExecuteFlag) {
      EXEC(DepthMask)( ctx, mask );
   }
}


void gl_save_DepthRange( GLcontext *ctx, GLclampd nearval, GLclampd farval )
{
   Node *n = alloc_instruction( ctx, OPCODE_DEPTH_RANGE, 2 );
   if (n) {
      n[1].f = (GLfloat) nearval;
      n[2].f = (GLfloat) farval;
   }
   if (ctx->ExecuteFlag) {
      EXEC(DepthRange)( ctx, nearval, farval );
   }
}


void gl_save_Disable( GLcontext *ctx, GLenum cap )
{
   Node *n = alloc_instruction( ctx, OPCODE_DISABLE, 1 );
   if (n) {
      n[1].e = cap;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Disable)( ctx, cap );
   }
}


void gl_save_DrawBuffer( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_DRAW_BUFFER, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(DrawBuffer)( ctx, mode );
   }
}


void gl_save_DrawPixels( GLcontext *ctx, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, const GLvoid *pixels )
{
   Node *n = alloc_instruction( ctx, OPCODE_DRAW_PIXELS, 5 );
   if (n) {
      n[1].i = (GLint) width;
      n[2].i = (GLint) height;
      n[3].e = format;
      n[4].e = type;
      n[5].data = (GLvoid *) pixels;
   }
   if (ctx->ExecuteFlag) {
      EXEC(DrawPixels)( ctx, width, height, format, type, pixels );
   }
}


void gl_save_EdgeFlag( GLcontext *ctx, GLboolean flag )
{
   Node *n = alloc_instruction( ctx, OPCODE_EDGE_FLAG, 1 );
   if (n) {
      n[1].b = flag;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EdgeFlag)( ctx, flag );
   }
}


void gl_save_Enable( GLcontext *ctx, GLenum cap )
{
   Node *n = alloc_instruction( ctx, OPCODE_ENABLE, 1 );
   if (n) {
      n[1].e = cap;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Enable)( ctx, cap );
   }
}


void gl_save_End( GLcontext *ctx )
{
   (void) alloc_instruction( ctx, OPCODE_END, 0 );
   if (ctx->ExecuteFlag) {
      EXEC(End)( ctx );
   }
}


void gl_save_EvalCoord1f( GLcontext *ctx, GLfloat u )
{
   Node *n = alloc_instruction( ctx, OPCODE_EVALCOORD1, 1 );
   if (n) {
      n[1].f = u;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EvalCoord1f)( ctx, u );
   }
}


void gl_save_EvalCoord2f( GLcontext *ctx, GLfloat u, GLfloat v )
{
   Node *n = alloc_instruction( ctx, OPCODE_EVALCOORD2, 2 );
   if (n) {
      n[1].f = u;
      n[2].f = v;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EvalCoord2f)( ctx, u, v );
   }
}


void gl_save_EvalMesh1( GLcontext *ctx,
                        GLenum mode, GLint i1, GLint i2 )
{
   Node *n = alloc_instruction( ctx, OPCODE_EVALMESH1, 3 );
   if (n) {
      n[1].e = mode;
      n[2].i = i1;
      n[3].i = i2;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EvalMesh1)( ctx, mode, i1, i2 );
   }
}


void gl_save_EvalMesh2( GLcontext *ctx, 
                        GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
   Node *n = alloc_instruction( ctx, OPCODE_EVALMESH2, 5 );
   if (n) {
      n[1].e = mode;
      n[2].i = i1;
      n[3].i = i2;
      n[4].i = j1;
      n[5].i = j2;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EvalMesh2)( ctx, mode, i1, i2, j1, j2 );
   }
}


void gl_save_EvalPoint1( GLcontext *ctx, GLint i )
{
   Node *n = alloc_instruction( ctx, OPCODE_EVALPOINT1, 1 );
   if (n) {
      n[1].i = i;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EvalPoint1)( ctx, i );
   }
}


void gl_save_EvalPoint2( GLcontext *ctx, GLint i, GLint j )
{
   Node *n = alloc_instruction( ctx, OPCODE_EVALPOINT2, 2 );
   if (n) {
      n[1].i = i;
      n[2].i = j;
   }
   if (ctx->ExecuteFlag) {
      EXEC(EvalPoint2)( ctx, i, j );
   }
}


void gl_save_Fogfv( GLcontext *ctx, GLenum pname, const GLfloat *params )
{
   Node *n = alloc_instruction( ctx, OPCODE_FOG, 5 );
   if (n) {
      n[1].e = pname;
      n[2].f = params[0];
      n[3].f = params[1];
      n[4].f = params[2];
      n[5].f = params[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(Fogfv)( ctx, pname, params );
   }
}


void gl_save_FrontFace( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_FRONT_FACE, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(FrontFace)( ctx, mode );
   }
}


void gl_save_Frustum( GLcontext *ctx, GLdouble left, GLdouble right,
                      GLdouble bottom, GLdouble top,
                      GLdouble nearval, GLdouble farval )
{
   Node *n = alloc_instruction( ctx, OPCODE_FRUSTUM, 6 );
   if (n) {
      n[1].f = left;
      n[2].f = right;
      n[3].f = bottom;
      n[4].f = top;
      n[5].f = nearval;
      n[6].f = farval;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Frustum)( ctx, left, right, bottom, top, nearval, farval );
   }
}


void gl_save_Hint( GLcontext *ctx, GLenum target, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_HINT, 2 );
   if (n) {
      n[1].e = target;
      n[2].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Hint)( ctx, target, mode );
   }
}


void gl_save_Indexi( GLcontext *ctx, GLint index )
{
   Node *n = alloc_instruction( ctx, OPCODE_INDEX, 1 );
   if (n) {
      n[1].i = index;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Indexi)( ctx, index );
   }
}


void gl_save_Indexf( GLcontext *ctx, GLfloat index )
{
   Node *n = alloc_instruction( ctx, OPCODE_INDEX, 1 );
   if (n) {
      n[1].i = (GLint) index;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Indexf)( ctx,index );
   }
}


void gl_save_IndexMask( GLcontext *ctx, GLuint mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_INDEX_MASK, 1 );
   if (n) {
      n[1].ui = mask;
   }
   if (ctx->ExecuteFlag) {
      EXEC(IndexMask)( ctx, mask );
   }
}


void gl_save_InitNames( GLcontext *ctx )
{
   (void) alloc_instruction( ctx, OPCODE_INIT_NAMES, 0 );
   if (ctx->ExecuteFlag) {
      EXEC(InitNames)( ctx );
   }
}


void gl_save_Lightfv( GLcontext *ctx, GLenum light, GLenum pname,
                      const GLfloat *params, GLint numparams )
{
   Node *n = alloc_instruction( ctx, OPCODE_LIGHT, 6 );
   if (OPCODE_LIGHT) {
      GLint i;
      n[1].e = light;
      n[2].e = pname;
      for (i=0;i<numparams;i++) {
	 n[3+i].f = params[i];
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(Lightfv)( ctx, light, pname, params, numparams );
   }
}


void gl_save_LightModelfv( GLcontext *ctx,
                           GLenum pname, const GLfloat *params )
{
   Node *n = alloc_instruction( ctx, OPCODE_LIGHT_MODEL, 5 );
   if (n) {
      n[1].e = pname;
      n[2].f = params[0];
      n[3].f = params[1];
      n[4].f = params[2];
      n[5].f = params[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(LightModelfv)( ctx, pname, params );
   }
}


void gl_save_LineStipple( GLcontext *ctx, GLint factor, GLushort pattern )
{
   Node *n = alloc_instruction( ctx, OPCODE_LINE_STIPPLE, 2 );
   if (n) {
      n[1].i = factor;
      n[2].us = pattern;
   }
   if (ctx->ExecuteFlag) {
      EXEC(LineStipple)( ctx, factor, pattern );
   }
}


void gl_save_LineWidth( GLcontext *ctx, GLfloat width )
{
   Node *n = alloc_instruction( ctx, OPCODE_LINE_WIDTH, 1 );
   if (n) {
      n[1].f = width;
   }
   if (ctx->ExecuteFlag) {
      EXEC(LineWidth)( ctx, width );
   }
}


void gl_save_ListBase( GLcontext *ctx, GLuint base )
{
   Node *n = alloc_instruction( ctx, OPCODE_LIST_BASE, 1 );
   if (n) {
      n[1].ui = base;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ListBase)( ctx, base );
   }
}


void gl_save_LoadMatrixf( GLcontext *ctx, const GLfloat *m )
{
   Node *n = alloc_instruction( ctx, OPCODE_LOAD_MATRIX, 16 );
   if (n) {
      GLuint i;
      for (i=0;i<16;i++) {
	 n[1+i].f = m[i];
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(LoadMatrixf)( ctx, m );
   }
}


void gl_save_LoadName( GLcontext *ctx, GLuint name )
{
   Node *n = alloc_instruction( ctx, OPCODE_LOAD_NAME, 1 );
   if (n) {
      n[1].ui = name;
   }
   if (ctx->ExecuteFlag) {
      EXEC(LoadName)( ctx, name );
   }
}


void gl_save_LogicOp( GLcontext *ctx, GLenum opcode )
{
   Node *n = alloc_instruction( ctx, OPCODE_LOGIC_OP, 1 );
   if (n) {
      n[1].e = opcode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(LogicOp)( ctx, opcode );
   }
}


void gl_save_Map1f( GLcontext *ctx,
                   GLenum target, GLfloat u1, GLfloat u2, GLint stride,
		   GLint order, const GLfloat *points, GLboolean retain )
{
   Node *n = alloc_instruction( ctx, OPCODE_MAP1, 6 );
   if (n) {
      n[1].e = target;
      n[2].f = u1;
      n[3].f = u2;
      n[4].i = stride;
      n[5].i = order;
      n[6].data = (void *) points;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Map1f)( ctx, target, u1, u2, stride, order, points, GL_TRUE );
   }
}


void gl_save_Map2f( GLcontext *ctx, GLenum target,
                    GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
                    GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
                    const GLfloat *points, GLboolean retain )
{
   Node *n = alloc_instruction( ctx, OPCODE_MAP2, 10 );
   if (n) {
      n[1].e = target;
      n[2].f = u1;
      n[3].f = u2;
      n[4].f = v1;
      n[5].f = v2;
      n[6].i = ustride;
      n[7].i = vstride;
      n[8].i = uorder;
      n[9].i = vorder;
      n[10].data = (void *) points;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Map2f)( ctx, target,
                        u1, u2, ustride, uorder,
                        v1, v2, vstride, vorder, points, GL_TRUE );
   }
}


void gl_save_MapGrid1f( GLcontext *ctx, GLint un, GLfloat u1, GLfloat u2 )
{
   Node *n = alloc_instruction( ctx, OPCODE_MAPGRID1, 3 );
   if (n) {
      n[1].i = un;
      n[2].f = u1;
      n[3].f = u2;
   }
   if (ctx->ExecuteFlag) {
      EXEC(MapGrid1f)( ctx, un, u1, u2 );
   }
}


void gl_save_MapGrid2f( GLcontext *ctx, 
                        GLint un, GLfloat u1, GLfloat u2,
		        GLint vn, GLfloat v1, GLfloat v2 )
{
   Node *n = alloc_instruction( ctx, OPCODE_MAPGRID2, 6 );
   if (n) {
      n[1].i = un;
      n[2].f = u1;
      n[3].f = u2;
      n[4].i = vn;
      n[5].f = v1;
      n[6].f = v2;
   }
   if (ctx->ExecuteFlag) {
      EXEC(MapGrid2f)( ctx, un, u1, u2, vn, v1, v2 );
   }
}


void gl_save_Materialfv( GLcontext *ctx,
                         GLenum face, GLenum pname, const GLfloat *params )
{
   Node *n = alloc_instruction( ctx, OPCODE_MATERIAL, 6 );
   if (n) {
      n[1].e = face;
      n[2].e = pname;
      n[3].f = params[0];
      n[4].f = params[1];
      n[5].f = params[2];
      n[6].f = params[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(Materialfv)( ctx, face, pname, params );
   }
}


void gl_save_MatrixMode( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_MATRIX_MODE, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(MatrixMode)( ctx, mode );
   }
}


void gl_save_MultMatrixf( GLcontext *ctx, const GLfloat *m )
{
   Node *n = alloc_instruction( ctx, OPCODE_MULT_MATRIX, 16 );
   if (n) {
      GLuint i;
      for (i=0;i<16;i++) {
	 n[1+i].f = m[i];
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(MultMatrixf)( ctx, m );
   }
}


void gl_save_NewList( GLcontext *ctx, GLuint list, GLenum mode )
{
   /* It's an error to call this function while building a display list */
   gl_error( ctx, GL_INVALID_OPERATION, "glNewList" );
}


void gl_save_Normal3fv( GLcontext *ctx, const GLfloat norm[3] )
{
   Node *n = alloc_instruction( ctx, OPCODE_NORMAL, 3 );
   if (n) {
      n[1].f = norm[0];
      n[2].f = norm[1];
      n[3].f = norm[2];
   }
   if (ctx->ExecuteFlag) {
      EXEC(Normal3fv)( ctx, norm );
   }
}


void gl_save_Normal3f( GLcontext *ctx, GLfloat nx, GLfloat ny, GLfloat nz )
{
   Node *n = alloc_instruction( ctx, OPCODE_NORMAL, 3 );
   if (n) {
      n[1].f = nx;
      n[2].f = ny;
      n[3].f = nz;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Normal3f)( ctx, nx, ny, nz );
   }
}


void gl_save_PixelMapfv( GLcontext *ctx,
                         GLenum map, GLint mapsize, const GLfloat *values )
{
   Node *n = alloc_instruction( ctx, OPCODE_PIXEL_MAP, 3 );
   if (n) {
      n[1].e = map;
      n[2].i = mapsize;
      n[3].data  = (void *) malloc( mapsize * sizeof(GLfloat) );
      MEMCPY( n[3].data, (void *) values, mapsize * sizeof(GLfloat) );
   }
   if (ctx->ExecuteFlag) {
      EXEC(PixelMapfv)( ctx, map, mapsize, values );
   }
}


void gl_save_PixelTransferf( GLcontext *ctx, GLenum pname, GLfloat param )
{
   Node *n = alloc_instruction( ctx, OPCODE_PIXEL_TRANSFER, 2 );
   if (n) {
      n[1].e = pname;
      n[2].f = param;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PixelTransferf)( ctx, pname, param );
   }
}


void gl_save_PixelZoom( GLcontext *ctx, GLfloat xfactor, GLfloat yfactor )
{
   Node *n = alloc_instruction( ctx, OPCODE_PIXEL_ZOOM, 2 );
   if (n) {
      n[1].f = xfactor;
      n[2].f = yfactor;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PixelZoom)( ctx, xfactor, yfactor );
   }
}


void gl_save_PointSize( GLcontext *ctx, GLfloat size )
{
   Node *n = alloc_instruction( ctx, OPCODE_POINTSIZE, 1 );
   if (n) {
      n[1].f = size;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PointSize)( ctx, size );
   }
}


void gl_save_PolygonMode( GLcontext *ctx, GLenum face, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_POLYGON_MODE, 2 );
   if (n) {
      n[1].e = face;
      n[2].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PolygonMode)( ctx, face, mode );
   }
}


void gl_save_PolygonStipple( GLcontext *ctx, const GLubyte *mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_POLYGON_STIPPLE, 1 );
   if (n) {
      void *data;
      n[1].data = malloc( 32 * 4 );
      data = n[1].data;   /* This needed for Acorn compiler */
      MEMCPY( data, mask, 32 * 4 );
   }
   if (ctx->ExecuteFlag) {
      EXEC(PolygonStipple)( ctx, mask );
   }
}


void gl_save_PolygonOffset( GLcontext *ctx, GLfloat factor, GLfloat units )
{
   Node *n = alloc_instruction( ctx, OPCODE_POLYGON_OFFSET, 2 );
   if (n) {
      n[1].f = factor;
      n[2].f = units;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PolygonOffset)( ctx, factor, units );
   }
}


void gl_save_PopAttrib( GLcontext *ctx )
{
   (void) alloc_instruction( ctx, OPCODE_POP_ATTRIB, 0 );
   if (ctx->ExecuteFlag) {
      EXEC(PopAttrib)( ctx );
   }
}


void gl_save_PopMatrix( GLcontext *ctx )
{
   (void) alloc_instruction( ctx, OPCODE_POP_MATRIX, 0 );
   if (ctx->ExecuteFlag) {
      EXEC(PopMatrix)( ctx );
   }
}


void gl_save_PopName( GLcontext *ctx )
{
   (void) alloc_instruction( ctx, OPCODE_POP_NAME, 0 );
   if (ctx->ExecuteFlag) {
      EXEC(PopName)( ctx );
   }
}


void gl_save_PrioritizeTextures( GLcontext *ctx,
                                 GLsizei num, const GLuint *textures,
                                 const GLclampf *priorities )
{
   GLint i;

   for (i=0;i<num;i++) {
      Node *n = alloc_instruction( ctx,  OPCODE_PRIORITIZE_TEXTURE, 2 );
      if (n) {
         n[1].ui = textures[i];
         n[2].f = priorities[i];
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(PrioritizeTextures)( ctx, num, textures, priorities );
   }
}


void gl_save_PushAttrib( GLcontext *ctx, GLbitfield mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_PUSH_ATTRIB, 1 );
   if (n) {
      n[1].bf = mask;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PushAttrib)( ctx, mask );
   }
}


void gl_save_PushMatrix( GLcontext *ctx )
{
   (void) alloc_instruction( ctx, OPCODE_PUSH_MATRIX, 0 );
   if (ctx->ExecuteFlag) {
      EXEC(PushMatrix)( ctx );
   }
}


void gl_save_PushName( GLcontext *ctx, GLuint name )
{
   Node *n = alloc_instruction( ctx, OPCODE_PUSH_NAME, 1 );
   if (n) {
      n[1].ui = name;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PushName)( ctx, name );
   }
}


void gl_save_RasterPos4f( GLcontext *ctx,
                          GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   Node *n = alloc_instruction( ctx, OPCODE_RASTER_POS, 4 );
   if (n) {
      n[1].f = x;
      n[2].f = y;
      n[3].f = z;
      n[4].f = w;
   }
   if (ctx->ExecuteFlag) {
      EXEC(RasterPos4f)( ctx, x, y, z, w );
   }
}


void gl_save_PassThrough( GLcontext *ctx, GLfloat token )
{
   Node *n = alloc_instruction( ctx, OPCODE_PASSTHROUGH, 1 );
   if (n) {
      n[1].f = token;
   }
   if (ctx->ExecuteFlag) {
      EXEC(PassThrough)( ctx, token );
   }
}


void gl_save_ReadBuffer( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_READ_BUFFER, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ReadBuffer)( ctx, mode );
   }
}


void gl_save_Rotatef( GLcontext *ctx, GLfloat angle,
                      GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat m[16];
   gl_rotation_matrix( angle, x, y, z, m );
   gl_save_MultMatrixf( ctx, m );  /* save and maybe execute */
}


void gl_save_Scalef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z )
{
   Node *n = alloc_instruction( ctx, OPCODE_SCALE, 3 );
   if (n) {
      n[1].f = x;
      n[2].f = y;
      n[3].f = z;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Scalef)( ctx, x, y, z );
   }
}


void gl_save_Scissor( GLcontext *ctx,
                      GLint x, GLint y, GLsizei width, GLsizei height )
{
   Node *n = alloc_instruction( ctx, OPCODE_SCISSOR, 4 );
   if (n) {
      n[1].i = x;
      n[2].i = y;
      n[3].i = width;
      n[4].i = height;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Scissor)( ctx, x, y, width, height );
   }
}


void gl_save_ShadeModel( GLcontext *ctx, GLenum mode )
{
   Node *n = alloc_instruction( ctx, OPCODE_SHADE_MODEL, 1 );
   if (n) {
      n[1].e = mode;
   }
   if (ctx->ExecuteFlag) {
      EXEC(ShadeModel)( ctx, mode );
   }
}


void gl_save_StencilFunc( GLcontext *ctx, GLenum func, GLint ref, GLuint mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_STENCIL_FUNC, 3 );
   if (n) {
      n[1].e = func;
      n[2].i = ref;
      n[3].ui = mask;
   }
   if (ctx->ExecuteFlag) {
      EXEC(StencilFunc)( ctx, func, ref, mask );
   }
}


void gl_save_StencilMask( GLcontext *ctx, GLuint mask )
{
   Node *n = alloc_instruction( ctx, OPCODE_STENCIL_MASK, 1 );
   if (n) {
      n[1].ui = mask;
   }
   if (ctx->ExecuteFlag) {
      EXEC(StencilMask)( ctx, mask );
   }
}


void gl_save_StencilOp( GLcontext *ctx,
                        GLenum fail, GLenum zfail, GLenum zpass )
{
   Node *n = alloc_instruction( ctx, OPCODE_STENCIL_OP, 3 );
   if (n) {
      n[1].e = fail;
      n[2].e = zfail;
      n[3].e = zpass;
   }
   if (ctx->ExecuteFlag) {
      EXEC(StencilOp)( ctx, fail, zfail, zpass );
   }
}


void gl_save_TexCoord4f( GLcontext *ctx, GLfloat s, GLfloat t,
                                         GLfloat r, GLfloat q )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEXCOORD, 4 );
   if (n) {
      n[1].f = s;
      n[2].f = t;
      n[3].f = r;
      n[4].f = q;
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexCoord4f)( ctx, s, t, r, q );
   }
}


void gl_save_TexEnvfv( GLcontext *ctx,
                       GLenum target, GLenum pname, const GLfloat *params )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEXENV, 6 );
   if (n) {
      n[1].e = target;
      n[2].e = pname;
      n[3].f = params[0];
      n[4].f = params[1];
      n[5].f = params[2];
      n[6].f = params[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexEnvfv)( ctx, target, pname, params );
   }
}


void gl_save_TexGenfv( GLcontext *ctx,
                       GLenum coord, GLenum pname, const GLfloat *params )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEXGEN, 6 );
   if (n) {
      n[1].e = coord;
      n[2].e = pname;
      n[3].f = params[0];
      n[4].f = params[1];
      n[5].f = params[2];
      n[6].f = params[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexGenfv)( ctx, coord, pname, params );
   }
}


void gl_save_TexParameterfv( GLcontext *ctx, GLenum target,
                             GLenum pname, const GLfloat *params )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEXPARAMETER, 6 );
   if (n) {
      n[1].e = target;
      n[2].e = pname;
      n[3].f = params[0];
      n[4].f = params[1];
      n[5].f = params[2];
      n[6].f = params[3];
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexParameterfv)( ctx, target, pname, params );
   }
}


void gl_save_TexImage1D( GLcontext *ctx, GLenum target,
                         GLint level, GLint components,
			 GLsizei width, GLint border,
                         GLenum format, GLenum type,
			 struct gl_image *teximage )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEX_IMAGE1D, 8 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = components;
      n[4].i = (GLint) width;
      n[5].i = border;
      n[6].e = format;
      n[7].e = type;
      n[8].data = teximage;
      if (teximage) {
         /* this prevents gl_TexImage2D() from freeing the image */
         teximage->RefCount = 1;
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexImage1D)( ctx, target, level, components, width,
                        border, format, type, teximage );
   }
}


void gl_save_TexImage2D( GLcontext *ctx, GLenum target,
                         GLint level, GLint components,
			 GLsizei width, GLsizei height, GLint border,
                         GLenum format, GLenum type,
			 struct gl_image *teximage )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEX_IMAGE2D, 9 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = components;
      n[4].i = (GLint) width;
      n[5].i = (GLint) height;
      n[6].i = border;
      n[7].e = format;
      n[8].e = type;
      n[9].data = teximage;
      if (teximage) {
         /* this prevents gl_TexImage2D() from freeing the image */
         teximage->RefCount = 1;
      }
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexImage2D)( ctx, target, level, components, width,
                        height, border, format, type, teximage );
   }
}


void gl_save_TexImage3DEXT( GLcontext *ctx, GLenum target,
                            GLint level, GLint components,
                            GLsizei width, GLsizei height, GLsizei depth,
                            GLint border,
                            GLenum format, GLenum type,
                            struct gl_image *teximage )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEX_IMAGE3D, 10 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = components;
      n[4].i = (GLint) width;
      n[5].i = (GLint) height;
      n[6].i = (GLint) depth; 
      n[7].i = border;
      n[8].e = format;
      n[9].e = type;
      n[10].data = teximage;
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexImage3DEXT)( ctx, target, level, components, width,
                           height, depth, border, format, type, teximage );
   }
}


void gl_save_TexSubImage1D( GLcontext *ctx,
                            GLenum target, GLint level, GLint xoffset,
                            GLsizei width, GLenum format, GLenum type,
                            struct gl_image *image )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEX_SUB_IMAGE1D, 7 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = xoffset;
      n[4].i = (GLint) width;
      n[5].e = format;
      n[6].e = type;
      n[7].data = image;
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexSubImage1D)( ctx, target, level, xoffset, width,
                           format, type, image );
   }
}


void gl_save_TexSubImage2D( GLcontext *ctx,
                            GLenum target, GLint level,
                            GLint xoffset, GLint yoffset,
                            GLsizei width, GLsizei height,
                            GLenum format, GLenum type,
                            struct gl_image *image )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEX_SUB_IMAGE2D, 9 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = xoffset;
      n[4].i = yoffset;
      n[5].i = (GLint) width;
      n[6].i = (GLint) height;
      n[7].e = format;
      n[8].e = type;
      n[9].data = image;
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexSubImage2D)( ctx, target, level, xoffset, yoffset,
                           width, height, format, type, image );
   }
}


void gl_save_TexSubImage3DEXT( GLcontext *ctx,
                               GLenum target, GLint level,
                               GLint xoffset, GLint yoffset,GLint zoffset,
                               GLsizei width, GLsizei height, GLsizei depth,
                               GLenum format, GLenum type,
                               struct gl_image *image )
{
   Node *n = alloc_instruction( ctx, OPCODE_TEX_SUB_IMAGE3D, 11 );
   if (n) {
      n[1].e = target;
      n[2].i = level;
      n[3].i = xoffset;
      n[4].i = yoffset;
      n[5].i = zoffset;
      n[6].i = (GLint) width;
      n[7].i = (GLint) height;
      n[8].i = (GLint) depth;
      n[9].e = format;
      n[10].e = type;
      n[11].data = image;
   }
   if (ctx->ExecuteFlag) {
      EXEC(TexSubImage3DEXT)( ctx, target, level, xoffset, yoffset, zoffset,
                              width, height, depth, format, type, image );
   }
}


void gl_save_Translatef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z )
{
   Node *n = alloc_instruction( ctx,  OPCODE_TRANSLATE, 3 );
   if (n) {
      n[1].f = x;
      n[2].f = y;
      n[3].f = z;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Translatef)( ctx, x, y, z );
   }
}


void gl_save_Vertex4f( GLcontext *ctx,
                       GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   Node *n = alloc_instruction( ctx,  OPCODE_VERTEX, 4 );
   if (n) {
      n[1].f = x;
      n[2].f = y;
      n[3].f = z;
      n[4].f = w;
   }
   if (ctx->ExecuteFlag) {
      (*ctx->Exec.Vertex4f)( ctx, x, y, z, w );
   }
}


void gl_save_Viewport( GLcontext *ctx,
                       GLint x, GLint y, GLsizei width, GLsizei height )
{
   Node *n = alloc_instruction( ctx,  OPCODE_VIEWPORT, 4 );
   if (n) {
      n[1].i = x;
      n[2].i = y;
      n[3].i = (GLint) width;
      n[4].i = (GLint) height;
   }
   if (ctx->ExecuteFlag) {
      EXEC(Viewport)( ctx, x, y, width, height );
   }
}


void gl_save_WindowPos4fMESA( GLcontext *ctx,
                              GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   Node *n = alloc_instruction( ctx,  OPCODE_WINDOW_POS, 4 );
   if (n) {
      n[1].f = x;
      n[2].f = y;
      n[3].f = z;
      n[4].f = w;
   }
   if (ctx->ExecuteFlag) {
      EXEC(WindowPos4fMESA)( ctx, x, y, z, w );
   }
}



/**********************************************************************/
/*                     Display list execution                         */
/**********************************************************************/


/*
 * Execute a display list.  Note that the ListBase offset must have already
 * been added before calling this function.  I.e. the list argument is
 * the absolute list number, not relative to ListBase.
 * Input:  list - list number in [1..]
 */
static void execute_list( GLcontext *ctx, GLuint list )
{
   Node *n;
   GLboolean done;
   OpCode opcode;

   if (!gl_IsList(ctx,list))
      return;

   ctx->CallDepth++;

   n = ctx->Shared->List[list-1];
   done = GL_FALSE;
   while (!done) {
      opcode = n[0].opcode;

      switch (opcode) {
	 /* Frequently called functions: */
         case OPCODE_VERTEX:
            (*ctx->Exec.Vertex4f)( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
            break;
         case OPCODE_NORMAL:
            ctx->Current.Normal[0] = n[1].f;
            ctx->Current.Normal[1] = n[2].f;
            ctx->Current.Normal[2] = n[3].f;
            break;
	 case OPCODE_COLOR_4UB:
            (*ctx->Exec.Color4ub)( ctx, n[1].ub, n[2].ub, n[3].ub, n[4].ub );
	    break;
	 case OPCODE_COLOR_4F:
            (*ctx->Exec.Color4f)( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
            break;
         case OPCODE_INDEX:
            ctx->Current.Index = n[1].ui;
            ctx->VB->MonoColor = GL_FALSE;
            break;
         case OPCODE_BEGIN:
            gl_Begin( ctx, n[1].e );
            break;
         case OPCODE_END:
            gl_End( ctx );
            break;
	 case OPCODE_TEXCOORD:
	    ctx->Current.TexCoord[0] = n[1].f;
	    ctx->Current.TexCoord[1] = n[2].f;
	    ctx->Current.TexCoord[2] = n[3].f;
	    ctx->Current.TexCoord[3] = n[4].f;
	    break;

	 /* Everything Else: */
         case OPCODE_ACCUM:
	    gl_Accum( ctx, n[1].e, n[2].f );
	    break;
         case OPCODE_ALPHA_FUNC:
	    gl_AlphaFunc( ctx, n[1].e, n[2].f );
	    break;
         case OPCODE_BIND_TEXTURE:
            gl_BindTexture( ctx, n[1].e, n[2].ui );
            break;
	 case OPCODE_BITMAP:
	    gl_Bitmap( ctx, (GLsizei) n[1].i, (GLsizei) n[2].i,
		       n[3].f, n[4].f,
		       n[5].f, n[6].f,
		       (struct gl_image *) n[7].data );
	    break;
	 case OPCODE_BLEND_COLOR:
	    gl_BlendColor( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
	    break;
	 case OPCODE_BLEND_EQUATION:
	    gl_BlendEquation( ctx, n[1].e );
	    break;
	 case OPCODE_BLEND_FUNC:
	    gl_BlendFunc( ctx, n[1].e, n[2].e );
	    break;
         case OPCODE_CALL_LIST:
	    /* Generated by glCallList(), don't add ListBase */
            if (ctx->CallDepth<MAX_LIST_NESTING) {
               execute_list( ctx, n[1].ui );
            }
            break;
         case OPCODE_CALL_LIST_OFFSET:
	    /* Generated by glCallLists() so we must add ListBase */
            if (ctx->CallDepth<MAX_LIST_NESTING) {
               execute_list( ctx, ctx->List.ListBase + n[1].ui );
            }
            break;
	 case OPCODE_CLEAR:
	    gl_Clear( ctx, n[1].bf );
	    break;
	 case OPCODE_CLEAR_COLOR:
	    gl_ClearColor( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
	    break;
	 case OPCODE_CLEAR_ACCUM:
	    gl_ClearAccum( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
	    break;
	 case OPCODE_CLEAR_DEPTH:
	    gl_ClearDepth( ctx, (GLclampd) n[1].f );
	    break;
	 case OPCODE_CLEAR_INDEX:
	    gl_ClearIndex( ctx, n[1].ui );
	    break;
	 case OPCODE_CLEAR_STENCIL:
	    gl_ClearStencil( ctx, n[1].i );
	    break;
         case OPCODE_CLIP_PLANE:
            {
               GLfloat equ[4];
               equ[0] = n[2].f;
               equ[1] = n[3].f;
               equ[2] = n[4].f;
               equ[3] = n[5].f;
               gl_ClipPlane( ctx, n[1].e, equ );
            }
            break;
	 case OPCODE_COLOR_MASK:
	    gl_ColorMask( ctx, n[1].b, n[2].b, n[3].b, n[4].b );
	    break;
	 case OPCODE_COLOR_MATERIAL:
	    gl_ColorMaterial( ctx, n[1].e, n[2].e );
	    break;
	 case OPCODE_COPY_PIXELS:
	    gl_CopyPixels( ctx, n[1].i, n[2].i,
			   (GLsizei) n[3].i, (GLsizei) n[4].i, n[5].e );
	    break;
         case OPCODE_COPY_TEX_IMAGE1D:
	    gl_CopyTexImage1D( ctx, n[1].e, n[2].i, n[3].e, n[4].i,
                               n[5].i, n[6].i, n[7].i );
            break;
         case OPCODE_COPY_TEX_IMAGE2D:
	    gl_CopyTexImage2D( ctx, n[1].e, n[2].i, n[3].e, n[4].i,
                               n[5].i, n[6].i, n[7].i, n[8].i );
            break;
         case OPCODE_COPY_TEX_SUB_IMAGE1D:
	    gl_CopyTexSubImage1D( ctx, n[1].e, n[2].i, n[3].i, n[4].i,
                                  n[5].i, n[6].i );
            break;
         case OPCODE_COPY_TEX_SUB_IMAGE2D:
	    gl_CopyTexSubImage2D( ctx, n[1].e, n[2].i, n[3].i, n[4].i,
                                  n[5].i, n[6].i, n[7].i, n[8].i );
            break;
         case OPCODE_COPY_TEX_SUB_IMAGE3D:
            gl_CopyTexSubImage3DEXT( ctx, n[1].e, n[2].i, n[3].i, n[4].i,
                                     n[5].i, n[6].i, n[7].i, n[8].i , n[9].i);
            break;
	 case OPCODE_CULL_FACE:
	    gl_CullFace( ctx, n[1].e );
	    break;
	 case OPCODE_DEPTH_FUNC:
	    gl_DepthFunc( ctx, n[1].e );
	    break;
	 case OPCODE_DEPTH_MASK:
	    gl_DepthMask( ctx, n[1].b );
	    break;
	 case OPCODE_DEPTH_RANGE:
	    gl_DepthRange( ctx, (GLclampd) n[1].f, (GLclampd) n[2].f );
	    break;
	 case OPCODE_DISABLE:
	    gl_Disable( ctx, n[1].e );
	    break;
	 case OPCODE_DRAW_BUFFER:
	    gl_DrawBuffer( ctx, n[1].e );
	    break;
	 case OPCODE_DRAW_PIXELS:
	    gl_DrawPixels( ctx, (GLsizei) n[1].i, (GLsizei) n[2].i,
			   n[3].e, n[4].e, n[5].data );
	    break;
	 case OPCODE_EDGE_FLAG:
            ctx->Current.EdgeFlag = n[1].b;
            break;
	 case OPCODE_ENABLE:
	    gl_Enable( ctx, n[1].e );
	    break;
	 case OPCODE_EVALCOORD1:
	    gl_EvalCoord1f( ctx, n[1].f );
	    break;
	 case OPCODE_EVALCOORD2:
	    gl_EvalCoord2f( ctx, n[1].f, n[2].f );
	    break;
	 case OPCODE_EVALMESH1:
	    gl_EvalMesh1( ctx, n[1].e, n[2].i, n[3].i );
	    break;
	 case OPCODE_EVALMESH2:
	    gl_EvalMesh2( ctx, n[1].e, n[2].i, n[3].i, n[4].i, n[5].i );
	    break;
	 case OPCODE_EVALPOINT1:
	    gl_EvalPoint1( ctx, n[1].i );
	    break;
	 case OPCODE_EVALPOINT2:
	    gl_EvalPoint2( ctx, n[1].i, n[2].i );
	    break;
	 case OPCODE_FOG:
	    {
	       GLfloat p[4];
	       p[0] = n[2].f;
	       p[1] = n[3].f;
	       p[2] = n[4].f;
	       p[3] = n[5].f;
	       gl_Fogfv( ctx, n[1].e, p );
	    }
	    break;
	 case OPCODE_FRONT_FACE:
	    gl_FrontFace( ctx, n[1].e );
	    break;
	 case OPCODE_HINT:
	    gl_Hint( ctx, n[1].e, n[2].e );
	    break;
	 case OPCODE_INDEX_MASK:
	    gl_IndexMask( ctx, n[1].ui );
	    break;
	 case OPCODE_INIT_NAMES:
	    gl_InitNames( ctx );
	    break;
         case OPCODE_LIGHT:
	    {
	       GLfloat p[4];
	       p[0] = n[3].f;
	       p[1] = n[4].f;
	       p[2] = n[5].f;
	       p[3] = n[6].f;
	       gl_Lightfv( ctx, n[1].e, n[2].e, p, 4 );
	    }
	    break;
         case OPCODE_LIGHT_MODEL:
	    {
	       GLfloat p[4];
	       p[0] = n[2].f;
	       p[1] = n[3].f;
	       p[2] = n[4].f;
	       p[3] = n[5].f;
	       gl_LightModelfv( ctx, n[1].e, p );
	    }
	    break;
	 case OPCODE_LINE_STIPPLE:
	    gl_LineStipple( ctx, n[1].i, n[2].us );
	    break;
	 case OPCODE_LINE_WIDTH:
	    gl_LineWidth( ctx, n[1].f );
	    break;
	 case OPCODE_LIST_BASE:
	    gl_ListBase( ctx, n[1].ui );
	    break;
	 case OPCODE_LOAD_MATRIX:
	    if (sizeof(Node)==sizeof(GLfloat)) {
	       gl_LoadMatrixf( ctx, &n[1].f );
	    }
	    else {
	       GLfloat m[16];
	       GLuint i;
	       for (i=0;i<16;i++) {
		  m[i] = n[1+i].f;
	       }
	       gl_LoadMatrixf( ctx, m );
	    }
	    break;
	 case OPCODE_LOAD_NAME:
	    gl_LoadName( ctx, n[1].ui );
	    break;
	 case OPCODE_LOGIC_OP:
	    gl_LogicOp( ctx, n[1].e );
	    break;
	 case OPCODE_MAP1:
	    gl_Map1f( ctx, n[1].e, n[2].f, n[3].f,
                      n[4].i, n[5].i, (GLfloat *) n[6].data, GL_TRUE );
	    break;
	 case OPCODE_MAP2:
	    gl_Map2f( ctx, n[1].e,
                      n[2].f, n[3].f,  /* u1, u2 */
		      n[6].i, n[8].i,  /* ustride, uorder */
		      n[4].f, n[5].f,  /* v1, v2 */
		      n[7].i, n[9].i,  /* vstride, vorder */
		      (GLfloat *) n[10].data,
                      GL_TRUE);
	    break;
	 case OPCODE_MAPGRID1:
	    gl_MapGrid1f( ctx, n[1].i, n[2].f, n[3].f );
	    break;
	 case OPCODE_MAPGRID2:
	    gl_MapGrid2f( ctx, n[1].i, n[2].f, n[3].f, n[4].i, n[5].f, n[6].f);
	    break;
	 case OPCODE_MATERIAL:
	    {
	       GLfloat params[4];
	       params[0] = n[3].f;
	       params[1] = n[4].f;
	       params[2] = n[5].f;
	       params[3] = n[6].f;
	       gl_Materialfv( ctx, n[1].e, n[2].e, params );
	    }
	    break;
         case OPCODE_MATRIX_MODE:
            gl_MatrixMode( ctx, n[1].e );
            break;
	 case OPCODE_MULT_MATRIX:
	    if (sizeof(Node)==sizeof(GLfloat)) {
	       gl_MultMatrixf( ctx, &n[1].f );
	    }
	    else {
	       GLfloat m[16];
	       GLuint i;
	       for (i=0;i<16;i++) {
		  m[i] = n[1+i].f;
	       }
	       gl_MultMatrixf( ctx, m );
	    }
	    break;
	 case OPCODE_PASSTHROUGH:
	    gl_PassThrough( ctx, n[1].f );
	    break;
	 case OPCODE_PIXEL_MAP:
	    gl_PixelMapfv( ctx, n[1].e, n[2].i, (GLfloat *) n[3].data );
	    break;
	 case OPCODE_PIXEL_TRANSFER:
	    gl_PixelTransferf( ctx, n[1].e, n[2].f );
	    break;
	 case OPCODE_PIXEL_ZOOM:
	    gl_PixelZoom( ctx, n[1].f, n[2].f );
	    break;
	 case OPCODE_POINTSIZE:
	    gl_PointSize( ctx, n[1].f );
	    break;
	 case OPCODE_POLYGON_MODE:
	    gl_PolygonMode( ctx, n[1].e, n[2].e );
	    break;
	 case OPCODE_POLYGON_STIPPLE:
	    gl_PolygonStipple( ctx, (GLubyte *) n[1].data );
	    break;
	 case OPCODE_POLYGON_OFFSET:
	    gl_PolygonOffset( ctx, n[1].f, n[2].f );
	    break;
	 case OPCODE_POP_ATTRIB:
	    gl_PopAttrib( ctx );
	    break;
	 case OPCODE_POP_MATRIX:
	    gl_PopMatrix( ctx );
	    break;
	 case OPCODE_POP_NAME:
	    gl_PopName( ctx );
	    break;
	 case OPCODE_PRIORITIZE_TEXTURE:
            gl_PrioritizeTextures( ctx, 1, &n[1].ui, &n[2].f );
	    break;
	 case OPCODE_PUSH_ATTRIB:
	    gl_PushAttrib( ctx, n[1].bf );
	    break;
	 case OPCODE_PUSH_MATRIX:
	    gl_PushMatrix( ctx );
	    break;
	 case OPCODE_PUSH_NAME:
	    gl_PushName( ctx, n[1].ui );
	    break;
	 case OPCODE_RASTER_POS:
            gl_RasterPos4f( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
	    break;
	 case OPCODE_READ_BUFFER:
	    gl_ReadBuffer( ctx, n[1].e );
	    break;
         case OPCODE_SCALE:
            gl_Scalef( ctx, n[1].f, n[2].f, n[3].f );
            break;
	 case OPCODE_SCISSOR:
	    gl_Scissor( ctx, n[1].i, n[2].i, n[3].i, n[4].i );
	    break;
	 case OPCODE_SHADE_MODEL:
	    gl_ShadeModel( ctx, n[1].e );
	    break;
	 case OPCODE_STENCIL_FUNC:
	    gl_StencilFunc( ctx, n[1].e, n[2].i, n[3].ui );
	    break;
	 case OPCODE_STENCIL_MASK:
	    gl_StencilMask( ctx, n[1].ui );
	    break;
	 case OPCODE_STENCIL_OP:
	    gl_StencilOp( ctx, n[1].e, n[2].e, n[3].e );
	    break;
         case OPCODE_TEXENV:
            {
               GLfloat params[4];
               params[0] = n[3].f;
               params[1] = n[4].f;
               params[2] = n[5].f;
               params[3] = n[6].f;
               gl_TexEnvfv( ctx, n[1].e, n[2].e, params );
            }
            break;
         case OPCODE_TEXGEN:
            {
               GLfloat params[4];
               params[0] = n[3].f;
               params[1] = n[4].f;
               params[2] = n[5].f;
               params[3] = n[6].f;
               gl_TexGenfv( ctx, n[1].e, n[2].e, params );
            }
            break;
         case OPCODE_TEXPARAMETER:
            {
               GLfloat params[4];
               params[0] = n[3].f;
               params[1] = n[4].f;
               params[2] = n[5].f;
               params[3] = n[6].f;
               gl_TexParameterfv( ctx, n[1].e, n[2].e, params );
            }
            break;
	 case OPCODE_TEX_IMAGE1D:
	    gl_TexImage1D( ctx,
                           n[1].e, /* target */
                           n[2].i, /* level */
                           n[3].i, /* components */
                           n[4].i, /* width */
                           n[5].e, /* border */
                           n[6].e, /* format */
                           n[7].e, /* type */
                           n[8].data );
	    break;
	 case OPCODE_TEX_IMAGE2D:
	    gl_TexImage2D( ctx,
                           n[1].e, /* target */
                           n[2].i, /* level */
                           n[3].i, /* components */
                           n[4].i, /* width */
                           n[5].i, /* height */
                           n[6].e, /* border */
                           n[7].e, /* format */
                           n[8].e, /* type */
                           n[9].data );
	    break;
         case OPCODE_TEX_IMAGE3D:
            gl_TexImage3DEXT( ctx,
                              n[1].e, /* target */
                              n[2].i, /* level */
                              n[3].i, /* components */
                              n[4].i, /* width */
                              n[5].i, /* height */
                              n[6].i, /* depth  */
                              n[7].e, /* border */
                              n[8].e, /* format */
                              n[9].e, /* type */
                              n[10].data );
            break;
         case OPCODE_TEX_SUB_IMAGE1D:
            gl_TexSubImage1D( ctx, n[1].e, n[2].i, n[3].i, n[4].i, n[5].e,
                              n[6].e, (struct gl_image *) n[7].data );
            break;
         case OPCODE_TEX_SUB_IMAGE2D:
            gl_TexSubImage2D( ctx, n[1].e, n[2].i, n[3].i, n[4].i, n[5].e,
                              n[6].i, n[7].e, n[8].e,
                              (struct gl_image *) n[9].data );
            break;
         case OPCODE_TEX_SUB_IMAGE3D:
            gl_TexSubImage3DEXT( ctx, n[1].e, n[2].i, n[3].i, n[4].i, n[5].i,
                                 n[6].i, n[7].i, n[8].i, n[9].e, n[10].e,
                                 (struct gl_image *) n[11].data );
            break;
         case OPCODE_TRANSLATE:
            gl_Translatef( ctx, n[1].f, n[2].f, n[3].f );
            break;
	 case OPCODE_VIEWPORT:
	    gl_Viewport( ctx,
                         n[1].i, n[2].i, (GLsizei) n[3].i, (GLsizei) n[4].i );
	    break;
	 case OPCODE_WINDOW_POS:
            gl_WindowPos4fMESA( ctx, n[1].f, n[2].f, n[3].f, n[4].f );
	    break;
	 case OPCODE_CONTINUE:
	    n = (Node *) n[1].next;
	    break;
	 case OPCODE_END_OF_LIST:
	    done = GL_TRUE;
	    break;
	 default:
            {
               char msg[1000];
               sprintf(msg, "Error in execute_list: opcode=%d", (int) opcode);
               gl_problem( ctx, msg );
            }
            done = GL_TRUE;
      }

      /* increment n to point to next compiled command */
      if (opcode!=OPCODE_CONTINUE) {
	 n += InstSize[opcode];
      }

   }
   ctx->CallDepth--;
}



/**********************************************************************/
/*                           GL functions                             */
/**********************************************************************/



/*
 * Test if a display list number is valid.
 */
GLboolean gl_IsList( GLcontext *ctx, GLuint list )
{
   if (list>0 && list<=MAX_DISPLAYLISTS && ctx->Shared->List[list-1]) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}



/*
 * Delete a sequence of consecutive display lists.
 */
void gl_DeleteLists( GLcontext *ctx, GLuint list, GLsizei range )
{
   GLuint i;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glDeleteLists" );
      return;
   }
   if (range<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glDeleteLists" );
      return;
   }
   for (i=list;i<list+range;i++) {
      if (i<=MAX_DISPLAYLISTS && ctx->Shared->List[i-1]) {
         destroy_list( ctx, i-1 );
         ctx->Shared->List[i-1] = NULL;
      }
   }
}



/*
 * Return a display list number, n, such that lists n through n+range-1
 * are free.
 */
GLuint gl_GenLists( GLcontext *ctx, GLsizei range )
{
   GLuint i, freecount;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glGenLists" );
      return 0;
   }
   if (range<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glGenLists" );
      return 0;
   }
   if (range==0) {
      return 0;
   }

   i = 0;
   freecount = 0;
   for (i=0; i<MAX_DISPLAYLISTS; i++ ) {
      if (ctx->Shared->List[i]==NULL) {
         freecount++;
         if (freecount==range) {
	    /* We found 'range' consecutive free lists. */
            /* Now reserve the IDs with empty display lists. */
	    GLuint k;
	    GLuint n = i-range+2;
	    for (k=n;k<n+range;k++) {
	       ctx->Shared->List[k-1] = make_empty_list();
	    }
            return n;
         }
      }
      else {
         freecount = 0;
      }
   }
   return 0;
}



/*
 * Begin a new display list.
 */
void gl_NewList( GLcontext *ctx, GLuint list, GLenum mode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glNewList" );
      return;
   }
   if (list==0 || list>MAX_DISPLAYLISTS) {
      gl_error( ctx, GL_INVALID_VALUE, "glNewList" );
      return;
   }
   if (mode!=GL_COMPILE && mode!=GL_COMPILE_AND_EXECUTE) {
      gl_error( ctx, GL_INVALID_ENUM, "glNewList" );
      return;
   }
   if (CurrentListPtr) {
      /* already compiling a display list */
      gl_error( ctx, GL_INVALID_OPERATION, "glNewList" );
      return;
   }

   /* Allocate new display list */
   CurrentListNum = list;
   CurrentListPtr = CurrentBlock = (Node *) malloc( sizeof(Node) * BLOCK_SIZE );
   CurrentPos = 0;

   ctx->CompileFlag = GL_TRUE;
   if (mode==GL_COMPILE) {
      ctx->ExecuteFlag = GL_FALSE;
   }
   else {
      /* Compile and execute */
      ctx->ExecuteFlag = GL_TRUE;
   }

   ctx->API = ctx->Save;  /* Switch the API function pointers */
}



/*
 * End definition of current display list.
 */
void gl_EndList( GLcontext *ctx )
{
   Node *n;

   /* Check that a list is under construction */
   if (!CurrentListPtr) {
      gl_error( ctx, GL_INVALID_OPERATION, "glEndList" );
      return;
   }

   n = alloc_instruction( ctx, OPCODE_END_OF_LIST, 0 );

   /* Install the list */
   if (ctx->Shared->List[CurrentListNum-1]) {
      destroy_list( ctx, CurrentListNum-1 );
   }
   ctx->Shared->List[CurrentListNum-1] = CurrentListPtr;

   CurrentListNum = 0;
   CurrentListPtr = NULL;
   ctx->ExecuteFlag = GL_TRUE;
   ctx->CompileFlag = GL_FALSE;

   ctx->API = ctx->Exec;   /* Switch the API function pointers */
}



void gl_CallList( GLcontext *ctx, GLuint list )
{
   /* VERY IMPORTANT:  Save the CompileFlag status, turn it off, */
   /* execute the display list, and restore the CompileFlag. */
   GLboolean save_compile_flag;
   save_compile_flag = ctx->CompileFlag;
   ctx->CompileFlag = GL_FALSE;
   execute_list( ctx, list );
   ctx->CompileFlag = save_compile_flag;
}



/*
 * Execute glCallLists:  call multiple display lists.
 */
void gl_CallLists( GLcontext *ctx,
                   GLsizei n, GLenum type, const GLvoid *lists )
{
   GLuint i, list;
   GLboolean save_compile_flag;

   /* Save the CompileFlag status, turn it off, execute display list,
    * and restore the CompileFlag.
    */
   save_compile_flag = ctx->CompileFlag;
   ctx->CompileFlag = GL_FALSE;

   for (i=0;i<n;i++) {
      list = translate_id( i, type, lists );
      execute_list( ctx, ctx->List.ListBase + list );
   }

   ctx->CompileFlag = save_compile_flag;
}



/*
 * Set the offset added to list numbers in glCallLists.
 */
void gl_ListBase( GLcontext *ctx, GLuint base )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glListBase" );
      return;
   }
   ctx->List.ListBase = base;
}




/***
 *** Debugging code
 ***/

static char tmp[1000];

static char *enum_string( GLenum k )
{
   /* TODO: Add many more constant/string entries */
   switch (k) {
      case GL_POINTS:		return "GL_POINTS";
      case GL_LINES:		return "GL_LINES";
      case GL_LINE_STRIP:	return "GL_LINE_STRIP";
      case GL_LINE_LOOP:	return "GL_LINE_LOOP";
      case GL_TRIANGLES:	return "GL_TRIANGLES";
      case GL_TRIANGLE_STRIP:	return "GL_TRIANGLE_STRIP";
      case GL_TRIANGLE_FAN:	return "GL_TRIANGLE_FAN";
      case GL_QUADS:		return "GL_QUADS";
      case GL_QUAD_STRIP:	return "GL_QUAD_STRIP";
      case GL_POLYGON:		return "GL_POLYGON";
      case GL_FRONT:		return "GL_FRONT";
      case GL_BACK:		return "GL_BACK";
      case GL_FRONT_AND_BACK:	return "GL_FRONT_AND_BACK";
      case GL_AMBIENT:		return "GL_AMBIENT";
      case GL_DIFFUSE:		return "GL_DIFFUSE";
      case GL_SPECULAR:		return "GL_SPECULAR";
      case GL_SHININESS:	return "GL_SHININESS";
      default:
         sprintf(tmp,"%X", k);
         return tmp;
   }
}


/*
 * Print the commands in a display list.  For debugging only.
 * TODO: many commands aren't handled yet.
 */
static void print_list( GLcontext *ctx, FILE *f, GLuint list )
{
   Node *n;
   GLboolean done;
   OpCode opcode;

   if (!glIsList(list)) {
      fprintf(f,"%d is not a display list ID\n",list);
      return;
   }

   n = ctx->Shared->List[list-1];

   fprintf( f, "START-LIST %d, address %p\n", list, (void*)n );

   done = GL_FALSE;
   while (!done) {
      opcode = n[0].opcode;

      switch (opcode) {
         case OPCODE_ACCUM:
            fprintf(f,"accum %d %g\n", n[1].e, n[2].f );
	    break;
         case OPCODE_BEGIN:
            fprintf(f,"Begin %s\n", enum_string(n[1].e) );
            break;
	 case OPCODE_BITMAP:
            fprintf(f,"Bitmap %d %d %g %g %g %g %p\n", n[1].i, n[2].i,
		       n[3].f, n[4].f, n[5].f, n[6].f, (void *) n[7].data );
	    break;
         case OPCODE_CALL_LIST:
            fprintf(f,"CallList %d\n", n[1].ui );
            break;
         case OPCODE_CALL_LIST_OFFSET:
            fprintf(f,"CallList %d + offset %d = %d\n", n[1].ui,
                    ctx->List.ListBase, ctx->List.ListBase + n[1].ui );
            break;
	 case OPCODE_COLOR_4F:
            fprintf(f,"Color4f %g %g %g %g\n", n[1].f, n[2].f, n[3].f, n[4].f);
	    break;
	 case OPCODE_COLOR_4UB:
            fprintf(f,"Color4ub %d %d %d %d\n", n[1].ub, n[2].ub,
                                                n[3].ub, n[4].ub );
	    break;
	 case OPCODE_DISABLE:
            fprintf(f,"Disable %s\n", enum_string(n[1].e));
	    break;
	 case OPCODE_ENABLE:
            fprintf(f,"Enable %s\n", enum_string(n[1].e));
	    break;
         case OPCODE_END:
            fprintf(f,"End\n");
            break;
         case OPCODE_INDEX:
            fprintf(f,"Index %d\n", n[1].ui );
            break;
	 case OPCODE_LINE_STIPPLE:
	    fprintf(f,"LineStipple %d %x\n", n[1].i, (int) n[2].us );
	    break;
	 case OPCODE_LOAD_MATRIX:
            fprintf(f,"LoadMatrix (or LoadIdentity)\n");
            fprintf(f,"  %8f %8f %8f %8f\n", n[1].f, n[5].f,  n[9].f, n[13].f);
            fprintf(f,"  %8f %8f %8f %8f\n", n[2].f, n[6].f, n[10].f, n[14].f);
            fprintf(f,"  %8f %8f %8f %8f\n", n[3].f, n[7].f, n[11].f, n[15].f);
            fprintf(f,"  %8f %8f %8f %8f\n", n[4].f, n[8].f, n[12].f, n[16].f);
	    break;
	 case OPCODE_MATERIAL:
            fprintf(f,"Material %s %s %g %g %g %g\n", enum_string(n[1].e),
                    enum_string(n[2].e), n[3].f, n[4].f, n[5].f, n[6].f );
            break;
	 case OPCODE_MULT_MATRIX:
            fprintf(f,"MultMatrix (or Rotate)\n");
            fprintf(f,"  %8f %8f %8f %8f\n", n[1].f, n[5].f,  n[9].f, n[13].f);
            fprintf(f,"  %8f %8f %8f %8f\n", n[2].f, n[6].f, n[10].f, n[14].f);
            fprintf(f,"  %8f %8f %8f %8f\n", n[3].f, n[7].f, n[11].f, n[15].f);
            fprintf(f,"  %8f %8f %8f %8f\n", n[4].f, n[8].f, n[12].f, n[16].f);
	    break;
         case OPCODE_NORMAL:
            fprintf(f,"Normal %g %g %g\n", n[1].f, n[2].f, n[3].f );
            break;
	 case OPCODE_POP_ATTRIB:
            fprintf(f,"PopAttrib\n");
	    break;
	 case OPCODE_POP_MATRIX:
            fprintf(f,"PopMatrix\n");
	    break;
	 case OPCODE_POP_NAME:
            fprintf(f,"PopName\n");
	    break;
	 case OPCODE_PUSH_ATTRIB:
            fprintf(f,"PushAttrib %x\n", n[1].bf );
	    break;
	 case OPCODE_PUSH_MATRIX:
            fprintf(f,"PushMatrix\n");
	    break;
	 case OPCODE_PUSH_NAME:
            fprintf(f,"PushName %d\n", n[1].ui );
	    break;
	 case OPCODE_RASTER_POS:
            fprintf(f,"RasterPos %g %g %g %g\n", n[1].f, n[2].f,n[3].f,n[4].f);
	    break;
         case OPCODE_SCALE:
            fprintf(f,"Scale %g %g %g\n", n[1].f, n[2].f, n[3].f );
            break;
	 case OPCODE_TEXCOORD:
            fprintf(f,"Texcoord %g %g %g %g\n", n[1].f, n[2].f, n[3].f,n[4].f);
	    break;
         case OPCODE_TRANSLATE:
            fprintf(f,"Translate %g %g %g\n", n[1].f, n[2].f, n[3].f );
            break;
         case OPCODE_VERTEX:
            fprintf(f,"Vertex %g %g %g %g\n", n[1].f, n[2].f, n[3].f, n[4].f );
            break;

	 /*
	  * meta opcodes/commands
	  */
	 case OPCODE_CONTINUE:
            fprintf(f,"DISPLAY-LIST-CONTINUE\n");
	    n = (Node *) n[1].next;
	    break;
	 case OPCODE_END_OF_LIST:
            fprintf(f,"END-LIST %d\n", list);
	    done = GL_TRUE;
	    break;
         default:
            if (opcode < 0 || opcode > OPCODE_END_OF_LIST) {
               fprintf(f,"ERROR IN DISPLAY LIST: opcode = %d, address = %p\n",
                       opcode, (void*) n);
               return;
            }
            else {
               fprintf(f,"command %d, %d operands\n",opcode,InstSize[opcode]);
            }
      }

      /* increment n to point to next compiled command */
      if (opcode!=OPCODE_CONTINUE) {
	 n += InstSize[opcode];
      }

   }
}



/*
 * Clients may call this function to help debug display list problems.
 * This function is _ONLY_FOR_DEBUGGING_PURPOSES_.  It may be removed,
 * changed, or break in the future without notice.
 */
void mesa_print_display_list( GLuint list )
{
   GLcontext *ctx;
#ifdef MULTI_THREADING
   ctx = gl_get_thread_context();
#else
   ctx = CC;
#endif
   print_list( ctx, stdout, list );
}
