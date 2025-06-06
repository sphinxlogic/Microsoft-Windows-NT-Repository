#
# Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
#
# Permission to use, copy, modify, distribute, and sell this software and its
# documentation for any purpose is hereby granted without fee, provided that
# the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation, and that the name of Thomas Roell not be used in
# advertising or publicity pertaining to distribution of the software without
# specific, written prior permission.  Thomas Roell makes no representations
# about the suitability of this software for any purpose.  It is provided
# "as is" without express or implied warranty.
#
# THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
# EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
# DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#
# Author:  Thomas Roell, roell@informatik.tu-muenchen.de
#

# Changed for XFree86 by Thomas Wolfram,
# (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)

# $XFree86: mit/server/ddx/x386/etc/install.sh,v 2.0 1993/07/24 04:51:51 dawes Exp $

#
# XFree86 version
#
VERSION=1.3

#
# dependencies
#
if [ ! -s /usr/options/kc.name ]
then
    echo "Please make first sure that the Kernel Configuration is installed"
    echo "correctly. Then try a reinstall of XFree86 $VERSION."
    exit 1
fi

if [ ! -s /usr/options/st.name ]
then
    echo "Please make first sure that the STREAMS Facilities are installed"
    echo "correctly. Then try a reinstall of XFree86 $VERSION."
    exit 1
fi

#
# Make a X11 label, so that following commercial products can be installed
# 
if [ ! -s /usr/options/xu.name ]
then
    echo "X11R5 XFree86 Version $VERSION" >/usr/options/xu.name
fi

#
# Install remove script
#
if [ ! -s /usr/lib/installed/Remove/xu.name ]
then
    echo -n 'echo "Removing XFree86 Version ' >/usr/lib/installed/Remove/xu.name
    echo -n $VERSION >>/usr/lib/installed/Remove/xu.name
    echo ':
/usr/X386/*"
/bin/rm -rf /usr/X386' >>/usr/lib/installed/Remove/xu.name
fi

#
# Setup STREAMS for maximum performance
#
echo "Setup STREAMS for maximum performance..."
cd /usr/X386/lib/X11/etc
cp pts.node /etc/conf/node.d/pts 2>/dev/null
cp sp.node /etc/conf/node.d/sp 2>/dev/null
cp ldterm.sdevice /etc/conf/sdevice.d/ldterm 2>/dev/null
cp ptem.sdevice /etc/conf/sdevice.d/ptem 2>/dev/null
cp ptm.sdevice /etc/conf/sdevice.d/ptm 2>/dev/null
cp pts.sdevice /etc/conf/sdevice.d/pts 2>/dev/null
cp sp.sdevice /etc/conf/sdevice.d/sp 2>/dev/null


/bin/sh /etc/conf/bin/idtune -m NSTREAM 128
/bin/sh /etc/conf/bin/idtune -m NQUEUE 512
/bin/sh /etc/conf/bin/idtune -m NBLK4096 4
/bin/sh /etc/conf/bin/idtune -m NBLK2048 32
/bin/sh /etc/conf/bin/idtune -m NBLK1024 32
/bin/sh /etc/conf/bin/idtune -m NBLK512 32
/bin/sh /etc/conf/bin/idtune -m NBLK256 64
/bin/sh /etc/conf/bin/idtune -m NBLK128 256
/bin/sh /etc/conf/bin/idtune -m NBLK64 256
/bin/sh /etc/conf/bin/idtune -m NBLK16 256
/bin/sh /etc/conf/bin/idtune -m NBLK4 128
/bin/sh /etc/conf/bin/idtune -m SHLBMAX 8


# increase number of pseudo terminals from 16 to 32

echo "Adjust kernel configuration files for 32 pseudo terminals..."

if [ `grep pty /etc/conf/sdevice.d/pty | cut -f3` != 16 ] ; then
	echo "There are already more then 16 pty's."
else


mv /etc/conf/sdevice.d/pty /etc/conf/sdevice.d/pty.old
nawk '{
 if ($1 == "pty") {
    print $1 "\t" $2 "\t" 32 "\t" $4 "\t" $5 "\t" $6 "\t" $7 "\t" $8 "\t" $9 "\t" $10
 }
 else
    print
}'   </etc/conf/sdevice.d/pty.old >/etc/conf/sdevice.d/pty
rm /etc/conf/sdevice.d/pty.old

mv /etc/conf/sdevice.d/ptx /etc/conf/sdevice.d/ptx.old
nawk '{
 if ($1 == "ptx") {
    print $1 "\t" $2 "\t" 32 "\t" $4 "\t" $5 "\t" $6 "\t" $7 "\t" $8 "\t" $9 "\t" $10
 }
 else
    print
}'   </etc/conf/sdevice.d/ptx.old >/etc/conf/sdevice.d/ptx
rm /etc/conf/sdevice.d/ptx.old

mv /etc/conf/cf.d/mdevice /etc/conf/cf.d/mdevice.old
nawk '{ 
 if ($1 ~ "pt[y|x]") { 
    print $1 "\t" $2 "\t" $3 "\t\t" $4 "\t" $5 "\t" $6 "\t" $7 "\t" 32 "\t" $9
 }
 else
    print
 }'  </etc/conf/cf.d/mdevice.old >/etc/conf/cf.d/mdevice
rm /etc/conf/cf.d/mdevice.old

echo "pty	ptyq0	c	16
pty	ptyq1	c	17
pty	ptyq2	c	18
pty	ptyq3	c	19
pty	ptyq4	c	20
pty	ptyq5	c	21
pty	ptyq6	c	22
pty	ptyq7	c	23
pty	ptyq8	c	24
pty	ptyq9	c	25
pty	ptyqa	c	26
pty	ptyqb	c	27
pty	ptyqc	c	28
pty	ptyqd	c	29
pty	ptyqe	c	30
pty	ptyqf	c	31" >>/etc/conf/node.d/pty

echo "ptx	ttyq0	c	16
ptx	ttyq1	c	17
ptx	ttyq2	c	18
ptx	ttyq3	c	19
ptx	ttyq4	c	20
ptx	ttyq5	c	21
ptx	ttyq6	c	22
ptx	ttyq7	c	23
ptx	ttyq8	c	24
ptx	ttyq9	c	25
ptx	ttyqa	c	26
ptx	ttyqb	c	27
ptx	ttyqc	c	28
ptx	ttyqd	c	29
ptx	ttyqe	c	30
ptx	ttyqf	c	31" >>/etc/conf/node.d/ptx

fi

echo "*** New Installation ***"
echo "If this is a new installation of XFree86 $VERSION use \"kconfig\" to build a new"
echo "kernel. XFree86 won't run without this new kernel."
echo ""
echo "<press return>"
read answer

#
# install additional termcap & terminfo entries
#
echo "Install additional termcap & terminfo entries..."
grep xterm   /etc/termcap >/dev/null || cat xterm.termcap >>/etc/termcap
grep sun-cmd /etc/termcap >/dev/null || cat sun.termcap   >>/etc/termcap

tic sun.terminfo 2>/dev/null
tic xterm.terminfo 2>/dev/null

if [ -s /usr/lib/loadfont/vga437.bdf ]
then
    echo "Convert VGA font for using under X11..."
    sed -e 's/FONT 8x16/FONT vga/' </usr/lib/loadfont/vga437.bdf \
    | bdftopcf -t >/usr/X386/lib/X11/fonts/misc/vga.pcf
    chmod 644 /usr/X386/lib/X11/fonts/misc/fonts.dir
    mkfontdir /usr/X386/lib/X11/fonts/misc
fi
echo READY

