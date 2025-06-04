FRAGAVOID, System_Management, a tool to prevent disk defragmentation

Fragmentn Avoid, DEFRAG, Avoid ~98% of dsk frag, 30%+ Disk Speedup w/src
Released by Acorn and GCE for the freeware CD or other general consumption.



	The Fragmentation Avoider

This utility actively prevents most disk fragmentation from occurring,
so that disks run less fragmented, and usually considerably faster. It
can be set up and left running with no attention; you configure it, turn
it on, and forget it, enjoying faster I/O and disks that grow fragmented
much more slowly than before. Your access to disks does not change at all
in detail.

This utility for Vax or Alpha came out of the Safety product available
from General Cybernetic Engineering and was extended by Acorn Software
to make its user interface more usable. It is presented in source as a
"calling card"; we hope that you will look over the SPDs for the
Acorn products and for the Safety components DPS and EACF in this area.
They provide many services for storage management unavailable from
any other places and include a complete "operator in a box" solution for
tape jukeboxes, a cluster-aware disk jukebox solution that allows failover,
and does not exhaust the MSCP server, full function, multi-level HSM,
realtime compressing storage (disk space trebler), reliable user UNDELETE
(which turns deletion to a "move to wastebasket"), and vast extensions to VMS
file security.

Now to the Frag Avoider.

The frag avoider will largely prevent disks from becoming fragmented
and will speed up disk access. In a nutshell, FA causes file extends to
be a fraction of the initial file size (with some safeguards so the
disk isn't filled etc.) so that VMS will not need to extend it as often.
This results in files less fragmented than they otherwise would be,
and writing doesn't need to "go to the well" as often. Some tests of
random loads have shown up to a 30% speedup. (Enjoy!!) It also by this
means, and by doing extends Contig Best Try, reduces the amount of
"chaff" space in the extent cache, so your files tend to be placed with
a "first fit" algorithm. The reduced fragmentation speeds read access.
It will not interfere with normal operation or even disk defragmenters,
but if you are nervous it can be turned off or on when you want. Normally
it is expected to be set up in your systartup_vms script however.

FA has been in use at several sites for a few years now. 

VMS disks in ordinary operation become fragmented rather quickly
once defragmented. This leads to files which require many index file
reads to locate and to lots of unnecessary disk head motion. On
optical disks, this is an even worse problem, since seek times are much
longer than magnetic disks.

There are several reasons for this rapid refragmentation which
deserve exploration. First is the way VMS uses its "extent cache".
Whenever disk space is released, pointers to it are kept in memory
awhile, so that space can be quickly allocated. The problem is that
whenever files are closed, they are generally truncated to return
extra allocated space to the system. The returned space is usually
in small pieces, and thus new space requests wind up being satisfied
from the last bunch of these little pieces of disk, rather than from
larger areas. When files are deleted, they leave these pieces around
which only get cut up still more with time. The result is fragmentation.

Also, it is often the case that files are left open for fairly long
times, and grow a few blocks at a time, typically allocating the
volume default allocation, but sometimes allocating a small fixed
number of blocks. Many of these files may grab 5-block extents
thousands of times over their lives; this inevitably produces badly
fragmented files which are hard to defragment because they're open.
It also wastes a lot of time calling the file system for every few
blocks.

The Fragmentation Avoider is a system for automatically addressing
these problems, and incidentally solving a few others.

The Fragmentation Avoider arranges for file extension to be done
"Contiguous Best Try" when this is possible. This causes VMS to
flush (i.e., forget about) its extent cache and attempt to find
a space big enough to hold the whole requested area before using
badly fragmented store. The program can do this for every extension
(the default and recommended behavior) or every Nth extension,
if you feel the need to have the extent cache used, but cleared
periodically. 

Fragmentation Avoider also controls the amount of space requested
when a program extends a file (i.e., makes it longer). It can
set a minimum extent request, so even explicit requests for one
block at a time can be increased, and is able to request that a
file be extended by 1/N of its current size, subject to free space
constraints on the volume and a maximum extend amount. It will
always request at least as much space as the program wanted, but
will attempt to extend by 1/N of the file size, 1/8 of the available
free space on the disk, or the maximum extent request, whichever
is least.

File extension is an expensive operation; by arranging it to be less
often needed, F.A. can permit file writing to enjoy up to a 30%
speed gain, in addition to permitting faster access to files written with
it. This is the gain from not having to do the extend operation
as often.

SUMMARY:

Fragmentation Avoider permits disk fragmentation to be controlled
by altering somewhat the VMS file extend operation in a way
that greatly slows the refragmentation of disk space. The package
can be installed on any disks where this is desired and its
parameters can be set on a per-disk basis. Disks where the 
Fragmentation Avoider is not installed are unaffected.


Installation:

See the AAAREADME.TXT. FA is installed with VMSINSTAL.

(And don't forget to read the SPDs!!!)

