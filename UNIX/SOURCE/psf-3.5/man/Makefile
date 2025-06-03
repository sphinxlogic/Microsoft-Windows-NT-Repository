# $Id: Makefile,v 3.2 1992/02/07 15:12:12 ajf Exp ajf $
#
#	Make the psf man pages
#	----------------------

SHELL=/bin/sh

MANS     = psf.1 psfmail.1 psfbs.1 psfdoub.1 psfmbox.1 psfman.1 psfnroff.1 psfbanner.1
DOCS     = psf.doc psfmail.doc psfbs.doc psfdoub.doc psfmbox.doc psfman.doc psfnroff.doc psfbanner.doc


# install man files
# ^^^^^^^^^^^^^^^^^

installman:
	@for f in $(MANS) ; do echo "	$$f" ;\
		 cp $$f $(MANDIR)/`basename $$f .1`.$(MANSUF) ; done

installcat:
	@for f in $(MANS) ; do echo "	$$f" ;\
		 $(NROFF) -man $$f $(COL) > $(MANDIR)/`basename $$f .1`.$(MANSUF) ; done

installdoc:
	@test -r psf.doc || ( echo "make doc  first" ; exit 1 )
	@for f in $(DOCS) ; do ff=`basename $$f .doc` ; echo "	$$ff.1" ; cp $$f $(MANDIR)/$$ff.$(MANSUF) ; done


# create *.doc from *.1 files
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^
doc:
	@for f in $(MANS) ; do ff=`basename $$f .1` ; echo "	$$f" ; $(NROFF) -man $$f $(COL) > $$ff.doc ; done


ci:
	ci -r$(RCSVER) -m"$(RCSCOM)" $(MANS) Makefile

co:
	co -l $(MANS)

# Remove anything that cannot be rebuilt (don't remove *.doc)
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
clean:
	rm -f *.B
