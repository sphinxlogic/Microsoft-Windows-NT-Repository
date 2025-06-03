#!/bin/sh

ARCH=i386
cp Xdiary.cdb /usr/X/lib/classdb/Xdiary
cp xdiary.pkgmr /usr/X/desktop/PackageMgr/xdiary
cp xdiary.icon /usr/X/lib/pixmaps
pkgmk -a${ARCH} -o -r /

