# $Id: Makefile,v 1.1 1996/09/27 01:22:36 brianp Exp $

# Mesa 3-D graphics library
# Version:  2.0
# Copyright (C) 1995-1996  Brian Paul
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


# $Log: Makefile,v $
# Revision 1.1  1996/09/27 01:22:36  brianp
# Initial revision
#



##### MACROS #####

VPATH = RCS

INCDIR = ../include
LIBDIR = ../lib

SOURCES = glu.c mipmap.c nurbs.c nurbscrv.c nurbssrf.c nurbsutl.c \
	project.c quadric.c tess.c tesselat.c polytest.c

OBJECTS = $(SOURCES:.c=.o)



##### RULES #####

.c.o:
	$(CC) -c -I$(INCDIR) $(CFLAGS) $<



##### TARGETS #####

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

targets: $(LIBDIR)/$(GLU_LIB)

# Make the library:
$(LIBDIR)/$(GLU_LIB): $(OBJECTS)
	$(MAKELIB) $(GLU_LIB) $(OBJECTS)
	$(RANLIB) $(GLU_LIB)
	mv $(GLU_LIB)* $(LIBDIR)

include ../Make-config

include depend



#
# Run 'make depend' to update the dependencies if you change what's included
# by any source file.
# 
dep: $(SOURCES)
	makedepend -fdepend -Y -I../include $(SOURCES)

