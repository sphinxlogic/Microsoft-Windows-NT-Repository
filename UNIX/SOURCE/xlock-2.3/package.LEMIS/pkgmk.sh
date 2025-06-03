#!/bin/sh

ARCH=i386
cp Xlock.cdb /usr/X/lib/classdb/Xlock
cp xlock.pkgmr /usr/X/desktop/PackageMgr/xlock
cp xlock.icon /usr/X/lib/pixmaps/xlock.icon
cp ../xlock.man /tmp/xlock.man
pkgmk -a${ARCH} -o -r /

