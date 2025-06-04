$ ! Procedure:	ALPHAZIP.COM
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ archivename = f$edit(P1,"COLLAPSE,UPCASE")
$ if (archivename .eqs. "?")
$   then
$   say "%ALPHAZIP <archivename> <filetype> <location> [<preprefix>]"
$   say "  <archivename> defaults to ARCHIVE"
$   say "  <filetype> defaults to *"
$   say "  <location> defaults to SYS$DISK:[]"
$   say "  <preprefix> has no default i.e. null"
$   say ""
$   say "  Files <preprefix><0..Z>*.<filetype>"
$   say "      --> <archivename>-<preprefix><0..Z>.zip"
$   say ""
$ else
$ filetype = f$edit(P2,"COLLAPSE,UPCASE")
$ location = f$edit(P3,"COLLAPSE,UPCASE")
$ preprefix = f$edit(P4,"COLLAPSE,UPCASE")
$ if (archivename .eqs. "") then archivename = "ARCHIVE"
$ if (filetype .eqs. "") then filetype = "*"
$ if (location .eqs. "") then location = "SYS$DISK:[]"
$ say "%Loading *.''filetype' into ''archivename'-''preprefix'x.ZIP"
$ prefixes = "\0\1\2\3\4\5\6\7\8\9\_\$" -
		+ "\A\B\C\D\E\F\G\H\I\J\K\L\M\N\O\P\Q\R\S\T\U\V\W\X\Y\Z\"
$ count = 0
$loop:
$   prefix = f$element(count,"\",prefixes)
$   count = count + 1
$   if (prefix .eqs. "\") then goto end_loop
$   if (prefix .eqs. "") then goto loop
$   call dozip 'preprefix''prefix' 'archivename' 'filetype' 'location'
$ goto loop
$end_loop:
$ endif !(archivename .eqs. "?")
$bail_out:
$ exitt 1
$
$dozip: subroutine
$ set noon
$ on control_y then goto exit_dozip
$ prefix = f$edit(P1,"COLLAPSE,UPCASE")
$ archivename = f$edit(P2,"COLLAPSE,UPCASE")
$ filetype = f$edit(P3,"COLLAPSE,UPCASE")
$ location = f$edit(P4,"COLLAPSE,UPCASE")
$ if (prefix .eqs. "")
$   then
$   say "%Bad call to DOZIP, null parameter"
$ else
$ searchspec = "''location'''prefix'*.''filetype'"
$ if (f$search(searchspec,99172) .nes. "")
$   then
$   say f$fao("!78*-!/  Loading !AS into !AS-!AS" -
		,searchspec,archivename,prefix)
$   zip "-jV"  'archivename'-'prefix' 'searchspec' -x *.zip
$ endif !(f$search("searchspec") .nes. "")
$ endif !(prefix .eqs. "")
$exit_dozip:
$ exitt 1
$ endsubroutine
