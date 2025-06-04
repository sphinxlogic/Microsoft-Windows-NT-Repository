LDDRIVER, SYSTEM_MANAGEMENT, VAX/Alpha Virtual disk driver

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

Another possibility is logging of all I/O requests for the driver, as well
as suspending/resuming I/O requests on certain conditions.

A physical device may be 'replaced' by a logical disk to enable logging of
all I/O of the physical disk. In that case we don't use any container file.

This driver can be used to create for example ODS-2 Cdrom's on a PC.
Just create a containerfile, hook it to a logical disk drive, mount it
and copy anything you like. Then take the container file to a pc, and
burn this file in raw format to a Cdrom.

The driver is now supported for both the VAX and the Alpha architectures.

The only limitation at present is that this tool needs at least OpenVMS
Version V5.4. Binaries are only supplied for version V5.5-2 and up for
OpenVMS VAX, and for V6.2 and up for OpenVMS Alpha. But since the full
sources are included a straight rebuild should be enough to get it
running on V5.4 and up. Be sure to read the releasenotes before an attempt
is made to rebuild from scratch.

The driver can be installed with VMSINSTAL. The LD062.B saveset contains
the sources, the other kits the binaries for various VMS versions.
After installation full help is available with HELP LD.

For any questions please contact the author:

        Jur van der Burg
        Digital Equipment B.V.
        Europalaan 44
        3526 KS Utrecht
        The Netherlands
	Internet: vdburg@mail.dec.com
        or        jur.vanderburg@digital.com

