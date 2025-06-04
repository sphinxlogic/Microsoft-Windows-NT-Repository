$! P1 = VAX or ALPHA (or blank) - architecture to build for...
$!
$ say = "write sys$output"
$!
$ cpu = f$edit(f$getsyi("arch_name"),"upcase")
$ target = p1
$ if target.eqs."" then target = cpu
$!
$ cdu = "set command/object"
$!
$ if target .eqs. "ALPHA"
$ then
$   cc = "cc/nomember_alignment/standard=vaxc"
$   options = "/sysexe"
$   if cpu .eqs. "VAX"
$   then
$       cc = "gemcc/nomember_alignment/standard=vaxc"
$       cdu = "set command/object/alpha"
$       link = "link/alpha"
$       define vaxc$include alpha$library
$       define c$include    alpha$library
$   endif
$ endif
$!
$ if target .eqs. "VAX"
$ then
$   if cpu .eqs. "ALPHA"
$   then
$       say "Cross compiling from Alpha to Vax not supported.."
$       exit
$   endif
$   DECC =  (f$trnlnm("DECC$CC_DEFAULT") .eqs. "/DECC") 	-
       .or. (f$search("SYS$LIBRARY:DECC$SHR.EXE") .nes. "") 	-
       .or. (f$TrnLnm("VAXCMSG") .eqs. "DECC$MSG")
$   If (DECC)
$   Then  options = ",sys$system:sys.stb/selective"
$   Else  options = ",c/option,sys$system:sys.stb/selective"
$   EndIF
$ endif
$!
$ cc verb
$ cdu verb_cld
$ cc verb_command
$ cc verb_disallows
$ cc verb_entity
$ cc verb_type
$ cc verb_find_entity
$ cc verb_file
$ cc init_cli
$ link/notraceback verb, verb_cld, verb_command, verb_disallows, -
	verb_entity, verb_file, verb_type, verb_find_entity, init_cli 'options'
$!
$ if target .nes. cpu
$ then
$   deassign vaxc$include
$   deassign c$include
$ endif
