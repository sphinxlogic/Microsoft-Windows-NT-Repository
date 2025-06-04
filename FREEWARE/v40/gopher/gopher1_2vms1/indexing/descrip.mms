! MMS file for building BUILD_INDEX.EXE and QUERY.EXE

GOPHERD_VERSION = 1.2VMS-0

.IFDEF DEC_C
OPTFILE = [-.optfiles]DECC.opt
.ELSE
OPTFILE = [-.optfiles]VAXC.opt
.ENDIF

PRODUCTION = ok
INDEXING_DEBUG = /nodebug
BUILD_INDEX_EXE = [-.exe]build_index.exe
QUERY_EXE = [-.exe]query.exe

!==============================================================================

! The query object file list
QUERY_OBJECT =-
 [-.obj]query.obj

! The build_index object file list
BUILD_INDEX_OBJECT =-
 [-.obj]build_index.obj,-
 [-.obj]indexcld.obj

! Link the executables

indexing : $(QUERY_EXE) $(BUILD_INDEX_EXE)
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexing Built!"
.ELSE
	@ write sys$output "DEBUG Indexing Built!"
.ENDIF

$(QUERY_EXE) : $(QUERY_OBJECT) [-.optfiles]ident.opt $(OPTFILE)
	link $(INDEXING_DEBUG) /exe=$(QUERY_EXE)-
 $(QUERY_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexed Query Built!"
.ELSE
	@ write sys$output "DEBUG Indexed Query Built!"
.ENDIF

$(BUILD_INDEX_EXE) : $(BUILD_INDEX_OBJECT) [-.optfiles]ident.opt $(OPTFILE)
	link $(INDEXING_DEBUG) /exe=$(BUILD_INDEX_EXE)-
 $(BUILD_INDEX_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexed BUILD_INDEX Built!"
.ELSE
	@ write sys$output "DEBUG Indexed BUILD_INDEX Built!"
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

[-.obj]build_index.obj : build_index.c

[-.obj]indexcld.obj : indexcld.cld
	set command/object=[-.obj] indexcld.cld

[-.obj]query.obj : query.c
