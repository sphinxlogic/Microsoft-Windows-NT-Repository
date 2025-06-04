#########################################################################
##   SSINIT:  A Loadable Executive Image (LEI) example -- build file   ##
#########################################################################

.FIRST :
    @ WRITEE SYS$OUTPUT "Copyright © 1992, 1994, 1996 by Brian Schenkenberger."
    @ WRITEE SYS$OUTPUT "ALL RIGHTS RESERVED."
    @ WRITEE SYS$OUTPUT "-----------------------------------------------------"

MAR = .MAR
EXE = .EXE
OBJ = .OBJ
OPT = .OPT

.IFDEF __ALPHA__	## qualifier macros for use on OpenVMS AXP
PAGESIZE  = 13
MFLAGS    = /LIST/MACHINE
LINKFLAGS = /MAP/FULL/NOTRACEBACK/NATIVE_ONLY/BPAGE=$(PAGESIZE)
SYSEXE	  = /SYSEXE=SELECTIVE

.ELSE			## qualifier macros for use on OpenVMS VAX

MFLAGS    = /LIST
LINKFLAGS = /MAP/FULL/NOTRACEBACK
SYSEXE	  = ,SYS$SYSTEM:SYS.STB/SELECTIVE

.ENDIF

##
## Loadable Executive Image (LEI) is SSINTERCEPT
##
	
LEI = SSINTERCEPT
DOX = SSDOINIHACK
LDR = SSINTLOADER

##
## SSINTERCEPT component dependency build rules
##

$(LEI)	: $(LEI)$(EXE),$(LDR)$(EXE)
    @ WRITE SYS$OUTPUT " - SSINTERCEPT build complete."

$(LEI)$(EXE)	: $(LEI)$(OBJ),$(DOX)$(OBJ),$(LEI)$(OPT)
  - $(LINK)$(LINKFLAGS)/SHAREABLE/NOSYSSHR/SYMBOL_TABLE -
        $(LEI)$(OBJ),$(DOX)$(OBJ)$(SYSEXE),$(LEI)$(OPT)/OPTION

$(LDR)$(EXE)	: $(LDR)$(OBJ)
    $(LINK)$(LINKFLAGS) $(MMS$SOURCE)$(SYSEXE)


$(LEI)$(OBJ)	: $(LEI)$(MAR)
$(DOX)$(OBJ)	: $(DOX)$(MAR)

$(LDR)$(OBJ)	: $(LDR)$(MAR)
