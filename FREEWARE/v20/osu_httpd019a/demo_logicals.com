$!
$! define www_root and www_system logicals.
$!
$ proc = f$environment("PROCEDURE")
$ root_dir = f$parse("1.;",proc,,,"NO_CONCEAL,SYNTAX_ONLY") - "][" - "]1.;"
$ arch = "ALPHA"
$ if f$getsyi("CPU") .lt. 128 then arch = "VAX"
$ define www_alphaexe 'root_dir'.ALPHA.]/trans=(term,conceal)
$ define www_vaxexe 'root_dir'.VAX.]/trans=(term,conceal)
$ define www_root 'root_dir'.VMS.]/trans=(term,conceal),www_'arch'exe:
$ define www_src 'root_dir'.SRC.]/trans=(term,conceal)
$ if f$trnlnm("WWW_SYSTEM") .eqs. "" then define www_system www_root:[system]
