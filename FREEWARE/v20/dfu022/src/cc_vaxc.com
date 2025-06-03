$! This procedure builds DFU using the VAXC compiler.
$!
$ vaxtype = f$getsyi("arch_name")
$ if vaxtype .eqs. "VAX"
$ then 
$ qualifiers = "/vaxc/warn=noinfo"
$ else
$ qualifiers = "/stand=vaxc/extern=common/share/warn=noinfo/nomember"
$ endif
$! Main file ...
$ cc'qualifiers' DFU
$! Subroutines ...
$ cc'qualifiers' DFU_IFSCAN
$ cc'qualifiers' DFU_FILEDIR
$ cc'qualifiers' DFU_BITMAP
$! Command table
$ set command/obj dfu_tables
$! Message file
$ mess/obj dfumsg
$! Link the stuff...
$ link/notrace dfu,dfu_ifscan,dfu_filedir,dfu_bitmap,dfu_tables,dfumsg,'vaxtype'rtl/opt
