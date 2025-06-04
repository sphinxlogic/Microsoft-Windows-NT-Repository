# VMS MMS/MMK Description file for MPACK 1.5
#
# Call like
#   $ MM[SK] /DESCRIPTION=MAKEFILE.VMS [/MACRO=(...)]
#
# Macro options (comma-separated list):
#   __VAXC__=1         to build using VAX C instead of DEC C
#   __DEBUG__=1        to build a debug version
#   USE_GETOPT=1       to use the supplied getopt()
#   USE_GETHOSTNAME=1  to use the supplied gethostname() function

# MMK doesn't support MMS' convenient MMS$ARCH_NAME
.ifdef __MMK__
.ifdef __ALPHA__
MMS$ARCH_NAME = Alpha
.else
MMS$ARCH_NAME = VAX
.endif
.endif

BINDIR = [-.$(MMS$ARCH_NAME)]

# Select the compiler
.ifdef __VAXC__
CC = CC/VAXC
.else
CC = CC/DECC
.endif

# Define debug options
.ifdef __DEBUG__
CDBG = /DEBUG /NOOPTIMIZE /LIST /SHOW=ALL
LDBG = /DEBUG /MAP
.else
CDBG =
LDBG =
.endif

# Compiler pecularities
.ifdef __VAXC__
COPT =
LOPT = , []VAXCRTL.OPT/OPTION
USE_GETOPT = 1
USE_GETHOSTNAME = 1
.else
.ifdef USE_GETOPT
PREFIX = /PREFIX=(ALL,EXCEPT=(getopt))
.else
PREFIX = /PREFIX=(ALL)
.endif
COPT = $(PREFIX) /WARNING=(DISABLE=(IMPLICITFUNC,MISSINGRETURN))
LOPT =
.endif

.ifdef USE_GETOPT
GETOPT_OBJ = , $(BINDIR)getopt.obj
.endif


CFLAGS = $(COPT) /OBJECT=$(MMS$TARGET) $(CDBG)
LINKFLAGS = /EXECUTABLE=$(MMS$TARGET) $(LDBG)


COMMONOBJS = $(BINDIR)codes.obj, $(BINDIR)vmsos.obj, $(BINDIR)string.obj, \
             $(BINDIR)xmalloc.obj, $(BINDIR)md5c.obj $(GETOPT_OBJ)

PACKOBJS = $(BINDIR)vmspk.obj, $(BINDIR)encode.obj, $(BINDIR)magic.obj

UNPACKOBJS = $(BINDIR)vmsunpk.obj, $(BINDIR)decode.obj, $(BINDIR)uudecode.obj, \
             $(BINDIR)part.obj


all : $(BINDIR)mpack.exe $(BINDIR)munpack.exe
	@ WRITE SYS$OUTPUT "All done."

$(BINDIR)mpacklib.olb : $(BINDIR)mpacklib.olb( $(COMMONOBJS) )
	@ CONTINUE

$(BINDIR)mpack.exe : $(PACKOBJS), $(BINDIR)mpacklib.olb
	$(LINK) $(LINKFLAGS) $(PACKOBJS), $(BINDIR)mpacklib.olb/LIBRARY $(LOPT)

$(BINDIR)munpack.exe : $(UNPACKOBJS), $(BINDIR)mpacklib.olb
	$(LINK) $(LINKFLAGS) $(UNPACKOBJS), $(BINDIR)mpacklib.olb/LIBRARY $(LOPT)


distclean :
	- DELETE /NOLOG /NOCONFIRM *.lis;*, *.map;*
	- DELETE /NOLOG /NOCONFIRM $(BINDIR)codes.obj;*,vmsos.obj;*,string.obj;*, \
		xmalloc.obj;*,md5c.obj;*,getopt.obj;*

realclean : distclean
	- DELETE /NOLOG /NOCONFIRM $(BINDIR)vmspk.obj;*,encode.obj;*,magic.obj;*
	- DELETE /NOLOG /NOCONFIRM $(BINDIR)vmsunpk.obj;*,decode.obj;*, \
		uudecode.obj;*,part.obj;*
	- DELETE /NOLOG /NOCONFIRM $(BINDIR)mpacklib.olb;*

clobber : realclean
	- DELETE /NOLOG /NOCONFIRM $(BINDIR)mpack.exe;*,munpack.exe;*
	- PURGE /NOLOG [-...]


$(BINDIR)vmsos.obj : vmsos.c xmalloc.h common.h part.h
.ifdef USE_GETHOSTNAME
	$(CC) $(CFLAGS) /DEFINE=(USE_GETHOSTNAME) $(MMS$SOURCE)
.endif

# MMS can't cope with the dependancy alone - stupid
$(BINDIR)vmsunpk.obj : unixunpk.c version.h part.h
	$(CC) $(CFLAGS) $(MMS$SOURCE)

$(BINDIR)codes.obj : codes.c xmalloc.h md5.h
$(BINDIR)string.obj : string.c
$(BINDIR)xmalloc.obj : xmalloc.c
$(BINDIR)md5c.obj : md5c.c md5.h
$(BINDIR)gethostname.obj : gethostname.c

$(BINDIR)getopt.obj : getopt.c

$(BINDIR)vmspk.obj : vmspk.c common.h version.h xmalloc.h
$(BINDIR)encode.obj : encode.c 
$(BINDIR)magic.obj : magic.c

$(BINDIR)decode.obj : decode.c xmalloc.h common.h part.h md5.h
$(BINDIR)uudecode.obj : uudecode.c xmalloc.h common.h part.h
$(BINDIR)part.obj : part.c part.h xmalloc.h
