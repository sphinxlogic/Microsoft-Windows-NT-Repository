#    Copyright (C) 1997, 2000 Aladdin Enterprises. All rights reserved.
# 
# This file is part of Aladdin Ghostscript.
# 
# Aladdin Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author
# or distributor accepts any responsibility for the consequences of using it,
# or for whether it serves any particular purpose or works at all, unless he
# or she says so in writing.  Refer to the Aladdin Ghostscript Free Public
# License (the "License") for full details.
# 
# Every copy of Aladdin Ghostscript must include a copy of the License,
# normally in a plain ASCII text file named PUBLIC.  The License grants you
# the right to copy, modify and redistribute Aladdin Ghostscript, but only
# under certain conditions described in the License.  Among other things, the
# License requires that the copyright notice and this notice be preserved on
# all copies.

# $Id: openvms.mmk,v 1.2 2000/03/10 08:02:59 lpd Exp $
# makefile for OpenVMS VAX and Alpha using MMK
#
# Please contact Jim Dunham (dunham@omtool.com) if you have questions.
# Addapted for MMK by Jouk Jansen (joukj@crys.chem.uva.nl)
# Support for VAX C on OpenVMS was removed in release 6.01 by Aladdin:
# DEC C is now used on both VAX and Alpha platforms.
#
# ------------------------------- Options ------------------------------- #

###### This section is the only part of the file you should need to edit.

# on the make command line specify:
#	mmk/descrip=[.src]openvms.mmk/macro=("DECWINDOWS1_2={0,1}")

# Define the directory for the final executable, and the
# source, generated intermediate file, and object directories
# for the graphics library (GL) and the PostScript/PDF interpreter (PS).

BINDIR=[.bin]
GLSRCDIR=[.src]
GLGENDIR=[.obj]
GLOBJDIR=[.obj]
PSSRCDIR=[.src]
PSGENDIR=[.obj]
PSOBJDIR=[.obj]
# Because of OpenVMS syntactic problems, the following redundant definitions
# are necessary.  If you are using more than one GENDIR and/or OBJDIR,
# you will have to edit the code below that creates these directories.
BIN_DIR=BIN.DIR
OBJ_DIR=OBJ.DIR

# create directories
.first
	if f$search("$(BIN_DIR)") .eqs. "" then create/directory/log $(BINDIR)
	if f$search("$(OBJ_DIR)") .eqs. "" then create/directory/log $(GLOBJDIR)

# Do not edit the next group of lines.

#.include $(COMMONDIR)vmscdefs.mak
#.include $(COMMONDIR)vmsdefs.mak
#.include $(COMMONDIR)generic.mak
.include $(GLSRCDIR)version.mak
DD=$(GLGENDIR)
GLD=$(GLGENDIR)
PSD=$(PSGENDIR)

# ------ Generic options ------ #

# Define the directory that will hold documentation at runtime.

GS_DOCDIR=GS_DOC
#GS_DOCDIR=SYS$COMMON:[GS]

# Define the default directory/ies for the runtime
# initialization and font files.  Separate multiple directories with ,.

GS_LIB_DEFAULT=GS_LIB
#GS_LIB_DEFAULT=SYS$COMMON:[GS],SYS$COMMON:[GS.FONT]

# Define whether or not searching for initialization files should always
# look in the current directory first.  This leads to well-known security
# and confusion problems, but users insist on it.
# NOTE: this also affects searching for files named on the command line:
# see the "File searching" section of Use.htm for full details.
# Because of this, setting SEARCH_HERE_FIRST to 0 is not recommended.

SEARCH_HERE_FIRST=1

# Define the name of the interpreter initialization file.
# (There is no reason to change this.)

GS_INIT=GS_INIT.PS

# Choose generic configuration options.

# Setting DEBUG=1 includes debugging features in the code

DEBUG=

# Setting TDEBUG=1 includes symbol table information for the debugger,
# and also enables stack tracing on failure.

TDEBUG=

# Setting CDEBUG=1 enables 'C' compiler debugging and turns off optimization
# Code is substantially slower and larger.

CDEBUG=

# Define the name of the executable file.

GS=GS

# Define the name of a pre-built executable that can be invoked at build
# time.  Currently, this is only needed for compiled fonts.  The usual
# alternatives are:
#   - the standard name of Ghostscript on your system (typically `gs'):
BUILD_TIME_GS=GS
#   - the name of the executable you are building now.  If you choose this
# option, then you must build the executable first without compiled fonts,
# and then again with compiled fonts.
#BUILD_TIME_GS=$(BINDIR)$(GS) -I$(PSLIBDIR)

# Define the directory where the IJG JPEG library sources are stored,
# and the major version of the library that is stored there.
# You may need to change this if the IJG library version changes.
# See jpeg.mak for more information.

JSRCDIR=[.jpeg-6b]
JVERSION=6

# Define the directory where the PNG library sources are stored,
# and the version of the library that is stored there.
# You may need to change this if the libpng version changes.
# See libpng.mak for more information.

PSRCDIR=[.libpng-1_0_5]
PVERSION=10005

# Define the directory where the zlib sources are stored.
# See zlib.mak for more information.

ZSRCDIR=[.zlib-1_1_3]

# Note that built-in third-party libraries aren't available.

SHARE_JPEG=0
SHARE_LIBPNG=0
SHARE_ZLIB=0

# Define the path to X11 include files

X_INCLUDE=DECW$INCLUDE

# ------ Platform-specific options ------ #

# Define the drive, directory, and compiler name for the 'C' compiler.
# COMP is the full compiler path name.

.ifdef DEBUG
SW_DEBUG=/DEBUG/NOOPTIMIZE
.else
SW_DEBUG=/NODEBUG/OPTIMIZE
.endif

SW_PLATFORM=/DECC/PREFIX=ALL/NESTED_INCLUDE=PRIMARY

# Define any other compilation flags. 
# Including defines for A4 paper size

.ifdef A4_PAPER
SW_PAPER=/DEFINE=("A4")
.else
SW_PAPER=
.endif

COMP=CC$(SW_DEBUG)$(SW_PLATFORM)$(SW_PAPER)

# LINK is the full linker path name

.ifdef TDEBUG
LINKER=LINK/DEBUG/TRACEBACK
.else
LINKER=LINK/NODEBUG/NOTRACEBACK
.endif

# INCDIR contains the include files
INCDIR=

# LIBDIR contains the library files
LIBDIR=

# Define the .dev module that implements thread and synchronization
# primitives for this platform.  Don't change this unless you really know
# what you're doing.

SYNC=posync

# ------ Devices and features ------ #

# Choose the device(s) to include.  See devs.mak for details,
# devs.mak and contrib.mak for the list of available devices.

DEVICE_DEVS=$(DD)x11.dev $(DD)x11alpha.dev $(DD)x11cmyk.dev $(DD)x11gray2.dev $(DD)x11gray4.dev $(DD)x11mono.dev
DEVICE_DEVS1=
DEVICE_DEVS2=
DEVICE_DEVS3=$(DD)deskjet.dev $(DD)djet500.dev $(DD)laserjet.dev $(DD)ljetplus.dev $(DD)ljet2p.dev $(DD)ljet3.dev $(DD)ljet3d.dev $(DD)ljet4.dev $(DD)ljet4d.dev
DEVICE_DEVS4=$(DD)cdeskjet.dev $(DD)cdjcolor.dev $(DD)cdjmono.dev $(DD)cdj550.dev $(DD)pj.dev $(DD)pjxl.dev $(DD)pjxl300.dev
DEVICE_DEVS5=$(DD)uniprint.dev
DEVICE_DEVS6=$(DD)bj10e.dev $(DD)bj200.dev $(DD)bjc600.dev $(DD)bjc800.dev
DEVICE_DEVS7=$(DD)faxg3.dev $(DD)faxg32d.dev $(DD)faxg4.dev
DEVICE_DEVS8=$(DD)pcxmono.dev $(DD)pcxgray.dev $(DD)pcx16.dev $(DD)pcx256.dev $(DD)pcx24b.dev $(DD)pcxcmyk.dev
DEVICE_DEVS9=$(DD)pbm.dev $(DD)pbmraw.dev $(DD)pgm.dev $(DD)pgmraw.dev $(DD)pgnm.dev $(DD)pgnmraw.dev
DEVICE_DEVS10=$(DD)tiffcrle.dev $(DD)tiffg3.dev $(DD)tiffg32d.dev $(DD)tiffg4.dev $(DD)tifflzw.dev $(DD)tiffpack.dev
DEVICE_DEVS11=$(DD)tiff12nc.dev $(DD)tiff24nc.dev
DEVICE_DEVS12=$(DD)psmono.dev $(DD)psgray.dev $(DD)psrgb.dev $(DD)bit.dev $(DD)bitrgb.dev $(DD)bitcmyk.dev
DEVICE_DEVS13=$(DD)pngmono.dev $(DD)pnggray.dev $(DD)png16.dev $(DD)png256.dev $(DD)png16m.dev
DEVICE_DEVS14=$(DD)jpeg.dev $(DD)jpeggray.dev
DEVICE_DEVS15=$(DD)pdfwrite.dev $(DD)pswrite.dev $(DD)epswrite.dev $(DD)pxlmono.dev $(DD)pxlcolor.dev
# Overflow from DEVS9
DEVICE_DEVS16=$(DD)pnm.dev $(DD)pnmraw.dev $(DD)ppm.dev $(DD)ppmraw.dev $(DD)pkm.dev $(DD)pkmraw.dev $(DD)pksm.dev $(DD)pksmraw.dev
DEVICE_DEVS17=
DEVICE_DEVS18=
DEVICE_DEVS19=
DEVICE_DEVS20=

# Choose the language feature(s) to include.  See gs.mak for details.

FEATURE_DEVS=$(PSD)psl3.dev $(PSD)pdf.dev

# Choose whether to compile the .ps initialization files into the executable.
# See gs.mak for details.

COMPILE_INITS=0

# Choose whether to store band lists on files or in memory.
# The choices are 'file' or 'memory'.

BAND_LIST_STORAGE=file

# Choose which compression method to use when storing band lists in memory.
# The choices are 'lzw' or 'zlib'.  lzw is not recommended, because the
# LZW-compatible code in Ghostscript doesn't actually compress its input.

BAND_LIST_COMPRESSOR=zlib

# Choose the implementation of file I/O: 'stdio', 'fd', or 'both'.
# See gs.mak and sfxfd.c for more details.

FILE_IMPLEMENTATION=stdio

# Define the name table capacity size of 2^(16+n).

EXTEND_NAMES=0

# Define whether the system constants are writable.

SYSTEM_CONSTANTS_ARE_WRITABLE=0

# Define the platform name.

PLATFORM=openvms_

# Define the name of the makefile -- used in dependencies.

MAKEFILE=$(GLSRCDIR)openvms.mmk
TOP_MAKEFILES=$(MAKEFILE)

# Define the platform options

PLATOPT=

# Patch a couple of PC-specific things that aren't relevant to OpenVMS builds,
# but that cause `make' to produce warnings.

PCFBASM=

# It is very unlikely that anyone would want to edit the remaining
#   symbols, but we describe them here for completeness:

# Define the suffix for command files (e.g., null or .bat).

CMD=

# Define the directory separator character (\ for MS-DOS, / for Unix,
# nothing for OpenVMS).

D=

# Define the brackets for passing preprocessor definitions to the C compiler.

D_=/DEFINE="
_D_=$(NULL)=
_D="

# Define the syntax of search paths for the C compiler.
# The OpenVMS compilers uses /INCLUDE=(dir1, dir2, ...dirn),
# and only a single /INCLUDE switch is allowed in the command line.

I_=/INCLUDE=(
II=,
_I=)

# Define the string for specifying the output file from the C compiler.

O_=/OBJECT=

# Define the quoting string for mixed-case arguments.
# (OpenVMS is the only platform where this isn't an empty string.)

Q="

# Define the extension for executable files (e.g., null or .exe).

XE=.exe

# Define the extension for executable files for the auxiliary programs
# (e.g., null or .exe).

XEAUX=.exe

# Define the list of files that `make clean' removes.

BEGINFILES=$(GLSRCDIR)OPENVMS.OPT $(GLSRCDIR)OPENVMS.COM

# Define the C invocation for the ansi2knr program.  We don't use this.

CCA2K=

# Define the C invocation for auxiliary programs (echogs, genarch).
# We don't need to define this separately.

CCAUX=

# Define the C invocation for normal compilation.

CC=$(COMP)

# Define the Link invocation.

LINK=$(LINKER)/MAP/EXE=$@ $+,$(GLSRCDIR)OPENVMS.OPT/OPTION

# Define the ANSI-to-K&R dependency.  We don't need this.

AK=

# Define the syntax for command, object, and executable files.

OBJ=obj

# Define the prefix for image invocations.

NULL=
EXP=MCR $(NULL)

# Define the prefix for shell invocations.

SH=

# Define generic commands.

CP_=@$(GLSRCDIR)COPY_ONE

# Define the command for deleting (a) file(s) (including wild cards)

RM_=@$(GLSRCDIR)RM_ONE

# Define the command for deleting multiple files / patterns.

RMN_=@$(GLSRCDIR)RM_ALL

# Define the arguments for genconf.

CONFILES=-p %s
CONFLDTR=-o

# Define the generic compilation rules.

.suffixes : .c .obj .exe

.obj.exe :
	$(LINK)

# ---------------------------- End of options ---------------------------- #

# Define various incantations of the 'c' compiler.

CC_=$(COMP)
CC_INT=$(CC_)
CC_LEAF=$(CC_)
CC_NO_WARN=$(CC_)

# ------------------- Include the generic makefiles ---------------------- #

all : macro [.lib]Fontmap. $(GS_XE)

#.include $(COMMONDIR)/ansidefs.mak
#.include $(COMMONDIR)/vmsdefs.mak
#.include $(COMMONDIR)/generic.mak
.include $(GLSRCDIR)gs.mak
.include $(GLSRCDIR)lib.mak
.include $(PSSRCDIR)int.mak
.include $(PSSRCDIR)cfonts.mak
.include $(GLSRCDIR)jpeg.mak
# zlib.mak must precede libpng.mak
.include $(GLSRCDIR)zlib.mak
.include $(GLSRCDIR)libpng.mak
.include $(GLSRCDIR)devs.mak
.include $(GLSRCDIR)contrib.mak


# ----------------------------- Main program ------------------------------ 

macro :
.ifdef A4_PAPER
	@ a4p = 1
.else
	@ a4p = 0
.endif
	@ decc = f$search("SYS$SYSTEM:DECC$COMPILER.EXE").nes.""
	@ decw12 = f$search("SYS$SHARE:DECW$XTLIBSHRR5.EXE").nes.""
	@ macro = ""
	@ if a4p.or.decc.or.decw12 then macro = "/MACRO=("
	@ if decw12 then macro = macro + "DECWINDOWS1_2=1,"
	@ if a4p then macro = macro + "A4_PAPER=1,"
	@ if macro.nes."" then macro = f$extract(0,f$length(macro)-1,macro)+ ")"
	$(MMS)$(MMSQUALIFIERS)'macro' $(GS_XE)

$(GS_XE) : openvms $(GLGEN)arch.h $(GLOBJDIR)gs.$(OBJ) $(INT_ALL) $(LIB_ALL)
	$(LINKER)/MAP/EXE=$@ $(GLOBJDIR)gs.$(OBJ),$(ld_tr)/OPTIONS,$(GLSRCDIR)OPENVMS.OPT/OPTION

# OpenVMS.dev

openvms__=$(GLOBJ)gp_getnv.$(OBJ) $(GLOBJ)gp_vms.$(OBJ) $(GLOBJ)gp_nofb.$(OBJ)
$(GLGEN)openvms_.dev : $(openvms__) $(GLGEN)nosync.dev
	$(SETMOD) $(GLGEN)openvms_ $(openvms__) -include $(GLGEN)nosync

$(ECHOGS_XE) :  $(GLOBJDIR)echogs.$(OBJ) 
$(GENARCH_XE) : $(GLOBJDIR)genarch.$(OBJ)
$(GENCONF_XE) : $(GLOBJDIR)genconf.$(OBJ)
$(GENDEV_XE) : $(GLOBJDIR)gendev.$(OBJ)
$(GENHT_XE) : $(GLOBJDIR)genht.$(OBJ)
$(GENINIT_XE) : $(GLOBJDIR)geninit.$(OBJ)

$(GLOBJDIR)echogs.$(OBJ) : $(GLSRCDIR)echogs.c
$(GLOBJDIR)genarch.$(OBJ) : $(GLSRCDIR)genarch.c $(GENARCH_DEPS)
$(GLOBJDIR)genconf.$(OBJ) : $(GLSRCDIR)genconf.c $(GENCONF_DEPS)
$(GLOBJDIR)gendev.$(OBJ) : $(GLSRCDIR)gendev.c $(GENDEV_DEPS)
# ****** NEED TO ADD $(GENHT_CFLAGS) HERE ******
$(GLOBJDIR)genht.$(OBJ) : $(GLSRCDIR)genht.c $(GENHT_DEPS)
$(GLOBJDIR)geninit.$(OBJ) : $(GLSRCDIR)geninit.c $(GENINIT_DEPS)

$(GLOBJ)gp_vms.$(OBJ) : $(GLSRC)gp_vms.c
	$(CC_)/include=($(GLGENDIR),$(GLSRCDIR))/obj=$(GLOBJ)gp_vms.$(OBJ) $(GLSRC)gp_vms.c

# Preliminary definitions

openvms : $(GLSRCDIR)openvms.com $(GLSRCDIR)openvms.opt
	@$(GLSRCDIR)OPENVMS

$(GLSRCDIR)openvms.com : $(GLSRCDIR)append_l.com
	@$(GLSRCDIR)APPEND_L $@ "$ DEFINE/JOB X11 $(X_INCLUDE)"
	@$(GLSRCDIR)APPEND_L $@ "$ DEFINE/JOB GS_LIB ''F$ENVIRONMENT(""DEFAULT"")'"
	@$(GLSRCDIR)APPEND_L $@ "$ DEFINE/JOB GS_DOC ''F$ENVIRONMENT(""DEFAULT"")'"
	@$(GLSRCDIR)APPEND_L $@ "$ DEFINE/JOB DECC$USER_INCLUDE ''F$ENVIRONMENT(""DEFAULT"")', DECW$INCLUDE, DECC$LIBRARY_INCLUDE, SYS$LIBRARY"
	@$(GLSRCDIR)APPEND_L $@ "$ DEFINE/JOB DECC$SYSTEM_INCLUDE ''F$ENVIRONMENT(""DEFAULT"")', DECW$INCLUDE, DECC$LIBRARY_INCLUDE, SYS$LIBRARY"
	@$(GLSRCDIR)APPEND_L $@ "$ DEFINE/JOB SYS "DECC$LIBRARY_INCLUDE,SYS$LIBRARY"

$(GLSRCDIR)openvms.opt :
.ifdef DECWINDOWS1_2
	@$(GLSRCDIR)APPEND_L $@ "SYS$SHARE:DECW$XMLIBSHR12.EXE/SHARE"
	@$(GLSRCDIR)APPEND_L $@ "SYS$SHARE:DECW$XTLIBSHRR5.EXE/SHARE"
	@$(GLSRCDIR)APPEND_L $@ "SYS$SHARE:DECW$XLIBSHR.EXE/SHARE"
.else
	@$(GLSRCDIR)APPEND_L $@ "SYS$SHARE:DECW$XMLIBSHR.EXE/SHARE"
	@$(GLSRCDIR)APPEND_L $@ "SYS$SHARE:DECW$XTSHR.EXE/SHARE"
	@$(GLSRCDIR)APPEND_L $@ "SYS$SHARE:DECW$XLIBSHR.EXE/SHARE"
.endif
	@$(GLSRCDIR)APPEND_L $@ ""Ident="""""GS $(GS_DOT_VERSION)"""""

# The platform-specific makefiles must also include rules for creating
# certain dynamically generated files:
#	gconfig_.h - this indicates the presence or absence of
#	    certain system header files that are located in different
#	    places on different systems.  (It could be generated by
#	    the GNU `configure' program.)
#	gconfigv.h - this indicates the status of certain machine-
#	    and configuration-specific features derived from definitions
#	    in the platform-specific makefile.

$(gconfig__h) : $(TOP_MAKEFILES) $(ECHOGS_XE)
	$(EXP)$(ECHOGS_XE) -w $(gconfig__h) -x 23 define "HAVE_SYS_TIME_H"

$(gconfigv_h) : $(TOP_MAKEFILES) $(ECHOGS_XE)
	$(EXP)$(ECHOGS_XE) -w $(gconfigv_h) -x 23 define "USE_ASM" 0
	$(EXP)$(ECHOGS_XE) -a $(gconfigv_h) -x 23 define "USE_FPU" 1
	$(EXP)$(ECHOGS_XE) -a $(gconfigv_h) -x 23 define "EXTEND_NAMES" 0$(EXTEND_NAMES)
	$(EXP)$(ECHOGS_XE) -a $(gconfigv_h) -x 23 define "SYSTEM_CONSTANTS_ARE_WRITABLE" 0$(SYSTEM_CONSTANTS_ARE_WRITABLE)
