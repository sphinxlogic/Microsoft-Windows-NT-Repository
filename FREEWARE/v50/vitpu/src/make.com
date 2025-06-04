$ do="@[-.exe]do"
$ if f$logical ("vi$root") .nes. "" THEN do="@[exe]do"
$ if p1 .eqs. "ALL" then p1="TPUSUBS,EXE,VI"
$ if p1 .eqs. "" then p1 = "VI"
$ opers =","+p1+","
$ i = 1
$!
$ NEXT_ELEM:
$	next = f$element (i, ",", opers)
$	i = i + 1
$	if (next .eqs. "") .or. (next .eqs. ",") then goto done
$	write sys$output "* Making ''next'"
$	on warning then goto go_err
$	goto 'next'
$ go_err:
$	write sys$output "   \''next'\"
$	goto next_elem 
$!
$ VI:
$	on warning then stop
$	do edit/tpu/command=stepwise.tpu/nodispay/nosection vi.tpu
$	do rename vi.gbl [-.exe]
$	set noon
$   mcr install
vi$root:[exe]vi.gbl/replace
$   set on
$	goto next_elem
$!
$ TPUSUBS:
$	on warning then stop
$	do macro tpusubs
$	do link/share/exe=[-.exe]tpusubs tpusubs/opt
$	goto next_elem
$!
$ EXE:
$	on warning then stop
$	do macro vi
$	do link/exe=[-.exe]vi vi
$	goto next_elem
$!
$ CLEAN:
$	on warning then stop
$	do purge/log VI$ROOT:[*...]*.*
$	do delete/log VI$ROOT:[SRC]*.obj;,VI$ROOT:[SRC]MAKE.OUT;
$	goto next_elem
$!
$ DONE:
$	on warning then stop
$	exit
