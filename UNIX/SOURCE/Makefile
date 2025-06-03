# Top-level Makefile for all packages on the CD-ROM
# Greg Lehey, LEMIS, 17 January 1994
TOOLS=/cdcopy/ETC/tools
# This can be mkmkpk, to always rebuild prototype file,
# mkpackage, to never rebuild it (and fail if it is missing),
# or make-package, to make it when needed.
MAKE-PACKAGE=make-package
INSTALL=install		# set if we want to install the package

# Other macros for this Makefile:
# COMPRESS: set to != "" if the directories should be compressed after making.
#	    In this case, the Makefile will also try to uncompress the directory
#           before making.
# ALL:      Set to != to make Imakefile-based packages from scratch. Has no
#	    effect on other packages
# CLEAN:    Set to non-0 if we want to make clean after installation
 
# Note that gcc and TeX come first and second, since most packages
# need them. gcc also needs TeX for the documentation, but TeX needs
# gcc to compile at all, so this just won't work completely. After
# building and installing TeX, you need to go through gcc again. 
PACKAGES=X11R5_ XFree86-1.3_ bash-1.13.5_ bin2c-1.00j_ bomb_ bugs_ chiaro_ clock_ cook_ emacs-19.22_ \
	ftp_ gcc-2.5.8_ gdb-4.11_ gdbm-1.7.1_ groff-1.09_ jet_ logtcp_ logwatch_ \
	man-1.1_ mitsumi_ od_ olvwm_ pcomm_ shellutils-1.9.2_ tardy_ texinfo-3.1_ tripwire_ tslip-2.3_ uucp-1.04_ \
	veos_ vgaset-1.1_ wuarchive.wustl.edu_ xc_ xgame_ xgetftp_ xgrasp_ xmail_ xodo_ xtest_ xtset_ \
	sas-1.25_ stream-1.0_ zsh-2.3.1_ xod-1.01_ XFree86-2.0_  XFree86-2.1_ SeeTeX-2.18.5_ libg++-2.5.3_ \
	binutils-2.3_ rcs-5.6.0.1_ cvs-1.3_ flex-2.4.6_ tcl7.3_ tk3.6_ bison-1.22_ \
	nethack-3.1.3_ make-3.70_ find-3.8_ xc-1.0_ glibc-1.07_ gas-2.2_ \
	patch-2.1_ textutils-1.9_ cdtools-1.3_ gawk-2.15.3_ m4-1.1_ xwho_ gperf-2.1_ \
	gnuchess-4.0.pl64_ banners_ TeX-3.14_ ecc-1.2.1_ fortune_ xtset-1.1_ elib-0.06_ \
	cube-1.0.2_ ghostscript-2.6.1_ xv-3.00_ fileutils-3.9_ elisp-manual-19-2.02.2_ \
	Xw_ para-0.25_


OLDPACKAGES= TeXfonts__  texinfo ImageMagick SeeTeX  Xpool aXe acm agrep \
	astrolog banners__ bash bison blast  \
	cdplayer__ cperf cpu cpio diff dvips ease  elm emacs expect \
	ezMenu__ f2c faces fgrep find flex fontutils gawk gdb \
	gdbm ghostview gnugo gnuplot grep gsfonts gzip hpcdtoppm \
	indent ispell invaders kaleidos kit less m4 make malawai \
	malloc__ man maze__ mpeg ms mtools mush oleo patch \
	pax pbmplus pcal perl psf psutils rcs regex rolodex screen seyon shar__ shellutils \
	sketchpad spider__ tar tcl texi2roff textutils tk transfig \
	trn vgaset wdiff xalarm xanim xbmbrowser xboard xbomb xcalendar xcd__ \
	xcoral xdiary xditview__ xdtree__ xephem__ xfig xfile__ xfishtank xfm \
	xgas__ xgrabsc xinfo xjewel xkal xlander xless xlife xload__ \
	xloadimage xlock xman xmandel__ xmines__ xmris xneko xpaint xpeg \
	xphoon xpilot xpipeman__ xpm xroach xruler__ xsnake__ xspringies__ \
	xswarm xtest__ xtetris xtetris__ xtmines__ xtool__ xv xview \
	xview3__ xviewgl xwave xwho__ xworm__

all:	$(PACKAGES)

$(PACKAGES):
	( package=`echo $@ | sed "s/_$$//"`; \
	export package; \
	echo package: $$package; \
	cd $$package; \
	install=${INSTALL} \
	make_package=${MAKE-PACKAGE} \
	clean=${CLEAN} \
	compress=${COMPRESS} \
	package.LEMIS/domake )


ImageMagick:
	(package=ImageMagick-2.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

TeXfonts__:
	(package=TeXfonts; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make all; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

Xpool:
	(package=Xpool-1.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

aXe:
	(package=aXe-5.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

acm:
	(package=acm-2.4; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

agrep:
	(package=agrep-2.04; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

astrolog:
	(package=astrolog-2.25; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

bison:
	(package=bison-1.21; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

blast:
	(package=blast-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

cdplayer__:
	(package=cdplayer; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

cpio:
	(package=cpio-2.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

cpu:
	(package=cpu-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

cperf:
	(package=cperf-2.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

diff:
	(package=diff-2.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

dvips:
	(package=dvips-5.49; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

ease:
	(package=ease-3.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

elm:
	(package=elm-2.4.17; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	if [ ! -d /opt/lib/elm ]; then \
	  mkdir -p /opt/lib/elm; \
	fi; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

emacs:
	(package=emacs-19.17; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	make dvi; \
	rm -f $$package/man/*.dvi; \
	make dvi; \
	make info; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

expect:
	(package=expect-3.24.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	if [ ! -d /opt/man/cat3 ]; then \
	  mkdir -p /opt/man/cat3; \
	fi; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

ezMenu__:
	(package=ezMenu; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

f2c:
	(package=f2c-3.2.90; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

faces:
	(package=faces-1.6.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make x11; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	if [ ! -d /opt/man/cat3 ]; then \
	  mkdir -p /opt/man/cat3; \
	fi; \
	make install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

fgrep:
	(package=fgrep-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

find:
	(package=find-3.8; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

flex:
	(package=flex-2.3.8; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make first_flex; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

fontutils:
	(package=fontutils-0.6; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	if [ ! -d /opt/lib/mf/macros ]; then \
	  mkdir -p /opt/lib/mf/macros; \
	fi; \
	if [ ! -d /opt/lib/tex/macros ]; then \
	  mkdir -p /opt/lib/tex/macros; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

gdbm:
	(package=gdbm-1.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install-man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

ghostview:
	(package=ghostview-1.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

gnuplot:
	(package=gnuplot-3.4.38.2.134; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make All; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make x11 TARGET=Install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

gnugo:
	(package=gnugo-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

grep:
	(package=grep-2.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

gsfonts:
	(package=gsfonts-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

gzip:
	(package=gzip-1.2.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

hpcdtoppm:
	(package=hpcdtoppm.0.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

indent:
	(package=indent-1.8; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	make indent.dvi; \
	make indent.info; \
	make indent.toc; \
	make indent.ps; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

ispell:
	(package=ispell-4.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

invaders:
	(package=invaders-2.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

kaleidos:
	(package=kaleidos-1.0.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

# Current kit Makefile does some installation from all target -
# don't do anything unless we're installing, then do everything.
kit:
	(package=kit-2.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

less:
	(package=less-1.77; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	rm install_*; \
	make install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

m4:
	(package=m4-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

make:
	(package=make-3.68; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make dvi; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

malawai:
	(package=malawai-0.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

malloc__:
	(package=malloc; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )
man:
	(package=man-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make dvi; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )


maze__:
	(package=maze; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

mpeg:
	(package=mpeg-1.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

ms:
	(package=ms-0.06; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

mtools:
	(package=mtools-2.0.7; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

mush:
	(package=mush-7.2.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

oleo:
	(package=oleo-1.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

patch:
	(package=patch-2.0.12u8; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

pax:
	(package=pax-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

pbmplus:
	(package=pbmplus-10dec91; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

pcal:
	(package=pcal-4.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

perl:
	(package=perl-4.036; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

psf:
	(package=psf-3.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make all; \
	make scripts; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make installcat; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

psutils:
	(package=psutils-2.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make all; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

rcs:
	(package=rcs5.6.0.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make all; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

regex:
	(package=regex-2.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

rolodex:
	(package=rolodex-2.0.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

sc:
	(package=sc-6.21; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

screen:
	(package=screen-3.2b; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

seyon:
	(package=seyon-2.0.6; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

shar__:
	(package=shar; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

shellutils:
	(package=shellutils-1.8; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install-catman; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

sketchpad:
	(package=sketchpad-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	(cd $$package/src; \
	xmkmf; \
	make depend; \
	make Makefiles; \
	make); \
	cd $$package; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

spider__:
	(package=spider; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

stream:
	(package=stream-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

tar:
	(package=tar-1.11.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

tcl:
	(package=tcl-6.4; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install-catman; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

texi2roff:
	(package=texi2roff-1.0a; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

texinfo:
	(package=texinfo-3.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	make texinfo.dvi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

textutils:
	(package=textutils-1.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make mostlyclean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

tk:
	(package=tk-3.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

transfig:
	(package=transfig-2.1.6; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	(package=fig2dev; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; xmkmf); \
	(package=fig2dev/dev; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; xmkmf); \
	(package=transfig; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; xmkmf); \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

trn:
	(package=trn-2.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

vgaset:
	(package=vgaset-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

wdiff:
	(package=wdiff-0.03; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xalarm:
	(package=xalarm-3.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xanim:
	(package=xanim-2.29.7; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xbmbrowser:
	(package=xbmbrowser-2.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xboard:
	(package=xboard-2.1.pl9; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xbomb:
	(package=xbomb-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xcalendar:
	(package=xcalendar-3.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xcd__:
	(package=xcd; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xcoral:
	(package=xcoral-1.72; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xdiary:
	(package=xdiary-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xditview__:
	(package=xditview; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xdtree__:
	(package=xdtree; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xephem__:
	(package=xephem; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xfig:
	(package=xfig-2.1.6; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xfile__:
	(package=xfile; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xfishtank:
	(package=xfishtank-2.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xfm:
	(package=xfm-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xfsm:
	(package=xfsm-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xgas__:
	(package=xgas; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xgrabsc:
	(package=xgrabsc-2.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xinfo:
	(package=xinfo-1.01.01; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xjewel:
	(package=xjewel-1.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xkal:
	(package=xkal-1.13; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package ezMenu; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xlander:
	(package=xlander-1.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xless:
	(package=xless-1.4.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xlife:
	(package=xlife-3.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xload__:
	(package=xload; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xloadimage:
	(package=xloadimage-3.01; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xlock:
	(package=xlock-2.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xman:
	(package=xman-3.1.5; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xmandel__:
	(package=xmandel; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xmines__:
	(package=xmines; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xmris:
	(package=xmris-3.03; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make depend; \
	make scram; \
	make sprites; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xneko:
	(package=xneko-1.14; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xpaint:
	(package=xpaint-2.0.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	sh install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xpeg:
	(package=xpeg-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make xpeg; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

# We have an Imakefile for this target, but the Makefile needs modification by
# hand. If you need to remake this target from scratch, read xphoon-18sep91/README.LEMIS; 
xphoon:
	(package=xphoon-18sep91; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xpilot:
	(package=xpilot-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
		depend, make; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xpipeman__:
	(package=xpipeman; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xpm:
	(package=xpm-3.2c; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xroach:
	(package=xroach-1.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xruler__:
	(package=xruler; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xsnake__:
	(package=xsnake; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xspringies:
	(package=xspringies-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xswarm:
	(package=xswarm-2.3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xtest__:
	(package=xtest; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xtetris:
	(package=xtetris-2.5.2; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xtetris__:
	(package=xtetris; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xtmines__:
	(package=xtmines; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xtool__:
	(package=xtool; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xv:
	(package=xv-2.20; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	  xmkmf; \
	  make Makefiles; \
	  (package=jpeg; \
	  if [ "$(COMPRESS)" != "" ]; then \
	    gunzip <  $$package.tar.gz | tar xf - ; \
	  fi; \
	  if [ ! -d $$package ]; then \
	    echo "*** $$package does not exist"; \
	    exit 1; \
	  fi; \
	  cd $$package; xmkmf); \
	  (package=tiff; \
	  if [ "$(COMPRESS)" != "" ]; then \
	    gunzip <  $$package.tar.gz | tar xf - ; \
	  fi; \
	  if [ ! -d $$package ]; then \
	    echo "*** $$package does not exist"; \
	    exit 1; \
	  fi; \
	  cd $$package; xmkmf); \
	  make depend; \
	  make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xview:
	(package=xview-3.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	sh mkimake; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xview3__:
	(package=xview3; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	make depend; \
	make; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xviewgl:
	(package=xviewgl-1.1; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xwave:
	(package=xwave-2.0; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xwho__:
	(package=xwho; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

xworm__:
	(package=xworm; \
	if [ "$(COMPRESS)" != "" ]; then \
	  gunzip <  $$package.tar.gz | tar xf - ; \
	fi; \
	if [ ! -d $$package ]; then \
	  echo "*** $$package does not exist"; \
	  exit 1; \
	fi; \
	cd $$package; \
	if [ "$(ALL)" != "" ]; then \
	xmkmf; \
	make Makefiles; \
	make depend; \
	make; \
	fi; \
	if [ "$(INSTALL)" != "" ]; then \
	if [ ! -s package.LEMIS/prototype -o "$(MAKE-PACKAGE)" = "mkmkpk" ]; then \
	  echo +++ Saving current directory state; \
	  saveopt; \
	fi; \
	echo +++ Installing package; \
	make install; \
	make install.man; \
	install-sysv; \
	linkdoc $$package; \
	echo +++ Building installable package; \
	$(MAKE-PACKAGE) . ; \
	fi; \
	if [ "$(CLEAN)" != "" ]; then \
	  echo +++ Making clean; \
	  make clean; \
	fi; \
	cd .. ; \
	if [ "$(COMPRESS)" != "" ]; then \
	  echo +++ Compressing directory into archive; \
	  tar cf - $$package | gzip >$$package.tar.gz; \
	  rm -rf $$package; \
	fi )

