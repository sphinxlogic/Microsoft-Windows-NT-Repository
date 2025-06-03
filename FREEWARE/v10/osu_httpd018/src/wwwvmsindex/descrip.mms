! MMS file for building BUILD_INDEX.EXE, QUERY.EXE and WWWQUERY.EXE

INDEXING_VERSION = 1.0

.IFDEF DEC_C
OPTFILE = []DECC.opt
.ELSE
OPTFILE = []VAXC.opt
.ENDIF

.IFDEF DEBUG
INDEXING_DEBUG = /debug/noopt
BUILD_INDEX_EXE = []build_index_debug.exe
QUERY_EXE = []query_debug.exe
WWWQUERY_EXE = []wwwquery_debug.exe
.ELSE
PRODUCTION = ok
INDEXING_DEBUG = /nodebug
BUILD_INDEX_EXE = []build_index.exe
QUERY_EXE = []query.exe
WWWQUERY_EXE = []wwwquery.exe
.ENDIF

!==============================================================================

! The query object file list
QUERY_OBJECT =-
 []query.obj

! The wwwquery object file list
WWWQUERY_OBJECT =-
 []wwwquery.obj

! The build_index object file list
BUILD_INDEX_OBJECT =-
 []build_index.obj,-
 []indexcld.obj

! Link the executables

indexing : $(QUERY_EXE) $(WWWQUERY_EXE) $(BUILD_INDEX_EXE)
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexing Built!"
.ELSE
	@ write sys$output "DEBUG Indexing Built!"
.ENDIF

$(QUERY_EXE) : $(QUERY_OBJECT) []ident.opt $(OPTFILE)
	link $(INDEXING_DEBUG) /exe=$(QUERY_EXE)-
 $(QUERY_OBJECT), []ident/opt, $(OPTFILE)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexed Query Built!"
.ELSE
	@ write sys$output "DEBUG Indexed Query Built!"
.ENDIF

$(WWWQUERY_EXE) : $(WWWQUERY_OBJECT) []ident.opt $(OPTFILE)
	link $(INDEXING_DEBUG) /exe=$(WWWQUERY_EXE)-
 $(WWWQUERY_OBJECT), []ident/opt, $(OPTFILE)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexed WWWQuery Built!"
.ELSE
	@ write sys$output "DEBUG Indexed WWWQuery Built!"
.ENDIF

$(BUILD_INDEX_EXE) : $(BUILD_INDEX_OBJECT) []ident.opt $(OPTFILE)
	link $(INDEXING_DEBUG) /exe=$(BUILD_INDEX_EXE)-
 $(BUILD_INDEX_OBJECT), []ident/opt, $(OPTFILE)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Indexed BUILD_INDEX Built!"
.ELSE
	@ write sys$output "DEBUG Indexed BUILD_INDEX Built!"
.ENDIF

!==============================================================================


! A default compilation procedure
.IFDEF DEC_C
.c.obj
	cc/object=[] $<
.ELSE
.c.obj
	cc/object=[] $<
.ENDIF

! Define dependencies for all object files

[]build_index.obj : build_index.c

[]indexcld.obj : indexcld.cld
	set command/object=[] indexcld.cld

[]query.obj : query.c

[]wwwquery.obj : wwwquery.c
