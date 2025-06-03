!
!  MMS description file for building scripts for DECThreads HTTP SERVER
!
!  The default TCP package is UCX.  To change it, invoke mms as
!     $ MMS/MACRO=TCP=xxxx
!
!  To build a server with traceback information (for debugging), invoke mms as
!     $ MMM/FORCE/MACRO=TRACE=1
!
!  To build a server with debugging enabled, invoke mms as 
!     $ MMS/MACRO=DEBUG=1
!
.IFDEF TCP
TCPOPT = /DEFINE=$(TCP)
TCP = ""
.ELSE
TCP = UCXTCP
TCPOPT = /DEFINE=UCX=1
.ENDIF

.IFDEF TRACE
S_LINKFLAGS = /TRACE/EXEC=$(MM$TARGET_NAME).EXE
.ELSE
S_LINKFLAGS = /NOTRACE/EXEC=$(MMS$TARGET_NAME).EXE
.ENDIF
LINKFLAGS = /EXEC=$(MMS$TARGET_NAME).EXE

.IFDEF DEBUG
CFLAGS    = $(CFLAGS)/DEBUG/NOOPT
S_LINKFLAGS = $(S_LINKFLAGS)/DEBUG
.ENDIF

SCRIPT_IMAGES : cgi_symbols.exe,gsql.exe,index_search.exe,vmshelpgate.exe,\
		mapimage.exe,html_preproc.exe,testcgi.exe,testform.exe,\
		cgi-mailto.exe
        @ write sys$output "Script images build done"

MAPIMAGE.EXE : mapimage.obj,scriptlib.obj
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
    @LINK_SCRIPT mapimage.obj,scriptlib.obj
    COPY mapimage.exe [-.bin]
!
!GETSTATS.EXE : getstats.obj,form_request.obj,cgilib.obj,scriptlib.obj
!    @LINK_SCRIPT getstats.obj,form_request.obj,cgilib.obj,scriptlib.obj $(TCP)
!    COPY getstats.exe [-.bin]

CGI-MAILTO.EXE : cgi-mailto.obj,verify_document.obj,cgilib.obj,scriptlib.obj
     @LINK_SCRIPT cgi-mailto.obj,verify_document.obj,cgilib.obj,scriptlib.obj $(TCP)
     COPY cgi-mailto.exe [-.bin]

CDCONTENTS.EXE : cdcontents.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT cdcontents.obj,cgilib.obj,scriptlib.obj
    COPY cdcontents.exe [-.bin]

CGI_SYMBOLS.EXE : cgi_symbols.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT cgi_symbols.obj,cgilib.obj,scriptlib.obj
    COPY cgi_symbols.exe [-.bin]

GSQL.EXE : gsql.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT gsql.obj,cgilib.obj,scriptlib.obj
    COPY gsql.exe [-.bin]

HTML_PREPROC.EXE : html_preproc.obj,scriptlib.obj
    @LINK_SCRIPT html_preproc,scriptlib.obj
    COPY html_preproc.exe [-]

TESTCGI.EXE : testcgi.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT testcgi.obj,cgilib.obj,scriptlib.obj
    COPY testcgi.exe [-.bin]

TESTFORM.EXE : testform.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT testform.obj,cgilib.obj,scriptlib.obj
    COPY testform.exe [-.bin]

INDEX_SEARCH.EXE : index_search.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT index_search.obj,cgilib.obj,scriptlib.obj
    COPY index_search.exe [-.bin]

VMSHELPGATE.EXE : vmshelpgate.obj,lbrio.obj,cgilib.obj,scriptlib.obj
    @LINK_SCRIPT vmshelpgate.obj,lbrio.obj,cgilib.obj,scriptlib.obj
    COPY vmshelpgate.exe [-.bin]

! object file dependancies...
gsql.obj : gsql.c,cgilib.h
    VAXCOMPILE = "CC"
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN VAXCOMPILE = "CC/STANDARD=VAXC"
    VAXCOMPILE $(CFLAGS) gsql.c

form_request.obj : form_request.c,cgilib.h
    CC $(CFLAGS) form_request.c

getstats.obj : getstats.c,getstats_12-orig.c,cgilib.h,scriptlib.h
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CCFLAGS) $(TCPOPT) getstats.c 'prefix_all'

mapimage.obj : mapimage.c,scriptlib.h
    CC $(CFLAGS) mapimage.c

lbrio.obj : lbrio.c,lbrio.h
    CC $(CFLAGS) lbrio.c

index_search.obj : index_search.c,cgilib.h
    CC $(CFLGS) index_search.c

vmshelpgate.obj : vmshelpgate.c,lbrio.h,cgilib.h
    CC $(CFLAGS) vmshelpgate.c

scriptlib.obj : scriptlib.c,scriptlib.h
    CC $(CFLAGS) scriptlib.c

cgilib.obj : cgilib.c,cgilib.h,scriptlib.h
    CC $(CFLAGS) cgilib.c

cdcontents.obj : cdcontents.c,cgilib.h
    CC $(CFLAGS) cdcontents.c

html_preproc.obj : html_preproc.c,scriptlib.h
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) html_preproc.c 'prefix_all'

privrequest.obj : privrequest.c
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) privrequest.c 'prefix_all'

testcgi.obj : testcgi.c,cgilib.h
    CC $(CFLAGS) testcgi.c

testform.obj : testform.c,cgilib.h
    CC $(CFLAGS) testform.c

cgi_symbols.obj : cgi_symbols.c,cgilib.h,scriptlib.h
    CC $(CFLAGS) cgi_symbols.c

cgi-mailto.obj : cgi-mailto.c,utl-itemlist.h
    CC $(CFLAGS) cgi-mailto.c

verify_document.obj : verify_document.c
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) verify_document 'prefix_all'
