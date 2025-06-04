$ set noon
$ say = "write sys$output"
$ selection = f$edit(P1,"COLLAPSE,UPCASE")
$ option = f$edit(P2,"COLLAPSE,UPCASE")
$ if (selection .eqs. "") then selection = "*"
$ if (option .eqs. "") then option = "%"
$ wildspec = "''selection'.''option'"
$ say "%Selection was ''wildspec'"
$loop:
$   filespec = f$search(wildspec,62534)
$   if (filespec .eqs. "") then goto end_loop
$   filespec = f$element(0,";",filespec)
$   filename = f$parse(filespec,,,"NAME")
$   filetype = f$parse(filespec,,,"TYPE")
$   zipfile = "''filename'-''filetype'" - "."
$   say "%Creating ''zipfile'..."
$   zip 'zipfile' 'filename''filetype'
$ goto loop
$end_loop:
$ exitt 1
