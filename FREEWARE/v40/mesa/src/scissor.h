/* $Id: scissor.h,v 1.1 1996/09/13 01:38:16 brianp Exp $ */

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
 * $Log: scissor.h,v $
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


#ifndef SCISSOR_H
#define SCISSOR_H


#include "types.h"


extern void gl_Scissor( GLcontext *ctx,
                        GLint x, GLint y, GLsizei width, GLsizei height );


extern GLint gl_scissor_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y, GLubyte mask[] );


extern GLuint gl_scissor_pixels( GLcontext *ctx,
                                 GLuint n, const GLint x[], const GLint y[],
                                 GLubyte mask[] );


#endif
