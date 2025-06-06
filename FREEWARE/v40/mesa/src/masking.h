/* $Id: masking.h,v 1.1 1996/09/13 01:38:16 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  2.0
 * Copyright (C) 1995-1996  Brian Paul
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
 * $Log: masking.h,v $
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#ifndef MASKING_H
#define MASKING_H


#include "types.h"



extern void gl_IndexMask( GLcontext *ctx, GLuint mask );

extern void gl_ColorMask( GLcontext *ctx, GLboolean red, GLboolean green,
                          GLboolean blue, GLboolean alpha );


/*
 * Implement glColorMask for a span of RGBA pixels.
 */
extern void gl_mask_color_span( GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                                GLubyte red[], GLubyte green[],
                                GLubyte blue[], GLubyte alpha[] );



/*
 * Implement glColorMask for an array of RGBA pixels.
 */
extern void gl_mask_color_pixels( GLcontext *ctx,
                                  GLuint n, const GLint x[], const GLint y[],
                                  GLubyte red[], GLubyte green[],
                                  GLubyte blue[], GLubyte alpha[],
                                  const GLubyte mask[] );



/*
 * Implement glIndexMask for a span of CI pixels.
 */
extern void gl_mask_index_span( GLcontext *ctx,
                                GLuint n, GLint x, GLint y, GLuint index[] );



/*
 * Implement glIndexMask for an array of CI pixels.
 */
extern void gl_mask_index_pixels( GLcontext *ctx,
                                  GLuint n, const GLint x[], const GLint y[],
                                  GLuint index[], const GLubyte mask[] );



#endif

