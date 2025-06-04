#  CALLMON - A Call Monitor for OpenVMS Alpha
#
#  File:     CALLMON.MMS
#  Author:   Thierry Lelegard
#  Version:  1.0
#  Date:     24-JUL-1996
#
#  Abstract: MMS description file for CALLMON.
#
#  Without debug support:  $ MMS/DESCRIPTION=CALLMON.MMS
#  With debug support:     $ MMS/DESCRIPTION=CALLMON.MMS/MACRO=DEBUG=1
#

#
#  Compilation flags.
#

.IFDEF DEBUG

DBG_CFLAGS    = /DEBUG/NOOPTIMIZE
DBG_M64FLAGS  = /DEBUG/NOOPTIMIZE
DBG_ADAFLAGS  = /DEBUG/NOOPTIMIZE
DBG_LINKFLAGS = /DEBUG

.ELSE

DBG_CFLAGS    = /NODEBUG/OPTIMIZE
DBG_M64FLAGS  = /NODEBUG/NOOPTIMIZE
DBG_ADAFLAGS  = /NODEBUG/OPTIMIZE
DBG_LINKFLAGS = /NODEBUG

.ENDIF

LIS_CFLAGS = /LIST=$(MMS$TARGET_NAME).LIS/MACHINE_CODE
LIS_M64FLAGS = /LIST=$(MMS$TARGET_NAME).LIS/MACHINE_CODE
LIS_ADAFLAGS = /LIST/MACHINE_CODE
LIS_LINKFLAGS = /MAP=$(MMS$TARGET_NAME).MAP/CROSS/FULL

CFLAGS = $(DBG_CFLAGS)$(LIS_CFLAGS)/PREFIX=ALL/OBJECT=$(MMS$TARGET_NAME).OBJ
M64FLAGS = $(DBG_M64FLAGS)$(LIS_M64FLAGS)/OBJECT=$(MMS$TARGET_NAME).OBJ
ADAFLAGS = $(DBG_ADAFLAGS)$(LIS_ADAFLAGS)/SMART
LINKFLAGS = $(DBG_LINKFLAGS)$(LIS_LINKFLAGS)/SYSEXE/EXEC=$(MMS$TARGET_NAME).EXE
ACSLINKFLAGS = $(DBG_LINKFLAGS)$(LIS_LINKFLAGS)/EXEC=$(MMS$TARGET_NAME).EXE

#
#  Additional rules:
#

.SUFFIXES : .OBJ .M64
.C.OBJ :
	$(CC) $(CFLAGS) $(MMS$SOURCE) + SYS$LIBRARY:SYS$LIB_C.TLB/LIBRARY
.M64.OBJ :
	MACRO/ALPHA $(M64FLAGS) $(MMS$SOURCE)
.DEFAULT :
	@ CONTINUE

default : callmon c_examples
all     : callmon c_examples ada_examples

cleantmp :
	if f$search("*.log").nes."" then delete *.log;*
	if f$search("*.dmp").nes."" then delete *.dmp;*
	if f$search("*.tmp").nes."" then delete *.tmp;*
	if f$search("*.i").nes."" then delete *.i;*

clean : cleantmp
	purge
	if f$search("*.obj").nes."" then delete *.obj;*
	if f$search("*.lis").nes."" then delete *.lis;*
	if f$search("*.map").nes."" then delete *.map;*

cleanall : clean
	if f$search("*.olb").nes."" then delete *.olb;*
	if f$search("*.exe").nes."" then delete *.exe;*
	if f$search("callmonmsg.h").nes."" then delete callmonmsg.h;*
	if f$search("callmonmsg_.ada").nes."" then delete callmonmsg_.ada;*

#
#  Ada environment is set up only if DEC Ada is installed on the system.
#

ADALIB = [-.ADALIB]

.FIRST :
	@ IF F$SEARCH("SYS$SYSTEM:ACS.EXE").NES."" .AND. -
	  F$SEARCH("$(ADALIB)ADALIB.ALB").EQS."" THEN -
	  ACS CREATE LIBRARY/NOLOG $(ADALIB)
	@ IF F$SEARCH("SYS$SYSTEM:ACS.EXE").NES."" THEN -
	  ACS SET LIBRARY/NOLOG $(ADALIB)

#
#  CALLMON library:
#

callmon : callmon.olb callmonmsg.h callmonmsg_.ada

callmon.olb : -
    callmon.olb(callmon$alphacode.obj) -
    callmon.olb(callmon$branch.obj) -
    callmon.olb(callmon$dump.obj) -
    callmon.olb(callmon$fixup.obj) -
    callmon.olb(callmon$image.obj) -
    callmon.olb(callmon$init.obj) -
    callmon.olb(callmon$intercept.obj) -
    callmon.olb(callmon$jacket.obj) -
    callmon.olb(callmon$memprot.obj) -
    callmon.olb(callmon$messages.obj) -
    callmon.olb(callmon$reloc.obj) -
    callmon.olb(callmon$routine.obj) -
    callmon.olb(callmon$util.obj)

CALLMON_H = callmon.h callmonmsg.h callmon$private.h

callmon$alphacode.obj : callmon$alphacode.c $(CALLMON_H)
callmon$branch.obj    : callmon$branch.c $(CALLMON_H)
callmon$dump.obj      : callmon$dump.c $(CALLMON_H)
callmon$fixup.obj     : callmon$fixup.c $(CALLMON_H)
callmon$image.obj     : callmon$image.c $(CALLMON_H)
callmon$init.obj      : callmon$init.c $(CALLMON_H)
callmon$intercept.obj : callmon$intercept.c $(CALLMON_H)
callmon$jacket.obj    : callmon$jacket.m64
callmon$memprot.obj   : callmon$memprot.c $(CALLMON_H)
callmon$messages.obj  : callmon$messages.msg
callmon$reloc.obj     : callmon$reloc.c $(CALLMON_H)
callmon$routine.obj   : callmon$routine.c $(CALLMON_H)
callmon$util.obj      : callmon$util.c $(CALLMON_H)

callmonmsg.h : callmon$messages.obj message_definition.exe
	GENDEF :== $SYS$DISK:[]MESSAGE_DEFINITION
	GENDEF -C CALLMON$MESSAGES.OBJ CALLMONMSG

callmonmsg_.ada : callmon$messages.obj message_definition.exe
	GENDEF :== $SYS$DISK:[]MESSAGE_DEFINITION
	GENDEF -ADA CALLMON$MESSAGES.OBJ CALLMONMSG

message_definition.exe : message_definition.obj
	$(LINK) $(LINKFLAGS) /NODEBUG $(MMS$SOURCE)

#
#  Example programs:
#

c_examples : -
    example_getvm.exe -
    example_motif.exe -
    example_shr_main.exe

example_getvm.obj    : example_getvm.c callmon.h callmonmsg.h
example_motif.obj    : example_motif.c
example_motif_vm.obj : example_motif_vm.c callmon.h callmonmsg.h
example_shr.obj      : example_shr.c
example_shr_main.obj : example_shr_main.c callmon.h callmonmsg.h

example_getvm.exe : example_getvm.obj callmon.olb
	$(LINK) $(LINKFLAGS) EXAMPLE_GETVM.OBJ, CALLMON.OLB/LIBRARY

example_motif.exe : example_motif.obj example_motif.opt -
    example_motif_vm.obj callmon.olb
	$(LINK) $(LINKFLAGS) EXAMPLE_MOTIF.OBJ, EXAMPLE_MOTIF_VM.OBJ, -
	    CALLMON.OLB/LIBRARY, EXAMPLE_MOTIF.OPT/OPTIONS

example_shr.exe : example_shr.obj example_shr.opt
	$(LINK) /NODEBUG/SHARE=$(MMS$TARGET) EXAMPLE_SHR.OBJ, -
	    EXAMPLE_SHR.OPT/OPTIONS

example_shr_main.exe : example_shr_main.obj example_shr_main.opt -
    example_shr.exe callmon.olb
	$(LINK) $(LINKFLAGS) EXAMPLE_SHR_MAIN.OBJ, CALLMON.OLB/LIBRARY, -
	    EXAMPLE_SHR_MAIN.OPT/OPTIONS

#
#  Ada example programs (not built by default):
#

ada_examples : example_tasking.exe

ada_clean :
	ACS DELETE UNIT /NOENTERED *

ADA_TASKING_SOURCES = CALLMON_.ADA, CALLMONMSG_.ADA, EXAMPLE_COLLECTION_.ADA, -
	EXAMPLE_COLLECTION.ADA, EXAMPLE_TASKING.ADA

example_tasking.exe : $(ADA_TASKING_SOURCES) callmon.olb
	ACS LOAD/WAIT/LOG $(ADA_TASKING_SOURCES)
	ACS RECOMPILE/WAIT/LOG $(ADAFLAGS) $(MMS$TARGET_NAME)
	ACS ENTER FOREIGN/REPLACE CALLMON.OLB/LIBRARY CALLMON
	LINK :== LINK/SYSEXE
	ACS LINK $(ACSLINKFLAGS) $(MMS$TARGET_NAME)
