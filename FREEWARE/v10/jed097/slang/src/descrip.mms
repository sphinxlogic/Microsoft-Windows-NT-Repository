#
#  MMS file to build S-Lang (Thanks to Hunter Goatley)
#
.IFDEF __MATTS_MMS__
.ELSE
EXE = .EXE
OBJ = .OBJ
.ENDIF

.IFDEF __ALPHA__
CC = CC/STANDARD=VAXC
OPTFILE =
OPTIONS =
.ELSE
OPTFILE = ,VAXCRTL.OPT
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF

CFLAGS = $(CFLAGS)/DEFINE=(FLOAT_TYPE)

OBJS =	SLANG$(OBJ),SLPARSE$(OBJ),SLMATH$(OBJ),SLFILE$(OBJ),-
	SLSTD$(OBJ),SLARRAY$(OBJ),SLREGEXP$(OBJ),SLERR$(OBJ),-
	SLKEYMAP$(OBJ),SLRLINE$(OBJ),SLTOKEN$(OBJ),SLGETKEY$(OBJ),-
	SLVMSTTY$(OBJ),SLMEMCMP$(OBJ),SLMEMCHR$(OBJ),SLMEMCPY$(OBJ)-
	SLDISPLY$(OBJ),SLSMG$(OBJ),SLSEARCH$(OBJ),SLCMD$(OBJ)-
	SLMALLOC$(OBJ),SLMEMSET$(OBJ)

HFILES = CONFIG.H,SLANG.H,SLARRAY.H,_SLANG.H

CALC$(EXE) : CALC$(OBJ),SLANG$(OLB)($(OBJS))$(OPTFILE)
        $(LINK)$(LINKFLAGS)/NOTRACE CALC$(OBJ),SLANG$(OLB)/LIBRARY$(OPTIONS)

CALC$(OBJ) :	CALC.C
#
#  Most modules depend on most of the .H files, so I'm going to be lazy
#  and just make them all depend on all of them.
#
$(OBJS) : $(HFILES)

#
#  Build the linker options file for OpenVMS VAX and VAX C.
#
VAXCRTL.OPT :
        @ open/write tmp vaxcrtl.opt
        @ write tmp "SYS$SHARE:VAXCRTL.EXE/SHARE"
        @ close tmp
