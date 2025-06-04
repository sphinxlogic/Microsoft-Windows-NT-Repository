$!  p1      = options file to create
$!  p2...   = keywords for shareables to use
$!
$!
$!  :vector=(proc=procedure)    -> SYMBOL_VECTOR or UNIVERSAL
$!  :threads
$!  :perl
$!  :dbgperl
$!  :linkopt    (options file comment)
$!  :jibpcbdef (generate .h file of jib/pcb defs)
$!  :nfbdef    (generate .h file of nfb defs)
$!  :comment    rest is a comment
$!
$ if f$edit(p2,"upcase") .eqs. ":JIBPCBDEF"  then goto gen_jibpcbdef
$ if f$edit(p2,"upcase") .eqs. ":NFBDEF"     then goto gen_nfbdef
$!----------CONFIG FILE FIRST
$ OPEN/READ fd configure.mms
$ cloop:
$   read/end=ecloop fd line
$   x = f$elem(0,"=",line)
$   y = f$extract(f$len(x)+1,f$len(line),line)
$   if x .eqs. "PERLDIR"   then perl_loc = y
$   if x .eqs. "PERLSETUP" then perl_setup = y
$ goto cloop
$ ecloop:
$ close fd
$!
$ p1 = f$edit(p1,"upcase")
$ oopt = "/WRITE"
$ if f$locate("/APPEND",p1) .lt. f$length(p1)
$ then
$   oopt = "/APPEND"
$   p1 = f$extract(0,f$locate("/APPEND",p1),p1)
$ endif
$ open'oopt' fd 'p1'
$ j = 1
$ loop1:
$   j = j + 1
$   if j .gt. 8 then goto eloop1
$   x = p'j'
$   if x .eqs. "" then goto loop1
$   if f$extract(0,1,x) .eqs. ":" then goto loop1
$   write fd x
$   p'j' = ""
$ goto loop1
$ eloop1:
$ j = 1
$ loop2:
$   j = j + 1
$   if j .gt. 8 then goto eloop2
$   x = p'j'
$   if x .eqs. "" then goto loop2
$   x = f$edit(x,"collapse,upcase")
$!
$   if f$locate(":THREADS",x) .lt. f$length(x)
$   then
$     write fd "sys$share:cma$open_rtl/share"
$     goto loop2
$   endif
$!
$   if f$locate(":PERL",x) .lt. f$length(x)
$   then
$     write fd "! PERL_LOC=''perl_loc'"
$     write fd "! PERL_SETUP=''perl_setup'"
$     write fd "perlshr/share"
$     goto loop2
$   endif
$!
$   if f$locate(":DBGPERL",x) .lt. f$length(x)
$   then
$     write fd "! PERL_LOC=''perl_loc'"
$     write fd "! PERL_SETUP=''perl_setup'"
$     write fd "dbgperlshr/share"
$     goto loop2
$   endif
$!
$   if f$locate(":LINKOPT",x) .lt. f$length(x)
$   then
$       write fd "! ",f$extract(1,f$len(x)-1,x)
$       goto loop2
$   endif
$!
$   if f$locate(":VECTOR",x) .lt. f$length(x)
$   then
$       j = f$locate("=",x)
$       y = f$extract(j,f$length(x)-j,x)
$       if f$getsyi("ARCH_NAME") .nes. "VAX"
$       then
$           write fd "SYMBOL_VECTOR''y'"
$       else
$           y = f$element(1,"(",y)
$           y = f$element(0,"=",y)
$           write fd "UNIVERSAL=''y'"
$       endif
$       goto loop2
$   endif
$   if f$locate(":COMMENT",x) .lt. f$length(x)
$   then
$       y = f$element(1,"=",x)
$       write fd "! ''y'"
$   endif
$!
$ goto loop2
$ eloop2:
$ close fd
$ exit
$!
$!
$ gen_pcbjibdef:
$ call generate_hfile 'p1' "$PCBDEF,$JIBDEF"
$ exit
$!
$ gen_nfbdef:
$ call generate_hfile 'p1' "$NFBDEF"
$ exit
$!
$!
$ generate_hfile: subroutine
$!
$!  make an .h file from lib.mlb macro files
$!
$ filename = p1
$ modules = p2
$ symbol = f$parse(p1,,,"name")
$ symbol =  "_" + f$edit(symbol,"upcase") + "_H"
$ lib/extract=('modules')/out=temp.mar/macro sys$library:lib.mlb
$ open/read fd    temp.mar
$ open/write fd2  'filename'
$ write fd2 "#ifndef ''symbol'"
$ write fd2 "#define ''symbol'"
$ loop:
$   read/end=eloop fd line
$   if f$extract(0,4,line) .nes. "$EQU" then goto loop
$   if f$locate("<^X",line) .lt. f$len(line)
$   then
$       j = f$locate("<^X",line)
$       k = f$locate(">",line)
$       line = "#define "+f$extract(4,j-4,line)+"0x"+f$extract(j+3,k-j-3,line)
$   else
$       line = "#define "+f$extract(4,f$len(line),line)
$   endif
$   write fd2 line
$ goto loop
$ eloop:
$ write fd2 "#endif"
$ close fd
$ close fd2
$ delete temp.mar;
$ exit
$ endsubroutine
