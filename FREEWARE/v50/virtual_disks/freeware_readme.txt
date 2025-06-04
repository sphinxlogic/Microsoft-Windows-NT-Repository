VIRTDSKS, Storage_Mgmt, Virtual disk and tape devices

This collection is a set of virtual disk and tape drivers implementing
disk on contiguous file (or area of disk), remote disk over DECnet,
remote tape over DECnet, disk on noncontiguous file, various journalling
disk variants, a striping driver or two, a shadowing driver or two, 
and more. The shadowing is somewhat limited but does allow use of
the disk while shadow copy is going on. Some *.doc and *.txt
files are included which describe more about how to use individual
pieces.

Glenn Everhart

Please check out safetyspd.txt here, a package I have which may also
be useful.

See also the [compressingdisk] directory for a compressing VMS disk (Vax
or Alpha) that works, and see [shadow_unequal_disks] for a virtual disk
that on Alpha will work with DEC volume shadowing and allow shadowing
partial volumes.

