#!/bin/sh

ARCH=i386
cp Xswarm.cdb /usr/X/lib/classdb/Xswarm
cp xswarm.pkgmr /usr/X/desktop/PackageMgr/xswarm
cp xswarm.icon /usr/X/lib/pixmaps
pkgmk -a${ARCH} -o -r /

