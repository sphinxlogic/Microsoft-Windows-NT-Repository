.ifdef __AXP__
ARCH    = AXP
.else
ARCH    = VAX
.endif

.ifdef GNUC
CC      = GCC
CFLAGS  =
POST    = _GNU
OPT     = GNUC_$(ARCH).OPT
LINKFLAGS= ,[]$(OPT)/OPT
.else
CC      = CC
CFLAGS  =
POST    =
OPT     =
LINKFLAGS=
.endif

OBJDIR  = [.OBJ]
CFLAGS  = $(XCFLAGS)/OPTIMIZE
EXE     = EXE_$(ARCH)$(POST)
OBJ     = OBJ_$(ARCH)$(POST)

! Do not use this, for maintainance only
.ifdef DEBUG
.ifdef __AXP__
CFLAGS  = $(CFLAGS)/PREFIX=ALL/LIS/DEFINE=DEBUG/CHECK=(POINTER_SIZE=ALL,UNINITIALIZED_VARIABLES)
.else
.endif
CFLAGS  = $(CFLAGS)/DEFINE=DEBUG
.else
CFLAGS  = $(CFLAGS)
LINKFLAGS= $(LINKFLAGS)/NOTRACE
.endif


OBJECTS = $(OBJDIR)CHKSUM.$(OBJ), $(OBJDIR)MD5.$(OBJ)




ALL : CHKSUM.$(EXE)
	@ write sys$output "CHKSUM.$(EXE) is ready"

CHKSUM.$(EXE) : $(OBJECTS)
	LINK/EXEC=$(MMS$TARGET) $(OBJECTS) $(LINKFLAGS)

$(OBJDIR)CHKSUM.$(OBJ) : chksum.c chksum.h CHKSUM_HELP.C 
	$(CC) $(CFLAGS) /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

$(OBJDIR)MD5.$(OBJ) : md5.c md5.h
	$(CC) $(CFLAGS)/DEFINE=STDC_HEADERS /OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

CHKSUM_HELP.C : CHKSUM_HELP.TXT
	TEXT2C CHKSUM_HELP.TXT CHKSUM_HELP.C 

! Clean distribution
clean :
	@ if f$search("$(OBJDIR)*.$(OBJ)").nes."" then DELETE/NOLOG/NOCONF $(OBJDIR)*.$(OBJ);*
	@ if f$search("*.$(EXE)").nes."" then DELETE/NOLOG/NOCONF *.$(EXE);*

cleanall : 
	@ if f$search("$(OBJDIR)*.OBJ*").nes."" then DELETE/NOLOG/NOCONF $(OBJDIR)*.OBJ*;*
	@ if f$search("*.EXE*").nes."" then DELETE/NOLOG/NOCONF *.EXE*;*



! For maintainance only. Do not execute if you do not have
! perl, vpipe and sed on your site or if you are missing the perl 
! conversion programs.
!

DONE : HELP CHECK
	@ delete/nolog/noconf *.exe*gnu*;*
	@ write sys$output "Ready to be packed ..."

VMS_BIN.ZIP : 
	ZIP/MOVE/VMS VMS_BIN.ZIP *.EXE*,[.OBJ]*.obj*

HELP : CHKSUM.MAN CHKSUM.HLP CHKSUM.HTML
	@ write sys$output "Help file and Manpage ready"

CHKSUM.MAN : CHKSUM.POD
	pod2man CHKSUM.POD >CHKSUM.MAN 

CHKSUM.HLP : CHKSUM.POD
	pod2hlp CHKSUM.POD 

CHKSUM.HTML : CHKSUM.POD
	pod2html CHKSUM.POD >CHKSUM.HTML
	if f$search("POD2HTML-DIRCACHE.;").nes."" then delete/nolog/noconf POD2HTML-*.;*
CHECK : 
	vpipe mcr []chksum.$(EXE) -m  chksum.c chksum.h chksum_help.c md5.c md5.h *.exe* chksum >checksum.tmp
	sed "s/ .*\]/ /" checksum.tmp >checksum.txt
	delete/nolog/noconf checksum.tmp;*
	@ write sys$output "checksum.txt file ready"

