# $Id: Makefile,v 1.3 1997/02/24 20:05:54 brianp Exp $

# Mesa 3-D graphics library
# Version:  2.2
# Copyright (C) 1995-1997  Brian Paul
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


# Makefile for core library


# $Log: Makefile,v $
# Revision 1.3  1997/02/24 20:05:54  brianp
# updated version to 2.2
#
# Revision 1.2  1996/09/27 01:33:24  brianp
# removed fortran.c
#
# Revision 1.1  1996/09/13 01:37:56  brianp
# Initial revision
#



##### MACROS #####

VPATH = RCS

INCDIR = ../include
LIBDIR = ../lib

CORE_SOURCES = accum.c alpha.c alphabuf.c api.c attrib.c bitmap.c \
	blend.c bresenhm.c clip.c context.c copypix.c depth.c \
	dlist.c draw.c drawpix.c enable.c eval.c feedback.c fog.c \
	get.c interp.c image.c light.c lines.c logic.c \
	masking.c matrix.c misc.c pb.c pixel.c points.c pointers.c \
	polygon.c readpix.c scissor.c span.c stencil.c teximage.c \
	texobj.c texture.c triangle.c varray.c vb.c vertex.c winpos.c \
	xform.c

DRIVER_SOURCES = cmesa.c glx.c osmesa.c svgamesa.c \
	xfonts.c xmesa1.c xmesa2.c xmesa3.c



OBJECTS = $(CORE_SOURCES:.c=.o) $(DRIVER_SOURCES:.c=.o)



##### RULES #####

.c.o:
	$(CC) -c -I$(INCDIR) $(CFLAGS) $<



##### TARGETS #####

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

targets: $(LIBDIR)/$(GL_LIB)

# Make the library
$(LIBDIR)/$(GL_LIB): $(OBJECTS)
	$(MAKELIB) $(GL_LIB) $(OBJECTS)
	$(RANLIB) $(GL_LIB)
	mv $(GL_LIB)* $(LIBDIR)

include ../Make-config

include depend



#
# Run 'make dep' to update the dependencies if you change what's included
# by any source file.
# 
dep: $(CORE_SOURCES) $(DRIVER_SOURCES)
	makedepend -fdepend -Y -I../include $(CORE_SOURCES) $(DRIVER_SOURCES)
