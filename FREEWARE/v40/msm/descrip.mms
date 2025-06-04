#
#	Description File for MINISM
#	Created automatically 28-Feb-1995 20:18 using MKMF V1.07-0227 (Dec C)
#	Logicals/Qualifiers used to create this file:
#		/EXECUTABLE=MINISM
#		/LIBFILE=USR$TOOLS:VCCOPT_DECW.OPT
#

.ifdef MSMDBG
CCcmd   = CC /define=(msmdbg)
.else   # not MSMDBG
CCcmd   = CC
.endif

#C	= CC
#CCcmd	= $(C)

OBJECTS = \
 CALLBACKS.OBJ \
 FILES.OBJ \
 HELP.OBJ \
 MINISM.OBJ \
 SUPPORT.OBJ \
 TASKS.OBJ

.ifdef DBG
CFLAGS=/stand=vaxc/nooptimize/debug/nolist
LFLAGS=/debug/nomap/traceback
.else
CFLAGS=/stand=vaxc/optimize/nodebug/nolist
LFLAGS=/nodebug/nomap/notraceback
.endif

MINISM.EXE : $(OBJECTS)
	link $(LFLAGS) MINISM.OPT/OPT
	purge /exclude=(*.c,*.h)

#
#	Object File Dependencies
#

CALLBACKS.OBJ : CALLBACKS.C externs.h includes.h
	$(CCcmd) $(CFLAGS) CALLBACKS.C

FILES.OBJ : FILES.C externs.h includes.h
	$(CCcmd) $(CFLAGS) FILES.C

HELP.OBJ : HELP.C externs.h includes.h
	$(CCcmd) $(CFLAGS) HELP.C

MINISM.OBJ : MINISM.C includes.h
	$(CCcmd) $(CFLAGS) MINISM.C

SUPPORT.OBJ : SUPPORT.C externs.h includes.h
	$(CCcmd) $(CFLAGS) SUPPORT.C

TASKS.OBJ : TASKS.C externs.h includes.h
	$(CCcmd) $(CFLAGS) TASKS.C


#	End DESCRIP.MMS
