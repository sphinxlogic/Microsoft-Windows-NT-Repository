# This file was generated automatically by configure.  Do not edit.
VPATH = .
links = 
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
#### host, target, and site specific Makefile frags come in here.

srcdir = .

all: bintest

nmtest:         $(srcdir)/bintest.c Makefile
		@linkflags=`echo "${LDFLAGS}" | sed -e 's/-T.*m68k\.gld/-T.\/m68k.gld/'` ; \
		$(CC) $(CFLAGS) -g $$linkflags -o bintest $(srcdir)/bintest.c $(LIBS)

clean:
		-rm -f *.o bintest *.diff *~ *.bad core m68k.gld

distclean:	clean
		-rm -f Makefile config.status nmtest.c

Makefile : $(srcdir)/Makefile.in $(host_makefile_frag) $(target_makefile_frag)
	$(SHELL) ./config.status

force:
bintest.o: $(srcdir)/bintest.c
