#  Make WWW under unix for a.n.other unix system (bsd)
#   Use this as a template

# For W3 distribution, machine type for subdirectories
WWW_MACH = djgpp

# The ASIS repository's name for the machine we are on
#ASIS_MACH = hardware/os

# Use this option to enable optional and *experimental* color style.
#ENABLE_COLOR_STYLE = \
 -DUSE_COLOR_STYLE \
 -DUSE_HASH \
 -DLINKEDSTYLES


CFLAGS = -O1 -DUSE_ZLIB -DDOSPATH -DNOUSERS -DDISP_PARTIAL \
-DDIRED_SUPPORT -DSOURCE_CACHE -DUSE_PRETTYSRC \
 $(ENABLE_COLOR_STYLE) \
-DEXP_FILE_UPLOAD \
-DWATT32 \
-I../Implementation \
-I../../../djgpp/watt32/inc -I../../../djgpp/watt32/inc/sys \
-I../../../src \
-I../../../curses \
-I../../.. $(INTLFLAGS)
LFLAGS =
CC = gcc
#INTLFLAGS = -DHAVE_GETTEXT -DHAVE_LIBINTL_H

# Directory for installed binary:
!BINDIR = /usr/local/bin

# Where is the W3 object library to be installed (not normally done)?
LIBDIR = $(WWW)/Library/Implementation/$(WWW_MACH)

#_________________ OK if normal W3 distribution
# Where is the WWW source root?
WWW = ../..

#  Where should temporary (object) files go?
WTMP = ../..

include $(WWW)/Library/Implementation/Version.make
#include $(WWW)/Library/Implementation/CommonMakefile
include ./CommonMakefile
