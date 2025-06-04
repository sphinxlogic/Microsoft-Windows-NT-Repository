$!
$! Validate that the user can have the VMS_ENABLE_CMS identifier
$! and the VMS_SOURCE identifier at the same time.  Define the logical
$! PLMENU$CMS_ACCESS_VALID to truee if it has been determined this
$! account has valid access
$!
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Validating Masterpack CMS library write access for this account..."
$ WRITE SYS$OUTPUT ""
$!
$ validated = f$trnlnm("PLMENU$CMS_ACCESS_VALID","LNM$PROCESS_TABLE")
$ if validated .eqs. "" then GOTO Perform_Access_Check
$ if validated then EXIT 1
$!
$Perform_Access_Check: 
$!$ SETVSC
$ STS$M_INHIB_MSG = 268435456
$ stat = 0 + STS$M_INHIB_MSG
$ found_cms = 0
$ found_vms_source = 0
$ index = -1
$!
$ ids = f$getjpi("","process_rights")
$rights_loop:
$!-----------
$ index = index + 1
$ id = f$element(index,",",ids)
$ if id .eqs. "," then GOTO end_rights_loop
$ if f$extract(0,4,id) .nes. "VMS_" then GOTO rights_loop
$ if id .eqs. "VMS_ENABLE_CMS" then found_cms = 1
$ if id .eqs. "VMS_SOURCE" then found_vms_source = 1
$ GOTO rights_loop
$!
$end_rights_loop:
$!--------------
$ if found_cms .and. found_vms_source
$ then
$   stat = 1                   
$   define/nolog/table=lnm$process PLMENU$CMS_ACCESS_VALID TRUE
$   GOTO validate_done
$ EndIf
$ if f$priv("BYPASS")
$ then
$   stat = 1
$   define/nolog/table=lnm$process PLMENU$CMS_ACCESS_VALID TRUE
$   GOTO validate_done
$ EndIf
$ define/nolog/table=lnm$process PLMENU$CMS_ACCESS_VALID FALSE
$!
$validate_done:
$ exit stat
