$ ! Procedure:	MUK.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ muk_version = "V1-004"
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ if (vax)
$   then
$   arch_name = "VAX"
$ else
$ arch_name = "ALPHA"
$ endif !(vax)
$ say = "write sys$output"
$ ask = "@olga$sys:sys_ask"
$ zip = "$olga$sys:zip ""-V"" -jq"
$ check_abort = "if (sys__control_z .or. sys__control_y) then goto bail_out"
$introduction:
$ say f$fao("!/!/!25* MUK - Make Upgrade Kit!10* (!AS)",muk_version)
$ type sys$input

  This procedure will generate a command procedure based on the contents of
  the current directory.  You will have the option of editing this file and
  when complete, a ZIP file will be created which can then be used to
  perform the upgrade via the C-Kermit Client/Server software that runs on
  some remote machines.

  Please confirm that this is what you want to do.
$ ask answer "Do you wish to proceed" "" BDR^Z
$ check_abort
$ if (sys__backwards .or. .not. answer) then goto bail_out
$get_kit_name:
$ type sys$input

  You will now be asked for a name to use for this kit.  This name will be
  used to generate the name of the command procedure and the ZIP file.
$ ask muk_kit_name "What will this kit be called" "" DSR^Z
$ check_abort
$ if (sys__backwards) then goto introduction
$ muk_kit_name == f$extract(0,31,f$edit(muk_kit_name,"COLLAPSE,UPCASE"))
$ ask answer "Please confirm that the kit will be called ''muk_kit_name'" -
		"" BDR^Z
$ check_abort
$ if (sys__backwards .or. .not. answer) then goto get_kit_name
$get_destination:
$ type sys$input

  You will now be asked for the final location of the files on the remote
  system.  It is assumed that all files will end up in this location.  If
  this is not the case, you can change the resulting command procedure
  when you edit it shortly.
$ ask muk_destination "Where will the files reside on the remote system" -
		"" DRS^Z
$ check_abort
$ if (sys__backwards) then goto get_kit_name
$ muk_destination == f$edit(muk_destination,"COLLAPSE,UPCASE")
$ length = f$length(muk_destination)
$ if ((f$locate("[",muk_destination) .eq. length) -
	.and. (f$locate("]",muk_destination) .eq. length))
$   then
$   muk_destination == muk_destination - ":" + ":"
$ endif !((f$locate("[",muk_destination) .eq. length) -
$ ask answer "Please confirm the destination is ''muk_destination'" "" BDR^Z
$ check_abort
$ if (sys__backwards .or. .not. answer) then goto get_destination
$ command_procedure = muk_kit_name + "_UPGRADE.COM"
$ zip_file = muk_kit_name + "_UPGRADE.ZIP"
$ upgrade_extras_first = muk_kit_name + "_UPGRADE.FIRST"
$ upgrade_extras_last = muk_kit_name + "_UPGRADE.LAST"
$ say ""
$ say "  Please wait while I create the procedure ''command_procedure' . . ."
$ say ""
$ name_and_type = f$parse(command_procedure,,,"NAME") -
		+ f$parse(command_procedure,,,"TYPE")
$ copy nla0: 'command_procedure'
$ wc = "write command"
$ close/nolog command
$ open/append command 'command_procedure'
$ wc "$ ! Procedure: ''name_and_type'"
$ wc "$ set noverify"
$ wc "$ set noon"
$ wc "$ say = ""write sys$output"""
$ wc "$ vax = (f$getsyi(""HW_MODEL"") .lt. 1024)"
$ wc "$ axp = (f$getsyi(""HW_MODEL"") .ge. 1024)"
$ wc "$ if (vax)"
$ wc "$   then"
$ wc "$   arch_name = ""VAX"""
$ wc "$ else"
$ wc "$ arch_name = ""ALPHA"""
$ wc "$ endif !(vax)"
$ if (f$search(upgrade_extras_first) .nes. "")
$   then
$   wc "$ @''upgrade_extras_first'"
$ endif !(f$search(upgrade_extras_first) .nes. "")
$loop:
$   this_file = f$search("*.*;",423176)
$   if (this_file .eqs. "") then goto end_loop
$   this_file = f$parse(this_file,,,"NAME") + f$parse(this_file,,,"TYPE")
$   if (this_file .eqs. name_and_type) then goto loop
$!$   say "%Processing ''this_file'"
$   wc "$ call copy_file ''this_file' ''muk_destination'"
$ goto loop
$end_loop:
$ if (f$search(upgrade_extras_last) .nes. "")
$   then
$   wc "$ @''upgrade_extras_last'"
$ endif !(f$search(upgrade_extras_last) .nes. "")
$ wc "$ exitt 1"
$ wc "$copy_file: subroutine"
$ wc "$ set noon"
$ wc "$ filespec = f$edit(P1,""COLLAPSE,UPCASE"")"
$ wc "$ destination = f$edit(P2,""COLLAPSE,UPCASE"")"
$ wc "$ if ((filespec .eqs. """") .or. (destination .eqs. """"))"
$ wc "$ then say ""%Required parameter missing from call to COPY_FILE"""
$ wc "$ else if (f$search(filespec) .eqs. """")"
$ wc "$ then say f$fao(""%File !AS not found"",filespec)"
$ wc "$ else copyy/nolog 'filespec' 'destination'"
$ wc "$ if (.not. $status) then -"
$ wc "$ say f$fao(""%Failed to copy !AS to !AS"",filespec,destination)"
$ wc "$ endif"
$ wc "$ endif"
$ wc "$exit_copy_file:"
$ wc "$ exitt 1"
$ wc "$ endsubroutine"
$ close/nolog command
$ say ""
$ say "  The procedure ''command_procedure' has been created."
$ask_about_edit:
$ ask answer "Would you like to edit the procedure" "" BDRZ
$ check_abort
$ if (.not. answer) then goto create_zip_file
$ type sys$input

  You can choose your favourite editor.  It will be invoked via the command

  $ EDIT/your_choice

  (Best to choose from EDT, TPU or TECO.)
$ ask muk_editor "Which editor would you like" "" DRS^Z
$ check_abort
$ if (sys__backwards) then goto ask_about_edit
$ define/user sys$input sys$command
$ editt/'muk_editor' 'command_procedure'
$ goto ask_about_edit
$create_zip_file:
$ if (f$search("''command_procedure';-1") .nes. "") then -
$   purgee/nolog 'command_procedure'
$ if (f$search(zip_file) .nes. "") then deletee/nolog 'zip_file';*
$ say ""
$ say "  Please wait while I create the ZIP file ''zip_file' . . ."
$ say ""
$ zip 'zip_file' *.*
$ say ""
$ say "  The ZIP file ''zip_file' has been created."
$ type sys$input

  Please copy the ZIP file to the required location and cleanup this
  directory.

$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	MUK.COM
$ !
$ ! Purpose:	This procedure will generate a command procedure which will
$ !		become part of an upgrade kit that can be used via the
$ !		C-Kermit Client/Server software.  A ZIP file will then be
$ !		created that can be used directly by the C-Kermit stuff.
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		11-Sep-1995, DBS; Version V1-001
$ !	001 -	Original version.
$ !		10-Jan-1996, DBS; Version V1-002
$ !	002 -	Include code in the _UPGRADE.COM procedure to setup vax/axp
$ !		details for use within the procedure.
$ !		12-Jan-1996, DBS; Version V1-003
$ !	003 -	Added capability to run procedures before and after the bulk
$ !		of the copy stuff - UPGRADE_EXTRAS_FIRST|LAST.COM
$ !		29-Feb-1996, DBS; Version V1-004
$ !	004 -	Changed the extras names to *.FIRST and *.LAST where * is the
$ !		name of the upgrade e.g. STUFF_UPGRADE.FIRST.
$ !-==========================================================================
