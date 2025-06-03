$ CC /NOLIST/OBJECT=CGI_SYMBOLS.OBJ cgi_symbols.c
$ CC /NOLIST/OBJECT=CGILIB.OBJ cgilib.c
$ CC /NOLIST/OBJECT=SCRIPTLIB.OBJ scriptlib.c
$ @LINK_SCRIPT cgi_symbols.obj,cgilib.obj,scriptlib.obj
$ COPY cgi_symbols.exe [-.bin]
$ VAXCOMPILE = "CC"
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN VAXCOMPILE = "CC/STANDARD=VAXC"
$ VAXCOMPILE /NOLIST/OBJECT=GSQL.OBJ gsql.c
$ @LINK_SCRIPT gsql.obj,cgilib.obj,scriptlib.obj
$ COPY gsql.exe [-.bin]
$ CC  index_search.c
$ @LINK_SCRIPT index_search.obj,cgilib.obj,scriptlib.obj
$ COPY index_search.exe [-.bin]
$ CC /NOLIST/OBJECT=VMSHELPGATE.OBJ vmshelpgate.c
$ CC /NOLIST/OBJECT=LBRIO.OBJ lbrio.c
$ @LINK_SCRIPT vmshelpgate.obj,lbrio.obj,cgilib.obj,scriptlib.obj
$ COPY vmshelpgate.exe [-.bin]
$ CC /NOLIST/OBJECT=MAPIMAGE.OBJ mapimage.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
$ @LINK_SCRIPT mapimage.obj,scriptlib.obj
$ COPY mapimage.exe [-.bin]
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=HTML_PREPROC.OBJ html_preproc.c 'prefix_all'
$ @LINK_SCRIPT html_preproc,scriptlib.obj
$ COPY html_preproc.exe [-]
$ CC /NOLIST/OBJECT=TESTCGI.OBJ testcgi.c
$ @LINK_SCRIPT testcgi.obj,cgilib.obj,scriptlib.obj
$ COPY testcgi.exe [-.bin]
$ CC /NOLIST/OBJECT=TESTFORM.OBJ testform.c
$ @LINK_SCRIPT testform.obj,cgilib.obj,scriptlib.obj
$ COPY testform.exe [-.bin]
$ CC /NOLIST/OBJECT=CGI-MAILTO.OBJ cgi-mailto.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=VERIFY_DOCUMENT.OBJ verify_document 'prefix_all'
$ @LINK_SCRIPT cgi-mailto.obj,verify_document.obj,cgilib.obj,scriptlib.obj TWGTCP
$ COPY cgi-mailto.exe [-.bin]
$ write sys$output "Script images build done"
