$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ cc/stand=vaxc xtga
$ link xtga, xtga/opt
$ cc/stand=vaxc tga2gif
$ link tga2gif
$ else
$ if p1 .nes. "" then goto 'P1'
$ cc/opt xtga
$ link/exe=xtga_vax xtga, sys$input/opt
name="XTGA"
ident="V1"
sys$library:vaxcrtl/share
SYS$SHARE:DECW$Xlibshr/SHAREABLE
SYS$SHARE:DECW$Xmlibshr/SHAREABLE
SYS$SHARE:DECW$Xtshr/SHAREABLE
$TGA2GIF:
$ cc/opt tga2gif
$ link/exe=tga2gif_vax tga2gif, sys$input/opt
sys$library:vaxcrtl/share
sys$library:decw$xlibshr/share
SYS$SHARE:DECW$Xmlibshr/SHAREABLE
sys$library:decw$xtshr/share
$ endif
