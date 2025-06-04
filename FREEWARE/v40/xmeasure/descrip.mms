#
#	Description File for Xmeasure
#	Created automatically 02-Nov-1993 23:16 using MKMF V1.04-0716
#	Logicals/Qualifiers used to create this file:
#		/EXECUTABLE=XMEASURE
#		/LIBFILE=USR$TOOLS:VCCOPT_DECW.OPT
#		/LOG=MKMF.LOG
#

C	= CC
CCcmd	= $(C)

OBJECTS = \
 CALLBACKS.OBJ \
 HELP.OBJ \
 MEASURE.OBJ \
 DELTA.OBJ

.ifdef DBG
CFLAGS=/standard=vaxc/nooptimize/debug/nolist
LFLAGS=/debug/nomap/traceback
.else
CFLAGS=/standard=vaxc/optimize/nodebug/nolist
LFLAGS=/nodebug/nomap/notraceback
.endif

XMEASURE.EXE : $(OBJECTS)
	link $(LFLAGS) XMEASURE.OPT/OPT
	purge /exclude=(*.c,*.h)

#
#	Object File Dependencies
#

CALLBACKS.OBJ : CALLBACKS.C externs.h includes.h
	$(CCcmd) $(CFLAGS) CALLBACKS.C

HELP.OBJ : HELP.C externs.h includes.h
	$(CCcmd) $(CFLAGS) HELP.C

MEASURE.OBJ : MEASURE.C includes.h
	$(CCcmd) $(CFLAGS) MEASURE.C

DELTA.OBJ : DELTA.C includes.h externs.h
	$(CCcmd) $(CFLAGS) DELTA.C

#	End DESCRIP.MMS
