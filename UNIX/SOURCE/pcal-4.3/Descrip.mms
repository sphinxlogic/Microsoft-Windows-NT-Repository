#   This is a descripion file for the VAX/VMS utility MMS to make PCAL.

# Set the configuration variables below to taste.

CFLAGS	= $(CFLAGS)
LFLAGS	= $(LINKFLAGS)
COPTS	= VAXCRTL.OPT /Option
OBJECTS = exprpars.obj moonphas.obj pcalutil.obj readfile.obj writefil.obj
OBJLIST = exprpars,moonphas,pcalutil,readfile,writefil

WSO =	@ Write Sys$Output

.suffixes :	.exe .obj .dvi .tex

#########################################################################
#
#	User-defined directives...
#
#########################################################################

#    This DOC section is VERY system dependent.  I use FERMILab's SETUP utility
#    to initialize products from 3rd party vendors, etc. and only have to
#    say SETUP TeX to get all the DECUS TeX distribution configured.  If
#    you are using the new version of TeX for VMS with LaTeX configured as
#    a verb, just use it as I have it below. Otherwise, you may have to
#    do this section by hand. :(
.tex.dvi :
	- Setup TeX
	- LaTeX /Batch /DVI_file = $*.dvi $*.tex

.obj.exe :
	$(LINK) $(LFLAGS) $*.obj,$(COPTS)

#########################################################################
#
#	Targets...
#
#########################################################################
DEFAULT	:
	$(WSO) "You must specify which target to make. Valid targets are:"
	$(WSO) " "
	$(WSO) "Programs:"
	$(WSO) " "
	$(WSO) "      ALL             - make pcal executable, manual, and"
	$(WSO) "                        clean up the directory"
	$(WSO) "      pcal            - make pcal executables"
	$(WSO) "      pcalinit        - make just pcalinit.h file"
	$(WSO) " "
	$(WSO) "Documentation: "
	$(WSO) " "
	$(WSO) "      HELP            - make VMS HELP library"
	$(WSO) "      DOC             - make TeX manual for pcal"
	$(WSO) " "
	$(WSO) "Maintanence: "
	$(WSO) " "
	$(WSO) "      BUILD_PROC      - use MMS to create a VMS command procedure"
	$(WSO) "                        to build the pcal package without MMS"
	$(WSO) "      CLEAN           - purge all files and delete all object files"

ALL :	pcalinit pcal help doc clean
	@ continue

pcalinit :	pcalinit.exe
	@ continue

pcal :		pcal.exe
	@ continue

HELP :		pcal.hlb
	@ continue

DOC :		pcal.dvi
	@ continue

CLEAN :
	@- Set Protection = Owner:RWED *.*;-1
	- Purge /NoLog /NoConfirm
	@- Set Protection = Owner:RWED *.obj;*,*.lis;*,*.aux;*
	- Delete /NoLog /NoConfirm *.obj;*,*.lis;*,*.aux;*
	@- Set Protection = (System:RWE, Owner:RWE) *.*

BUILD_PROC :
	$(mms) $(mmsqualifiers) /NoAction /From_Sources /Output = BUILD_PCAL.COM ALL

#########################################################################
#
#	Extended target dependencies...
#
#########################################################################
pcal.exe :	pcalinit.h pcal.obj $(OBJECTS)
	$(LINK) $(LFLAGS) pcal,$(OBJLIST),$(COPTS)
	@ Set Protection = (System:RWE, Owner:RWE, Group:RE, World:RE) *.*

pcalinit.h :	pcalinit.exe pcalinit.ps
	MCR Sys$Disk:[]pcalinit pcalinit.ps pcalinit.h

pcal.hlb :	pcal.hlp
	Library /Create /Help PCAL.HLB PCAL.HLP
	@ Set Protection = (System:RWE, Owner:RWE, Group:RE, World:RE) PCAL.HLB
	@ Set File /Truncate PCAL.HLB

#########################################################################
#
#	Simple target dependencies...
#
#########################################################################
exprpars.exe :	exprpars.c pcaldefs.h
moonphas.exe :	moonphas.c pcaldefs.h pcalglob.h
pcal.exe :	pcal.c pcaldefs.h pcalglob.h pcallang.h
pcalutil.exe :	pcalutil.c pcaldefs.h pcalglob.h pcallang.h
readfile.exe :	readfile.c pcaldefs.h pcalglob.h pcallang.h
writefil.exe :	writefil.c pcaldefs.h pcalglob.h pcallang.h pcalinit.h
pcalinit.exe :	pcalinit.c
pcal.dvi :	pcal.tex troffman.sty
