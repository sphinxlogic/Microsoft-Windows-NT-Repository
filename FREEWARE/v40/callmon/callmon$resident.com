$!  CALLMON - A Call Monitor for OpenVMS Alpha
$!
$!  File:     CALLMON$RESIDENT.COM
$!  Author:   Thierry Lelegard
$!  Version:  0.0
$!  Date:     19-JUL-1996
$!
$!  Abstract: This command procedure finds all resident images in the
$!            system and define a logical name for each of them. Because
$!            of these logical names, all subsequent commands will use a
$!            non-resident version of each image.
$!
$!            This command procedure is not really a part of CALLMON,
$!            it is a useful utility before using CALLMON.
$!
$ on warning then goto exit
$!
$!  Generate a list of installed images in a temporary file
$!
$ listfile := sys$scratch:callmon$list'f$getjpi("","pid")'.tmp
$ if f$search(listfile).nes."" then delete 'listfile';*
$ define/user sys$output 'listfile'
$ install list
$!
$!  Read the output of "install list" and find resident images
$!
$ file = ""
$ defspec = "SYS$SYSTEM:.EXE;"
$ open/read callmon$listfile 'listfile'
$ list_loop:
$   read/end=end_list_loop callmon$listfile line
$   if f$length(line).eq.0 then goto list_loop
$   !
$   ! Process error messages
$   !
$   if f$extract(0,1,line).eqs."%"
$   then
$     file = ""
$     write sys$error line
$     goto list_loop
$   endif
$   !
$   ! Process new directory name
$   !
$   if f$extract(0,1,line).nes." "
$   then
$     file = ""
$     defspec = f$edit(line,"collapse")
$     goto list_loop
$   endif
$   !
$   ! Process new file name and version
$   !
$   if (line-";").nes.line then -
$     file = f$element(0," ",f$edit(line,"trim,compress"))
$   resident = (line-"Resid").nes.line
$   protected = (line-"Prot").nes.line
$   !
$   ! Found a resident image. Protected images cannot be overriden.
$   !
$   if file.nes."" .and. resident 
$   then
$     file = f$parse(";",file,defspec)
$     name = f$parse(file,,,"name")
$     if protected
$     then
$       write sys$output "''name' is protected, no logical name defined"
$     else
$       write sys$output "''name' = ''file'"
$       define/nolog 'name' 'file'
$     endif
$   endif
$   goto list_loop
$ end_list_loop:
$!
$ exit:
$ set noon
$ if f$trnlnm("callmon$listfile").nes."" then close callmon$listfile
$ if f$search(listfile).nes."" then delete 'listfile';*
$ exit 1
