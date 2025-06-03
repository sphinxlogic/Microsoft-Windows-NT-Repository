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
$ CC /NOLIST/OBJECT=TSERVER_TCP.OBJ /DEFINE=TCPWARE tserver_tcp.c
$ CC /NOLIST/OBJECT=TUTIL.OBJ tutil.c
$ CC /NOLIST/OBJECT=PROTECT.OBJ protect.c
$ CC /NOLIST/OBJECT=AUTHCOM.OBJ authcom.c
$ CC /NOLIST/OBJECT=DECNET_SEARCHLIST.OBJ decnet_searchlist.c
$ CC /NOLIST/OBJECT=MANAGE_REQUEST.OBJ manage_request.c
$ CC /NOLIST/OBJECT=LOAD_DYNAMIC.OBJ load_dynamic.c
$ @link_server.com TCPWARE /NOTRACE/EXEC=HTTP_SERVER.EXE
$ CC /NOLIST/OBJECT=CEL_AUTHENTICATOR.OBJ cel_authenticator.c
$ CC /NOLIST/OBJECT=AUTHLIB.OBJ authlib.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
$ LINK /EXEC=CEL_AUTHENTICATOR.EXE CEL_AUTHENTICATOR.OBJ,AUTHLIB.OBJ,CC_LIBS/OPT
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=PRIVREQUEST.OBJ privrequest.c 'prefix_all'
$ @link_prequest.com
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=HTTP_DIRSERV.OBJ http_dirserv.c 'prefix_all'
$ CC /NOLIST/OBJECT=DIRSERV_RULES.OBJ dirserv_rules.c
$ IF F$GETSYI("CPU") .ge. 128 then inc_lib = "sys$share:sys$lib_c/library+sys$disk:[]"
$ CC /NOLIST/OBJECT=TSERVER_DECNET.OBJ 'inc_lib'tserver_decnet.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG PTHREAD_LIBS PTHREAD_LIBS_AXP
$ LINK /NOTRACE/EXEC=HTTP_DIRSERV.EXE http_dirserv/opt,PTHREAD_LIBS/opt
$ set default [.documents]
$ if f$search("indexdemo.idx") .eqs. "" then @indexdemo_sel_idx.com
$ if f$file("indexdemo.idx","ORG") .nes. "IDX" then @indexdemo_sel_idx.com
$ set default [-]
$ write sys$output "Base images built and index unpacked"
