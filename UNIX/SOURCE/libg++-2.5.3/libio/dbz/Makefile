# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = rdbz.c rdbzmain.c
host_alias = i386-unknown-sysv4.2
host_cpu = i386
host_vendor = unknown
host_os = sysv4.2
host_canonical = i386-unknown-sysv4.2
target_alias = i386-unknown-sysv4.2
target_cpu = i386
target_vendor = unknown
target_os = sysv4.2
target_canonical = i386-unknown-sysv4.2
srcdir = .
CFLAGS = -g
C_FLAGS = $(CFLAGS) -I$(srcdir) -I.. -I$(srcdir)/.. $(XTRAFLAGS) -DDBZ_FINISH='_IO_flush_all()'
CC = `if [ -f ../../../gcc/gcc ] ; \
	then echo ../../../gcc/gcc -B../../../gcc/ ; \
	else echo gcc ; fi`
LIBIO = ../libio.a ../../libiberty/libiberty.a
LIBS = $(LIBIO)
# LIBS = ../libcnews.a
DBM =
RFC = -DHAVERFCIZE
CASE = case.o
DEBUG = -DDBZDEBUG
LINTFLAGS = -h $(DEBUG) $(RFC) -I$(srcdir)
LDFLAGS =
# =()<NEWSBIN = @<NEWSBIN>@>()=
NEWSBIN = /usr/lib/newsbin
# workaround for System V make bug
SHELL = /bin/sh

# database sizes for performance tests, regression, and regression prime-find
TSIZE=12007
RSIZE=4019
RPSIZE=2679

#### host and target dependent Makefile fragments come in here.
##

# history files for regression and performance tests
RHIST=hist3.3
R2HIST=hist10
THIST=hist13

#all:	dbz
all:
install:
install-info:
info:

check: r rclean

bininstall:	dbz
	cp dbz $(NEWSBIN)

cmp:	dbz
	cmp dbz $(NEWSBIN)/dbz

newsinstall:
	: nothing

u:	dbz.o
	ar ruv ../libcnews.a dbz.o
	cmp dbz.h ../h/dbz.h

t:	tdbz fake

lint:
	lint $(LINTFLAGS) dbzmain.c dbz.c

.c.o:
	$(CC) $(C_FLAGS) -c $<

rdbz.o:	rdbz.c
	$(CC) $(C_FLAGS) $(DEBUG) -DDEFSIZE=$(RSIZE) -c rdbz.c

rdbzmain.o: rdbzmain.c
	$(CC) $(C_FLAGS) $(RFC) -c rdbzmain.c

tdbz.o:	$(srcdir)/dbz.c
	cp $(srcdir)/dbz.c tdbz.c
	$(CC) $(C_FLAGS) -DDEFSIZE=$(TSIZE) -c tdbz.c
	rm tdbz.c

dbz:	dbzmain.o $(CASE)
	$(CC) $(LDFLAGS) dbzmain.o $(CASE) $(PRE) $(DBM) $(LIBS) $(POST) -o $@

tdbz:	dbzmain.o tdbz.o $(CASE)
	$(CC) $(LDFLAGS) dbzmain.o tdbz.o $(CASE) $(PRE) $(LIBS) $(POST) -o $@

rdbz:	rdbzmain.o rdbz.o $(CASE)
	$(CC) $(LDFLAGS) rdbzmain.o rdbz.o $(CASE) $(PRE) $(LIBS) $(POST) -o $@

fake:	fake.o random.o
	$(CC) $(LDFLAGS) fake.o random.o $(PRE) $(LIBS) $(POST) -o $@

byteflip:	byteflip.o
	$(CC) $(LDFLAGS) byteflip.o $(PRE) $(LIBS) $(POST) -o $@

hist10:	fake
	./fake -t -e 75 10000 >$@

hist3.3:	fake
	./fake -t -e 75 3300 >$@

hist13:	fake
	./fake -t -e 75 13000 >$@

r:	rdbz $(RHIST) $(R2HIST) byteflip $(srcdir)/getmap $(srcdir)/revbytes $(srcdir)/altbytes stamp-r8
	: success!

stamp-r0:
	: 'WARNING:  creates about 2MB of debris; do "make rclean" afterward'
	rm -f dbase dbase[23] dbase.* dbase[23].*
	test ! -d xx || rmdir xx
	@touch stamp-r0

stamp-r1: stamp-r0
	: crude check of synthetic history file
	( sed 25q $(RHIST) ; tail -25 $(RHIST) ) >histjunk
	cmp histjunk $(srcdir)/firstlast25
	rm histjunk
	@touch stamp-r1

r2a: stamp-r1
	: basic tests, exercising as many options as possible
	cp $(RHIST) dbase
	mkdir xx
	chmod -w xx
	./rdbz -E 1000 -0 -M -i -S -u -U -C xx dbase
	rmdir xx
	sed '/>	0/d' $(RHIST) >dbase.used
	test "`cat dbase.used | wc -l`" -eq "`sed -n '2s/ .*//p' dbase.dir`" ;

stamp-r2: r2a
	cp $(RHIST) dbase2
	./rdbz -E 1000 -0 -p $(RPSIZE) -t '	' dbase2
	cmp $(RHIST) dbase
	cmp dbase dbase2
	cmp dbase.dir dbase2.dir
	cmp dbase.pag dbase2.pag
	./rdbz -E 1000 -0 -c dbase
	./rdbz -E 1000 -0 -c -i -q -M -U dbase
	@touch stamp-r2

stamp-r3: stamp-r2
	: build a database and then add to it
	sed 1000q $(RHIST) >dbase2
	sed 1,1000d $(RHIST) >dbase2.add
	./rdbz -E 1000 -0 dbase2
	./rdbz -E 1000 -0 -a dbase2 dbase2.add
	cmp dbase dbase2
	cmp dbase.dir dbase2.dir
	cmp dbase.pag dbase2.pag
	@touch stamp-r3

stamp-r4: stamp-r3
	: build based on existing one, test extraction and readonly files
	./rdbz -E 1000 -0 -f dbase dbase2
	test "`cat dbase.used | wc -l`" -eq "`awk 'NR==2{print $$1}' dbase2.dir`" ;
	test "`cat dbase.used | wc -l`" -eq "`awk 'NR==2{print $$2}' dbase2.dir`" ;
	chmod -w dbase2.dir dbase2.pag
	./rdbz -E 1000 -x dbase2 dbase >dbase.temp
	cmp dbase.used dbase.temp
	@touch stamp-r4

stamp-r5: stamp-r4
	: try some small case perversions
	sed 's/\(@[^ 	]*\)A/\1a/' dbase >dbase.ick
	./rdbz -E 1000 -x dbase2 dbase.ick >dbase.temp
	cmp dbase.used dbase.temp
	sed -n 's/A\([^ 	]*@\)/a\1/p' dbase >dbase.ick
	./rdbz -x dbase2 dbase.ick >dbase.temp
	test ! -s dbase.temp ;
	rm -f dbase2.dir dbase2.pag
	@touch stamp-r5

stamp-r6: stamp-r5
	: try it without tags, case-insensitive, with case perversions
	./rdbz -E 1000 -0 -p '0 b 1' dbase2
	tr '[A-M][n-z]' '[a-m][N-Z]' <dbase2 >dbase.ick
	./rdbz -E 1000 -x dbase2 dbase.ick >dbase.temp
	cmp dbase.used dbase.temp
	rm -f dbase.temp dbase.ick
	@touch stamp-r6

stamp-r7: stamp-r6
	: test various perversions of byte ordering
	awk -f $(srcdir)/revbytes dbase.dir >dbase2.dir
	./byteflip `$(srcdir)/getmap dbase.dir` `$(srcdir)/getmap dbase2.dir` <dbase.pag >dbase2.pag
	cp dbase dbase2
	./rdbz -E 1000 -0 -c dbase2
	awk -f $(srcdir)/altbytes dbase.dir >dbase2.dir
	dd conv=swab <dbase.pag >dbase2.pag
	./rdbz -E 1000 -0 -c dbase2
	cp dbase2 dbase3
	./rdbz -E 1000 -0 -f dbase2 dbase3
	./rdbz -E 1000 -0 -c dbase3
	test " `$(srcdir)/getmap dbase2.dir`" = " `$(srcdir)/getmap dbase3.dir`" ;
	@touch stamp-r7

stamp-r8: stamp-r7
	: test massive overflow, throw in case sensitivity and tag mask
	cp $(R2HIST) dbase
	./rdbz -E 1000 -0 -p '0 0 7ffc0000' dbase
	./rdbz -E 1000 -0 -cq dbase
	sed 100q dbase | egrep '[aA].*	' | tr aA Aa >dbase.ick
	./rdbz -x dbase dbase.ick >dbase.temp
	test ! -s dbase.temp ;
	@touch stamp-r8

rclean:
	rm -f dbase dbase[23] dbase.* dbase[23].* fake fake.o random.o
	rm -f rdbz rdbz.o rdbzmain.o $(RHIST) $(R2HIST) byteflip byteflip.o
	rm -f histjunk core stamp-r? *~
	test ! -d xx || rmdir xx

clean:	rclean
	rm -f *.o dbz [a-z]dbz [a-z][a-z]dbz junk* PostScript.out
	rm -f hist* dbase* *.bak mon.out gmon.out core dbm.h

distclean: clean
	rm -rf Makefile config.status rdbz.c rdbzmain.c

realclean: distclean
