$ EXEDIR = "[-.system]"
$ CC /NOLIST/OBJECT=TARGATE.OBJ targate.c
$ CC /NOLIST/OBJECT=TLOGGER.OBJ tlogger.c
$ CC /NOLIST/OBJECT=RULE_FILE_SS.OBJ/object=rule_file_ss.obj rule_file.c/define=SCRIPTSERVER
$ CC /NOLIST/OBJECT=TUTIL.OBJ tutil.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=TARSCAN.OBJ tarscan.c 'prefix_all'
$ CC /NOLIST/OBJECT=CLIENT_LIMIT.OBJ client_limit.c
$ CC /NOLIST/OBJECT=SUFFIX_MAP.OBJ suffix_map.c
$ CC /NOLIST/OBJECT=PRESENTATION_MAP.OBJ presentation_map.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=FILE_ACCESS.OBJ /DEFINE=(DATE_FORMAT_850) file_access.c 'prefix_all'
$ IF F$GETSYI("CPU") .ge. 128 then inc_lib = "sys$share:sys$lib_c/library+sys$disk:[]"
$ CC /NOLIST/OBJECT=TSERVER_DECNET.OBJ 'inc_lib'tserver_decnet.c
$ CC /NOLIST/OBJECT=DECNET_ACCESS.OBJ decnet_access.c
$ CC /NOLIST/OBJECT=DECNET_SEARCHLIST.OBJ decnet_searchlist.c
$ CC /NOLIST/OBJECT=IDENT_MAP.OBJ ident_map.c
$ CC /NOLIST/OBJECT=USER_INFO.OBJ user_info.c
$ CC /NOLIST/OBJECT=LOAD_DYNAMIC.OBJ load_dynamic.c
$ CC /NOLIST/OBJECT=COUNTERS.OBJ counters.c
$ CC /NOLIST/OBJECT=MESSAGE_SERVICE.OBJ message_service.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=CGIFORK_MST.OBJ cgifork_mst.c 'prefix_all'
$ CC /NOLIST/OBJECT=MAPIMAGE_MST.OBJ mapimage_mst.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" 	THEN DEFINE/NOLOG PTHREAD_LIBS PTHREAD_LIBS_AXP
$ LINK /NOTRACEBACK/EXEC=TARGATE.EXE targate/opt,PTHREAD_LIBS/opt
$ COPY targate.exe 'EXEDIR'
