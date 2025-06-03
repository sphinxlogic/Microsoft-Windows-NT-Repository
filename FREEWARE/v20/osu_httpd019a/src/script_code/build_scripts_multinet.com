$ EXEDIR = "[-.system]
$ BINDIR = "[-.bin]
$ CC /NOLIST/OBJECT=CGI_SYMBOLS.OBJ cgi_symbols.c
$ CC /NOLIST/OBJECT=CGILIB.OBJ cgilib.c
$ CC /NOLIST/OBJECT=SCRIPTLIB.OBJ scriptlib.c
$ @LINK_SCRIPT cgi_symbols.obj,cgilib.obj,scriptlib.obj
$ COPY cgi_symbols.exe 'BINDIR'
$ VAXCOMPILE = "CC"
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN VAXCOMPILE = "CC/STANDARD=VAXC"
$ VAXCOMPILE /NOLIST/OBJECT=GSQL.OBJ gsql.c
$ @LINK_SCRIPT gsql.obj,cgilib.obj,scriptlib.obj
$ COPY gsql.exe 'BINDIR'
$ CC  index_search.c
$ @LINK_SCRIPT index_search.obj,cgilib.obj,scriptlib.obj
$ COPY index_search.exe 'BINDIR'
$ CC /NOLIST/OBJECT=VMSHELPGATE.OBJ vmshelpgate.c
$ CC /NOLIST/OBJECT=LBRIO.OBJ lbrio.c
$ @LINK_SCRIPT vmshelpgate.obj,lbrio.obj,cgilib.obj,scriptlib.obj
$ COPY vmshelpgate.exe 'BINDIR'
$ CC /NOLIST/OBJECT=MAPIMAGE.OBJ mapimage.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
$ @LINK_SCRIPT mapimage.obj,scriptlib.obj
$ COPY mapimage.exe 'BINDIR'
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=HTML_PREPROC.OBJ html_preproc.c 'prefix_all'
$ CC /NOLIST/OBJECT=ACCESS_DB.OBJ access_db.c
$ @LINK_SCRIPT html_preproc,scriptlib.obj,access_db.obj
$ COPY html_preproc.exe 'EXEDIR'
$ CC /NOLIST/OBJECT=TESTCGI.OBJ testcgi.c
$ @LINK_SCRIPT testcgi.obj,cgilib.obj,scriptlib.obj
$ COPY testcgi.exe 'BINDIR'
$ CC /NOLIST/OBJECT=TESTFORM.OBJ testform.c
$ @LINK_SCRIPT testform.obj,cgilib.obj,scriptlib.obj
$ COPY testform.exe 'BINDIR'
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=CGI-MAILTO.OBJ cgi-mailto.c 'prefix_all'
$ @LINK_SCRIPT cgi-mailto.obj,cgilib.obj,scriptlib.obj MULTINET
$ COPY cgi-mailto.exe 'BINDIR'
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=PH.OBJ ph.c 'prefix_all'
$ @LINK_SCRIPT ph.obj,cgilib.obj,scriptlib.obj MULTINET
$ COPY ph.exe 'BINDIR'
$ write sys$output "Script images build done"
