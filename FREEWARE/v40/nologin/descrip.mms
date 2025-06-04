#
#   Description File for NOLOGIN
#   Created automatically 24-Mar-1998 17:14 using MKMF V1.07-0227 (Dec C)
#   Logicals/Qualifiers used to create this file:
#       /LOG=NOLOGIN.LOG
#

C   = CC
CCcmd   = $(C)
CLD = SET COMMAND/OBJECT
CLDcmd  = $(CLD)
MAC = MACRO
MARcmd  = $(MAC)

OBJECTS = \
 NOLOGIN.OBJ \
 UTILITY.OBJ \
 NOLOGIN_CLD.OBJ \
 CMDLIN.OBJ

.ifdef DBG
CFLAGS=/stand=vaxc/nooptimize/debug/nolist
MFLAGS=/debug/nolist
LFLAGS=/sysexe/debug/nomap/traceback
.else
CFLAGS=/stand=vaxc/optimize/nodebug/nolist
MFLAGS=/nodebug/nolist
LFLAGS=/sysexe/nodebug/nomap/notraceback
.endif

NOLOGIN.EXE : $(OBJECTS)
    link $(LFLAGS) NOLOGIN.OPT/OPT
    purge /exclude=(*.c,*.h,*.cld,*.mar)

#
#   Object File Dependencies
#

NOLOGIN.OBJ : NOLOGIN.C VERSION.H
    $(CCcmd) $(CFLAGS) NOLOGIN.C

UTILITY.OBJ : UTILITY.C VERSION.H
    $(CCcmd) $(CFLAGS) UTILITY.C

NOLOGIN_CLD.OBJ : NOLOGIN.CLD
    $(CLDcmd) /OBJ=NOLOGIN_CLD.OBJ NOLOGIN.CLD 

CMDLIN.OBJ : CMDLIN.MAR
    $(MARcmd) $(MFLAGS)  CMDLIN.MAR


#   End DESCRIP.MMS
