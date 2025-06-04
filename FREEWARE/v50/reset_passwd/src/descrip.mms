#   Description File for RESET_PASSWD (VAX or Alpha Version)
#   Created automatically 24-Jan-1999 17:14 using MKMF V1.07-0227 (Dec C)
#   Logicals/Qualifiers used to create this file:
#       /LOG=RESET_PASSWD.LOG
#

C   = CC
CCcmd   = $(C)
CLD = SET COMMAND/OBJECT
CLDcmd  = $(CLD)
MAC = MACRO
MARcmd  = $(MAC)

OBJECTS = \
 RESET_PASSWD.OBJ \
 CMDLIN.OBJ

.ifdef __DBG__
CFLAGS=/stand=vaxc/nooptimize/debug/nolist
MFLAGS=/debug/nolist
LFLAGS=/exec=reset_passwd.exe/debug/nomap/traceback
.else
CFLAGS=/stand=vaxc/optimize/nodebug/nolist
MFLAGS=/nodebug/nolist
LFLAGS=/exec=reset_passwd.exe/nodebug/nomap/notraceback
.endif

RESET_PASSWD.EXE : $(OBJECTS)
.ifdef __AXP__
    link $(LFLAGS)/sysexe RESET_PASSWD/OPT,sys$loadable_images:dcldef.stb
.else
    link $(LFLAGS) RESET_PASSWD/OPT
.endif
    purge /exclude=(*.c,*.h,*.mar)

#
#   Object File Dependencies
#

RESET_PASSWD.OBJ : RESET_PASSWD.C 
    $(CCcmd) $(CFLAGS) RESET_PASSWD.C

.ifdef __AXP__
CMDLIN.OBJ : CMDLIN-AXP.MAR
    $(MARcmd) $(MFLAGS) /obj=cmdlin.obj  CMDLIN-AXP.MAR
.else
CMDLIN.OBJ : CMDLIN-VAX.MAR
    $(MARcmd) $(MFLAGS) /obj=cmdlin.obj  CMDLIN-VAX.MAR
.endif

#   End DESCRIP.MMS
