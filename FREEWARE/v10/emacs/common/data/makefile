DESTDIR=
LIBDIR=/usr/local/lib
BINDIR=/usr/local/bin
MANDIR=/usr/man/man1
MANEXT=1

all:

mostlyclean clean:
	-rm -f core

distclean realclean:
	-rm -f DOC* core

SOURCES = [0-9A-QS-Z]* README *.[ch16] emacs.* etags.* ledit.l ms-* \
	news.texi rc2log refcard.tex spook-lines termcap.* ulimit.hack \
	vcdiff vipcard.tex xmouse.doc

unlock:
	chmod u+w $(SOURCES)

relock:
	chmod u-w $(SOURCES)
