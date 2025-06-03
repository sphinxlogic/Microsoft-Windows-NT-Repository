! MMS file for building manager tools:
!	FORCEX.C

GOPHERD_VERSION = 1.2VMS-1

.IFDEF DEC_C
OPTFILE = [-.optfiles]DECC.opt
.ELSE
OPTFILE = [-.optfiles]VAXC.opt
.ENDIF

PRODUCTION = ok
MGRTOOLS_DEBUG = /nodebug
FORCEX_EXE = [-.exe]forcex.exe

!==============================================================================

! The forcex object file list
FORCEX_OBJECT =-
 [-.obj]forcex.obj

! Link the executables

forcex : $(FORCEX_EXE)
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Manager Tools Built!"
.ELSE
	@ write sys$output "DEBUG Manager Tools Built!"
.ENDIF

$(FORCEX_EXE) : $(FORCEX_OBJECT) [-.optfiles]ident.opt $(OPTFILE)
	link $(MGRTOOLS_DEBUG) /exe=$(FORCEX_EXE)-
 $(FORCEX_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION FORCEX Built!"
.ELSE
	@ write sys$output "DEBUG FORCEX Built!"
.ENDIF


!==============================================================================


! A default compilation procedure
.IFDEF DEC_C
.c.obj
	cc/warning=(disable=implicitfunc)/object=[-.obj] $<
.ELSE
.c.obj
	cc/object=[-.obj] $<
.ENDIF

! Define dependencies for all object files

[-.obj]forcex.obj : forcex.c

