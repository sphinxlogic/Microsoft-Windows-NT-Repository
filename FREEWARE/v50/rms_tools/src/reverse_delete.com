$!
$! This procedure deletes the contents of the specified directory -- and in 
$! reverse alphanumeric order, for reasons of directory access performance.
$!
$!  P1 (required) = target file specification to delete.   Wildcards permitted.
$!                  can optionally include DIRECTORY file selection qualifiers
$!  P2 (optional) = delete command qualifiers (/LOG or /CONFIRM)
$
$ if p1 .eqs. "" then goto help
$ tmpfile = f$fao("sys$scratch:delete_!XL.tmp",f$getjpi("0","PID"))
$ tmpfile = f$parse(";",tmpfile,,,"SYNTAX_ONLY")
$ directory/out='tmpfile'/col=1/nohead/notrail 'p1
$ sort/key=(pos:1,siz:-1,desc) 'tmpfile' 'tmpfile'
$ close/nolog sorted 'tmpfile'
$ open/read/error=clean_up sorted 'tmpfile'
$init:
$ read/end=clean_up sorted f
$ l2 = f$len(f)
$ if f$loc(";",f) .eq. l2 then goto init
$ files = f
$ loop:
$ read/end=done sorted f
$ l1 = f$len(f)
$ if f$locate(";",f) .eq. l1 then goto loop
$ if l2 .lt. 200 
$   then 
$     files = files + "," + f
$     l2 = l2 + l1 + 1
$   else 
$     delete 'p2' 'files'
$     files = f
$     l2 = l1
$   endif
$ goto loop
$done:
$ delete 'p2' 'files'
$clean_up:
$ close/nolog sorted
$ exit
$help:
$ type sys$input

  Usage:

  P1 (required) = wildcard file spec, with optional DIR selection qualifiers
  P2 (optional) = delete command qualifiers (/LOG or /CONFIRM)

$ exit
