RAMDISK, Storage Management, Fast RAMDISK driver for OpenVMS VAX

								March 1993

Here's yet another new version of the RAMDISK driver I wrote lo these
many moons ago.  This version fixes a few bugs, and adds some enhancements.

The most important bug fix has to do with mount verification support for
VMS V5.5:  DEC changed the way this behaved, with some consequences for
RAMDRIVER.  The basic difference is that the mount verify code will place
IO$_PACKACK (among other things) IRPs directly onto the RAMDRIVER's I/O
queue, which is not what it expected to find there.  The result was often
a system crash.  This has been fixed.  (Cluster transitions will put disks
into mount verify these days, including ramdisks.).

The most important new feature is that the RAMDRIVER no longer uses non-paged
pool for its disk "blocks"  - it swipes pages off of the free page list and
maps them into system address space.  The nice thing about this scheme is that
you can give the pages back to the system as something besides non-paged pool
when you're done using the RAMDRVIER.

MSCP support is included in this version, though why anyone would find it
useful mystifies me.  The biggest problem with MSCP support turns out to be
brain damage in the MSCP server -- it doesn't bother to set IRP$V_FUNC in
IRP$W_FLAGS for read requests (like everything else does) -- and we were
dispatching based on this flag.  Also bear in mind that while access to
the RAMDISK on a local machine can be multi-threaded, access via the MSCP
server is single threaded.  Most of the performance advantage gained by
not having any rotational latency is negated by the number of data copies
that go on when using MSCP to access a remote ramdisk.  Note that the device
name for the RAMDISK must be RDA0 if you intend to use it via MSCP.

Before anyone asks, I have no idea if you can bind two of them into a shadow
set or into a stripe set :-)

The original text file that came with the 1988 version of this is called
ORIGINAL-RAMD-BLURB.TXT.  The previous new blurb file (which came with the
last version) is called OLD-RAMD-BLURB.TXT.

Included once again is a MAKE.COM, which will build everything, and an INITVD
program to initialize the RAMDISK.  You still need lots of spare SPTREQ to use
this.  If you attempt to initialize a RAMDISK and win an %SYSTEM-F-INSFSPTS,
then you need to raise the SYSGEN parameter SPTREQ and reboot.

To short circuit the inevitable deluge of questions about how to use it, build
the driver (use $ @MAKE), and then use SYSGEN from an account with CMKRNL to
load it:

$ Run SYS$SYSTEM:SYSGEN
SYSGEN> LOAD disk:[dir]RAMDRIVER
SYSGEN> CONNECT RDA0/NOADAPTER/DRIVER=RAMDRIVER
SYSGEN> EXIT
$

Then use INITVD to expand the size of the disk to the desired number of blocks
(you must have sufficient free memory on your system to support the size of the
disk you want):

$ Run INITVD
Init-VD> RDA0 size
$

Then, initialize, mount, and use it in the standard way:

$ Initialize RDA0 FOOBAR
$ Mount/System RDA0 FOOBAR
$ Create/Directory RDA0:[TMP]

etc.

No, the contents of the RAMDISK are not saved across system boots (yes, people
asked me this question :-).

RAMDRIVER is still a spare-time project.  I am grateful to TGV, Inc (my new
employer) for their tolerance of my fixing RAMDRIVER, and to Ehud Gavron
(gavron@aces.com) for testing it thoroughly and several helpful suggestions.

This version is for VAX only;  the next spare-time project is an Alpha version.

This is still UNSUPPORTED code.  Use it at your own risk.  Enhancements and bug
fixes welcome;  I'll deal with bug reports as spare time allows.

Please  *DON'T*  send bug reports or questions about RAMDRIVER to service@tgv.com;
this is not a TGV product, and is not supported by TGV.


Gerard K. Newman,  TGV, Inc.  603 Mission St.  Santa Cruz, CA  95060
gkn@tgv.com | 619.591.4717 (voice+fax) | 800.TGV.3440 | 408.427.4366
