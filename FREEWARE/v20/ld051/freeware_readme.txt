LDDRIVER, SYSTEM MANAGEMENT, VAX Virtual disk driver

The logical disk utility is a system management tool available
to any user for controlling logical disk usage. It can also be used
for testing out things like volume shadowing, striping, host-based
raid, or software which does dangerous things to disks without disturbing
a real disk.

A Logical Disk is a file available on a Physical Disk, which acts as a
real Physical Disk. The Logical Disks can be created in any directory
of the Physical Disk.

A large disk can be divided into smaller sections, each a Logical Disk,
supporting the same I/O functions as the Physical Disk. By giving the
Logical Disk File a good  protection level and mounting it private or
with device protection, you are able to add a number of protection
levels to your file system.

Logical disks may be just a single disk, part of a volumeset, part of
a stripeset, part of a host-based shadowset, part of a host-based raid
set or any combination.

The file to be used for the logical disk may be placed on any physical disk,
in any directory. A backup can be made to protect the disk.

A physical device may be 'replaced' by a logical disk to enable logging of
all I/O of the physical disk. In that case we don't use any container file.

The only limitation at present is that this tool needs at least OpenVMS VAX
Version V5.5, and that it does not yet run on OpenVMS AXP. This support will
be created in the future.

The driver can be installed with VMSINSTAL. The first saveset (LD051.A)
contains the kit, while the second saveset (LD051.B) contains the sources.
After installation full help is available with HELP LD.

For any questions please contact the author:

	Jur van der Burg
	Digital Equipment B.V.
	Utrecht, Holland
	Internet: vdburg@utrtsc.uto.dec.com.

