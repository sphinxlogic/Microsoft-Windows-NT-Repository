$! SOX symbol definitions.
$soxloc = f$environment("default") + "SOX.EXE"
$sox :== $'soxloc	!define SOX location.  (default = current dir)
