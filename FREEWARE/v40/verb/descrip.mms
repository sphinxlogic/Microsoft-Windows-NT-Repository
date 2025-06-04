# DESCRIP.MMS 
#
# to compile on ALPHA/VMS         use /Macro="__ALPHA__=1"
#               VAX/VMS with DECC use /Macro="__DECC=1"
#
.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
OLB = .OLB
.ENDIF

.IFDEF __ALPHA__
CC = CC/NOMEMBER_ALIGNMENT/L_DOUBLE=64
SYSEXE = /SYSEXE
OPTIONS =
.ELSE
.IFDEF __DECC
CC = CC
SYSEXE = ,SYS$SYSTEM:SYS.STB/SELECTIVE_SEARCH
OPTIONS = 
.ELSE
CC = cc/vaxc/optimize=noinline/nodebug
SYSEXE = ,SYS$SYSTEM:SYS.STB/SELECTIVE_SEARCH
OPTIONS = ,C.OPT/OPTIONS
.ENDIF
.ENDIF

LINKFLAGS = $(LINKFLAGS)/notraceback

VERB = verb verb_cld verb_command verb_disallows verb_entity -
       verb_file verb_type verb_find_entity init_cli

verb$(EXE) : VERB$(OLB)($(VERB))
	$(LINK)$(LINKFLAGS) VERB$(OLB)/INCLUDE=VERB/LIBRARY$(OPTIONS)$(SYSEXE)
