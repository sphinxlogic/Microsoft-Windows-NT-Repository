include ../Makefile.config

install:
	
	$(INSTALL)  gopherd.conf.5 $(MAN5DIR)
	$(INSTALL)  gopherd.8 $(MAN8DIR)
	$(INSTALL)  gopherindex.8 $(MAN8DIR)
	$(INSTALL)  gopher.1 $(MAN1DIR)
	$(INSTALL)  gophfilt.1 $(MAN1DIR)

all:
	$(NROFF)    gopherd.conf.5 >gopherd.conf.cat
	$(NROFF)    gopherd.8      >gopherd.cat
	$(NROFF)    gopherindex.8  >gopherindex.cat
	$(NROFF)    gopher.1       >gopher.cat
	$(NROFF)    gophfilt.1     >gophfilt.cat

clean:
	-rm -f *~
