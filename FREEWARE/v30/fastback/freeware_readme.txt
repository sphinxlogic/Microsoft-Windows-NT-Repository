FASTBACK, Utilities, Facility to access BACK/PHYS or HSC Backup saves as disks

Fast Backup

This package allows use of VMS Backup BACKUP/PHYSICAL by giving a means
to present a back/phys image as a disk to the system. Thus you can
use back/phys, which is about twice as fast as regular backup,
but still access saved data as individual files. The saveset must
all fit on one tape (need not be first saveset on the tape though)
and the utility can use CRC and XOR blocks to reconstruct bad data.
  The "disk" will appear on device DTA0: (which ought to please those
old enough to remember DECtape). The package has been tested on 8mm,
9track tape, 4mm, and TK cartridges and should work on any tape like
device. It uses fast skips to move around the tape and has a large
internal cache to speed operation.

Offered as shareware by Glenn C. Everhart, 25 Sleigh Ride Rd., Glen Mills
PA 19342-1440. If you use and like the package a donation of $10 would
be appreciated. 

The dthost_start.com file will get things started for you.

Build:

$macro dthstimg
$macro dtdriver
$link dtdriver+sys$system:sys.stb/sel+sys$input/opt
base=0
$link dthstimg+sys$system:sys.stb/sel

AXP Use:

The code herein should work with FQdriver also, provided that the UCB
definitions match in fdhstimg and the driver used. This will allow
operation on AXP VMS as well as on VAX. Use fqdriver_s2.mar for AXP
VMS 6.1.

Glenn C. Everhart
Everhart@Arisia.GCE.Com
