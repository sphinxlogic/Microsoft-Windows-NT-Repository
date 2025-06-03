$!                       L D $ S T A R T U P . C O M
$!
$!                       Copyright © 1993 by
$!                    Digital Equipment Corporation, Maynard
$!                     Massachusetts.  All rights reserved.
$!
$!  This software is furnished under a license and may be used and copied
$!  only in accordance with the terms of such license and with the inclusion
$!  of the above copyright notice.  This software or any other copies
$!  thereof may not be provided or otherwise made available to any other
$!  person.  No title to and ownership of the software is hereby transferred.
$!
$!  The information in this software is subject to change without notice and
$!  should not be construed as a commitment by Digital Equipment Corporation.
$!
$!  Digital assumes no responsibility for the use or reliability of its
$!  software on equipment that is not supplied by Digital.
$!
$!***************************************************************************
$!
$ set noon
$ run sys$system:sysgen
reload lddriver
connect lda0/noadapter
$!
$ if f$file_attributes("sys$system:ld.exe","known")
$ then
$   install replace sys$system:ld.exe
$ else
$   install create sys$system:ld.exe/open/head/share/priv=(phy_io,syslck,share)
$ endif
$!
$ exit
