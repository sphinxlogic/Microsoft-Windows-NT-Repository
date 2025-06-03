$! build-flex.com
$! ---------------
$!
$! generate, compile, link .flex files
$!
$ arch_name = f$getsyi("ARCH_NAME")
$ if arch_name .eqs. "VAX" 
$  then 
$     cc_qualifiers=""
$     vaxcrtl_lib=", sys$Library:vaxcrtl/lib"
$  else ! "Alpha"
$     cc_qualifiers="/prefix=ALL /standard=vaxc /g_float"
$     vaxcrtl_lib=""
$  endif
$ ! 
$ flex 'p1'.flex
$ rename lexyy.c 'p1'.c;
$ cc 'cc_qualifiers' 'p1'
$ link 'p1', flexlib.olb/lib'vaxcrtl_lib'
$ purge/confirm 'p1'.*
$ exit
