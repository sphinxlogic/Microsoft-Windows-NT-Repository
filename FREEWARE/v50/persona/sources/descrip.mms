#   Description File for PERSONA (VAX or Alpha Version)
#   Created automatically 24-Jan-1999 17:14 using MKMF V1.07-0227 (Dec C)
#   Logicals/Qualifiers used to create this file:
#       /LOG=PERSONA.LOG
#

C   = CC
CCcmd   = $(C)
CLD = SET COMMAND/OBJECT
CLDcmd  = $(CLD)
MAC = MACRO
MARcmd  = $(MAC)

OBJECTS = \
 PERSONA.OBJ \
 UTILITY.OBJ \
 PERSONA_CLD.OBJ \
 CMDLIN.OBJ

.ifdef __DBG__
CFLAGS=/stand=vaxc/nooptimize/debug/nolist
MFLAGS=/debug/nolist
LFLAGS=/exec=persona.exe/debug/nomap/traceback
.else
CFLAGS=/stand=vaxc/optimize/nodebug/nolist
MFLAGS=/nodebug/nolist
LFLAGS=/exec=persona.exe/nodebug/nomap/notraceback
.endif

PERSONA.EXE : $(OBJECTS)
.ifdef __AXP__
    link $(LFLAGS)/sysexe PERSONA/OPT,sys$loadable_images:dcldef.stb
.else
    link $(LFLAGS) PERSONA/OPT
.endif
    purge /exclude=(*.c,*.h,*.cld,*.mar)

#
#   Object File Dependencies
#

PERSONA.OBJ : PERSONA.C VERSION.H DEFS.H
    $(CCcmd) $(CFLAGS) PERSONA.C

UTILITY.OBJ : UTILITY.C VERSION.H DEFS.H
    $(CCcmd) $(CFLAGS) UTILITY.C

PERSONA_CLD.OBJ : PERSONA.CLD
    $(CLDcmd) /OBJ=PERSONA_CLD.OBJ PERSONA.CLD 

.ifdef __AXP__
CMDLIN.OBJ : CMDLIN-AXP.MAR
    $(MARcmd) $(MFLAGS) /obj=cmdlin.obj  CMDLIN-AXP.MAR
.else
CMDLIN.OBJ : CMDLIN-VAX.MAR
    $(MARcmd) $(MFLAGS) /obj=cmdlin.obj  CMDLIN-VAX.MAR
.endif

#   End DESCRIP.MMS
