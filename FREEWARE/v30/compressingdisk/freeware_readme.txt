CMP-Disk, Storage Management, Realtime Compressing virtual disk, saves 2:1 to 3:1 space w/src

Need to store a lot of archival information online and lack the space?
This utility can save factors of 3:1 in storage space for any VMS
file structure.

This area contains a compressing VMS disk for Vax or Alpha
(cmphighc.zip), Alpha versions of disk drivers for remote virtual
disk over DECnet and host processes, and for files stored on
somewhat encrypted files on local disks. The remote disk can be used
for remote backup or access and will attempt to recover if the net
link fails. 

Also present are some other odds and ends, and the SPD for a
user undelete (reliable, this one) and enhanced VMS security module
called SAFETY from the author.

You are requested to read this also.

The compressing disk works by taking a compressed image of an entire
disk in a utility. The virtual disk driver (called dtdriver) and
server program communicate to present the compressed image to the
system as a readonly disk whose storage occupies normally 1/2 or
less of the blocks the original disk does. For databases, the savings
can be even more impressive. The containers can be accessed across
DECnet from many sites at once if desired.

Either of two algorithms may be used...a blindingly fast one, for
slow VAXen normally, or the zlib (zip) one which compresses better
but is slower. Build it for the one you want.
