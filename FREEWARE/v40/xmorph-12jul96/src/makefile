# Makefile for xmorph
#
# Written and Copyright (C) 1994,1995 by Michael J. Gourlay
#
# NO WARRENTIES, EXPRESS OR IMPLIED.
#
# INSTRUCTIONS:
#
# 0) Read the README file.  Then read this Makefile.
#
# 1) Make a backup copy of this Makefile.
#
# 2) Edit each of the variables before the "SRCS=" statement.
#    This is pretty vanilla X and C code.  You'll need X11 libraries,
#    including the X Toolkit, X Miscellaneous utilities, and the Athena
#    widgets.
#
# 3) HPUX 7.0B has a problem with long tokens, so the help strings in
#    help_menu.c will have to be trimmed if you run HPUX 7.0B's native C
#    compiler.
#
# 4) From the shell, type "make depend".
#    If you don't have makedepend, just delete all of the lines (if
#    there are any) below the "depend:" rule at the end of this
#    Makefile.
#
# 5) From the shell, type "make".
#
# 6) If there are any errors or compiler complaints, find out why and
#    fix them :).  Then send email to the author explaining the cause of
#    the problem, and its solution.  If it's a big deal, I'll put your
#    name in lights for the next release.  See the README file for
#    the author's internet email address.
#
#    But seriously folks, if you can't figure out the problem, send
#    the author email.  Before complaining, though, read the man
#    pages for your C compiler and your linker.  If you're using
#    static linking and you get a linker problem, try dynamic
#    libraries, and vice versa.
#
#
#    If all goes well, there will be an executable called "xmorph".
#
# 7) From the shell, type "xmorph".
#
#    There are help pages accessible from within xmorph.  If there are
#    any bugs, fix them and/or report them to the author.  See the
#    README file for an email address.


##
## C COMPILER
##
#
# You must use an ANSI C compiler.
#

CC=ucbcc
CC=apcc -DANSI
CC=gcc -W -Wunused -Wuninitialized -Wshadow -DANSI
CC=gcc -DANSI
CC=cc


##
## C COMPILER FLAGS
##
#
# Read the man page for your C compiler before sending me email about
# problems.
#
# SOLARIS: On some Solaris machines, linking with the openwin libraries
# REQUIRES including the openwin headers (not MIT headers!).
#
# SUNOS: Add the option -DSUNOS version if you want to do memory checking.
# -- Also see LIBS for another flag relating to memory checking.
#
# HPUX 7.0B: native C compiler needs more symbol table space: +Ns4000
#
#CFLAGS=-g +Ns4000

CFLAGS=-O
CFLAGS=-g -O -I/usr/openwin/include -DSUNOS
CFLAGS=-g -O -I/usr/openwin/include
CFLAGS=-g


##
## LIBRARIES
##
#
# Read the man page for your linker before sending me email about
# problems.
#
# Must have the X libraries: X11, MIT Athena Widgets, X Toolkit, X Misc Utils
# Must have the math library.
#
# FreeBSD: Do not use static libraries, because for some reason they
# don't work.
#
# SUNOS: Might have to use static X libraries because dynamic X
# libraries are incomplete.
#
# SUNOS: Use the /usr/lib/debug/malloc.o module if you want to do
# -- memory checking.
# -- Also see the CFLAGS comments about memory checking under SUNOS.
#
# SOLARIS: to link to openwin libraries, add -L/usr/openwin/lib
#
LIBS=-L/usr/openwin/lib -lXaw -lXmu -lXext -lXt -lX11 -lm /usr/lib/debug/malloc.o
LIBS=-L/usr/openwin/lib -lXaw -lXmu -lXext -lXt -lX11 -lm
LIBS=-lXaw -lXmu -lXext -lXt -lX11 -lm


#### ===========================
####
#### Nothing to edit below here.
####
#### ===========================


SRCS=\
     diw_map.c \
     file.c \
     file_menu.c \
     help_menu.c \
     image.c \
     image_cb.c \
     mesh.c \
     mesh_cb.c \
     mesh_menu.c \
     mjg_dialog.c \
     my_malloc.c \
     sequence.c \
     spline.c \
     spl-array.c \
     tga-24.c \
     tga-write.c \
     warp.c \
     warp_menu.c \
     xmorph.c

OBJS=\
     diw_map.o \
     file.o \
     file_menu.o \
     help_menu.o \
     image.o \
     image_cb.o \
     mesh.o \
     mesh_cb.o \
     mesh_menu.o \
     mjg_dialog.o \
     my_malloc.o \
     sequence.o \
     spline.o \
     spl-array.o \
     tga-24.o \
     tga-write.o \
     warp.o \
     warp_menu.o \
     xmorph.o

DIST=FILES Makefile Makefile.orig HISTORY README xmorph.README \
     *.[ch] *.xbm xmorph.man

all: xmorph

xmorph:   $(OBJS)
	$(CC) $(CFLAGS) -o xmorph $(OBJS) $(LIBS)

.c$(OBJ): $(SRCS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

test-tga: $(OBJS) test-tga.c
	$(CC) $(CFLAGS) -o test-tga test-tga.c tga-write.o

clean:
	rm -f $(OBJS) core
	rm -f test-tga

veryclean: clean
	rm -f xmorph

back:
	if [ ! -d backup ]; then mkdir backup ; fi
	ls -ltr > FILES
	cp -p Makefile Makefile.orig
	cp -p README xmorph.README
	cp -p $(DIST) backup
	@echo
	@echo "NOW MOVE THE BACKUP DIRECTORY:  mv backup `date +%d%b%y | tr '[A-Z]' '[a-z]'`"
	@echo

dist:
	tar cLvf - $(DIST) | compress > xmorph.tar.Z

ctags:
	ctags -t *.[ch]

unproto:
	unprotoize $(SRCS)

grind:
	for file in $(SRCS) *.h ; do tgrind -p $$file ; done

ctrace:
	-mkdir trace
	-for cfile in $(SRCS) ; do \
	  ctrace -P -t 20 $$cfile > trace/$$cfile ; \
	done

depend:
	makedepend $(CFLAGS) $(SRCS)
# DO NOT DELETE THIS LINE -- make depend depends on it.

diw_map.o: /usr/include/stdio.h /usr/include/X11/Xos.h
diw_map.o: /usr/include/X11/Xosdefs.h /usr/include/sys/types.h
diw_map.o: /usr/include/sgidefs.h /usr/include/sys/bsd_types.h
diw_map.o: /usr/include/sys/select.h /usr/include/string.h
diw_map.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
diw_map.o: /usr/include/unistd.h /usr/include/sys/time.h
diw_map.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
diw_map.o: /usr/include/X11/Xfuncproto.h /usr/include/stddef.h
diw_map.o: /usr/include/X11/Xutil.h /usr/include/X11/Intrinsic.h
diw_map.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
diw_map.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
diw_map.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
diw_map.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/Form.h
diw_map.o: /usr/include/X11/Xaw/MenuButton.h /usr/include/X11/Xaw/Command.h
diw_map.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
diw_map.o: /usr/include/X11/Xmu/Converters.h
diw_map.o: /usr/include/X11/Xaw/SimpleMenu.h /usr/include/X11/Shell.h
diw_map.o: /usr/include/X11/Xaw/SmeBSB.h /usr/include/X11/Xaw/Sme.h
diw_map.o: /usr/include/X11/Xaw/SmeLine.h /usr/include/X11/Xaw/Scrollbar.h
diw_map.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
diw_map.o: /usr/include/X11/bitmaps/gray select.xbm unselect.xbm diw_map.h
diw_map.o: my_malloc.h image.h warp.h spline.h spl-array.h image_cb.h mesh.h
diw_map.o: mjg_dialog.h /usr/include/X11/Xaw/Dialog.h
diw_map.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
diw_map.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
diw_map.o: /usr/include/X11/Xaw/AsciiSink.h
file.o: /usr/include/stdio.h /usr/include/string.h file.h
file_menu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
file_menu.o: /usr/include/sys/types.h /usr/include/sgidefs.h
file_menu.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
file_menu.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
file_menu.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
file_menu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
file_menu.o: /usr/include/string.h /usr/include/X11/Core.h
file_menu.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
file_menu.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
file_menu.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/MenuButton.h
file_menu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
file_menu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
file_menu.o: /usr/include/X11/Xaw/SimpleMenu.h /usr/include/X11/Shell.h
file_menu.o: /usr/include/X11/Xaw/SmeBSB.h /usr/include/X11/Xaw/Sme.h
file_menu.o: /usr/include/X11/Xaw/SmeLine.h /usr/include/stdio.h
file_menu.o: /usr/include/stdlib.h mjg_dialog.h /usr/include/X11/Xos.h
file_menu.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
file_menu.o: /usr/include/unistd.h /usr/include/sys/time.h
file_menu.o: /usr/include/X11/Xaw/Dialog.h /usr/include/X11/Xaw/Form.h
file_menu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
file_menu.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
file_menu.o: /usr/include/X11/Xaw/AsciiSink.h mesh_cb.h image_cb.h
file_menu.o: file_menu.h
help_menu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
help_menu.o: /usr/include/sys/types.h /usr/include/sgidefs.h
help_menu.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
help_menu.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
help_menu.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
help_menu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
help_menu.o: /usr/include/string.h /usr/include/X11/Core.h
help_menu.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
help_menu.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
help_menu.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/MenuButton.h
help_menu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
help_menu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
help_menu.o: /usr/include/X11/Xaw/SimpleMenu.h /usr/include/X11/Shell.h
help_menu.o: /usr/include/X11/Xaw/SmeBSB.h /usr/include/X11/Xaw/Sme.h
help_menu.o: /usr/include/X11/Xaw/SmeLine.h /usr/include/stdio.h
help_menu.o: /usr/include/stdlib.h mjg_dialog.h /usr/include/X11/Xos.h
help_menu.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
help_menu.o: /usr/include/unistd.h /usr/include/sys/time.h
help_menu.o: /usr/include/X11/Xaw/Dialog.h /usr/include/X11/Xaw/Form.h
help_menu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
help_menu.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
help_menu.o: /usr/include/X11/Xaw/AsciiSink.h help_menu.h
image.o: /usr/include/X11/Xos.h /usr/include/X11/Xosdefs.h
image.o: /usr/include/sys/types.h /usr/include/sgidefs.h
image.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
image.o: /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h
image.o: /usr/include/unistd.h /usr/include/sys/time.h
image.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
image.o: /usr/include/X11/Xfuncproto.h /usr/include/stddef.h
image.o: /usr/include/X11/Xutil.h diw_map.h /usr/include/X11/Intrinsic.h
image.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
image.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
image.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
image.o: /usr/include/X11/StringDefs.h my_malloc.h image.h tga.h
image.o: /usr/include/stdio.h
image_cb.o: /usr/include/stdio.h /usr/include/string.h /usr/include/memory.h
image_cb.o: diw_map.h /usr/include/X11/Xos.h /usr/include/X11/Xosdefs.h
image_cb.o: /usr/include/sys/types.h /usr/include/sgidefs.h
image_cb.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
image_cb.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
image_cb.o: /usr/include/unistd.h /usr/include/sys/time.h
image_cb.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
image_cb.o: /usr/include/X11/Xfuncproto.h /usr/include/stddef.h
image_cb.o: /usr/include/X11/Xutil.h /usr/include/X11/Intrinsic.h
image_cb.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
image_cb.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
image_cb.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
image_cb.o: /usr/include/X11/StringDefs.h my_malloc.h image.h mesh.h
image_cb.o: mjg_dialog.h /usr/include/X11/Shell.h
image_cb.o: /usr/include/X11/Xaw/Dialog.h /usr/include/X11/Xaw/Form.h
image_cb.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
image_cb.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
image_cb.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/Command.h
image_cb.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
image_cb.o: /usr/include/X11/Xmu/Converters.h image_cb.h
mesh.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/sgidefs.h
mesh.o: diw_map.h /usr/include/X11/Xos.h /usr/include/X11/Xosdefs.h
mesh.o: /usr/include/sys/types.h /usr/include/sys/bsd_types.h
mesh.o: /usr/include/sys/select.h /usr/include/string.h /usr/include/fcntl.h
mesh.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
mesh.o: /usr/include/sys/time.h /usr/include/X11/Xlib.h /usr/include/X11/X.h
mesh.o: /usr/include/X11/Xfuncproto.h /usr/include/stddef.h
mesh.o: /usr/include/X11/Xutil.h /usr/include/X11/Intrinsic.h
mesh.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
mesh.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
mesh.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
mesh.o: /usr/include/X11/StringDefs.h my_malloc.h image.h file.h mesh.h
mesh_cb.o: /usr/include/stdio.h diw_map.h /usr/include/X11/Xos.h
mesh_cb.o: /usr/include/X11/Xosdefs.h /usr/include/sys/types.h
mesh_cb.o: /usr/include/sgidefs.h /usr/include/sys/bsd_types.h
mesh_cb.o: /usr/include/sys/select.h /usr/include/string.h
mesh_cb.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
mesh_cb.o: /usr/include/unistd.h /usr/include/sys/time.h
mesh_cb.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
mesh_cb.o: /usr/include/X11/Xfuncproto.h /usr/include/stddef.h
mesh_cb.o: /usr/include/X11/Xutil.h /usr/include/X11/Intrinsic.h
mesh_cb.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
mesh_cb.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
mesh_cb.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
mesh_cb.o: /usr/include/X11/StringDefs.h my_malloc.h image.h mjg_dialog.h
mesh_cb.o: /usr/include/X11/Shell.h /usr/include/X11/Xaw/Dialog.h
mesh_cb.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Text.h
mesh_cb.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
mesh_cb.o: /usr/include/X11/Xaw/AsciiSrc.h /usr/include/X11/Xaw/AsciiSink.h
mesh_cb.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
mesh_cb.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
mesh_cb.o: mesh.h mesh_cb.h
mesh_menu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
mesh_menu.o: /usr/include/sys/types.h /usr/include/sgidefs.h
mesh_menu.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
mesh_menu.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
mesh_menu.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
mesh_menu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
mesh_menu.o: /usr/include/string.h /usr/include/X11/Core.h
mesh_menu.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
mesh_menu.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
mesh_menu.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/Form.h
mesh_menu.o: /usr/include/X11/Xaw/MenuButton.h /usr/include/X11/Xaw/Command.h
mesh_menu.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
mesh_menu.o: /usr/include/X11/Xmu/Converters.h
mesh_menu.o: /usr/include/X11/Xaw/SimpleMenu.h /usr/include/X11/Shell.h
mesh_menu.o: /usr/include/X11/Xaw/SmeBSB.h /usr/include/X11/Xaw/Sme.h
mesh_menu.o: /usr/include/X11/Xaw/SmeLine.h /usr/include/stdio.h
mesh_menu.o: /usr/include/stdlib.h mesh_cb.h mesh_menu.h
mjg_dialog.o: /usr/include/stdio.h /usr/include/stdlib.h
mjg_dialog.o: /usr/include/sgidefs.h /usr/include/string.h mjg_dialog.h
mjg_dialog.o: /usr/include/X11/Xos.h /usr/include/X11/Xosdefs.h
mjg_dialog.o: /usr/include/sys/types.h /usr/include/sys/bsd_types.h
mjg_dialog.o: /usr/include/sys/select.h /usr/include/fcntl.h
mjg_dialog.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
mjg_dialog.o: /usr/include/sys/time.h /usr/include/X11/Xlib.h
mjg_dialog.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
mjg_dialog.o: /usr/include/stddef.h /usr/include/X11/Xutil.h
mjg_dialog.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xresource.h
mjg_dialog.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
mjg_dialog.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
mjg_dialog.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
mjg_dialog.o: /usr/include/X11/Shell.h /usr/include/X11/Xaw/Dialog.h
mjg_dialog.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Text.h
mjg_dialog.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
mjg_dialog.o: /usr/include/X11/Xaw/AsciiSrc.h
mjg_dialog.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/Command.h
mjg_dialog.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
mjg_dialog.o: /usr/include/X11/Xmu/Converters.h my_malloc.h
my_malloc.o: /usr/include/stdio.h /usr/include/stdlib.h
my_malloc.o: /usr/include/sgidefs.h /usr/include/malloc.h my_malloc.h
sequence.o: /usr/include/stdio.h /usr/include/string.h /usr/include/math.h
sequence.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
sequence.o: /usr/include/sys/types.h /usr/include/sgidefs.h
sequence.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
sequence.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
sequence.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
sequence.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
sequence.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
sequence.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
sequence.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
sequence.o: /usr/include/X11/Xaw/Scrollbar.h
sequence.o: /usr/include/X11/Xmu/Converters.h diw_map.h
sequence.o: /usr/include/X11/Xos.h /usr/include/fcntl.h
sequence.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
sequence.o: /usr/include/sys/time.h my_malloc.h image.h mjg_dialog.h
sequence.o: /usr/include/X11/Shell.h /usr/include/X11/Xaw/Dialog.h
sequence.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Text.h
sequence.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
sequence.o: /usr/include/X11/Xaw/AsciiSrc.h /usr/include/X11/Xaw/AsciiSink.h
sequence.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
sequence.o: /usr/include/X11/Xaw/Simple.h warp.h mesh.h sequence.h
spline.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/sgidefs.h
spline.o: /usr/include/math.h spline.h my_malloc.h
spl-array.o: /usr/include/stdio.h /usr/include/stdlib.h
spl-array.o: /usr/include/sgidefs.h /usr/include/math.h spline.h spl-array.h
spl-array.o: my_malloc.h
tga-24.o: /usr/include/stdio.h /usr/include/memory.h my_malloc.h file.h tga.h
tga-24.o: image.h /usr/include/X11/Xos.h /usr/include/X11/Xosdefs.h
tga-24.o: /usr/include/sys/types.h /usr/include/sgidefs.h
tga-24.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
tga-24.o: /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h
tga-24.o: /usr/include/unistd.h /usr/include/sys/time.h
tga-24.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
tga-24.o: /usr/include/X11/Xfuncproto.h /usr/include/stddef.h
tga-24.o: /usr/include/X11/Xutil.h /usr/include/X11/Intrinsic.h
tga-24.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
tga-24.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
tga-24.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
tga-24.o: /usr/include/X11/StringDefs.h
tga-write.o: /usr/include/stdio.h /usr/include/memory.h file.h tga.h image.h
tga-write.o: /usr/include/X11/Xos.h /usr/include/X11/Xosdefs.h
tga-write.o: /usr/include/sys/types.h /usr/include/sgidefs.h
tga-write.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
tga-write.o: /usr/include/string.h /usr/include/fcntl.h
tga-write.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
tga-write.o: /usr/include/sys/time.h /usr/include/X11/Xlib.h
tga-write.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
tga-write.o: /usr/include/stddef.h /usr/include/X11/Xutil.h
tga-write.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xresource.h
tga-write.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
tga-write.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
tga-write.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
warp.o: /usr/include/stdio.h my_malloc.h warp.h
warp_menu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
warp_menu.o: /usr/include/sys/types.h /usr/include/sgidefs.h
warp_menu.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
warp_menu.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
warp_menu.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
warp_menu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
warp_menu.o: /usr/include/string.h /usr/include/X11/Core.h
warp_menu.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
warp_menu.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
warp_menu.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/MenuButton.h
warp_menu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
warp_menu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
warp_menu.o: /usr/include/X11/Xaw/SimpleMenu.h /usr/include/X11/Shell.h
warp_menu.o: /usr/include/X11/Xaw/SmeBSB.h /usr/include/X11/Xaw/Sme.h
warp_menu.o: /usr/include/X11/Xaw/SmeLine.h /usr/include/stdio.h
warp_menu.o: /usr/include/stdlib.h mjg_dialog.h /usr/include/X11/Xos.h
warp_menu.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
warp_menu.o: /usr/include/unistd.h /usr/include/sys/time.h
warp_menu.o: /usr/include/X11/Xaw/Dialog.h /usr/include/X11/Xaw/Form.h
warp_menu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
warp_menu.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
warp_menu.o: /usr/include/X11/Xaw/AsciiSink.h sequence.h diw_map.h
warp_menu.o: my_malloc.h image.h warp_menu.h
xmorph.o: /usr/include/stdio.h /usr/include/X11/Intrinsic.h
xmorph.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
xmorph.o: /usr/include/sgidefs.h /usr/include/sys/bsd_types.h
xmorph.o: /usr/include/sys/select.h /usr/include/X11/X.h
xmorph.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
xmorph.o: /usr/include/stddef.h /usr/include/X11/Xutil.h
xmorph.o: /usr/include/X11/Xresource.h /usr/include/string.h
xmorph.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
xmorph.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
xmorph.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
xmorph.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Box.h
xmorph.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Paned.h
xmorph.o: /usr/include/X11/Xaw/Scrollbar.h diw_map.h /usr/include/X11/Xos.h
xmorph.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h /usr/include/unistd.h
xmorph.o: /usr/include/sys/time.h my_malloc.h image.h sequence.h file_menu.h
xmorph.o: mesh_menu.h warp_menu.h help_menu.h
