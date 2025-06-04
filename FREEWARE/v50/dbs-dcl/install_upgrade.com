$ ! Procedure:	INSTALL_UPGRADE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ interactive = (f$mode() .eqs. "INTERACTIVE")
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ say = "write sys$output"
$ ask = "@olga$sys:sys_ask"
$ zip = "$olga$sys:zip ""-V"""
$ unzip = "$olga$sys:unzip"
$ true = 1
$ false = 0
$ sys__control_y == 0
$ sys__control_z == 0
$ sys__backwards == 0
$ if_abort = "if (sys__control_y .or. sys__control_z) then"
$ if_backwards = "if (sys__backwards) then"
$ pid = f$getjpi("","PID")
$ directory_spec = "TEMP_''PID'.DIR;1"
$ subdirectory = "[.TEMP_''pid']"
$ kit_list = f$edit(P1,"COLLAPSE,UPCASE")
$ cleanup_directory = f$edit(P2,"COLLAPSE,UPCASE")
$ cleanup_zip_file = f$edit(P3,"COLLAPSE,UPCASE")
$ log_actions = f$edit(P4,"COLLAPSE,UPCASE")
$ if (.not. interactive)
$   then
$   if (cleanup_directory .eqs. "") then cleanup_directory = "Yes"
$   if (cleanup_zip_file .eqs. "") then cleanup_zip_file = "No"
$ endif !(.not. interactive)
$ if (log_actions)
$   then
$   log_qual = "/LOG"
$ else
$ log_qual = "/NOLOG"
$ endif !(log_actions)
$ if ((kit_list .eqs. "") .and. .not. interactive) then goto bail_out
$check_kit_name:
$ if (kit_list .nes. "") then goto kit_name_ok
$ ask answer "Which upgrade do you wish to install" "" DNRS^Z
$ if_abort goto bail_out
$ if_backwards goto bail_out
$ kit_list = f$edit(answer,"COLLAPSE,UPCASE")
$ goto check_kit_name
$kit_name_ok:
$ counter = 0
$kit_loop:
$   kit_installed = false
$   kit = f$element(counter,",",kit_list)
$   counter = counter + 1
$   if (kit .eqs. "") then goto kit_loop
$   if (kit .eqs. ",") then goto end_kit_loop
$   zip_file = "''kit'_UPGRADE.ZIP"
$   com_file = "''kit'_UPGRADE.COM"
$   if (f$search(zip_file) .eqs. "")
$     then
$     say "%Can't find ZIP file ''zip_file'"
$   else
$   say f$fao("!/!6%D !8%T ----- Starting upgrade of !AS -----!/",0,0,kit)
$   if (f$search(directory_spec) .eqs. "")
$     then
$     say "%Creating sub-directory ''subdirectory'"
$     createe/directory 'subdirectory'/protection=(s=rwed,o=rwed)
$   else
$   say "%Using existing sub-directory ''subdirectory'"
$   set file/nolog 'directory_spec'/protection=(s=rwed,o=rwed)
$   endif !(f$search(directory_spec) .eqs. "")
$   saved_default = f$environment("DEFAULT")
$   set default 'subdirectory'
$   say "%Unpacking ZIP file"
$   unzip 'saved_default''zip_file'
$   if (f$search(com_file) .eqs. "")
$     then
$     say "%Can't find upgrade procedure ''com_file'"
$   else
$   say "%Executing upgrade procedure ''com_file'"
$   @'com_file'
$   kit_installed = true
$   endif !(f$search(com_file) .eqs. "")
$   if (interactive)
$     then
$     if (cleanup_directory .nes. "")
$       then
$       answer == "''cleanup_directory'"
$     else
$     ask answer "Do you want to delete the files in ''subdirectory'" "" BDR^Z
$     endif !(cleanup_directory .nes. "")
$   else
$   answer == "''cleanup_directory'"
$   endif !(interactive)
$   if_abort goto bail_out
$   if_backwards goto bail_out
$   if (answer)
$     then
$     say "%Cleaning up temporary directory"
$     deletee'log_qual' *.*;*
$   endif !(answer)
$   set default 'saved_default'
$   if (answer)
$     then
$     say "%Deleting temporary directory"
$     set file/nolog 'directory_spec'/protection=(s=rwed,o=rwed)
$     deletee'log_qual' 'directory_spec'
$   endif !(answer)
$   if (kit_installed)
$     then
$     if (interactive)
$       then
$       if (cleanup_zip_file .nes. "")
$         then
$         answer == "''cleanup_zip_file'"
$       else
$       ask answer "Do you want to delete ''zip_file'" "" BDR^Z
$       endif !(cleanup_zip_file .nes. "")
$     else
$     answer == "''cleanup_zip_file'"
$     endif !(interactive)
$     if_abort goto bail_out
$     if_backwards goto bail_out
$     if (answer)
$       then
$       say "%Deleting ZIP file"
$       deletee'log_qual' 'zip_file';*
$     else
$     say "%The ZIP file ''zip_file' will NOT be deleted"
$     endif !(answer)
$   else
$   say "%The ZIP file ''zip_file' will NOT be deleted"
$   endif !(kit_installed)
$   say f$fao("!/!6%D !8%T ----- Finished upgrade of !AS -----",0,0,kit)
$   endif !(f$search(zip_file) .eqs. "")
$  goto kit_loop
$end_kit_loop:
$bail_out:
$ say ""
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	INSTALL_UPGRADE.COM
$ !
$ ! Purpose:	To install an upgrade from a ZIP file in the format used by
$ !		the Kermit Client/Server upgrades.
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		26-Sep-1995, DBS; Version V1-001
$ !	001 -	Original version.
$ !		27-Sep-1995, DBS; Version V1-002
$ !	002 -	Added checks to only ask questions when interactive.
$ !		12-Dec-1995, DBS; Version V1-003
$ !	003 -	Check HW_MODEL rather than ARCH_TYPE (for older systems).
$ !		30-Sep-1997, DBS; Version V1-004
$ !	004 -	Added the option of specifying a list of kits.
$ !		08-Oct-1997, DBS; Version V1-005
$ !	005 -	Added control of deletion of directory and zip file via P2/P3
$ !		and use P4 to determine use of /LOG of certain actions.
$ !		Also fix a long standing bug where non-interactive use
$ !		produced some erros.
$ !-==========================================================================
