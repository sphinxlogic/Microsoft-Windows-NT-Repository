# : Makefile.SH,v 7029Revision: 4.0.1.4 7029Date: 92/06/08 11:40:43 $
#
# $Log:	Makefile.SH,v $
# Revision 4.0.1.4  92/06/08  11:40:43  lwall
# patch20: cray didn't give enough memory to /bin/sh
# patch20: various and sundry fixes
# 
# Revision 4.0.1.3  91/11/05  15:48:11  lwall
# patch11: saberized perl
# patch11: added support for dbz
# 
# Revision 4.0.1.2  91/06/07  10:14:43  lwall
# patch4: cflags now emits entire cc command except for the filename
# patch4: alternate make programs are now semi-supported
# patch4: uperl.o no longer tries to link in libraries prematurely
# patch4: installperl now installs x2p stuff too
# 
# Revision 4.0.1.1  91/04/11  17:30:39  lwall
# patch1: C flags are now settable on a per-file basis
# 
# Revision 4.0  91/03/20  00:58:54  lwall
# 4.0 baseline.
# 
# 

CC = gcc
YACC = bison -y
bin = /opt/bin
scriptdir = /opt/bin
privlib = /opt/lib/perl
mansrc = /opt/man/man1
manext = 1
LDFLAGS = 
CLDFLAGS = 
SMALL = 
LARGE =  
mallocsrc = 
mallocobj = 
SLN = ln -s
RMS = rm -f

libs = -L/usr/ucblib -lsocket -lnsl -ldbm -lmalloc -lm -lx -lc -lucb 

public = perl taintperl 

shellflags = 

# To use an alternate make, set  in config.sh.
MAKE = make


CCCMD = `sh $(shellflags) cflags $@`

private = 

scripts = h2ph

manpages = perl.man h2ph.man

util =

sh = Makefile.SH makedepend.SH h2ph.SH

h1 = EXTERN.h INTERN.h arg.h array.h cmd.h config.h form.h handy.h
h2 = hash.h perl.h regcomp.h regexp.h spat.h stab.h str.h util.h

h = $(h1) $(h2)

c1 = array.c cmd.c cons.c consarg.c doarg.c doio.c dolist.c dump.c
c2 = eval.c form.c hash.c $(mallocsrc) perl.c regcomp.c regexec.c
c3 = stab.c str.c toke.c util.c usersub.c

c = $(c1) $(c2) $(c3)

s1 = array.c cmd.c cons.c consarg.c doarg.c doio.c dolist.c dump.c
s2 = eval.c form.c hash.c perl.c regcomp.c regexec.c
s3 = stab.c str.c toke.c util.c usersub.c perly.c

saber = $(s1) $(s2) $(s3)

obj1 = array.o cmd.o cons.o consarg.o doarg.o doio.o dolist.o dump.o
obj2 = eval.o form.o $(mallocobj) perl.o regcomp.o regexec.o
obj3 = stab.o str.o toke.o util.o

obj = $(obj1) $(obj2) $(obj3)

tobj1 = tarray.o tcmd.o tcons.o tconsarg.o tdoarg.o tdoio.o tdolist.o tdump.o
tobj2 = teval.o tform.o thash.o $(mallocobj) tregcomp.o tregexec.o
tobj3 = tstab.o tstr.o ttoke.o tutil.o

tobj = $(tobj1) $(tobj2) $(tobj3)

lintflags = -hbvxac

addedbyconf = Makefile.old bsd eunice filexp loc pdp11 usg v7

# grrr
SHELL = /bin/sh

.c.o:
	$(CCCMD) $*.c

all: $(public) $(private) $(util) uperl.o $(scripts)
	cd x2p; $(MAKE) all
	touch all

# This is the standard version that contains no "taint" checks and is
# used for all scripts that aren't set-id or running under something set-id.
# The $& notation is tells Sequent machines that it can do a parallel make,
# and is harmless otherwise.

perl: $& perly.o $(obj) hash.o usersub.o
	$(CC) $(LARGE) $(CLDFLAGS) $(obj) hash.o perly.o usersub.o $(libs) -o perl

# This command assumes that /usr/include/dbz.h and /usr/lib/dbz.o exist.

dbzperl: $& perly.o $(obj) zhash.o usersub.o
	$(CC) $(LARGE) $(CLDFLAGS) $(obj) zhash.o /usr/lib/dbz.o perly.o usersub.o $(libs) -o dbzperl

zhash.o: hash.c $(h)
	$(RMS) zhash.c
	$(SLN) hash.c zhash.c
	$(CCCMD) -DWANT_DBZ zhash.c
	$(RMS) zhash.c

uperl.o: $& perly.o $(obj) hash.o
	-ld $(LARGE) $(LDFLAGS) -r $(obj) hash.o perly.o -o uperl.o

saber: $(saber)
	# load $(saber)
	# load /lib/libm.a

# This version, if specified in Configure, does ONLY those scripts which need
# set-id emulation.  Suidperl must be setuid root.  It contains the "taint"
# checks as well as the special code to validate that the script in question
# has been invoked correctly.

suidperl: $& tperly.o sperl.o $(tobj) usersub.o
	$(CC) $(LARGE) $(CLDFLAGS) sperl.o $(tobj) tperly.o usersub.o $(libs) \
	    -o suidperl

# This version interprets scripts that are already set-id either via a wrapper
# or through the kernel allowing set-id scripts (bad idea).  Taintperl must
# NOT be setuid to root or anything else.  The only difference between it
# and normal perl is the presence of the "taint" checks.

taintperl: $& tperly.o tperl.o $(tobj) usersub.o
	$(CC) $(LARGE) $(CLDFLAGS) tperl.o $(tobj) tperly.o usersub.o $(libs) \
	    -o taintperl

# Replicating all this junk is yucky, but I don't see a portable way to fix it.

tperly.o: perly.c perly.h $(h)
	$(RMS) tperly.c
	$(SLN) perly.c tperly.c
	$(CCCMD) -DTAINT tperly.c
	$(RMS) tperly.c

tperl.o: perl.c perly.h patchlevel.h perl.h $(h)
	$(RMS) tperl.c
	$(SLN) perl.c tperl.c
	$(CCCMD) -DTAINT tperl.c
	$(RMS) tperl.c

sperl.o: perl.c perly.h patchlevel.h $(h)
	$(RMS) sperl.c
	$(SLN) perl.c sperl.c
	$(CCCMD) -DTAINT -DIAMSUID sperl.c
	$(RMS) sperl.c

tarray.o: array.c $(h)
	$(RMS) tarray.c
	$(SLN) array.c tarray.c
	$(CCCMD) -DTAINT tarray.c
	$(RMS) tarray.c

tcmd.o: cmd.c $(h)
	$(RMS) tcmd.c
	$(SLN) cmd.c tcmd.c
	$(CCCMD) -DTAINT tcmd.c
	$(RMS) tcmd.c

tcons.o: cons.c $(h) perly.h
	$(RMS) tcons.c
	$(SLN) cons.c tcons.c
	$(CCCMD) -DTAINT tcons.c
	$(RMS) tcons.c

tconsarg.o: consarg.c $(h)
	$(RMS) tconsarg.c
	$(SLN) consarg.c tconsarg.c
	$(CCCMD) -DTAINT tconsarg.c
	$(RMS) tconsarg.c

tdoarg.o: doarg.c $(h)
	$(RMS) tdoarg.c
	$(SLN) doarg.c tdoarg.c
	$(CCCMD) -DTAINT tdoarg.c
	$(RMS) tdoarg.c

tdoio.o: doio.c $(h)
	$(RMS) tdoio.c
	$(SLN) doio.c tdoio.c
	$(CCCMD) -DTAINT tdoio.c
	$(RMS) tdoio.c

tdolist.o: dolist.c $(h)
	$(RMS) tdolist.c
	$(SLN) dolist.c tdolist.c
	$(CCCMD) -DTAINT tdolist.c
	$(RMS) tdolist.c

tdump.o: dump.c $(h)
	$(RMS) tdump.c
	$(SLN) dump.c tdump.c
	$(CCCMD) -DTAINT tdump.c
	$(RMS) tdump.c

teval.o: eval.c $(h)
	$(RMS) teval.c
	$(SLN) eval.c teval.c
	$(CCCMD) -DTAINT teval.c
	$(RMS) teval.c

tform.o: form.c $(h)
	$(RMS) tform.c
	$(SLN) form.c tform.c
	$(CCCMD) -DTAINT tform.c
	$(RMS) tform.c

thash.o: hash.c $(h)
	$(RMS) thash.c
	$(SLN) hash.c thash.c
	$(CCCMD) -DTAINT thash.c
	$(RMS) thash.c

tregcomp.o: regcomp.c $(h)
	$(RMS) tregcomp.c
	$(SLN) regcomp.c tregcomp.c
	$(CCCMD) -DTAINT tregcomp.c
	$(RMS) tregcomp.c

tregexec.o: regexec.c $(h)
	$(RMS) tregexec.c
	$(SLN) regexec.c tregexec.c
	$(CCCMD) -DTAINT tregexec.c
	$(RMS) tregexec.c

tstab.o: stab.c $(h)
	$(RMS) tstab.c
	$(SLN) stab.c tstab.c
	$(CCCMD) -DTAINT tstab.c
	$(RMS) tstab.c

tstr.o: str.c $(h) perly.h
	$(RMS) tstr.c
	$(SLN) str.c tstr.c
	$(CCCMD) -DTAINT tstr.c
	$(RMS) tstr.c

ttoke.o: toke.c $(h) perly.h
	$(RMS) ttoke.c
	$(SLN) toke.c ttoke.c
	$(CCCMD) -DTAINT ttoke.c
	$(RMS) ttoke.c

tutil.o: util.c $(h)
	$(RMS) tutil.c
	$(SLN) util.c tutil.c
	$(CCCMD) -DTAINT tutil.c
	$(RMS) tutil.c

perly.h: perly.c
	@ echo Dummy dependency for dumb parallel make
	touch perly.h

perly.c: perly.y perly.fixer
	@ \
case "$(YACC)" in \
    *bison*) echo 'Expect' 25 shift/reduce and 59 reduce/reduce conflicts;; \
    *) echo 'Expect' 27 shift/reduce and 57 reduce/reduce conflicts;; \
esac
	$(YACC) -d perly.y
	sh $(shellflags) ./perly.fixer y.tab.c perly.c
	mv y.tab.h perly.h
	echo 'extern YYSTYPE yylval;' >>perly.h

perly.o: perly.c perly.h $(h)
	$(CCCMD) perly.c

install: all
	./perl installperl

clean:
	rm -f *.o all perl taintperl suidperl perly.c
	cd x2p; $(MAKE) clean

realclean: clean
	cd x2p; $(MAKE) realclean
	rm -f *.orig */*.orig *~ */*~ core $(addedbyconf) h2ph h2ph.man
	rm -f perly.c perly.h t/perl Makefile config.h makedepend makedir
	rm -f makefile x2p/Makefile x2p/makefile cflags x2p/cflags
	rm -f c2ph pstruct

# The following lint has practically everything turned on.  Unfortunately,
# you have to wade through a lot of mumbo jumbo that can't be suppressed.
# If the source file has a /*NOSTRICT*/ somewhere, ignore the lint message
# for that spot.

lint: perly.c $(c)
	lint $(lintflags) $(defs) perly.c $(c) > perl.fuzz

depend: makedepend
	- test -f perly.h || cp /dev/null perly.h
	./makedepend
	- test -s perly.h || /bin/rm -f perly.h
	cd x2p; $(MAKE) depend

test: perl
	- cd t && chmod +x TEST */*.t
	- cd t && (rm -f perl; $(SLN) ../perl perl) && ./perl TEST </dev/tty

clist:
	echo $(c) | tr ' ' '\012' >.clist

hlist:
	echo $(h) | tr ' ' '\012' >.hlist

shlist:
	echo $(sh) | tr ' ' '\012' >.shlist

# AUTOMATICALLY GENERATED MAKE DEPENDENCIES--PUT NOTHING BELOW THIS LINE
# If this runs make out of memory, delete /usr/include lines.
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
array.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
array.o: /usr/include/dirent.h
array.o: /usr/include/errno.h
array.o: /usr/include/netinet/in.h
array.o: /usr/include/sys/cred.h
array.o: /usr/include/sys/dirent.h
array.o: /usr/include/sys/errno.h
array.o: /usr/include/sys/filio.h
array.o: /usr/include/sys/ioccom.h
array.o: /usr/include/sys/poll.h
array.o: /usr/include/sys/reg.h
array.o: /usr/include/sys/regset.h
array.o: /usr/include/sys/select.h
array.o: /usr/include/sys/siginfo.h
array.o: /usr/include/sys/sockio.h
array.o: /usr/include/sys/stream.h
array.o: /usr/include/sys/strmdep.h
array.o: /usr/include/sys/time.h
array.o: /usr/include/sys/times.h
array.o: /usr/include/sys/tss.h
array.o: /usr/include/sys/ttold.h
array.o: /usr/include/sys/ttydev.h
array.o: /usr/include/sys/ucontext.h
array.o: /usr/include/sys/vnode.h
array.o: /usr/ucbinclude/ndbm.h
array.o: /usr/ucbinclude/setjmp.h
array.o: /usr/ucbinclude/stdio.h
array.o: /usr/ucbinclude/sys/ioctl.h
array.o: /usr/ucbinclude/sys/param.h
array.o: /usr/ucbinclude/sys/signal.h
array.o: /usr/ucbinclude/sys/sysmacros.h
array.o: /usr/ucbinclude/sys/ttychars.h
array.o: /usr/ucbinclude/sys/types.h
array.o: EXTERN.h
array.o: arg.h
array.o: array.c
array.o: array.h
array.o: cmd.h
array.o: config.h
array.o: form.h
array.o: handy.h
array.o: hash.h
array.o: perl.h
array.o: regexp.h
array.o: spat.h
array.o: stab.h
array.o: str.h
array.o: util.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
cmd.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/varargs.h
cmd.o: /usr/include/dirent.h
cmd.o: /usr/include/errno.h
cmd.o: /usr/include/netinet/in.h
cmd.o: /usr/include/sys/cred.h
cmd.o: /usr/include/sys/dirent.h
cmd.o: /usr/include/sys/errno.h
cmd.o: /usr/include/sys/filio.h
cmd.o: /usr/include/sys/ioccom.h
cmd.o: /usr/include/sys/poll.h
cmd.o: /usr/include/sys/reg.h
cmd.o: /usr/include/sys/regset.h
cmd.o: /usr/include/sys/select.h
cmd.o: /usr/include/sys/siginfo.h
cmd.o: /usr/include/sys/sockio.h
cmd.o: /usr/include/sys/stream.h
cmd.o: /usr/include/sys/strmdep.h
cmd.o: /usr/include/sys/time.h
cmd.o: /usr/include/sys/times.h
cmd.o: /usr/include/sys/tss.h
cmd.o: /usr/include/sys/ttold.h
cmd.o: /usr/include/sys/ttydev.h
cmd.o: /usr/include/sys/ucontext.h
cmd.o: /usr/include/sys/vnode.h
cmd.o: /usr/ucbinclude/ndbm.h
cmd.o: /usr/ucbinclude/setjmp.h
cmd.o: /usr/ucbinclude/stdio.h
cmd.o: /usr/ucbinclude/sys/ioctl.h
cmd.o: /usr/ucbinclude/sys/param.h
cmd.o: /usr/ucbinclude/sys/signal.h
cmd.o: /usr/ucbinclude/sys/sysmacros.h
cmd.o: /usr/ucbinclude/sys/ttychars.h
cmd.o: /usr/ucbinclude/sys/types.h
cmd.o: EXTERN.h
cmd.o: arg.h
cmd.o: array.h
cmd.o: cmd.c
cmd.o: cmd.h
cmd.o: config.h
cmd.o: form.h
cmd.o: handy.h
cmd.o: hash.h
cmd.o: perl.h
cmd.o: regexp.h
cmd.o: spat.h
cmd.o: stab.h
cmd.o: str.h
cmd.o: util.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
cons.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
cons.o: /usr/include/dirent.h
cons.o: /usr/include/errno.h
cons.o: /usr/include/netinet/in.h
cons.o: /usr/include/sys/cred.h
cons.o: /usr/include/sys/dirent.h
cons.o: /usr/include/sys/errno.h
cons.o: /usr/include/sys/filio.h
cons.o: /usr/include/sys/ioccom.h
cons.o: /usr/include/sys/poll.h
cons.o: /usr/include/sys/reg.h
cons.o: /usr/include/sys/regset.h
cons.o: /usr/include/sys/select.h
cons.o: /usr/include/sys/siginfo.h
cons.o: /usr/include/sys/sockio.h
cons.o: /usr/include/sys/stream.h
cons.o: /usr/include/sys/strmdep.h
cons.o: /usr/include/sys/time.h
cons.o: /usr/include/sys/times.h
cons.o: /usr/include/sys/tss.h
cons.o: /usr/include/sys/ttold.h
cons.o: /usr/include/sys/ttydev.h
cons.o: /usr/include/sys/ucontext.h
cons.o: /usr/include/sys/vnode.h
cons.o: /usr/ucbinclude/ndbm.h
cons.o: /usr/ucbinclude/setjmp.h
cons.o: /usr/ucbinclude/stdio.h
cons.o: /usr/ucbinclude/sys/ioctl.h
cons.o: /usr/ucbinclude/sys/param.h
cons.o: /usr/ucbinclude/sys/signal.h
cons.o: /usr/ucbinclude/sys/sysmacros.h
cons.o: /usr/ucbinclude/sys/ttychars.h
cons.o: /usr/ucbinclude/sys/types.h
cons.o: EXTERN.h
cons.o: arg.h
cons.o: array.h
cons.o: cmd.h
cons.o: config.h
cons.o: cons.c
cons.o: form.h
cons.o: handy.h
cons.o: hash.h
cons.o: perl.h
cons.o: perly.h
cons.o: regexp.h
cons.o: spat.h
cons.o: stab.h
cons.o: str.h
cons.o: util.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
consarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
consarg.o: /usr/include/dirent.h
consarg.o: /usr/include/errno.h
consarg.o: /usr/include/netinet/in.h
consarg.o: /usr/include/sys/cred.h
consarg.o: /usr/include/sys/dirent.h
consarg.o: /usr/include/sys/errno.h
consarg.o: /usr/include/sys/filio.h
consarg.o: /usr/include/sys/ioccom.h
consarg.o: /usr/include/sys/poll.h
consarg.o: /usr/include/sys/reg.h
consarg.o: /usr/include/sys/regset.h
consarg.o: /usr/include/sys/select.h
consarg.o: /usr/include/sys/siginfo.h
consarg.o: /usr/include/sys/sockio.h
consarg.o: /usr/include/sys/stream.h
consarg.o: /usr/include/sys/strmdep.h
consarg.o: /usr/include/sys/time.h
consarg.o: /usr/include/sys/times.h
consarg.o: /usr/include/sys/tss.h
consarg.o: /usr/include/sys/ttold.h
consarg.o: /usr/include/sys/ttydev.h
consarg.o: /usr/include/sys/ucontext.h
consarg.o: /usr/include/sys/vnode.h
consarg.o: /usr/ucbinclude/ndbm.h
consarg.o: /usr/ucbinclude/setjmp.h
consarg.o: /usr/ucbinclude/stdio.h
consarg.o: /usr/ucbinclude/sys/ioctl.h
consarg.o: /usr/ucbinclude/sys/param.h
consarg.o: /usr/ucbinclude/sys/signal.h
consarg.o: /usr/ucbinclude/sys/sysmacros.h
consarg.o: /usr/ucbinclude/sys/ttychars.h
consarg.o: /usr/ucbinclude/sys/types.h
consarg.o: EXTERN.h
consarg.o: arg.h
consarg.o: array.h
consarg.o: cmd.h
consarg.o: config.h
consarg.o: consarg.c
consarg.o: form.h
consarg.o: handy.h
consarg.o: hash.h
consarg.o: perl.h
consarg.o: regexp.h
consarg.o: spat.h
consarg.o: stab.h
consarg.o: str.h
consarg.o: util.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
doarg.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
doarg.o: /usr/include/dirent.h
doarg.o: /usr/include/errno.h
doarg.o: /usr/include/netinet/in.h
doarg.o: /usr/include/sys/cred.h
doarg.o: /usr/include/sys/dirent.h
doarg.o: /usr/include/sys/errno.h
doarg.o: /usr/include/sys/filio.h
doarg.o: /usr/include/sys/ioccom.h
doarg.o: /usr/include/sys/poll.h
doarg.o: /usr/include/sys/reg.h
doarg.o: /usr/include/sys/regset.h
doarg.o: /usr/include/sys/select.h
doarg.o: /usr/include/sys/siginfo.h
doarg.o: /usr/include/sys/sockio.h
doarg.o: /usr/include/sys/stream.h
doarg.o: /usr/include/sys/strmdep.h
doarg.o: /usr/include/sys/time.h
doarg.o: /usr/include/sys/times.h
doarg.o: /usr/include/sys/tss.h
doarg.o: /usr/include/sys/ttold.h
doarg.o: /usr/include/sys/ttydev.h
doarg.o: /usr/include/sys/ucontext.h
doarg.o: /usr/include/sys/vnode.h
doarg.o: /usr/ucbinclude/ndbm.h
doarg.o: /usr/ucbinclude/setjmp.h
doarg.o: /usr/ucbinclude/signal.h
doarg.o: /usr/ucbinclude/stdio.h
doarg.o: /usr/ucbinclude/sys/ioctl.h
doarg.o: /usr/ucbinclude/sys/param.h
doarg.o: /usr/ucbinclude/sys/signal.h
doarg.o: /usr/ucbinclude/sys/sysmacros.h
doarg.o: /usr/ucbinclude/sys/ttychars.h
doarg.o: /usr/ucbinclude/sys/types.h
doarg.o: EXTERN.h
doarg.o: arg.h
doarg.o: array.h
doarg.o: cmd.h
doarg.o: config.h
doarg.o: doarg.c
doarg.o: form.h
doarg.o: handy.h
doarg.o: hash.h
doarg.o: perl.h
doarg.o: regexp.h
doarg.o: spat.h
doarg.o: stab.h
doarg.o: str.h
doarg.o: util.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
doio.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
doio.o: /usr/include/dirent.h
doio.o: /usr/include/errno.h
doio.o: /usr/include/grp.h
doio.o: /usr/include/netdb.h
doio.o: /usr/include/netinet/in.h
doio.o: /usr/include/pwd.h
doio.o: /usr/include/sys/cred.h
doio.o: /usr/include/sys/dirent.h
doio.o: /usr/include/sys/errno.h
doio.o: /usr/include/sys/filio.h
doio.o: /usr/include/sys/ioccom.h
doio.o: /usr/include/sys/ipc.h
doio.o: /usr/include/sys/msg.h
doio.o: /usr/include/sys/netconfig.h
doio.o: /usr/include/sys/poll.h
doio.o: /usr/include/sys/reg.h
doio.o: /usr/include/sys/regset.h
doio.o: /usr/include/sys/select.h
doio.o: /usr/include/sys/sem.h
doio.o: /usr/include/sys/shm.h
doio.o: /usr/include/sys/siginfo.h
doio.o: /usr/include/sys/socket.h
doio.o: /usr/include/sys/sockio.h
doio.o: /usr/include/sys/stream.h
doio.o: /usr/include/sys/strmdep.h
doio.o: /usr/include/sys/time.h
doio.o: /usr/include/sys/times.h
doio.o: /usr/include/sys/tss.h
doio.o: /usr/include/sys/ttold.h
doio.o: /usr/include/sys/ttydev.h
doio.o: /usr/include/sys/ucontext.h
doio.o: /usr/include/sys/utime.h
doio.o: /usr/include/sys/vnode.h
doio.o: /usr/include/utime.h
doio.o: /usr/ucbinclude/ndbm.h
doio.o: /usr/ucbinclude/setjmp.h
doio.o: /usr/ucbinclude/stdio.h
doio.o: /usr/ucbinclude/sys/fcntl.h
doio.o: /usr/ucbinclude/sys/file.h
doio.o: /usr/ucbinclude/sys/ioctl.h
doio.o: /usr/ucbinclude/sys/param.h
doio.o: /usr/ucbinclude/sys/signal.h
doio.o: /usr/ucbinclude/sys/sysmacros.h
doio.o: /usr/ucbinclude/sys/ttychars.h
doio.o: /usr/ucbinclude/sys/types.h
doio.o: EXTERN.h
doio.o: arg.h
doio.o: array.h
doio.o: cmd.h
doio.o: config.h
doio.o: doio.c
doio.o: form.h
doio.o: handy.h
doio.o: hash.h
doio.o: perl.h
doio.o: regexp.h
doio.o: spat.h
doio.o: stab.h
doio.o: str.h
doio.o: util.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
dolist.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
dolist.o: /usr/include/dirent.h
dolist.o: /usr/include/errno.h
dolist.o: /usr/include/netinet/in.h
dolist.o: /usr/include/sys/cred.h
dolist.o: /usr/include/sys/dirent.h
dolist.o: /usr/include/sys/errno.h
dolist.o: /usr/include/sys/filio.h
dolist.o: /usr/include/sys/ioccom.h
dolist.o: /usr/include/sys/poll.h
dolist.o: /usr/include/sys/reg.h
dolist.o: /usr/include/sys/regset.h
dolist.o: /usr/include/sys/select.h
dolist.o: /usr/include/sys/siginfo.h
dolist.o: /usr/include/sys/sockio.h
dolist.o: /usr/include/sys/stream.h
dolist.o: /usr/include/sys/strmdep.h
dolist.o: /usr/include/sys/time.h
dolist.o: /usr/include/sys/times.h
dolist.o: /usr/include/sys/tss.h
dolist.o: /usr/include/sys/ttold.h
dolist.o: /usr/include/sys/ttydev.h
dolist.o: /usr/include/sys/ucontext.h
dolist.o: /usr/include/sys/vnode.h
dolist.o: /usr/ucbinclude/ndbm.h
dolist.o: /usr/ucbinclude/setjmp.h
dolist.o: /usr/ucbinclude/stdio.h
dolist.o: /usr/ucbinclude/sys/ioctl.h
dolist.o: /usr/ucbinclude/sys/param.h
dolist.o: /usr/ucbinclude/sys/signal.h
dolist.o: /usr/ucbinclude/sys/sysmacros.h
dolist.o: /usr/ucbinclude/sys/ttychars.h
dolist.o: /usr/ucbinclude/sys/types.h
dolist.o: EXTERN.h
dolist.o: arg.h
dolist.o: array.h
dolist.o: cmd.h
dolist.o: config.h
dolist.o: dolist.c
dolist.o: form.h
dolist.o: handy.h
dolist.o: hash.h
dolist.o: perl.h
dolist.o: regexp.h
dolist.o: spat.h
dolist.o: stab.h
dolist.o: str.h
dolist.o: util.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
dump.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
dump.o: /usr/include/dirent.h
dump.o: /usr/include/errno.h
dump.o: /usr/include/netinet/in.h
dump.o: /usr/include/sys/cred.h
dump.o: /usr/include/sys/dirent.h
dump.o: /usr/include/sys/errno.h
dump.o: /usr/include/sys/filio.h
dump.o: /usr/include/sys/ioccom.h
dump.o: /usr/include/sys/poll.h
dump.o: /usr/include/sys/reg.h
dump.o: /usr/include/sys/regset.h
dump.o: /usr/include/sys/select.h
dump.o: /usr/include/sys/siginfo.h
dump.o: /usr/include/sys/sockio.h
dump.o: /usr/include/sys/stream.h
dump.o: /usr/include/sys/strmdep.h
dump.o: /usr/include/sys/time.h
dump.o: /usr/include/sys/times.h
dump.o: /usr/include/sys/tss.h
dump.o: /usr/include/sys/ttold.h
dump.o: /usr/include/sys/ttydev.h
dump.o: /usr/include/sys/ucontext.h
dump.o: /usr/include/sys/vnode.h
dump.o: /usr/ucbinclude/ndbm.h
dump.o: /usr/ucbinclude/setjmp.h
dump.o: /usr/ucbinclude/stdio.h
dump.o: /usr/ucbinclude/sys/ioctl.h
dump.o: /usr/ucbinclude/sys/param.h
dump.o: /usr/ucbinclude/sys/signal.h
dump.o: /usr/ucbinclude/sys/sysmacros.h
dump.o: /usr/ucbinclude/sys/ttychars.h
dump.o: /usr/ucbinclude/sys/types.h
dump.o: EXTERN.h
dump.o: arg.h
dump.o: array.h
dump.o: cmd.h
dump.o: config.h
dump.o: dump.c
dump.o: form.h
dump.o: handy.h
dump.o: hash.h
dump.o: perl.h
dump.o: regexp.h
dump.o: spat.h
dump.o: stab.h
dump.o: str.h
dump.o: util.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
eval.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
eval.o: /usr/include/dirent.h
eval.o: /usr/include/errno.h
eval.o: /usr/include/netinet/in.h
eval.o: /usr/include/sys/cred.h
eval.o: /usr/include/sys/dirent.h
eval.o: /usr/include/sys/errno.h
eval.o: /usr/include/sys/filio.h
eval.o: /usr/include/sys/ioccom.h
eval.o: /usr/include/sys/poll.h
eval.o: /usr/include/sys/reg.h
eval.o: /usr/include/sys/regset.h
eval.o: /usr/include/sys/select.h
eval.o: /usr/include/sys/siginfo.h
eval.o: /usr/include/sys/sockio.h
eval.o: /usr/include/sys/stream.h
eval.o: /usr/include/sys/strmdep.h
eval.o: /usr/include/sys/time.h
eval.o: /usr/include/sys/times.h
eval.o: /usr/include/sys/tss.h
eval.o: /usr/include/sys/ttold.h
eval.o: /usr/include/sys/ttydev.h
eval.o: /usr/include/sys/ucontext.h
eval.o: /usr/include/sys/vnode.h
eval.o: /usr/ucbinclude/ndbm.h
eval.o: /usr/ucbinclude/setjmp.h
eval.o: /usr/ucbinclude/signal.h
eval.o: /usr/ucbinclude/stdio.h
eval.o: /usr/ucbinclude/sys/fcntl.h
eval.o: /usr/ucbinclude/sys/file.h
eval.o: /usr/ucbinclude/sys/ioctl.h
eval.o: /usr/ucbinclude/sys/param.h
eval.o: /usr/ucbinclude/sys/signal.h
eval.o: /usr/ucbinclude/sys/sysmacros.h
eval.o: /usr/ucbinclude/sys/ttychars.h
eval.o: /usr/ucbinclude/sys/types.h
eval.o: EXTERN.h
eval.o: arg.h
eval.o: array.h
eval.o: cmd.h
eval.o: config.h
eval.o: eval.c
eval.o: form.h
eval.o: handy.h
eval.o: hash.h
eval.o: perl.h
eval.o: regexp.h
eval.o: spat.h
eval.o: stab.h
eval.o: str.h
eval.o: util.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
form.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
form.o: /usr/include/dirent.h
form.o: /usr/include/errno.h
form.o: /usr/include/netinet/in.h
form.o: /usr/include/sys/cred.h
form.o: /usr/include/sys/dirent.h
form.o: /usr/include/sys/errno.h
form.o: /usr/include/sys/filio.h
form.o: /usr/include/sys/ioccom.h
form.o: /usr/include/sys/poll.h
form.o: /usr/include/sys/reg.h
form.o: /usr/include/sys/regset.h
form.o: /usr/include/sys/select.h
form.o: /usr/include/sys/siginfo.h
form.o: /usr/include/sys/sockio.h
form.o: /usr/include/sys/stream.h
form.o: /usr/include/sys/strmdep.h
form.o: /usr/include/sys/time.h
form.o: /usr/include/sys/times.h
form.o: /usr/include/sys/tss.h
form.o: /usr/include/sys/ttold.h
form.o: /usr/include/sys/ttydev.h
form.o: /usr/include/sys/ucontext.h
form.o: /usr/include/sys/vnode.h
form.o: /usr/ucbinclude/ndbm.h
form.o: /usr/ucbinclude/setjmp.h
form.o: /usr/ucbinclude/stdio.h
form.o: /usr/ucbinclude/sys/ioctl.h
form.o: /usr/ucbinclude/sys/param.h
form.o: /usr/ucbinclude/sys/signal.h
form.o: /usr/ucbinclude/sys/sysmacros.h
form.o: /usr/ucbinclude/sys/ttychars.h
form.o: /usr/ucbinclude/sys/types.h
form.o: EXTERN.h
form.o: arg.h
form.o: array.h
form.o: cmd.h
form.o: config.h
form.o: form.c
form.o: form.h
form.o: handy.h
form.o: hash.h
form.o: perl.h
form.o: regexp.h
form.o: spat.h
form.o: stab.h
form.o: str.h
form.o: util.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
hash.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
hash.o: /usr/include/dirent.h
hash.o: /usr/include/errno.h
hash.o: /usr/include/netinet/in.h
hash.o: /usr/include/sys/cred.h
hash.o: /usr/include/sys/dirent.h
hash.o: /usr/include/sys/errno.h
hash.o: /usr/include/sys/filio.h
hash.o: /usr/include/sys/ioccom.h
hash.o: /usr/include/sys/poll.h
hash.o: /usr/include/sys/reg.h
hash.o: /usr/include/sys/regset.h
hash.o: /usr/include/sys/select.h
hash.o: /usr/include/sys/siginfo.h
hash.o: /usr/include/sys/sockio.h
hash.o: /usr/include/sys/stream.h
hash.o: /usr/include/sys/strmdep.h
hash.o: /usr/include/sys/time.h
hash.o: /usr/include/sys/times.h
hash.o: /usr/include/sys/tss.h
hash.o: /usr/include/sys/ttold.h
hash.o: /usr/include/sys/ttydev.h
hash.o: /usr/include/sys/ucontext.h
hash.o: /usr/include/sys/vnode.h
hash.o: /usr/ucbinclude/ndbm.h
hash.o: /usr/ucbinclude/setjmp.h
hash.o: /usr/ucbinclude/stdio.h
hash.o: /usr/ucbinclude/sys/fcntl.h
hash.o: /usr/ucbinclude/sys/file.h
hash.o: /usr/ucbinclude/sys/ioctl.h
hash.o: /usr/ucbinclude/sys/param.h
hash.o: /usr/ucbinclude/sys/signal.h
hash.o: /usr/ucbinclude/sys/sysmacros.h
hash.o: /usr/ucbinclude/sys/ttychars.h
hash.o: /usr/ucbinclude/sys/types.h
hash.o: EXTERN.h
hash.o: arg.h
hash.o: array.h
hash.o: cmd.h
hash.o: config.h
hash.o: form.h
hash.o: handy.h
hash.o: hash.c
hash.o: hash.h
hash.o: perl.h
hash.o: regexp.h
hash.o: spat.h
hash.o: stab.h
hash.o: str.h
hash.o: util.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
perl.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
perl.o: /usr/include/dirent.h
perl.o: /usr/include/errno.h
perl.o: /usr/include/netinet/in.h
perl.o: /usr/include/sys/cred.h
perl.o: /usr/include/sys/dirent.h
perl.o: /usr/include/sys/errno.h
perl.o: /usr/include/sys/filio.h
perl.o: /usr/include/sys/ioccom.h
perl.o: /usr/include/sys/poll.h
perl.o: /usr/include/sys/reg.h
perl.o: /usr/include/sys/regset.h
perl.o: /usr/include/sys/select.h
perl.o: /usr/include/sys/siginfo.h
perl.o: /usr/include/sys/sockio.h
perl.o: /usr/include/sys/stream.h
perl.o: /usr/include/sys/strmdep.h
perl.o: /usr/include/sys/time.h
perl.o: /usr/include/sys/times.h
perl.o: /usr/include/sys/tss.h
perl.o: /usr/include/sys/ttold.h
perl.o: /usr/include/sys/ttydev.h
perl.o: /usr/include/sys/ucontext.h
perl.o: /usr/include/sys/vnode.h
perl.o: /usr/ucbinclude/ndbm.h
perl.o: /usr/ucbinclude/setjmp.h
perl.o: /usr/ucbinclude/stdio.h
perl.o: /usr/ucbinclude/sys/ioctl.h
perl.o: /usr/ucbinclude/sys/param.h
perl.o: /usr/ucbinclude/sys/signal.h
perl.o: /usr/ucbinclude/sys/sysmacros.h
perl.o: /usr/ucbinclude/sys/ttychars.h
perl.o: /usr/ucbinclude/sys/types.h
perl.o: EXTERN.h
perl.o: arg.h
perl.o: array.h
perl.o: cmd.h
perl.o: config.h
perl.o: form.h
perl.o: handy.h
perl.o: hash.h
perl.o: patchlevel.h
perl.o: perl.c
perl.o: perl.h
perl.o: perly.h
perl.o: regexp.h
perl.o: spat.h
perl.o: stab.h
perl.o: str.h
perl.o: util.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
regcomp.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
regcomp.o: /usr/include/dirent.h
regcomp.o: /usr/include/errno.h
regcomp.o: /usr/include/netinet/in.h
regcomp.o: /usr/include/sys/cred.h
regcomp.o: /usr/include/sys/dirent.h
regcomp.o: /usr/include/sys/errno.h
regcomp.o: /usr/include/sys/filio.h
regcomp.o: /usr/include/sys/ioccom.h
regcomp.o: /usr/include/sys/poll.h
regcomp.o: /usr/include/sys/reg.h
regcomp.o: /usr/include/sys/regset.h
regcomp.o: /usr/include/sys/select.h
regcomp.o: /usr/include/sys/siginfo.h
regcomp.o: /usr/include/sys/sockio.h
regcomp.o: /usr/include/sys/stream.h
regcomp.o: /usr/include/sys/strmdep.h
regcomp.o: /usr/include/sys/time.h
regcomp.o: /usr/include/sys/times.h
regcomp.o: /usr/include/sys/tss.h
regcomp.o: /usr/include/sys/ttold.h
regcomp.o: /usr/include/sys/ttydev.h
regcomp.o: /usr/include/sys/ucontext.h
regcomp.o: /usr/include/sys/vnode.h
regcomp.o: /usr/ucbinclude/ndbm.h
regcomp.o: /usr/ucbinclude/setjmp.h
regcomp.o: /usr/ucbinclude/stdio.h
regcomp.o: /usr/ucbinclude/sys/ioctl.h
regcomp.o: /usr/ucbinclude/sys/param.h
regcomp.o: /usr/ucbinclude/sys/signal.h
regcomp.o: /usr/ucbinclude/sys/sysmacros.h
regcomp.o: /usr/ucbinclude/sys/ttychars.h
regcomp.o: /usr/ucbinclude/sys/types.h
regcomp.o: EXTERN.h
regcomp.o: INTERN.h
regcomp.o: arg.h
regcomp.o: array.h
regcomp.o: cmd.h
regcomp.o: config.h
regcomp.o: form.h
regcomp.o: handy.h
regcomp.o: hash.h
regcomp.o: perl.h
regcomp.o: regcomp.c
regcomp.o: regcomp.h
regcomp.o: regexp.h
regcomp.o: spat.h
regcomp.o: stab.h
regcomp.o: str.h
regcomp.o: util.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
regexec.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
regexec.o: /usr/include/dirent.h
regexec.o: /usr/include/errno.h
regexec.o: /usr/include/netinet/in.h
regexec.o: /usr/include/sys/cred.h
regexec.o: /usr/include/sys/dirent.h
regexec.o: /usr/include/sys/errno.h
regexec.o: /usr/include/sys/filio.h
regexec.o: /usr/include/sys/ioccom.h
regexec.o: /usr/include/sys/poll.h
regexec.o: /usr/include/sys/reg.h
regexec.o: /usr/include/sys/regset.h
regexec.o: /usr/include/sys/select.h
regexec.o: /usr/include/sys/siginfo.h
regexec.o: /usr/include/sys/sockio.h
regexec.o: /usr/include/sys/stream.h
regexec.o: /usr/include/sys/strmdep.h
regexec.o: /usr/include/sys/time.h
regexec.o: /usr/include/sys/times.h
regexec.o: /usr/include/sys/tss.h
regexec.o: /usr/include/sys/ttold.h
regexec.o: /usr/include/sys/ttydev.h
regexec.o: /usr/include/sys/ucontext.h
regexec.o: /usr/include/sys/vnode.h
regexec.o: /usr/ucbinclude/ndbm.h
regexec.o: /usr/ucbinclude/setjmp.h
regexec.o: /usr/ucbinclude/stdio.h
regexec.o: /usr/ucbinclude/sys/ioctl.h
regexec.o: /usr/ucbinclude/sys/param.h
regexec.o: /usr/ucbinclude/sys/signal.h
regexec.o: /usr/ucbinclude/sys/sysmacros.h
regexec.o: /usr/ucbinclude/sys/ttychars.h
regexec.o: /usr/ucbinclude/sys/types.h
regexec.o: EXTERN.h
regexec.o: arg.h
regexec.o: array.h
regexec.o: cmd.h
regexec.o: config.h
regexec.o: form.h
regexec.o: handy.h
regexec.o: hash.h
regexec.o: perl.h
regexec.o: regcomp.h
regexec.o: regexec.c
regexec.o: regexp.h
regexec.o: spat.h
regexec.o: stab.h
regexec.o: str.h
regexec.o: util.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
stab.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
stab.o: /usr/include/dirent.h
stab.o: /usr/include/errno.h
stab.o: /usr/include/netinet/in.h
stab.o: /usr/include/sys/cred.h
stab.o: /usr/include/sys/dirent.h
stab.o: /usr/include/sys/errno.h
stab.o: /usr/include/sys/filio.h
stab.o: /usr/include/sys/ioccom.h
stab.o: /usr/include/sys/poll.h
stab.o: /usr/include/sys/reg.h
stab.o: /usr/include/sys/regset.h
stab.o: /usr/include/sys/select.h
stab.o: /usr/include/sys/siginfo.h
stab.o: /usr/include/sys/sockio.h
stab.o: /usr/include/sys/stream.h
stab.o: /usr/include/sys/strmdep.h
stab.o: /usr/include/sys/time.h
stab.o: /usr/include/sys/times.h
stab.o: /usr/include/sys/tss.h
stab.o: /usr/include/sys/ttold.h
stab.o: /usr/include/sys/ttydev.h
stab.o: /usr/include/sys/ucontext.h
stab.o: /usr/include/sys/vnode.h
stab.o: /usr/ucbinclude/ndbm.h
stab.o: /usr/ucbinclude/setjmp.h
stab.o: /usr/ucbinclude/signal.h
stab.o: /usr/ucbinclude/stdio.h
stab.o: /usr/ucbinclude/sys/ioctl.h
stab.o: /usr/ucbinclude/sys/param.h
stab.o: /usr/ucbinclude/sys/signal.h
stab.o: /usr/ucbinclude/sys/sysmacros.h
stab.o: /usr/ucbinclude/sys/ttychars.h
stab.o: /usr/ucbinclude/sys/types.h
stab.o: EXTERN.h
stab.o: arg.h
stab.o: array.h
stab.o: cmd.h
stab.o: config.h
stab.o: form.h
stab.o: handy.h
stab.o: hash.h
stab.o: perl.h
stab.o: regexp.h
stab.o: spat.h
stab.o: stab.c
stab.o: stab.h
stab.o: str.h
stab.o: util.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
str.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
str.o: /usr/include/dirent.h
str.o: /usr/include/errno.h
str.o: /usr/include/netinet/in.h
str.o: /usr/include/sys/cred.h
str.o: /usr/include/sys/dirent.h
str.o: /usr/include/sys/errno.h
str.o: /usr/include/sys/filio.h
str.o: /usr/include/sys/ioccom.h
str.o: /usr/include/sys/poll.h
str.o: /usr/include/sys/reg.h
str.o: /usr/include/sys/regset.h
str.o: /usr/include/sys/select.h
str.o: /usr/include/sys/siginfo.h
str.o: /usr/include/sys/sockio.h
str.o: /usr/include/sys/stream.h
str.o: /usr/include/sys/strmdep.h
str.o: /usr/include/sys/time.h
str.o: /usr/include/sys/times.h
str.o: /usr/include/sys/tss.h
str.o: /usr/include/sys/ttold.h
str.o: /usr/include/sys/ttydev.h
str.o: /usr/include/sys/ucontext.h
str.o: /usr/include/sys/vnode.h
str.o: /usr/ucbinclude/ndbm.h
str.o: /usr/ucbinclude/setjmp.h
str.o: /usr/ucbinclude/stdio.h
str.o: /usr/ucbinclude/sys/ioctl.h
str.o: /usr/ucbinclude/sys/param.h
str.o: /usr/ucbinclude/sys/signal.h
str.o: /usr/ucbinclude/sys/sysmacros.h
str.o: /usr/ucbinclude/sys/ttychars.h
str.o: /usr/ucbinclude/sys/types.h
str.o: EXTERN.h
str.o: arg.h
str.o: array.h
str.o: cmd.h
str.o: config.h
str.o: form.h
str.o: handy.h
str.o: hash.h
str.o: perl.h
str.o: perly.h
str.o: regexp.h
str.o: spat.h
str.o: stab.h
str.o: str.c
str.o: str.h
str.o: util.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
toke.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
toke.o: /usr/include/dirent.h
toke.o: /usr/include/errno.h
toke.o: /usr/include/netinet/in.h
toke.o: /usr/include/sys/cred.h
toke.o: /usr/include/sys/dirent.h
toke.o: /usr/include/sys/errno.h
toke.o: /usr/include/sys/filio.h
toke.o: /usr/include/sys/ioccom.h
toke.o: /usr/include/sys/poll.h
toke.o: /usr/include/sys/reg.h
toke.o: /usr/include/sys/regset.h
toke.o: /usr/include/sys/select.h
toke.o: /usr/include/sys/siginfo.h
toke.o: /usr/include/sys/sockio.h
toke.o: /usr/include/sys/stream.h
toke.o: /usr/include/sys/strmdep.h
toke.o: /usr/include/sys/time.h
toke.o: /usr/include/sys/times.h
toke.o: /usr/include/sys/tss.h
toke.o: /usr/include/sys/ttold.h
toke.o: /usr/include/sys/ttydev.h
toke.o: /usr/include/sys/ucontext.h
toke.o: /usr/include/sys/vnode.h
toke.o: /usr/ucbinclude/ndbm.h
toke.o: /usr/ucbinclude/setjmp.h
toke.o: /usr/ucbinclude/stdio.h
toke.o: /usr/ucbinclude/sys/fcntl.h
toke.o: /usr/ucbinclude/sys/file.h
toke.o: /usr/ucbinclude/sys/ioctl.h
toke.o: /usr/ucbinclude/sys/param.h
toke.o: /usr/ucbinclude/sys/signal.h
toke.o: /usr/ucbinclude/sys/sysmacros.h
toke.o: /usr/ucbinclude/sys/ttychars.h
toke.o: /usr/ucbinclude/sys/types.h
toke.o: EXTERN.h
toke.o: arg.h
toke.o: array.h
toke.o: cmd.h
toke.o: config.h
toke.o: form.h
toke.o: handy.h
toke.o: hash.h
toke.o: perl.h
toke.o: perly.h
toke.o: regexp.h
toke.o: spat.h
toke.o: stab.h
toke.o: str.h
toke.o: toke.c
toke.o: util.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
util.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/varargs.h
util.o: /usr/include/dirent.h
util.o: /usr/include/errno.h
util.o: /usr/include/netinet/in.h
util.o: /usr/include/sys/cred.h
util.o: /usr/include/sys/dirent.h
util.o: /usr/include/sys/errno.h
util.o: /usr/include/sys/filio.h
util.o: /usr/include/sys/ioccom.h
util.o: /usr/include/sys/poll.h
util.o: /usr/include/sys/reg.h
util.o: /usr/include/sys/regset.h
util.o: /usr/include/sys/select.h
util.o: /usr/include/sys/siginfo.h
util.o: /usr/include/sys/sockio.h
util.o: /usr/include/sys/stream.h
util.o: /usr/include/sys/strmdep.h
util.o: /usr/include/sys/time.h
util.o: /usr/include/sys/times.h
util.o: /usr/include/sys/tss.h
util.o: /usr/include/sys/ttold.h
util.o: /usr/include/sys/ttydev.h
util.o: /usr/include/sys/ucontext.h
util.o: /usr/include/sys/vnode.h
util.o: /usr/ucbinclude/ndbm.h
util.o: /usr/ucbinclude/setjmp.h
util.o: /usr/ucbinclude/signal.h
util.o: /usr/ucbinclude/stdio.h
util.o: /usr/ucbinclude/sys/fcntl.h
util.o: /usr/ucbinclude/sys/file.h
util.o: /usr/ucbinclude/sys/ioctl.h
util.o: /usr/ucbinclude/sys/param.h
util.o: /usr/ucbinclude/sys/signal.h
util.o: /usr/ucbinclude/sys/sysmacros.h
util.o: /usr/ucbinclude/sys/ttychars.h
util.o: /usr/ucbinclude/sys/types.h
util.o: EXTERN.h
util.o: arg.h
util.o: array.h
util.o: cmd.h
util.o: config.h
util.o: form.h
util.o: handy.h
util.o: hash.h
util.o: perl.h
util.o: regexp.h
util.o: spat.h
util.o: stab.h
util.o: str.h
util.o: util.c
util.o: util.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/ctype.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/limits.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/stdlib.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/string.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/byteorder.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/sys/stat.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/syslimits.h
usersub.o: /opt/lib/gcc-lib/i386-univel-sysv4.2/2.4.5/include/time.h
usersub.o: /usr/include/dirent.h
usersub.o: /usr/include/errno.h
usersub.o: /usr/include/netinet/in.h
usersub.o: /usr/include/sys/cred.h
usersub.o: /usr/include/sys/dirent.h
usersub.o: /usr/include/sys/errno.h
usersub.o: /usr/include/sys/filio.h
usersub.o: /usr/include/sys/ioccom.h
usersub.o: /usr/include/sys/poll.h
usersub.o: /usr/include/sys/reg.h
usersub.o: /usr/include/sys/regset.h
usersub.o: /usr/include/sys/select.h
usersub.o: /usr/include/sys/siginfo.h
usersub.o: /usr/include/sys/sockio.h
usersub.o: /usr/include/sys/stream.h
usersub.o: /usr/include/sys/strmdep.h
usersub.o: /usr/include/sys/time.h
usersub.o: /usr/include/sys/times.h
usersub.o: /usr/include/sys/tss.h
usersub.o: /usr/include/sys/ttold.h
usersub.o: /usr/include/sys/ttydev.h
usersub.o: /usr/include/sys/ucontext.h
usersub.o: /usr/include/sys/vnode.h
usersub.o: /usr/ucbinclude/ndbm.h
usersub.o: /usr/ucbinclude/setjmp.h
usersub.o: /usr/ucbinclude/stdio.h
usersub.o: /usr/ucbinclude/sys/ioctl.h
usersub.o: /usr/ucbinclude/sys/param.h
usersub.o: /usr/ucbinclude/sys/signal.h
usersub.o: /usr/ucbinclude/sys/sysmacros.h
usersub.o: /usr/ucbinclude/sys/ttychars.h
usersub.o: /usr/ucbinclude/sys/types.h
usersub.o: EXTERN.h
usersub.o: arg.h
usersub.o: array.h
usersub.o: cmd.h
usersub.o: config.h
usersub.o: form.h
usersub.o: handy.h
usersub.o: hash.h
usersub.o: perl.h
usersub.o: regexp.h
usersub.o: spat.h
usersub.o: stab.h
usersub.o: str.h
usersub.o: usersub.c
usersub.o: util.h
Makefile: Makefile.SH config.sh ; /bin/sh Makefile.SH
makedepend: makedepend.SH config.sh ; /bin/sh makedepend.SH
h2ph: h2ph.SH config.sh ; /bin/sh h2ph.SH
# WARNING: Put nothing here or make depend will gobble it up!
