# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = 
host_alias = i386-unknown-sysv4.2
host_cpu = i386
host_vendor = unknown
host_os = sysv4.2
host_canonical = i386-unknown-sysv4.2
target_alias = i386-unknown-sysv4.2
target_cpu = i386
target_vendor = unknown
target_os = sysv4.2
target_canonical = i386-unknown-sysv4.2
package_makefile_frag = Make.pack
target_makefile_frag = ../../target-mkfrag
# A makefile for the stuff now in libg++/etc

srcdir = .

#### package, host, target, and site dependent Makefile fragments come in here.
# Start of package fragment generated by ./../../config.shared.
# TOPDIR=$${rootme}../../../ invsubdir= subdir=
srcdir = .
SUBDIRS = 
prefix = /opt
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
datadir = $(prefix)/lib
mandir = $(prefix)/man
man1dir = $(mandir)/man1
man2dir = $(mandir)/man2
man3dir = $(mandir)/man3
man4dir = $(mandir)/man4
man5dir = $(mandir)/man5
man6dir = $(mandir)/man6
man7dir = $(mandir)/man7
man8dir = $(mandir)/man8
man9dir = $(mandir)/man9
infodir = $(prefix)/info
includedir = $(prefix)/include
gxx_includedir = $(libdir)/g++-include
docdir = $(datadir)/doc

SHELL = /bin/sh

INSTALL = ${rootme}$(srcdir)/../../../install.sh -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)

AR = `if [ -f $${rootme}../../../binutils/ar ] ; \
	then echo $${rootme}../../../binutils/ar ; \
	else echo ar ; fi`
AR_FLAGS = rc
RANLIB = `if [ -f $${rootme}../../../binutils/ranlib ] ; \
	then echo $${rootme}../../../binutils/ranlib ; \
	else echo ranlib ; fi`
NM = `if [ -f $${rootme}../../../binutils/nm ] ; \
	then echo $${rootme}../../../binutils/nm ; \
	else echo nm ; fi`

MAKEINFO = `if [ -f $${rootme}../../../texinfo/C/makeinfo ] ; \
	then echo $${rootme}../../../texinfo/C/makeinfo ; \
	else echo makeinfo ; fi`
TEXIDIR=${srcdir}/../../..//texinfo
TEXI2DVI = TEXINPUTS=${TEXIDIR}:$$TEXINPUTS texi2dvi

CC = `if [ -f $${rootme}../../../gcc/xgcc ] ; \
	then echo $${rootme}../../../gcc/xgcc -B$${rootme}../../../gcc/ ; \
	else echo cc ; fi`
CXX = `if [ -f $${rootme}../../../gcc/xgcc ] ; \
	then echo $${rootme}../../../gcc/xgcc -B$${rootme}../../../gcc/ ; \
	else echo gcc ; fi`

XTRAFLAGS = `if [ -f $${rootme}../../../gcc/xgcc ] ;\
	then echo -I$${rootme}../../../gcc/include;\
	else echo ; fi`
WRAP_C_INCLUDES =
CFLAGS = -g
CXXFLAGS = -g -O
PROTODIR = $(srcdir)/../../src/gen
LIBS = ../../libg++.a
IO_DIR = ../libio
# IO_DIR = no-stream
.PHONY: all
all: $(NOTHING)


CXXINCLUDES =  -I. -I../../../libio -I$(srcdir) -I$(srcdir)/../../$(IO_DIR) -I$(srcdir)/../../src $(WRAP_C_INCLUDES) $(XTRAFLAGS)
CINCLUDES = $(XTRAFLAGS)
.SUFFIXES: .o .C .cc
.c.o:
	$(CC) -c $(CFLAGS) $(CINCLUDES) $<
.C.o:
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDES) $<
.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CXXINCLUDES) $<


.PHONY: install
install:
.PHONY: check
.PHONY: info dvi install-info clean-info
info:
dvi:
install-info:
.PHONY: do-clean-info clean-info do-clean-dvi clean-dvi
do-clean-info:
do-clean-dvi:
clean-info: do-clean-info
clean-dvi: do-clean-dvi

.PHONY: boltcc
boltcc:
	rootme=`pwd`/ ; export rootme ; $(MAKE) $(FLAGS_TO_PASS)


# clean rules
.PHONY: mostlyclean clean distclean realclean
mostlyclean: clean-dvi
	rm -rf *.o \#* core  pXPlex* pPlex* test.pl test2.pl 
clean: clean-dvi
	rm -rf *.o \#* core  pXPlex* pPlex* test.pl test2.pl  graph graph.shar libgraph.a 
distclean: clean
	rm -rf config.status Makefile *~ Make.pack
realclean: clean clean-info
	rm -rf config.status Makefile *~ Make.pack depend *.info*

.PHONY: force
force:

# with the gnu make, this is done automatically.

Makefile: $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status

.NOEXPORT:
MAKEOVERRIDES=


# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

eGetOpt.o : eGetOpt.cc \
  eGetOpt.h \
  $(srcdir)/../../$(IO_DIR)/stream.h \
  $(srcdir)/../../$(IO_DIR)/iostream.h \
  $(srcdir)/../../$(IO_DIR)/streambuf.h 
ePlotFile.o : ePlotFile.cc \
  ePlotFile.h \
  $(srcdir)/../../$(IO_DIR)/PlotFile.h \
  $(srcdir)/../../$(IO_DIR)/fstream.h \
  $(srcdir)/../../$(IO_DIR)/iostream.h \
  $(srcdir)/../../$(IO_DIR)/streambuf.h 
graph.o : graph.cc \
  read_data.h \
  $(srcdir)/../../$(IO_DIR)/stream.h \
  $(srcdir)/../../$(IO_DIR)/iostream.h \
  $(srcdir)/../../$(IO_DIR)/streambuf.h \
  pXPlex.h \
  eGetOpt.h \
  ePlotFile.h \
  $(srcdir)/../../$(IO_DIR)/PlotFile.h \
  $(srcdir)/../../$(IO_DIR)/fstream.h \
  tick_intrvl.h \
  $(srcdir)/../../$(IO_DIR)/strstream.h
read_data.o : read_data.cc \
  read_data.h \
  $(srcdir)/../../$(IO_DIR)/stream.h \
  $(srcdir)/../../$(IO_DIR)/iostream.h \
  $(srcdir)/../../$(IO_DIR)/streambuf.h \
  pXPlex.h
tick_intrvl.o : tick_intrvl.cc \
  tick_intrvl.h 

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
# End of package fragment generated by ./../../config.shared.
# Warning this fragment is automatically generated
RANLIB = true
# Use -nostdinc++ flag if your version of gcc understands it.
NOSTDINC = -nostdinc++

# If the C include files are C++-ready (with extern "C"), uncomment next line:
# WRAP_C_INCLUDES =

# Flags to pass to gen-params when building _G_config.h.
# For example: G_CONFIG_ARGS = size_t="unsigned long"
G_CONFIG_ARGS =
##

check: graph $(srcdir)/test.dat $(srcdir)/test2.dat
	./graph -d -c 3 -g 2 -l "a diagonal line" -X "x axis" -Y "y axis" -m 1\
	        <$(srcdir)/test2.dat >test2.pl
	./graph -d <$(srcdir)/test.dat >test.pl
	@echo use plot to look at the plot files test.pl and test2.pl
run_tests: check

OBJECTS = pPlex.o pXPlex.o eGetOpt.o ePlotFile.o read_data.o tick_intrvl.o
DEPEND_SOURCES = $(srcdir)/*.cc

libgraph.a : $(OBJECTS)
	rm -f libgraph.a
	$(AR) $(AR_FLAGS) libgraph.a $(OBJECTS)
	$(RANLIB) libgraph.a

graph : graph.o libgraph.a
	$(CXX) graph.o libgraph.a $(LIBS) -lm -o $@

graph.o: pXPlex.h pPlex.h

pXPlex.h pXPlex.cc:
	PROTODIR=$(PROTODIR); export PROTODIR; ../../genclass/genclass point val XPlex p
pPlex.h pPlex.cc:
	PROTODIR=$(PROTODIR); export PROTODIR; ../../genclass/genclass point val Plex p

# GNU tail doesn't handle -r - use the 'tac' program instead.
REVERSE_LINES = tail -r
#REVERSE_LINES = tac

$(srcdir)/test.dat:
	-echo 0 0 1 1 2 0 | spline | $(REVERSE_LINES) > $(srcdir)/test.dat

$(srcdir)/test2.dat:
	echo 0 0 >$(srcdir)/test2.dat
	echo 1 1 "label for 1 1" >>$(srcdir)/test2.dat
	echo 2 2 >>$(srcdir)/test2.dat
	echo 3 3 "label for 3 3" >>$(srcdir)/test2.dat
	echo 4 4 >>$(srcdir)/test2.dat

DIST = Makefile graph.tex ChangeLog \
eGetOpt.cc		read_data.cc \
eGetOpt.h		read_data.h \
ePlotFile.cc		tick_intrvl.cc \
ePlotFile.h		tick_intrvl.h \
graph.cc		pdefs.h

graph.tar.Z : $(DIST) CHECKSUMS
	tar cfz $@ $^ CHECKSUMS

CHECKSUMS : $(DIST)
	sum $(DIST) >CHECKSUMS

dist : graph-dist.tar.Z

graph-dist.tar.Z : $(DIST)
	-rm -rf graph-dist
	mkdir graph-dist
	ln $(DIST) graph-dist
	tar cfz graph-dist.tar.Z $(DIST)
	rm -rf graph-dist

graph.shar : pdefs.h graph.cc Makefile graph.texinfo
	shar  pdefs.h graph.cc Makefile graph.texinfo > $@
