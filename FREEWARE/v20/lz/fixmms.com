$!
$! Hack command file to eliminate long lines in lz command file.
$! Usage:
$!	@ fixmms input output
$!
$	on error then exit
$	on warning then exit
$	on control_y then exit
$	tab[0,7] = 9				! Define <TAB> character
$	tabs = tab + tab			! 2 tabs at long line
$	open/read  src 'p1'
$	open/write dst 'p2'
$ loop:	read/end=endfile src line
$	spaceover = ""
$ more:	i = f$locate(tabs, line)
$	if (i .ne. f$length(line)) then goto gotcha
$	write dst spaceover, line
$	goto loop
$ gotcha:
$	write dst spaceover, f$extract(0, i, line), "-"
$	line = f$extract(i + 2, f$length(line), line)
$	spaceover = tabs
$	goto more
$ endfile:
$	close src
$	close dst
$	exit
