$! ver = f$verify(0)
$!  Makefile for VMS
$ axp = 0
$ if (p1 .eqs. "INSTALL") then goto install
$ defs = ""
$!
$! If you do not want floating point, comment out next line
$    defs = "FLOAT_TYPE,"
$!
$! S-Lang Include directory (where slang.olb is located)
$    slang_dir = "[-.-.slang.src]"
$!
$  Link_Flags = ""
$  Link_Libs = ""
$! If compiling with GCC, uncomment next line
$! goto has_gcc
$ if (p1 .eqs. "GCC") then goto has_gcc
$!
$! There should be no need to change anything below here
$!
$ axp = f$getsyi("HW_MODEL").ge.1024
$ if axp
$ then	C_C = "cc/standard=vaxc"
$	Link_Flags = "/NONATIVE_ONLY"
$ else 
$       C_C = "cc"
$ endif
$ goto start
$ has_gcc:
$  C_C = "gcc/nocase_hack/warnings"
$  Link_Libs = ",GNU_CC:[000000]GCCLIB/LIB"
$!
$ start:
$ defs = defs + "HAS_MEMCPY,HAS_MEMSET,HAS_MEMCMP"
$ bfiles = "vmsmail," + "sysdep," + "buffer," + "cmds," + "misc," + "file," -
    + "main," + "ins," + "sig," + "screen," + "paste," + "ledit," -
    + "line," + "vterm," + "search," + "text," + "keymap," + "replace," -
     + "window,undo,vfile,intrin,vmshelp,syntax,abbrev"
$!
$ jfiles = bfiles + ",display"
$ xfiles = bfiles + ",xterm"
$ if (p2 .eqs. "XJED") then goto make_xjed_label
$!
$ files = jfiles
$ make_return = "make_xjed_label"
$ jexec = "jed.exe"
$ goto simple_make
$!
$ make_xjed_label:
$  files = xfiles
$  make_return = "exit_label"
$  jexec = "xjed.exe"
$  goto simple_make
$!
$  exit_label:
$  exit
$!
$!
$!  simple make
$!
$  simple_make:
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
$    write sys$output "''C_C'/define=(''defs',JED)/include=(''slang_dir') ''f'.c"
$    'C_C'/define=('defs')/include=('slang_dir') 'f'.c
$    goto next_file
$  do_link:
$  if axp
$  then
$    if make_return .eqs. "make_xjed_label"
$    then
$      set verify
$      link/exec='jexec' 'Link_Flags' 'files', 'slang_dir'slang/LIBR 'Link_Libs'
$      set noverify
$    else
$      set verify
$      link/exec='jexec' 'Link_Flags' 'files', 'slang_dir'slang/LIBR 'Link_Libs', -
	 sys$input/opt
         sys$share:decw$xlibshr.exe/share
$      set noverify
$    endif
$  else
$    if make_return .eqs. "make_xjed_label"
$    then
$      set verify
$      link/exec='jexec' 'Link_Flags' 'files', 'slang_dir'slang/LIB 'Link_Libs',  -
	 sys$input/opt
         SYS$LIBRARY:VAXCRTL/SHARE
$      set noverify
$    else
$      set verify
$      link/exec='jexec' 'Link_Flags' 'files', 'slang_dir'slang/LIB 'Link_Libs', -
	 sys$input/opt
         sys$library:decw$dwtlibshr/share
         sys$share:decw$xlibshr/share
         SYS$LIBRARY:VAXCRTL/SHARE
$      set noverify
$    endif
$  endif
$!  rename /log 'jexec' [-.bin]
$  goto 'make_return'
$!
$!  Installation
$!  
$ Install:
$   write sys$output "Sorry, no installation implemented yet."
$!   destdef = f$trnlnm("JED_LIBRARY")
$!   again:
$!   def = ""
$!   if (destdef .nes. "") then def = "(default ''destdef')"
$!   write sys$output "Installation Directory"
$!   inquire dest "''def'"
$!   if (dest .eqs. "") then dest = destdef
$!   if (dest .eqs. "") then goto again
$!   if (f$parse(dest) .nes. "") then goto do_copy
$!      write sys$output "Directory does not exist.   Creating it."
$!      create/dir/prot=(w:re) 'dest'
$!  do_copy:
$!   copy/log/prot=(w:re) *.sl, *.hlp, *.rc, *.info, jed.*in, jed.exe, *.com  -
 !     'dest'
$  The_Exit:
$  if (ver .eq. 1) then set verify
$  exit
