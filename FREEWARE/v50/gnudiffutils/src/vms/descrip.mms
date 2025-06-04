# VMS description file for GNU diffutils 2.7.2

PACKAGE = diffutils
VERSION = 2.7.2 for OpenVMS
VERSION_US = 2_7_2
SRCDIR = [-]

DEFAULT_EDITOR_PROGRAM =
DEFAULT_DIFF_PROGRAM = gdiff
NULL_DEVICE = _NLA0:
PR_PROGRAM =

### MMK doesn't support MMS$ARCH_NAME and MMS doesn't support __ALPHA__

.IFDEF __MMK__
.IFDEF __ALPHA__
MMS$ARCH_NAME = Alpha
.ELSE
MMS$ARCH_NAME = VAX
.ENDIF
.ENDIF

OBJ = .$(MMS$ARCH_NAME)_OBJ
OLB = .$(MMS$ARCH_NAME)_OLB
E = .EXE

CC = CC/DECC

.SUFFIXES :
.SUFFIXES : $(E) $(OLB) $(OBJ) .C .H

CDEFINE = "HAVE_CONFIG_H=1"

# to build in debug, just do MM(K|S)/MACRO=__DEBUG__=1

.ifdef __DEBUG__
CDBGFLAGS = /DEBUG /NOOPTIMIZE /LIST /SHOW=ALL
LDBGFLAGS = /DEBUG /TRACE /MAP
.else
CDBGFLAGS = 
LDBGFLAGS = /NOTRACE
.endif

CFLAGS = /PREFIX=ALL/OBJ=$(OBJ) $(CDBGFLAGS)
LINKFLAGS = $(LDBGFLAGS)

COMPILE = $(CC) /INCLUDE=([],$(SRCDIR)) $(CFLAGS)

.c$(OBJ) :
	$(COMPILE) /DEFINE=($(CDEFINE)) $(MMS$SOURCE)

$(OBJ)$(OLB) :
	@ IF F$SEARCH("$(MMS$TARGET)") .EQS. "" -
		THEN LIBRARY/CREATE/LOG $(MMS$TARGET)
	@ LIBRARY /REPLACE /LOG $(MMS$TARGET) $(MMS$SOURCE)

LIBOBJS = alloca$(OBJ), analyze$(OBJ), cmpbuf$(OBJ), context$(OBJ), \
	dir$(OBJ), ed$(OBJ), error$(OBJ), fnmatch$(OBJ), fnmatch$(OBJ), \
	freesoft$(OBJ), getopt$(OBJ), getopt$(OBJ), getopt1$(OBJ), \
	getopt1$(OBJ), ifdef$(OBJ), io$(OBJ), normal$(OBJ), regex$(OBJ), \
	side$(OBJ), util$(OBJ), version$(OBJ), \
	vmsputline$(OBJ), vmsutil$(OBJ), xmalloc$(OBJ)

SRCS = diff.c analyze.c cmpbuf.c cmpbuf.h io.c context.c ed.c normal.c \
       ifdef.c alloca.c util.c dir.c version.c diff.h regex.c regex.h side.c \
       system.h diff3.c sdiff.c cmp.c error.c getopt.c getopt1.c getopt.h \
       fnmatch.c fnmatch.h vmsputline.c vmsutil.c

PROGRAMS = gcmp.exe gdiff.exe gdiff3.exe gsdiff.exe

all : $(PROGRAMS)
	@ WRITE SYS$OUTPUT "All made."

diff_o = analyze$(OBJ),cmpbuf$(OBJ),dir$(OBJ),freesoft$(OBJ),io$(OBJ),util$(OBJ),\
	alloca$(OBJ),context$(OBJ),ed$(OBJ),error$(OBJ),ifdef$(OBJ),normal$(OBJ),side$(OBJ),\
	fnmatch$(OBJ),getopt$(OBJ),getopt1$(OBJ),regex$(OBJ),version$(OBJ),xmalloc$(OBJ),\
	vmsputline$(OBJ),vmsutil$(OBJ)

diff3_o = error$(OBJ),freesoft$(OBJ),getopt$(OBJ),getopt1$(OBJ),util$(OBJ),\
	version$(OBJ),xmalloc$(OBJ),vmsutil$(OBJ)

sdiff_o = error$(OBJ),freesoft$(OBJ),getopt$(OBJ),getopt1$(OBJ),util$(OBJ),\
	version$(OBJ),xmalloc$(OBJ),vmsutil$(OBJ)

cmp_o = cmpbuf$(OBJ),error$(OBJ),freesoft$(OBJ),getopt$(OBJ),getopt1$(OBJ),\
	util$(OBJ),version$(OBJ),xmalloc$(OBJ),vmsutil$(OBJ)

difflib$(OLB) : difflib$(OLB)( $(diff_o) $(diff3_o) $(sdiff_o) $(cmp_o) )
	LIBRARY /COMPRESS $(MMS$TARGET) /OUTPUT=$(MMS$TARGET)

gdiff$(E) : difflib$(OLB) diff$(OBJ)
    LINK /EXECUTABLE=$(MMS$TARGET) $(LINKFLAGS) diff$(OBJ), difflib$(OLB)/LIBRARY

gdiff3$(E) : difflib$(OLB) diff3$(OBJ)
    LINK /EXECUTABLE=$(MMS$TARGET) $(LINKFLAGS) diff3$(OBJ), difflib$(OLB)/LIBRARY

gsdiff$(E) : difflib$(OLB) sdiff$(OBJ)
    LINK /EXECUTABLE=$(MMS$TARGET) $(LINKFLAGS) sdiff$(OBJ), difflib$(OLB)/LIBRARY

gcmp$(E) : difflib$(OLB) cmp$(OBJ)
    LINK /EXECUTABLE=$(MMS$TARGET) $(LINKFLAGS) cmp$(OBJ), difflib$(OLB)/LIBRARY

alloca$(OBJ) : $(SRCDIR)alloca.c $(SRCDIR)config.h
analyze$(OBJ) : $(SRCDIR)analyze.c $(SRCDIR)cmpbuf.h $(SRCDIR)diff.h \
	$(SRCDIR)system.h $(SRCDIR)config.h 
cmp$(OBJ) : $(SRCDIR)cmp.c $(SRCDIR)cmpbuf.h $(SRCDIR)getopt.h \
	$(SRCDIR)system.h $(SRCDIR)config.h
cmpbuf$(OBJ) : $(SRCDIR)cmpbuf.c $(SRCDIR)cmpbuf.h $(SRCDIR)diff.h \
	$(SRCDIR)system.h $(SRCDIR)config.h
context$(OBJ) : $(SRCDIR)context.c $(SRCDIR)diff.h $(SRCDIR)system.h \
	$(SRCDIR)config.h $(SRCDIR)regex.h
diff$(OBJ) : $(SRCDIR)diff.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h \
           $(SRCDIR)fnmatch.h $(SRCDIR)getopt.h $(SRCDIR)regex.h
diff3$(OBJ) : $(SRCDIR)diff3.c $(SRCDIR)getopt.h $(SRCDIR)system.h $(SRCDIR)config.h
dir$(OBJ) : $(SRCDIR)dir.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
ed$(OBJ) : $(SRCDIR)ed.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
error$(OBJ) : $(SRCDIR)error.c $(SRCDIR)config.h
fnmatch$(OBJ) : $(SRCDIR)fnmatch.c $(SRCDIR)fnmatch.h $(SRCDIR)diff.h \
	$(SRCDIR)system.h $(SRCDIR)config.h
freesoft$(OBJ) : $(SRCDIR)freesoft.c
getopt$(OBJ) : $(SRCDIR)getopt.c $(SRCDIR)getopt.h $(SRCDIR)diff.h \
	$(SRCDIR)system.h $(SRCDIR)config.h
getopt1$(OBJ) : $(SRCDIR)getopt1.c $(SRCDIR)getopt.h $(SRCDIR)diff.h \
	$(SRCDIR)system.h  $(SRCDIR)config.h
ifdef$(OBJ) : $(SRCDIR)ifdef.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
io$(OBJ) : $(SRCDIR)io.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
normal$(OBJ) : $(SRCDIR)normal.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
regex$(OBJ) : $(SRCDIR)regex.c $(SRCDIR)diff.h $(SRCDIR)system.h \
	$(SRCDIR)regex.h $(SRCDIR)config.h
sdiff$(OBJ) : $(SRCDIR)sdiff.c $(SRCDIR)getopt.h $(SRCDIR)system.h $(SRCDIR)config.h
side$(OBJ) : $(SRCDIR)side.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
util$(OBJ) : $(SRCDIR)util.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h
version$(OBJ) : version.c $(SRCDIR)diff.h $(SRCDIR)system.h $(SRCDIR)config.h

version.c : vmsutil.c
	@ OPEN /WRITE version_c $(MMS$TARGET)
	@ WRITE version_c "#include <config.h>"
	@ WRITE version_c "char const version_string[] = ""(GNU diffutils) $(VERSION) $(MMS$ARCH_NAME)" + -
			 " built on " + f$edit(f$extract(0,11,f$time()),"trim") + """;"
	@ CLOSE version_c

vmsputline$(OBJ) : vmsputline.c $(SRCDIR)config.h
vmsutil$(OBJ) : vmsutil.c $(SRCDIR)config.h
xmalloc$(OBJ) : $(SRCDIR)xmalloc.c $(SRCDIR)config.h

cmp$(OBJ) : $(SRCDIR)cmp.c
	$(COMPILE) /DEFINE=($(CDEFINE),"NULL_DEVICE"="""$(NULL_DEVICE)""") $(MMS$SOURCE)

diff3$(OBJ) : $(SRCDIR)diff3.c descrip.mms
	$(COMPILE) /DEFINE=($(CDEFINE),"DEFAULT_DIFF_PROGRAM"="""$(DEFAULT_DIFF_PROGRAM)""") $(MMS$SOURCE)

sdiff$(OBJ) : $(SRCDIR)sdiff.c descrip.mms
	$(COMPILE) /DEFINE=($(CDEFINE),"DEFAULT_EDITOR_PROGRAM"="""$(DEFAULT_EDITOR_PROGRAM)""","DEFAULT_DIFF_PROGRAM"="""$(DEFAULT_DIFF_PROGRAM)""") $(MMS$SOURCE)

util$(OBJ) : $(SRCDIR)util.c
	$(COMPILE) /DEFINE=($(CDEFINE),"PR_PROGRAM"="""$(PR_PROGRAM)""") $(MMS$SOURCE)

$(SRCDIR)config.h : config.h_vms
	COPY/LOG $(MMS$SOURCE) $(MMS$TARGET)

# We need more tests.
check : check_cmp check_diff check_diff3 check_sdiff

check_cmp :
	mcr []gcmp -c test1.tmp test2.tmp

check_diff :
	mcr []gdiff test2.tmp test3.tmp

check_diff3 :
	gdiff := $sys$disk:[]gdiff
	mcr []gdiff3 test1.tmp test2.tmp test3.tmp

check_sdiff :
	gdiff := $sys$disk:[]gdiff
	mcr []gsdiff.exe test1.tmp test3.tmp

mostlyclean :
	- DELETE *$(OBJ);*,*.lis;*,*.map;*/EXCLUDE=(cmp$(OBJ), diff3$(OBJ), sdiff$(OBJ), diff$(OBJ))

clean : mostlyclean
	- DELETE gdiff$(E);*, gdiff3$(E);*, gsdiff$(E);*, gcmp$(E);*

distclean : clean
        - PURGE
	- DELETE version.c;*,$(SRCDIR)config.h;*
