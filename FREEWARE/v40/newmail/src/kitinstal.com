$!
$!			K I T I N S T A L . C O M
$!
$!
$!***************************************************************************
$!
$!
$!	This procedure installs NEWMAIL on VMS Version 5 using VMSINSTAL
$!
$!
$!  Take care of interrupts
$!
$	on control_y then vmi$callback control_y
$!
$!  Set up error handling
$!
$	on warning then exit $status
$!
$!  Process request types
$!
$	if p1 .eqs. "VMI$_INSTALL" then goto product_install
$	exit vmi$_unsupported
$!
$!  Perform product installation
$!
$Product_install:
$!
$!  Check VMS version number
$!
$	product$type = "RELEASED"
$	product$version = "050"
$	vmi_type = f$element(0,",",vmi$vms_version)
$	vmi_type = f$element(0," ",vmi_type)
$	if (vmi_type .eqs. "RELEASED") .or. (vmi_type .eqs. "UPDATE") -
	then goto 10$
$	if vmi_type .eqs. "UPGRADE" then goto 20$
$	goto wrong_version
$10$:	vmi_version = f$element(1,",",vmi$vms_version)
$	if f$extract(0,2,product$version) .nes. f$extract(0,2,vmi_version) -
	then goto wrong_version
$	if f$extract(2,1,product$version) .les. f$extract(2,1,vmi_version) -
	then goto version_ok
$	goto wrong_version
$20$:	vmi_version = f$element(1,",",vmi$vms_version)
$	vmi$callback find_file newmail_ vmi$root:[syslib]mailshr.exe "" s -
		newmail_find_status
$	if newmail_find_status .eqs. "S" then goto version_ok
$Wrong_version:
$	vmi$callback message e version -
	"This kit must be installed on a VMS Version 5 system.
$	exit vmi$_failure
$Version_OK:
$!
$!  Check for disk space
$!
$	vmi$callback check_net_utilization product_space 200
$	if .not. product_space then exit vmi$_failure
$!
$!  Disable safe installation
$!
$	vmi$callback set safety conditional 300
$!
$!  Ask about PURGE
$!
$	vmi$callback set purge ask
$!
$!  No IVP provided
$!
$	vmi$callback set ivp no
$!
$!  Prompt for installation type
$!
$	install_utility := true
$	install_source := false
$Ask_install_option:
$	type sys$input
There are three types of installations that may be performed for this kit.
They are:

  1. Install the NEWMAIL utility.
  2. Install the utility and source kit.
  3. Install source kit only.

$ eod
$	vmi$callback ask option_number "Type of installation to perform" "1" I
$	if (option_number.lt.1).or.(option_number.gt.3) then goto ask_install_option
$	if option_number.eq.1 then goto install_product
$	type sys$input
Product source will be installed in subdirectory [.NEWMAIL] in SYS$EXAMPLES:
$	install_source := true
$	if option_number.eq.3 then install_utility := false
$!	vmi$callback restore_saveset b
$!
$!  Perform product installation
$!
$Install_product:
$	type sys$input
No more question will be asked.
$Install_utility:
$	if .not.install_utility then goto install_source
$	link /notrace /executable=vmi$kwd:newmail -
		vmi$kwd:new_install.opt/options
$	vmi$callback provide_image product_ newmail.exe vmi$root:[sysexe]
$	vmi$callback provide_dcl_command newmail.cld
$	vmi$callback provide_dcl_help newmail.hlp
$	vmi$callback provide_file product_ newmail050.release_notes vmi$root:[syshlp]
$Install_source:
$	if .not.install_source then goto product_done
$	vmi$callback create_directory user sys$common:[syshlp.examples.newmail] ""
$	define new_source_dir sys$common:[syshlp.examples.newmail]
$	vmi$callback provide_file product_ newmail.cld new_source_dir:
$	vmi$callback provide_file product_ newmail.mms new_source_dir:
$	vmi$callback provide_file product_ newmail.opt new_source_dir:
$	vmi$callback provide_file product_ newmail.bas new_source_dir:
$	vmi$callback provide_file product_ test_cld.cld new_source_dir:
$	vmi$callback provide_file product_ $mailmsgdef.bas new_source_dir:
$	vmi$callback provide_file product_ mail$routines.bas new_source_dir:
$Product_done:
$	exit vmi$_success
