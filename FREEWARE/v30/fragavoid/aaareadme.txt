Released by GCE and Acorn for the freeware CD or other general consumption.

	The Fragmentation Avoider

This utility for Vax or Alpha came out of the Safety product available
from General Cybernetic Engineering. and was further updated at Acorn
Software for ease of use and separate installability. It is presented
in source as a "calling card"; we hope that you will look over the SPDs
for the Acorn Software products in this area, and for the SPDs of the
GCE Safety components (DPS and EACF) in this area.


Now to the frag inhibitor.

The frag inhibitor will largely prevent disks from becoming fragmented
and will speed up disk access. In a nutshell, FI causes file extends to
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

Acorn Software also sells:

Squash - full function compressing disk on VMS. Allows r/w access,
compression in realtime, and works with all VMS utilities and any
file structure. (Yes, you can put Spiralog on it.)

Virtual Branches - Gives automatic, clusterwide support to accessing
jukeboxes of disks; no user interaction is needed. Unlike the
competition, VB can fail over its control, works perfectly in VMS
clusters of all sorts, is NOT subject to cluster hangs, infinite
mount verify loops, or running out of MSCP server units as the
competition is, and offers transparent access to disks, with arbitration
to ensure that the disks actually get work done, rather than swapping
constantly before work can be done. Most jukeboxes on the market are
supported.

Branches - An "operator in a box" for tape jukeboxes. Set it and forget
it, works on VMS Vax or Alpha, with almost any tape jukebox on the
market and is invisible to other tape management packages.

HSManager - Hierarchical Storage for VMS. Works multi-level, uses
standard utilities for file moving and storage, supports soft
linking as well as shelving, highly configurable, monitors disk
space and responds intelligently, and even speeds up I/O on VMS
disk file structures. Also is ready for ANY VMS based file system,
not just ODS-2. Simple setup via a fullscreen configuration tool is
supported.

See SQUASHSPD.TXT, VIRTUALBRANCHESSPD.TXT, BRNCH-SPD.TXT, and HSMSPD.TXT.

Contact sales@acornsw.com for further information.



General Cybernetic Engineering also sells:

Safety (tm), the complete data safety package for OpenVMS. Safety
provides:

* Reliable UNDELETE (turns the DELETE operation into a wastebasket
	one, allows routing deleted files, selective protection etc.

* Vastly enhanced file security to protect your critical data. Virus
	or Trojan protection, finegrain access controls, file hiding,
	file passwords, softlinks, space management, privilege control,
	and more give simple-to-use controls available nowhere else in
	the industry. 

"Set it and forget it" operation is facilitated in Safety for all functions.

See DPSSPD.TXT and EACFSPD.TXT.

Contact sales@gce.com for ordering information or info@gce.com for
further technical information.
