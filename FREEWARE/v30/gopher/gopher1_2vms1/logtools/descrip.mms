! MMS file for building GLOG.EXE

GOPHERD_VERSION = 1.2VMS-1

.IFDEF DEC_C
OPTFILE = [-.optfiles]DECC.opt
.ELSE
OPTFILE = [-.optfiles]VAXC.opt
.ENDIF

PRODUCTION = ok
INDEXING_DEBUG = /nodebug
GLOG_EXE = [-.exe]glog.exe

!==============================================================================

! The query object file list
GLOG_OBJECT =-
 [-.obj]glog.obj

! Link the executables

indexing : $(GLOG_EXE)
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Log Analyzer Built!"
.ELSE
	@ write sys$output "DEBUG Log Analyzer Built!"
.ENDIF

$(GLOG_EXE) : $(GLOG_OBJECT) [-.optfiles]ident.opt $(OPTFILE)
	link $(INDEXING_DEBUG) /exe=$(GLOG_EXE)-
 $(GLOG_OBJECT), [-.optfiles]ident/opt, $(OPTFILE)/opt

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

[-.obj]glog.obj : glog.c
