$ @ident ! determine Nmail version number (sets nm$ident and nm$vvu)
$ x = f$edit(f$extract(0,1,f$element(1," ",nm$ident)),"upcase")
$ saveset  = "NMAIL" + nm$vvu
$ if x .nes. "V" then saveset = "NMAIL" + x + nm$vvu
$ relnotes = "NMAIL" + nm$vvu + ".RELEASE_NOTES"
$ dirnam = ".kit''nm$vvu'"
$ delete/symbol/global nm$ident
$ delete/symbol/global nm$vvu
$
$ msg  = "Create ''saveset' Kit"
$ dash = "--------------------------------------------------------------------"
$ write sys$output msg
$ write sys$output f$extract(0,f$len(msg),dash)
$
$ savprv = f$setprv("sysprv")
$
$ write sys$output "Creating directory..."
$ create/directory ['dirnam']/prot=(system=rwed,owner=rwed)
$ if f$search("[''dirnam']*.*;*") .nes. "" then delete ['dirnam']*.*;*
$
$ write sys$output "Copying files..."
$ copy kitinstal.com         ['dirnam'];0
$ copy kitclean.com          ['dirnam'];0
$ copy kitlink.com           ['dirnam'];0
$ copy ident.com	     ['dirnam'];0
$ copy nm$daemon_vax.exe     ['dirnam'];0
$ copy nm$queman_vax.exe     ['dirnam'];0
$ copy nm_mailshr_vax.exe    ['dirnam'];0
$ copy nm_mailshrp_vax.exe   ['dirnam'];0
$ copy nm$daemon_alpha.exe   ['dirnam'];0
$ copy nm$queman_alpha.exe   ['dirnam'];0
$ copy nm_mailshr_alpha.exe  ['dirnam'];0
$ copy nm_mailshrp_alpha.exe ['dirnam'];0
$ copy nm$error.dat          ['dirnam'];0
$ copy nm$startup.com        ['dirnam'];0
$ copy nm$help.hlp           ['dirnam'];0
$ copy nm$uguide.doc         ['dirnam'];0
$ copy nm$sysmgr.doc         ['dirnam'];0
$ copy nm$rnotes.doc         ['dirnam']'relnotes';0
$ copy nm$dcl.cld            ['dirnam'];0
$ copy nmail_vax.olb         ['dirnam'];0
$ copy nmail_alpha.olb 	     ['dirnam'];0
$ copy ident.opt             ['dirnam'];0
$ set file ['dirnam']*.* /owner=[1,4]
$
$ open/write $k kitinfo.tmp
$ write $k "SPKITBLD$KITNAME   = """,saveset,""""
$ write $k "SPKITBLD$SAVESET_A = ""[",dirnam,"]*.*"""
$ close $k
$
$ write sys$output "Building saveset..."
$ @sys$update:spkitbld n/a 'f$envir("default")' n/a kitinfo.tmp
$
$ write sys$output "Tidying up..."
$ delete kitinfo.tmp;*,['dirnam']*.*;*
$ delete 'f$extract(1,999,dirnam)'.dir;1
$
$ set file 'saveset'.% /protection=(system:rwed,owner:rwed,group:re,world:re)
$ directory 'saveset'.% /date /size=all
$
$ foo = f$setprv("''savprv'")
$
$ write sys$output ""
$ write sys$output "Done"
