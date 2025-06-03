#compile = gcc -c -fdollars-in-identifiers -I /usr/include/netinet/ -I /usr/include/sys/
compile = cc -call_shared -c -threads
load = ld
bindir = ../sysosf

objs = http_server.o tlogger.o rule_file.o tutil.o suffix_map.o \
	ident_map.o manage_request.o ss_emul.o load_dynamic.o authcom.o\
	http_execute.o send_document.o file_access_unix.o tserver_tcp.o \
	script_execute.o message_service.o mapimage_mst.o document_cache.o \
	presentation_map.o protect.o counters.o cgifork_mst.o

httpd : makefile $(objs) privrequest testcgi_mst omnimap_mst
	cc  -o httpd $(objs)  -lm -threads
	cp httpd $(bindir)/httpd
	chmod a+x $(bindir)/*.csh

privrequest :  privrequest.c
	cc privrequest.c -o privrequest
	cp privrequest $(bindir)/privrequest

session.h : access.h tutil.h
	touch session.h

tserver_tcp.o : tserver_tcp_sockets.c pthread_np.h tutil.h tserver_tcp.h
	$(compile) -o tserver_tcp.o tserver_tcp_sockets.c

http_server.o : http_server.c tutil.h pthread_np.h tserver_tcp.h \
	decnet_searchlist.h counters.h
	$(compile) http_server.c

tlogger.o : tlogger.c pthread_np.h
	$(compile) tlogger.c

rule_file.o : rule_file.c access.h tutil.h tserver_tcp.h counters.h \
	decnet_searchlist.h message_service.h
	$(compile) rule_file.c

tutil.o : tutil.c tutil.h
	$(compile) tutil.c

suffix_map.o : suffix_map.c tutil.h
	$(compile) suffix_map.c

ident_map.o : ident_map.c tutil.h access.h
	$(compile) ident_map.c

load_dynamic.o : load_dynamic.c ident_map.h access.h file_access.h
	$(compile) load_dynamic.c

dirserv_rules_d.o : dirserv_rules.c tutil.h file_access.h pthread_np.h \
	dirserv_options.h
	$(compile) -o dirserv_rules_d.o -DDYNAMIC_MST dirserv_rules.c

file_access_unix.o : file_access_unix.c file_access.h pthread_np.h
	$(compile) file_access_unix.c

file_access_unix_d.o : file_access_unix.c file_access.h pthread_np.h
	$(compile) -o file_access_unix_d.o -DDYNAMIC_MST file_access_unix.c

manage_request.o : manage_request.c tutil.h tserver_tcp.h
	$(compile) manage_request.c

ss_emul.o : ss_emul.c
	$(compile) ss_emul.c

http_execute.o : http_execute.c session.h tserver_tcp.h
	$(compile) http_execute.c

send_document.o : send_document.c file_access.h tutil.h tserver_tcp.h
	$(compile) send_document.c

script_execute.o : script_execute.c decnet_searchlist.h decnet_access.h \
	pthread_np.h session.h tserver_tcp.h message_service.h
	$(compile) script_execute.c

message_service.o : message_service.c message_service.h tutil.h pthread_np.h
	$(compile) message_service.c

mapimage_mst.o : mapimage_mst.c message_service.h tutil.h file_access.h
	$(compile) mapimage_mst.c

cgifork_mst.o : cgifork_mst.c message_service.h tutil.h file_access.h
	$(compile) cgifork_mst.c

document_cache.o : document_cache.c tutil.h pthread_np.h
	$(compile) document_cache.c

presentation_map.o : presentation_map.c tutil.h
	$(compile) presentation_map.c

protect.o : protect.c file_access.h session.h tserver_tcp.h
	$(compile) protect.c

counters.o : counters.c counters.h
	$(compile) counters.c

authcom.o : authcom.c authcom.h pthread_np.h access.h tutil.h tserver_tcp.h
	$(compile) authcom.c

testcgi_mst.o : testcgi_mst.c mst_share.h message_service.h tutil.h
	$(compile) testcgi_mst.c

omnimap_mst.o : omnimap_mst.c mst_share.h message_service.h tutil.h \
        file_access.h
	$(compile) omnimap_mst.c

mst_share.o : mst_share.c tutil.h mst_share.h message_service.h
	$(compile) mst_share.c

dirserv_mst.o : dirserv_mst.c mst_share.h pthread_np.h tutil.h \
	file_access.h dirserv_options.h
	$(compile) dirserv_mst.c

fetch : makefile fetch_http.c
	cc -o fetch fetch_http.c

testcgi_mst : testcgi_mst.o makefile mst_share.o tutil.o
	ld -o testcgi_mst -shared testcgi_mst.o mst_share.o tutil.o \
	-expect_unresolved '*' -lmach -lm -lc_r -lpthreads
	cp testcgi_mst $(bindir)/testcgi_mst

omnimap_mst : omnimap_mst.o makefile mst_share.o tutil.o file_access_unix_d.o
	ld -o omnimap_mst -shared omnimap_mst.o mst_share.o tutil.o \
	file_access_unix_d.o -expect_unresolved '*' -lmach -lm -lc_r -lpthreads
	cp omnimap_mst $(bindir)/omnimap_mst

dirserv_mst : dirserv_mst.o dirserv_rules_d.o tutil.o file_access_unix_d.o \
	mst_share.o makefile
	ld -o dirserv_mst -shared dirserv_mst.o dirserv_rules_d.o tutil.o \
	file_access_unix_d.o mst_share.o -expect_unresolved '*' -lmach -lm \
	-lc_r -lpthreads
	cp dirserv_mst $(bindir)/dirserv_mst
