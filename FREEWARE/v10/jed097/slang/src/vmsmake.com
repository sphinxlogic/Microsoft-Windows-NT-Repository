$ ver = f$verify(0)
$!  Makefile for VMS
$ if (p1 .eqs. "INSTALL") then goto install
$ defs = "FLOAT_TYPE,NO_STDLIB_H,HAS_MEMCPY,HAS_MEMSET,HAS_MEMCMP"
$ axp = f$getsyi("HW_MODEL").ge.1024
$! 
$! If compiling with GCC, uncomment next line
$! goto has_gcc
$ if (p1 .eqs. "GCC") then goto has_gcc
$!
$! There should be no need to change anything below here
$!
$ if axp
$ then	C_C = "cc/standard=vaxc"
$	Link_Flags = "/NONATIVE_ONLY"
$ else	C_C = "cc"
$	Link_Flags = ""
$ endif
$ C_C = C_C
$ goto start
$ has_gcc:
$  C_C = "gcc/warnings/nocase_hack"
$  Link_FLags = ",GNU_CC:[000000]GCCLIB/LIB"
$!
$ start:
$ files = "slang,slparse,slmath,slfile,slstd,slarray,slregexp,slerr"
$ files = files + ",slrline,slgetkey,slvmstty,slkeymap,sltoken"
$ files = files + ",slmemcpy,slmemcmp,slmemset,slmemchr"
$ files = files + ",sldisply,slsmg,slsearch,slcmd,slmalloc,slclass"
$!
$  if (p1 .eqs. "LINK") then goto do_link
$!
$!  simple make
$!
$  count = 0
$  next_file:
$    f = f$element(count, ",", files)
$    count = count + 1
$    if (f .eqs. ",") then goto do_link
$    objf = f$search("''f'.obj")
$    if (objf .eqs. "") then goto compile_it
$    tobj = f$file_attr(objf, "RDT")
$    tc  = f$file_attr("''f'.c", "RDT")
$    if (f .eqs. "sysdep")
$    then
$      if ( f$cvtime(tobj) .lts. f$cvtime(f$file_attr("vms.c","RDT"))) -
	 then goto compile_it
$    endif
$    if (f$cvtime(tc) .lts. f$cvtime(tobj)) then goto next_file
$  compile_it:  
$    write sys$output "''C_C'/define=(''defs') ''f'.c"
$    'C_C'/define=('defs') 'f'.c
$    goto next_file
$  do_link:
$  write sys$output "library/creat slang ''files'"
$  library/creat slang 'files'
$  purge slang.olb
$  goto The_Exit
$  The_Exit:
$    write sys$output "''C_C'/define=(''defs') calc.c"
$    'C_C'/define=('defs') calc.c
$    write sys$output "link/exec=calc calc ''Link_Flags', slang/LIBRARY"
$    if axp
$    then
$      link/exec=calc calc, slang/LIBRARY 'Link_Flags'
$    else
$      link/exec=calc calc, slang/LIBRARY 'Link_Flags',  sys$input/opt
       SYS$LIBRARY:VAXCRTL/SHARE
$    endif
$  exit
     
