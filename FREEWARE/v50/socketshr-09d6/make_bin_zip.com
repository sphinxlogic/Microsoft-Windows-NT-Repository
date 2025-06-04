$ on error then goto exit
$ on control_y then goto exit
$ set on
$ zip "-V" -@ -u -j socketshr_bin
protocols.
readme.txt
services.
socketshr.h
file.h
types.h
ioctl.h
socketshr_xfr_alpha.opt
socketshr_xfr_vax.opt
socketshr_xfr_vax_decc.opt
socketshr_xfr_vax_vaxc.opt
link.com
[.netlib]socketshr_xfr_vax.obj
[.netlib]socketshr_netlib1.exe
[.netlib]socketshr_netlib1.alpha_exe
[.netlib]socketlib_netlib1_vax.olb
[.netlib]socketlib_netlib1_alpha.olb
[.netlib]socketshr_netlib1_vax.opt
[.netlib]socketshr_netlib1_alpha.opt
[.ucx]socketshr_ucx.exe
[.ucx]socketshr_ucx.alpha_exe
[.ucx]socketlib_ucx_vax.olb
[.ucx]socketlib_ucx_alpha.olb
[.ucx]socketshr_ucx_vax.opt
[.ucx]socketshr_ucx_alpha.opt
$!
$ exit:
