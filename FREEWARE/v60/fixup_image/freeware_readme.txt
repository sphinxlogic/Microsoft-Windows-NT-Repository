FIXUP_IMAGE, UTILITIES, Fix an image's shareable image information

Copyright ©2001, Applied Synergy, Inc.  All rights reserved.

FIXUP_IMAGE can edit the shareable image information in an executable image
file.  This can be used to avoid the dreaded:

%DCL-W-ACTIMAGE, error activating image VAXCRTL
-CLI-E-IMGNAME, image file DKA0:[SYS0.SYSCOMMON.][SYSLIB]VAXCRTL.EXE
-SYSTEM-F-SHRIDMISMAT, ident mismatch with shareable image

In some cases, it can also remove extraneous references to shareables that do
not exist on the current system.  For example, it can remove this message which
can occur when you try to run an image linked under VMS V6 or later on a VMS V5
system:

%DCL-W-ACTIMAGE, error activating image CMA$TIS_SHR
-CLI-E-IMAGEFNF, image file not found DKA0:[SYS0.SYSCOMMON.][SYSLIB]CMA$TIS_SHR.EXE;


FIXUP_IMAGE V2.0 only supports VAX executables.

--------------------------------------------------------------------------------
FEEDBACK

Comments, suggestions, and questions about this software can be directed
to eMail address:

	fixup_image@applied-synergy.com

Applied Synergy, Inc. may or may not respond to these messages.

--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT ©2001, Applied Synergy, Inc. ALL RIGHTS RESERVED.
Permission is granted for not-for-profit redistribution, provided all source
and object code remain unchanged from the original distribution, and that all
copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".  The author and Applied Synergy, Inc. make
no representations or warranties with respect to the software and specifically
disclaim any implied warranties of merchantability or fitness for any
particular purpose.
