$ set noon
$ ff = "x"
$ ff[0,8] = 12 
$ esc = "x"
$ esc[0,8] = 27 
$ feep = "x"
$ feep[0,8] = 7
$ ansi = (f$getdvi("sys$command","devdepend2") .and. %x01000000) .ne. 0
$ rv = ""
$ xrv = ""
$ if ansi then rv = "''esc'[7m" 
$ if ansi then xrv = "''esc'[m" 
$
$ filespec = p1
$ if filespec .eqs. "" then filespec = "*.b32"
$ wild = 0
$ if f$locate("*",filespec) .ne. f$length(filespec) then wild = 1
$ if f$locate("%",filespec) .ne. f$length(filespec) then wild = 1
$
$nextfile:
$ file = f$search(filespec)
$ if file .eqs. "" then goto nomore
$ write sys$output file
$
$ open/read in 'file'
$ n = 0                                                          
$ g = 0
$ f = 0
$10:
$ read/error=50 in line 
$ n = n + 1
$ if f$locate(ff,line) .eq. f$len(line) then goto 10
$ f = f + 1
$ if f$len(line) .eq. 1 then goto 10
$ g = g + 1
$ write sys$output rv,"<FF> embedded in line ",n,xrv,feep
$ goto 10
$50:
$ close in
$ write sys$output "''n' lines, ''f' formfeeds, ''g' dubious cases"
$ if wild then goto nextfile
$
$nomore:
$ exit
