DESTDIR=
LIBDIR=sys$common:[syslib]
BINDIR=sys$common:[sysexe]
MANDIR=sys$common:[syshlp]helplib.hlb
MANEXT=HLP

all :

mostlyclean clean :
	@ !

distclean realclean :
	-delete DOC*.*.*

SOURCES = [0-9A-QS-Z]* README *.[ch16] emacs.* etags.* ledit.l ms-* \
	news.texi rc2log refcard.tex spook-lines termcap.* ulimit.hack \
	vcdiff vipcard.tex xmouse.doc

unlock:
	set file/prot=(o:rwed) *.*

relock:
	set file/prot=(o:re) *.*
