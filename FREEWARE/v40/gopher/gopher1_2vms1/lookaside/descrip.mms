! MMS file for building LOOKASIDE.EXE
!
!    19931221	JLWilkinson	jlw@psulias.psu.edu
!		adapted Foteos Macrides' UCX special handling for DECC
!    19931125	JLWilkinson	jlw@psulias.psu.edu
!		original version

GOPHERD_VERSION = 1.2VMS-1
.IFDEF DEC_C
OPTFILE = [-.optfiles]DECC.opt
.ELSE
OPTFILE = [-.optfiles]VAXC.opt
.ENDIF

PRODUCTION = ok
LOOKASIDE_DEBUG = /nodebug
LOOKASIDE_EXE = [-.exe]lookaside.exe

!==============================================================================

! The lookaside object file list
LOOKASIDE_OBJECT =-
 [-.obj]lookaside.obj

! Link the executables

$(LOOKASIDE_EXE) : $(LOOKASIDE_OBJECT) [-.optfiles]ident.opt $(OPTFILE)

.IFDEF DEC_C
.IFDEF VESTED_EDT
	link $(LOOKASIDE_DEBUG) /exe=$(LOOKASIDE_EXE) /nonative-
 $(LOOKASIDE_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt
.ELSE
	link $(LOOKASIDE_DEBUG) /exe=$(LOOKASIDE_EXE) -
 $(LOOKASIDE_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt
.ENDIF
.ELSE
	link $(LOOKASIDE_DEBUG) /exe=$(LOOKASIDE_EXE)-
 $(LOOKASIDE_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt
.ENDIF
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Lookaside Editor Built!"
.ELSE
	@ write sys$output "DEBUG Lookaside Editor Built!"
.ENDIF

!==============================================================================

! A default compilation procedure
.IFDEF DEC_C
.c.obj
	define/nolog C$USER_INCLUDE [-],[-.GOPHERD]
	define/nolog DECC$USER_INCLUDE [-],[-GOPHERD]
	cc$/warning=(disable=implicitfunc)/object=[-.obj]\
			/define=(SERVER) $<
.ELSE
.c.obj
	cc/include=([-],[-.gopherd])/object=[-.obj]/define=(SERVER) $<
.ENDIF

! Define dependencies for all object files

[-.obj]lookaside.obj : lookaside.c [-.gopherd]gsgopherobj.h
.IFDEF DEC_C
.IFDEF VESTED_EDT
	cc/warning=(disable=(implicitfunc,globalext))/tie/object=[-.obj]\
/include=([-],[-.gopherd]) $<
.ELSE
	cc/warning=(disable=(implicitfunc,globalext))/object=[-.obj]\
/include=([-],[-.gopherd]) $<
.ENDIF
.ELSE
	cc/include=([-],[-.gopherd])/object=[-.obj] $<
.ENDIF
