$!
$! EMU5_ROOT:[DOC]EMU_USER_FIGURES.COM
$! Convert Postscript image files to GIF
$! Created by DECdocument
$! HTML file: EMU5_ROOT:[DOC]EMU_USER.HTML
$!
$ if  f$getsyi("hw_model") .gt. 1024
$ then
$   arch = "AXP"
$ else
$   arch = "VAX"
$ endif
$ convert_ps_to_gif == "$doc$root:[ghostscript]gs_''arch'.exe
$ define gs_device "gif8"
$!
$ define gs_output_file "emu_menu_gra.gif"
$ convert_ps_to_gif emu_menu_gra.eps
$     ! GIF image anchor is in: emu_user.html at line: 424 
$
