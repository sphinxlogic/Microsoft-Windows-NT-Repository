$! COMPILE.COM for FastLogin
$! Must be called on VAX V6.1, Alpha V6.1 and Alpha V6.2.
$! On Alpha V6.1, we build an image without proxy support.
$!
$ alpha = f$getsyi("arch_type").eq.2
$ major = f$integer(f$extract(1,1,f$getsyi("version")))
$ minor = f$integer(f$extract(3,1,f$getsyi("version")))
$!
$ if alpha .and. major.eq.6 .and. minor.lt.2
$ then arch = "ALPHA_V61"
$ else arch = f$edit(f$getsyi("arch_name"),"upcase")
$ endif
$!
$ write sys$output "Generating FASTLOGIN_''arch'.OBJ"
$ cc/object=fastlogin_'arch' fastlogin.c
$!
$ write sys$output "Generating FASTLOGIN_''arch'.EXE"
$ if alpha
$ then
$   link/share=fastlogin_'arch'.exe fastlogin_'arch'.obj, sys$input:/options
    symbol_vector=(lgi$loginout_callouts=data)   ! Alpha version
$ else
$   link/share=fastlogin_'arch'.exe fastlogin_'arch'.obj, sys$input:/options
    universal=lgi$loginout_callouts              ! VAX version
$ endif
