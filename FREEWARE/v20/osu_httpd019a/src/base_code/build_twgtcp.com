$ EXEDIR = "[-.system]"
$ CC /NOLIST/OBJECT=CLIENT_LIMIT.OBJ client_limit.c
$ CC /NOLIST/OBJECT=DECNET_ACCESS.OBJ decnet_access.c
$ SET FILE session.h/ext=0
$ CC /NOLIST/OBJECT=SCRIPT_EXECUTE.OBJ script_execute.c
$ CC /NOLIST/OBJECT=DOCUMENT_CACHE.OBJ document_cache.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=FILE_ACCESS.OBJ /DEFINE=(DATE_FORMAT_850) file_access.c 'prefix_all'
$ CC /NOLIST/OBJECT=HTTP_EXECUTE.OBJ http_execute.c
$ CC /NOLIST/OBJECT=HTTP_SERVER.OBJ http_server.c
$ CC /NOLIST/OBJECT=IDENT_MAP.OBJ ident_map.c
$ CC /NOLIST/OBJECT=USER_INFO.OBJ user_info.c
$ CC /NOLIST/OBJECT=SEND_DOCUMENT.OBJ send_document.c
$ CC /NOLIST/OBJECT=SUFFIX_MAP.OBJ suffix_map.c
$ CC /NOLIST/OBJECT=PRESENTATION_MAP.OBJ presentation_map.c
$ CC /NOLIST/OBJECT=TLOGGER.OBJ tlogger.c
$ CC /NOLIST/OBJECT=RULE_FILE.OBJ rule_file.c
$ IF "TWGTCP" .eqs. "BSDTCP" .and. (F$GETSYI("CPU") .GE. 128 .OR. 	F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC") THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=TSERVER_TCP.OBJ /DEFINE=TWGTCP tserver_tcp.c 'prefix_all'
$ CC /NOLIST/OBJECT=TUTIL.OBJ tutil.c
$ CC /NOLIST/OBJECT=PROTECT.OBJ protect.c
$ CC /NOLIST/OBJECT=AUTHCOM.OBJ authcom.c
$ CC /NOLIST/OBJECT=DECNET_SEARCHLIST.OBJ decnet_searchlist.c
$ CC /NOLIST/OBJECT=MANAGE_REQUEST.OBJ manage_request.c
$ CC /NOLIST/OBJECT=LOAD_DYNAMIC.OBJ load_dynamic.c
$ CC /NOLIST/OBJECT=MESSAGE_SERVICE.OBJ message_service.c
$ CC /NOLIST/OBJECT=MAPIMAGE_MST.OBJ mapimage_mst.c
$ CC /NOLIST/OBJECT=COUNTERS.OBJ counters.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=CGIFORK_MST.OBJ cgifork_mst.c 'prefix_all'
$ @link_server.com TWGTCP /NOTRACEBACK/EXEC=HTTP_SERVER.EXE
$ COPY http_server.exe 'EXEDIR'
$ CC /NOLIST/OBJECT=CEL_AUTHENTICATOR.OBJ cel_authenticator.c
$ CC /NOLIST/OBJECT=AUTHLIB.OBJ authlib.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
$ LINK /NOTRACEBACK/EXEC=CEL_AUTHENTICATOR.EXE CEL_AUTHENTICATOR.OBJ,AUTHLIB.OBJ,CC_LIBS/OPT
$ COPY cel_authenticator.exe 'EXEDIR'
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=PRIVREQUEST.OBJ privrequest.c 'prefix_all'
$ @link_prequest.com TWGTCP
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=HTTP_DIRSERV.OBJ http_dirserv.c 'prefix_all'
$ CC /NOLIST/OBJECT=DIRSERV_RULES.OBJ dirserv_rules.c
$ IF F$GETSYI("CPU") .ge. 128 then inc_lib = "sys$share:sys$lib_c/library+sys$disk:[]"
$ CC /NOLIST/OBJECT=TSERVER_DECNET.OBJ 'inc_lib'tserver_decnet.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG PTHREAD_LIBS PTHREAD_LIBS_AXP
$ LINK /NOTRACEBACK/EXEC=HTTP_DIRSERV.EXE http_dirserv/opt,PTHREAD_LIBS/opt
$ COPY http_dirserv.exe 'EXEDIR'
$ CC /NOLIST/OBJECT=OMNIMAP_MST.OBJ omnimap_mst.c
$ CC /NOLIST/OBJECT=MST_SHARE.OBJ mst_share.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=FILE_ACCESS_D.OBJ /DEFINE=(DATE_FORMAT_850,DYNAMIC_MST) file_access.c 'prefix_all'/object=file_access_d.obj
$ @link_mst.com omnimap omnimap_mst,file_access_d
$ COPY omnimap_mst.exe 'EXEDIR'http_omnimap_mst.exe
$ CC /NOLIST/OBJECT=TESTCGI_MST.OBJ TESTCGI_MST.C
$ @link_mst.com testcgi testcgi_mst.obj
$ COPY testcgi_mst.exe 'EXEDIR'http_testcgi_mst.exe
$ curdef = f$environment("DEFAULT")
$ set default [-.documents]
$ if f$search("indexdemo.idx") .eqs. "" then @indexdemo_sel_idx.com
$ if f$file("indexdemo.idx","ORG") .nes. "IDX" then @indexdemo_sel_idx.com
$ set default 'curdef'
$ write sys$output "Base images built and index unpacked"
