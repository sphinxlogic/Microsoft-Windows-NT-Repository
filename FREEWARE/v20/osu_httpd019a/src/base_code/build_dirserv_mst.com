$ EXEDIR = "[-.system]"
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=DIRSERV_MST.OBJ dirserv_mst.c 'prefix_all'
$ CC /NOLIST/OBJECT=DIRSERV_RULES_D.OBJ/object=dirserv_rules_d.obj dirserv_rules.c/define=(DYNAMIC_MST)
$ CC /NOLIST/OBJECT=TUTIL.OBJ tutil.c
$ IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC"        	THEN prefix_all = "/prefix=all"
$ CC /NOLIST/OBJECT=FILE_ACCESS_D.OBJ /DEFINE=(DATE_FORMAT_850,DYNAMIC_MST) file_access.c 'prefix_all'/object=file_access_d.obj
$ CC /NOLIST/OBJECT=MST_SHARE.OBJ mst_share.c
$ @link_mst.com dirserv dirserv_mst.obj,dirserv_rules_d.obj,file_access_d.obj
$ COPY dirserv_mst.exe 'EXEDIR'http_dirserv_mst.exe
