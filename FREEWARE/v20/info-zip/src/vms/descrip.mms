!==========================================================================
! MMS description file for UnZip/UnZipSFX 5.12                    26 Aug 94
!==========================================================================
!
! To build UnZip that uses shared libraries, edit the USER CUSTOMIZATION
! lines below to taste, then do
!	mms
! or
!	mmk
! if you use Matt's Make (free MMS-compatible make utility).
!
! (One-time users will find it easier to use the MAKE.COM command file,
! which generates both UnZip and UnZipSFX.  Just type "@[.VMS]MAKE", or
! "@[.VMS]MAKE GCC" if you want to use GNU C.)

! To build UnZip without shared libraries,
!	mms noshare

! To delete all .OBJ, .EXE and .HLP files,
!	mms clean

DO_THE_BUILD :
        @ decc = f$search("SYS$SYSTEM:DECC$COMPILER.EXE").nes.""
        @ axp = f$getsyi("HW_MODEL").ge.1024
        @ macro = ""
        @ if axp.or.decc then macro = "/MACRO=("
        @ if decc then macro = macro + "__DECC__=1,"
        @ if axp then macro = macro + "__ALPHA__=1,"
        @ if macro.nes."" then macro = f$extract(0,f$length(macro)-1,macro)+ ")"
        $(MMS)$(MMSQUALIFIERS)'macro' default

.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
OLB = .OLB
.ENDIF

!!!!!!!!!!!!!!!!!!!!!!!!!!! USER CUSTOMIZATION !!!!!!!!!!!!!!!!!!!!!!!!!!!!
! uncomment the following line if you want the VMS CLI$ interface:
!VMSCLI = VMSCLI,

! add VMSWILD, RETURN_CODES, RETURN_SEVERITY, and/or any other optional
! macros (except VMSCLI, above) to the following line for a custom version:
COMMON_DEFS =
!!!!!!!!!!!!!!!!!!!!!!!! END OF USER CUSTOMIZATION !!!!!!!!!!!!!!!!!!!!!!!!

CC = cc

.IFDEF __ALPHA__
CC_OPTIONS = /STANDARD=VAXC/ANSI/NOWARNINGS/INCLUDE=[]
CC_DEFS = MODERN,
OPTFILE =
OPTIONS =
.ELSE
.IFDEF __DECC__
CC_OPTIONS = /STANDARD=VAXC/ANSI/NOWARNINGS/INCLUDE=[]
CC_DEFS = MODERN,
.ELSE
CC_OPTIONS = /INCLUDE=[]
CC_DEFS =
.ENDIF
OPTFILE = ,[.vms]vmsshare.opt
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF

.IFDEF __DEBUG__
CDEB = /DEBUG/NOOPTIMIZE
LDEB = /DEBUG
.ELSE
CDEB =
LDEB = /NOTRACE
.ENDIF

CFLAGS_SFX  = $(CFLAGS) $(CC_OPTIONS) $(CDEB) -
              /def=($(CC_DEFS) $(COMMON_DEFS) $(VMSCLI) SFX, VMS)
CFLAGS      = $(CFLAGS) $(CC_OPTIONS) $(CDEB) -
              /def=($(CC_DEFS) $(COMMON_DEFS) $(VMSCLI) VMS)

LINKFLAGS   = $(LDEB)


COMMON_OBJS =	unzip$(OBJ),-
		crypt$(OBJ),-
		envargs$(OBJ),-
		explode$(OBJ),-
		extract$(OBJ),-
		file_io$(OBJ),-
		inflate$(OBJ),-
		match$(OBJ),-
		unreduce$(OBJ),-
		unshrink$(OBJ),-
		zipinfo$(OBJ),-
		VMS=[.vms]vms$(OBJ)

.IFDEF VMSCLI
OBJS =	$(COMMON_OBJS),-
	VMS_UNZIP_CLD=[.vms]unz_cld$(OBJ),-
	VMS_UNZIP_CMDLINE=[.vms]cmdline$(OBJ)
OBJX =	UNZIP=unzipsfx$(OBJ),-
	VMS_UNZIP_CLD=[.vms]unz_cld$(OBJ),-
	VMS_UNZIP_CMDLINE=cmdline_$(OBJ),-
	crypt$(OBJ),-
	EXTRACT=extract_$(OBJ),-
	file_io$(OBJ),-
	inflate$(OBJ),-
	match$(OBJ),-
	VMS=[.vms]vms_$(OBJ)
.ELSE
OBJS =	$(COMMON_OBJS)
OBJX =	UNZIP=unzipsfx$(OBJ),-
	crypt$(OBJ),-
	EXTRACT=extract_$(OBJ),-
	file_io$(OBJ),-
	inflate$(OBJ),-
	match$(OBJ),-
	VMS=[.vms]vms_$(OBJ)
.ENDIF

default	:	unzip$(EXE) unzipsfx$(EXE) unzip.hlp
	@	!	Do nothing.

unzip$(EXE) :	UNZIP$(OLB)($(OBJS))$(OPTFILE)
	$(LINK)$(LINKFLAGS) UNZIP$(OLB)/INCLUDE=UNZIP/LIBRARY$(OPTIONS), -
	[.vms]unzip.opt/OPT

unzipsfx$(EXE) :	UNZIPSFX$(OLB)($(OBJX))$(OPTFILE)
	$(LINK)$(LINKFLAGS) UNZIPSFX$(OLB)/INCLUDE=UNZIP/LIBRARY$(OPTIONS), -
	[.vms]unzipsfx.opt/OPT

noshare :	$(OBJS)
	$(LINK) /EXE=$(MMS$TARGET) $(OBJS),SYS$LIBRARY:VAXCRTL.OLB/LIB, -
	[.vms]unzip.opt/OPT

clean :
	! delete *.obj;*, *.olb;*, unzip$(exe);*, unzipsfx$(exe);*, -
	!  unzip.hlp;*, [.vms]*.obj;*, [.vms]unzip.rnh;*
	@[.vms]clean "$(OBJS)"
	@[.vms]clean "$(OBJX)"
	@[.vms]clean unzip$(olb),unzipsfx$(olb)
	@[.vms]clean unzip$(exe),unzipsfx$(exe)
	@[.vms]clean unzip.hlp,[.vms]unzip.rnh

crypt$(OBJ) 		: crypt.c unzip.h zip.h crypt.h
envargs$(OBJ)		: envargs.c unzip.h
explode$(OBJ)		: explode.c unzip.h
extract$(OBJ)		: extract.c unzip.h crypt.h
file_io$(OBJ)		: file_io.c unzip.h crypt.h tables.h
inflate$(OBJ)		: inflate.c inflate.h unzip.h
match$(OBJ)		: match.c unzip.h
unreduce$(OBJ)		: unreduce.c unzip.h
unshrink$(OBJ)		: unshrink.c unzip.h
unzip$(OBJ)		: unzip.c unzip.h crypt.h version.h
unzip.hlp		: [.vms]unzip.rnh
zipinfo$(OBJ)		: zipinfo.c unzip.h
[.vms]cmdline$(OBJ)	: [.vms]cmdline.c version.h
[.vms]unz_cld$(OBJ)	: [.vms]unz_cld.cld

cmdline_$(OBJ)		: [.vms]cmdline.c version.h
	$(CC) $(CFLAGS_SFX) /INCLUDE=SYS$DISK:[] /OBJ=$(MMS$TARGET) [.vms]cmdline.c

extract_$(OBJ)		: extract.c unzip.h crypt.h
	$(CC) $(CFLAGS_SFX) /OBJ=$(MMS$TARGET) extract.c

unzipsfx$(OBJ)		: unzip.c unzip.h crypt.h version.h
	$(CC) $(CFLAGS_SFX) /OBJ=$(MMS$TARGET) unzip.c

[.vms]vms$(OBJ)		: [.vms]vms.c [.vms]vms.h unzip.h
	@ x = ""
	@ if f$search("SYS$LIBRARY:SYS$LIB_C.TLB").nes."" then x = "+SYS$LIBRARY:SYS$LIB_C.TLB/LIBRARY"
	$(CC) $(CFLAGS) /INCLUDE=SYS$DISK:[] /OBJ=$(MMS$TARGET) [.vms]vms.c'x'

[.vms]vms_$(OBJ)	: [.vms]vms.c [.vms]vms.h unzip.h
	@ x = ""
	@ if f$search("SYS$LIBRARY:SYS$LIB_C.TLB").nes."" then x = "+SYS$LIBRARY:SYS$LIB_C.TLB/LIBRARY"
	$(CC) $(CFLAGS_SFX) /INCLUDE=SYS$DISK:[] /OBJ=$(MMS$TARGET) [.vms]vms.c'x'


.IFDEF VMSCLI

[.vms]unzip.rnh 	: [.vms]unzip_cli.help
	@ set default [.vms]
 	edit/tpu/nosection/nodisplay/command=cvthelp.tpu unzip_cli.help
	rename unzip_cli.rnh unzip.rnh
	@ set default [-]

.ELSE

[.vms]unzip.rnh 	: [.vms]unzip_def.rnh
	copy [.vms]unzip_def.rnh [.vms]unzip.rnh

.ENDIF
