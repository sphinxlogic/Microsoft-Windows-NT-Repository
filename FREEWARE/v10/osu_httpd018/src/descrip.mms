!
!  MMS description file for builing DECThreads HTTP SERVER
!
!  The default TCP package is UCX.  To change it, invoke mms as
!     $ MMS/MACRO=TCP=xxxx
!
!  To build a server with traceback information (for debugging), invoke mms as
!     $ MMS/FORCE/MACRO=TRACE=1
!
!  To build a server using a shareable image for the TCP-code, invoke mms as
!     $ MMS/MACRO=(SHARE_TCP=xxxx)
!
!  To build a server with debugging enabled, invoke mms as 
!     $ MMS/MACRO=DEBUG=1
!
.IFDEF SHARE_TCP
TCP = $(SHARE_TCP)
TCPLINK = SHARE
TCPMODULE = tserver_tcpshr.exe
.ELSE
.IFDEF TCP
TCPLINK = $(TCP)
.ELSE
TCPLINK = UCXTCP
.ENDIF
TCPMODULE = tserver_tcp.obj
.ENDIF
.IFDEF DATE_FORMAT
DFMT = /DEFINE=($(DATE_FORMAT))
.ELSE
DFMT = /DEFINE=(DATE_FORMAT_850)
.ENDIF
!
.IFDEF TCP
TCPOPT = /DEFINE=$(TCP)
.ELSE
TCP = UCXTCP
TCPOPT = /DEFINE=(UCX=1,UCXTCP=1)
.ENDIF
!
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

CORE_IMAGES : http_server.exe,cel_authenticator.exe,privrequest.exe,\
		http_dirserv.exe
        @ set default [.documents]
	@ if f$search("indexdemo.idx") .eqs. "" then @indexdemo_sel_idx.com
        @ if f$file("indexdemo.idx","ORG") .nes. "IDX" then @indexdemo_sel_idx.com
        @ set default [-]
        @ write sys$output "Base images built and index unpacked"

HTTP_SERVER.EXE : client_limit.obj, decnet_access.obj, script_execute.obj,\
	document_cache.obj, file_access.obj, http_execute.obj,http_server.obj,\
	ident_map.obj,user_info.obj,send_document.obj,suffix_map.obj,\
	presentation_map.obj,tlogger.obj,rule_file.obj,$(TCPMODULE),\
	tutil.obj,protect.obj,HTTP_SERVER.OPT,authcom.obj,link_server.com,\
	decnet_searchlist.obj,manage_request.obj,load_dynamic.obj
    @link_server.com $(TCPLINK) $(S_LINKFLAGS)

TSERVER_TCPSHR.EXE : tserver_tcp.obj,tutil.obj
    @link_tcpshare.com $(TCP) $(S_LINKFLAGS)

PRIVREQUEST.EXE : privrequest.obj,link_prequest.com
    @link_prequest.com

SAMPLE_AUTHENTICATOR.EXE : sample_authenticator.obj, authlib.obj
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
    LINK $(LINKFLAGS) SAMPLE_AUTHENTICATOR.OBJ,AUTHLIB.OBJ,CC_LIBS/OPT

CEL_AUTHENTICATOR.EXE : cel_authenticator.obj, authlib.obj
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG CC_LIBS CC_LIBS_AXP
    LINK $(LINKFLAGS) CEL_AUTHENTICATOR.OBJ,AUTHLIB.OBJ,CC_LIBS/OPT

TARGATE.EXE : targate.obj,tlogger.obj,rule_file.obj,tutil.obj,tarscan.obj,\
	client_limit.obj,suffix_map.obj,presentation_map.obj,file_access.obj,\
	tserver_decnet.obj,decnet_access.obj,decnet_searchlist.obj,\
	ident_map.obj,user_info.obj,targate.opt
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG PTHREAD_LIBS PTHREAD_LIBS_AXP
    LINK $(LINKFLAGS) targate/opt,PTHREAD_LIBS/opt

http_dirserv.exe : http_dirserv.obj,tlogger.obj,tutil.obj,client_limit.obj,\
	dirserv_rules.obj,file_access.obj,tserver_decnet.obj,http_dirserv.opt
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
	THEN DEFINE/NOLOG PTHREAD_LIBS PTHREAD_LIBS_AXP
    LINK $(S_LINKFLAGS) http_dirserv/opt,PTHREAD_LIBS/opt


session.h : access.h,tutil.h
    SET FILE session.h/ext=0

authcom.obj : authcom.c,authcom.h,pthread_np.h,access.h,tutil.h,tserver_tcp.h
    CC $(CFLAGS) authcom.c

client_limit.obj : client_limit.c
    CC $(CFLAGS) client_limit.c

decnet_access.obj : decnet_access.c,decnet_access.h,tutil.h,pthread_np.h
    CC $(CFLAGS) decnet_access.c

script_execute.obj : script_execute.c,decnet_searchlist.h,decnet_access.h,\
	pthread_np.h,session.h,tserver_tcp.h
    CC $(CFLAGS) script_execute.c

decnet_searchlist.obj : decnet_searchlist.c,decnet_searchlist.h,tutil.h,pthread_np.h
    CC $(CFLAGS) decnet_searchlist.c

document_cache.obj : document_cache.c,tutil.h,pthread_np.h
    CC $(CFLAGS) document_cache.c

file_access.obj : file_access.c,file_access.h,pthread_np.h
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) $(DFMT) file_access.c 'prefix_all'

http_execute.obj : http_execute.c,session.h,tserver_tcp.h
    CC $(CFLAGS) http_execute.c

http_server.obj : http_server.c,tutil.h,pthread_np.h,decnet_searchlist.h,tserver_tcp.h
    CC $(CFLAGS) http_server.c

ident_map.obj : ident_map.c,ident_map.h,tutil.h access.h
    CC $(CFLAGS) ident_map.c

load_dynamic.obj : load_dynamic.c,ident_map.h,access.h,file_access.h
    CC $(CFLAGS) load_dynamic.c

user_info.obj : user_info.c,tutil.h,pthread_np.h
    CC $(CFLAGS) user_info.c

send_document.obj : send_document.c,file_access.h,session.h,tserver_tcp.h
    CC $(CFLAGS) send_document.c

suffix_map.obj : suffix_map.c,tutil.h
    CC $(CFLAGS) suffix_map.c

presentation_map.obj : presentation_map.c,tutil.h
    CC $(CFLAGS) presentation_map.c

rule_file.obj : rule_file.c,ident_map.h,access.h,tutil.h,decnet_searchlist.h,\
	tserver_tcp.h
    CC $(CFLAGS) rule_file.c

tlogger.obj : tlogger.c,pthread_np.h
    CC $(CFLAGS) tlogger.c

targate.obj : targate.c,pthread_np.h,tutil.h,file_access.h,tarscan.h,\
	decnet_searchlist.h
    CC $(CFLAGS) targate.c

http_dirserv.obj : http_dirserv.c,pthread_np.h,tutil.h,tserver_decnet.h,\
	file_access.h,decnet_searchlist.h,dirserv_options.h
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) http_dirserv.c 'prefix_all'

dirserv_rules.obj : dirserv_rules.c,pthread_np.h,tutil.h,file_access.h,\
	tserver_decnet.h,dirserv_options.h
    CC $(CFLAGS) dirserv_rules.c

tarscan.obj : tarscan.c,tarscan.h,tutil.h
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) tarscan.c 'prefix_all'

tserver_decnet.obj : tserver_decnet.c,pthread_np.h,tutil.h
   IF F$GETSYI("CPU") .ge. 128 then inc_lib = "sys$share:sys$lib_c/library+sys$disk:[]"
    CC $(CFLAGS) 'inc_lib'tserver_decnet.c

tserver_tcp.obj : tserver_tcp.c,pthread_np.h,tutil.h,tserver_tcp.h
    CC $(CFLAGS) $(TCPOPT) tserver_tcp.c

tutil.obj : tutil.c,tutil.h
    CC $(CFLAGS) tutil.c

protect.obj : protect.c,file_access.h,session.h,tserver_tcp.h
    CC $(CFLAGS) protect.c

sample_authenticator.obj : sample_authenticator.c,authlib.h
    CC $(CFLAGS) sample_authenticator.c

cel_authenticator.obj : cel_authenticator.c,authlib.h
    CC $(CFLAGS) cel_authenticator.c

authlib.obj : authlib.c,authlib.h,authcom.h
    CC $(CFLAGS) authlib.c

manage_request.obj : manage_request.c,tutil.h,tserver_tcp.h
    CC $(CFLAGS) manage_request.c

privrequest.obj : privrequest.c
    IF F$GETSYI("CPU") .GE. 128 .OR. F$TRNLNM("DECC$CC_DEFAULT") .EQS. "/DECC" -
       	THEN prefix_all = "/prefix=all"
    CC $(CFLAGS) privrequest.c 'prefix_all'
