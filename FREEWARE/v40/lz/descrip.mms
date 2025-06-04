.ifdef VAXC
cflags		= /nolist/obj=$(mms$target)
adlib		= ,sys$library:vaxcrtl/lib
.else
cflags		= /decc/standard=vaxc/nolist/obj=$(mms$target)
adlib		= ,sys$library:deccrtl/lib/inc=cma$tis
.endif

linkflags	= /trace/nomap/exec=$(mms$target)

lzcomp_objs	= lzcmp1.obj lzcmp2.obj lzcmp3.obj lzcomp.obj

lzdcmp_objs	= lzdcm1.obj lzdcm2.obj lzdcm3.obj lzdcmp.obj

common_objs	= lzio.obj lzvio.obj lzdcl.obj

lz		: lzcomp.exe lzdcmp.exe lz.hlb lz.com
	@ ! do nothing here

list		: lz.lst
	linepr lz.h *.cld lz*.c

#
# You cannot build the full kit without access to Decus C tools,
# (getrno and scopy), which are not included in the lz distribution.
#
full_kit	: lzcomp.exe lzdcmp.exe lz.hlb lz.com lzcomp.mem lzdcmp.mem
	@ ! do nothing here

#
# Build and test lz
#
triptest	: lzcomp.exe lzdcmp.exe triptest.com
	@ triptest

#
# Note: the link continuation lines are preceeded by two <tab>'s
# which are recognized by fixmms.com.  lz.com is in the dependency
# list so the programs are re-linked when descrip.mms changes.
#
lzcomp.exe	: $(lzcomp_objs) $(common_objs) lz.com
	link $(linkflags) -
		lzcmp1.obj,lzcmp2.obj,lzcmp3.obj,lzcomp.obj, -
		lzio.obj,lzvio.obj,lzdcl.obj$(adlib)

lzdcmp.exe	: $(lzdcmp_objs) $(common_objs) lz.com
	link $(linkflags) -
		lzdcm1.obj,lzdcm2.obj,lzdcm3.obj,lzdcmp.obj, -
		lzio.obj,lzvio.obj,lzdcl.obj$(adlib)

lz.com		: descrip.mms fixmms.com
	mms/noaction/from_sources/output=lz.tmp
	@fixmms lz.tmp lz.com
	delete lz.tmp;*

#
# Documentation is created using Decus C tools.  These are stored
# in a directory defined by the BIN: logical:
#	getrno		reads .C source, producing .RNO
#	scopy		fixes file attributes.
#
lzcomp.mem	: lzcomp.rno
	scopy   :== $bin:scopy
	runoff lzcomp
	scopy lzcomp.mem

lzcomp.rno	: lzcmp1.c
	getrno :== $bin:getrno
	getrno lzcmp1.c -o lzcomp.rno

lzdcmp.mem	: lzdcmp.rno
	scopy   :== $bin:scopy
	runoff lzdcmp
	scopy lzdcmp.mem

lzdcmp.rno	: lzdcm1.c
	getrno :== $bin:getrno
	getrno lzdcm1.c -o lzdcmp.rno

#
# The archive creator uses the Decus C text archive programs.
# I store these in a tools: directory.  If you change this,
# be sure to edit makefile.txt (Unix make command file)
#

archive		: lzarch.arc lz1.arc lz2.arc lz3.arc lz4.arc
	copy nl: nl:

lzarch_arc	= 1streadme.txt tools:archx.c tools:archc.c
lz1a_arc	= readme.txt descrip.mms makefile.txt lz.com fixmms.com
lz1b_arc	= lzcomp.mem lzdcmp.mem
lz2_arc		= lzcmp1.c lzcmp2.c lzcmp3.c
lz3_arc		= lzdcm1.c lzdcm2.c lzdcm3.c lz.hlp
lz4_arc		= lz.h lzcomp.cld lzdcmp.cld lzdcl.c lzio.c lzvio.c

lzarch.arc	: $(lzarch_arc)
	archc :== $bin:archc
	archc $(lzarch_arc) >lzarch.arc

lz1.arc		: $(lz1a_arc) $(lz1b_arc)
	archc :== $bin:archc
	archc $(lz1a_arc) >lz1.arc
	archc $(lz1b_arc) >>lz1.arc

lz2.arc		: $(lz2_arc)
	archc :== $bin:archc
	archc $(lz2_arc) >lz2.arc

lz3.arc		: $(lz3_arc)
	archc :== $bin:archc
	archc $(lz3_arc) >lz3.arc

lz4.arc		: $(lz4_arc)
	archc :== $bin:archc
	archc $(lz4_arc) >lz4.arc

#
# Object module dependencies
#

lzcmp1.obj	: lz.h lzcmp1.c

lzcmp2.obj	: lz.h lzcmp2.c

lzcmp3.obj	: lz.h lzcmp3.c

lzdcm1.obj	: lz.h lzdcm1.c

lzdcm2.obj	: lz.h lzdcm2.c

lzdcm3.obj	: lz.h lzdcm3.c

lzio.obj	: lz.h lzio.c

lzvio.obj	: lz.h lzvio.c

lzdcl.obj	: lz.h lzdcl.c

lzcomp.obj	: lzcomp.cld

lzdcmp.obj	: lzdcmp.cld

lz.hlb		: lz.hlp
