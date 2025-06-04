$ !
$ !                       K I T I N S T A L . C O M 
$ !
$ !                       Copyright © 1993, 1994 by
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
$ say "      Installing Logical Disk Utility version V5.1"
$ say ""
$!
$ vmi$callback set purge ask
$!
$ vmi$callback check_vms_version version_check "055" ! minimum needed
$ !
$ if .not. version_check
$ then
$   vmi$callback message e incvmsver -
         "This installation requires at least VMS version V5.5"
$   exit vmi$_failure
$ endif
$ !
$ platform = "vax"
$ platform_type = f$getsyi("hw_model")
$ if platform_type .gt. 1023 then platform = "alpha"
$ if platform .eqs. "alpha"
$ then
$   vmi$callback message e incarch -
         "This product is not yet supported on Alpha"
$   exit vmi$_failure
$ endif
$!
$ vms_type = f$element(0,",",vmi$vms_version)
$ vms_version = f$element(1,",",vmi$vms_version)
$ full_vms_version = f$element(1,",",vmi$full_vms_version)
$ driver = "lddriver_v60"
$ if vms_version .eqs. "055" then driver = "lddriver_v55"
$ !
$ say ""
$ set on
$ on error then goto error
$ on severe_error then goto error
$ vmi$callback message i linking "Linking the LD management utility"
$ link/nodebug/notrace/exe=vmi$kwd:ld.exe -
vmi$kwd:ld,vmi$kwd:ldmsg,sys$input/opt
identification="LD V5.1"
sys$library:vaxcrtl.exe/share
$ vmi$callback message i linking "Linking the LD driver"
$ define/user sys$output nla0:
$ define/user sys$error nla0:
$ link/symbol_table=vmi$kwd:lddriver/exec=vmi$kwd:lddriver vmi$kwd:'driver',sys$input/opt
base=0
$!
$ vmi$callback provide_file vms$ ld.exe vmi$root:[sysexe]
$ vmi$callback provide_file vms$ lddriver.exe vmi$root:[sys$ldr]
$ vmi$callback provide_file vms$ lddriver.stb vmi$root:[sys$ldr]
$ vmi$callback provide_dcl_help ld.hlp
$ vmi$callback provide_dcl_command ldcld.cld
$ vmi$callback provide_file vms$ ld$startup.com vmi$root:[sys$startup]
$!
$ say ""
$ say "*****************************************************************"
$ if vms_version .eqs. "055"
$ then
$    say "Please enter the following line in your SYSTARTUP_V5 procedure:"
$ else
$    say "Please enter the following line in your SYSTARTUP_VMS procedure:"
$ endif
$ say "$ @SYS$STARTUP:LD$STARTUP"
$ say "*****************************************************************"
$ say ""
$!	
$ exit vmi$_success
$!
$error:
$ exit vmi$_failure
