XCOMM!/bin/sh

XCOMM $XFree86: mit/server/ddx/x386/LinkKit/mkmf.cpp,v 1.5 1993/04/11 13:53:14 dawes Exp $
XCOMM
XCOMM build Makefile for link kit
XCOMM
touch X386Conf.tmpl
if [ x"$XWINHOME" != x ]; then
    CONFIG_DIR_SPEC=-I$XWINHOME/lib/X11/config
else
    CONFIG_DIR_SPEC=CONFIGDIRSPEC
fi
(set -x
  imake -I. $CONFIG_DIR_SPEC -DUseInstalled -DInit -DX386LinkKit=1 \
        -DTOPDIR=. -DCURDIR=.
)
make Configure
(set -x
  make Makefiles
XCOMM make clean
  make depend
)
