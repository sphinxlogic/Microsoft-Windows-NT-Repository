$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ compile := cc/stand=vaxc/opt
$ xgif_opt = "xgif"
$ else
$ compile := cc/opt
$ xgif_opt = "xgif_vax"
$ endif
$
$ compile xgif
$ compile xgifload
$ link xgif, xgifload, 'xgif_opt'/opt
