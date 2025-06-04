!++
! MMS file for GNU e?grep
!
! Hunter Goatley
!
! Based on MMS file by Henrik Tougaard,  ht@datani.dk
!--
!
!  Uncomment the following line to compile using GNU C
!
!GNU_C = TRUE


MAKEFILE = DESCRIP.MMS
LIBS = SYS$SHARE:VAXCRTL.EXE/SHARE

.IFDEF __ALPHA__
CC = CC/STANDARD=VAXC/PREFIX=ALL
OPTIONS =
OPTIONS_FILE =
.ELSE
.IFDEF GNU_C
CC = GCC
OPTIONS = ,GNU_CC:[000000]OPTIONS.OPT/OPTIONS
.ELSE
OPTIONS = ,GREP.OPT/OPTIONS
OPTIONS_FILE = ,grep.opt
.ENDIF
.ENDIF

LINKFLAGS = $(LINKFLAGS)/NOTRACE/NOMAP

OBJS = shell_mung$(obj), dfa$(obj), regex$(obj), getopt$(obj), alloca$(obj)
GOBJ = grep$(obj)
EOBJ = egrep$(obj)

grep : grep$(exe), egrep$(exe), grep.hlp
	!grep and egrep build complete

grep$(exe) : $(OBJS), $(XALLOCOBJ), $(GOBJ) $(OPTIONS_FILE)
	$(LINK) $(LINKFLAGS) $(GOBJ), $(OBJS) $(OPTIONS)

grep.opt : $(MAKEFILE)
	open/write opt grep.opt
	write opt "! GNU e?grep linker options file"
      @ write opt "$(LIBS)"
	close opt

clean :
	delete/noconfirm grep$(exe);*, egrep.*;*, *$(obj);*, grep.hlp;

egrep$(exe) : $(OBJS), $(EOBJ) $(OPTIONS_FILE)
	$(LINK) $(LINKFLAGS) $(EOBJ), $(OBJS) $(OPTIONS)

grep.hlp : grep.rnh
	RUNOFF GREP.RNH

egrep$(obj) : grep.c
	copy/nolog grep.c egrep.c
	$(CC) $(CFLAGS) /define=EGREP egrep.c
	delete/nolog/noconfirm egrep.c;*

dfa$(obj) egrep$(obj) grep$(obj) : dfa.h
egrep$(obj) grep$(obj) regex$(obj) : regex.h
