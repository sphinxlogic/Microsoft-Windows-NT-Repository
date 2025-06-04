$! DCL Script to Spawn and attach to a kept editor
$! Allows reading-in a new file on ATTACH as well, using wildcards as well.
$! By Peter Galbraith, Dalhousie University. (GALBRTH@AC.DAL.CA)
$ tt = f$getdvi("sys$command","devnam") - "_" - "_" - ":"
$ eve_name = f$extract(0,15,"Edit_" + tt)
$ priv_list = f$setprv("NOWORLD,NOGROUP")
$ pid = 0
$other:
$ proc = f$getjpi(f$pid(pid),"PRCNAM")
$ if proc .eqs. eve_name then goto attach
$ if pid .ne. 0 then goto other
$spawn:
$ priv_list = f$setprv(priv_list)
$ write sys$error "[Spawning a new kept Eve]"
$ t1 = f$edit(p1+" "+p2+" "+p3+" "+p4+" "+p5+" "+p6+" "+p7+" "+p8,"COLLAPSE")
$ define/nolog/job kept_edit "''eve_name'"
$ define/nolog sys$input sys$command
$ section_q = ""
$ if f$trnlnm("TPU$SECTION").eqs."" then $section_q = "/SECT=EVE$SECTION"
$ spawn/process="''eve_name'"/nolog EDIT/TPU 'section_q' 't1'
$!
$!  To use it as /COMMAND, correct and uncomment the following lines
$!
$!$ spawn/process="''eve_name'"/nolog EDIT/TPU -
$!    /COMMAND=Your_Disk:[Your_Path]KEPT_EVE$ADVANCED.TPU 'section_q' 't1'
$ write sys$error "Attached to DCL in directory ''f$env("DEFAULT")'"
$ exit
$attach:
$ priv_list = f$setprv(priv_list)
$ ! Use this logical if you want the kept_editor to follow your default dir
$ define/nolog/job edit_new_default 'f$environment("default")'
$ if p1 .nes. "" 
$    then
$    new_file = f$search(p1)
$    if new_file .eqs. "" 
$       then 
$       write sys$error "File not found"
$       exit
$    endif
$ ! If you want to get the first file matching wildcards, uncomment this line
$ !   define/nolog/job edit_new_file 'new_file'
$ ! If you'd rather pass the wildcards to GET_FILE to get the choice buffer
$ ! then use the following instead. 
$    define/nolog/job edit_new_file 'f$parse(p1)'
$ ! End of the second choice - comment/uncomment up to this line
$ endif
$ write sys$error "[Attaching to kept Eve]"
$ define /nolog sys$input sys$command
$ attach "''eve_name'" 
$ if f$trnlnm("EDIT_NEW_FILE").nes."" then $ deassign/job edit_new_file
$ write sys$error "Attached to ''f$process()' in directory ''f$env("DEFAULT")'"
$ exit
