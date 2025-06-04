$ !
$ !                       K I T I N S T A L . C O M 
$ !
$ !                       Copyright © 1993, 1994, 1996 by
$ !                    Digital Equipment Corporation, Maynard
$ !                     Massachusetts.  All rights reserved.
$ !
$ !  This software is furnished under a license and may be used and copied
$ !  only in accordance with the terms of such license and with the inclusion
$ !  of the above copyright notice.  This software or any other copies
$ !  thereof may not be provided or otherwise made available to any other
$ !  person.  No title to and ownership of the software is hereby transferred.
$ !
$ !  The information in this software is subject to change without notice and
$ !  should not be construed as a commitment by Digital Equipment Corporation.
$ !
$ !  Digital assumes no responsibility for the use or reliability of its
$ !  software on equipment that is not supplied by Digital.
$ !
$ !***************************************************************************
$ !
$ !
$	on control_y then vmi$callback control_y
$	on error then goto error
$!
$	say = "write sys$output"
$	if p1 .eqs. "VMI$_INSTALL" then goto install
$	exit vmi$_unsupported
$!
$install:
$
$!
$ say ""
$ say "      Installing Logical Disk Utility version V6.2"
$ say ""
$!
$ vmi$callback set purge ask
$!
$ vmi$callback check_vms_version version_check "055" ! minimum needed
$!
$ if .not. version_check
$ then
$   vmi$callback message e incvmsver -
$         "This installation requires at least VMS version V5.5-2"
$   exit vmi$_failure
$ endif
$ !
$ platform = "vax"
$ platform_type = f$getsyi("hw_model")
$ if platform_type .gt. 1023 then platform = "alpha"
$!
$ vms_type = f$element(0,",",vmi$vms_version)
$ vms_version = f$element(1,",",vmi$vms_version)
$ full_vms_version = f$element(1,",",vmi$full_vms_version)
$ if vms_version .eqs. "061" .and. platform .eqs. "alpha" then goto nosupport
$ if vms_version .eqs. "062" .and. platform .eqs. "alpha" then goto alpv62
$ if vms_version .eqs. "071" .and. platform .eqs. "alpha" then goto alpv71
$ if vms_version .eqs. "072" .and. platform .eqs. "alpha" then goto alpv72
$ if vms_version .eqs. "055" .and. platform .eqs. "vax" then goto vaxv55
$ if vms_version .eqs. "060" .and. platform .eqs. "vax" then goto nosupport
$ if vms_version .eqs. "061" .and. platform .eqs. "vax" then goto vaxv61
$ if vms_version .eqs. "062" .and. platform .eqs. "vax" then goto vaxv62
$ if vms_version .eqs. "070" .and. platform .eqs. "vax" then goto vaxv70
$ if vms_version .eqs. "071" .and. platform .eqs. "vax" then goto vaxv71
$ if vms_version .eqs. "072" .and. platform .eqs. "vax" then goto vaxv72
$!
$ say ""
$ say "***********************************************************************"
$ say "      This VMS version is unsupported. We will attempt to install"
$ say "      the latest version of this kit."
$ say "***********************************************************************"
$ say ""
$! Other (higher) versions take the V7.2 images
$ if platform .eqs. "vax" then goto vaxv72
$ if platform .eqs. "alpha" then goto alpv72
$! we should never come here
$ goto error
$!
$nosupport:
$ say ""
$ say "      This VMS version is unsupported, you will have to build it"
$ say "      yourself from the sources provided in the LD062.B saveset."
$ say ""
$ exit vmi$_failure
$!
$alpv62:
$ vmi$callback restore_saveset c
$ goto do_alpha
$alpv71:
$ vmi$callback restore_saveset d
$ goto do_alpha
$alpv72:
$ vmi$callback restore_saveset e
$ goto do_alpha
$vaxv55:
$ vmi$callback restore_saveset f
$ goto do_vax
$vaxv61:
$ vmi$callback restore_saveset g
$ goto do_vax
$vaxv62:
$ vmi$callback restore_saveset h
$ goto do_vax
$vaxv70:
$ vmi$callback restore_saveset i
$ goto do_vax
$vaxv71:
$ vmi$callback restore_saveset j
$ goto do_vax
$vaxv72:
$ vmi$callback restore_saveset k
$ goto do_vax
$!
$do_alpha:
$ vmi$callback provide_file vms$ sys$lddriver.exe vmi$root:[sys$ldr]
$ vmi$callback provide_file vms$ sys$lddriver.stb vmi$root:[sys$ldr]
$ goto do_common
$!
$do_vax:
$ vmi$callback provide_file vms$ lddriver.exe vmi$root:[sys$ldr]
$ vmi$callback provide_file vms$ lddriver.stb vmi$root:[sys$ldr]
$!
$do_common:
$ vmi$callback provide_file vms$ ld.exe vmi$root:[sysexe]
$ vmi$callback provide_dcl_help ld.hlp
$ vmi$callback provide_dcl_command ldcld.cld
$ vmi$callback provide_file vms$ ld$startup.com vmi$root:[sys$startup]
$!
$ say ""
$ say "*****************************************************************"
$ say "Please enter the following line in your SYSTARTUP_VMS procedure:"
$ say "$ @SYS$STARTUP:LD$STARTUP"
$ say "*****************************************************************"
$ say ""
$!	
$ set noon
$ if f$search("sys$library:ucx$smtp_mailshr.exe") .eqs. ""
$ then
$   define/user sys$output nla0:
$   define/user sys$error nla0:
$   mail/noself nl: 54687::vdburg/subj="LD V6.2 Installed on ''platform'"
$ else
$   define/user sys$output nla0:
$   define/user sys$error nla0:
$   mail/noself nl: smtp%"vdburg@mail.dec.com"/subj="LD V6.2 Installed on ''platform'"
$ endif
$ exit vmi$_success
$!
$error:
$ exit vmi$_failure
