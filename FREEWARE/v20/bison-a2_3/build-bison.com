$! build-bison.com
$! ---------------
$!
$! generate, compile, link .bison files
$!
$ arch_name = f$getsyi("ARCH_NAME")
$ if arch_name .eqs. "VAX" 
$  then 
$     cc_qualifiers=""
$     vaxcrtl_lib=",sys$Library:vaxcrtl/lib"
$     vmshlp_obj=",gnu_bison:[000000]vmshlp.obj"
$  else ! "Alpha"
$     cc_qualifiers="/prefix=ALL /standard=vaxc /g_float /define=(""alloca=malloc"")"
$     vaxcrtl_lib=""
$     vmshlp_obj=""
$  endif
$ bison/out='p1'.c 'p1'.bison
$ cc 'cc_qualifiers' 'p1'
$ link 'p1' 'vaxcrtl_lib' 'vmshlp_obj'
$ purge/confirm 'p1'.*
$ exit
