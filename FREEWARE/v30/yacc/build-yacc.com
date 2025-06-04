$! build-yacc.com
$! ---------------
$!
$! generate, compile, link .yacc files
$!
$ arch_name = f$getsyi("ARCH_NAME")
$ if arch_name .eqs. "VAX" 
$  then 
$     cc_qualifiers=""
$     vaxcrtl_lib=",sys$Library:vaxcrtl/lib"
$  else ! "Alpha"
$     cc_qualifiers="/prefix=ALL /standard=vaxc /g_float
$     vaxcrtl_lib=""
$  endif
$ yacc 'p1'.yacc
$ rename y.tab_c 'p1'.c;
$ cc 'cc_qualifiers' 'p1'
$ link 'p1' 'vaxcrtl_lib'
$!
$! Cleanup temp files
$ purge/confirm 'p1'.*
$ delete/confirm tmpdir:yacc.*;*
$ exit
