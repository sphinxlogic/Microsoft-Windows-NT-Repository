$ default = f$environent("DEFAULT")
$ dir = f$environment("PROCEDURE")
$ dir = f$parse(dir,,,"DEVICE") + f$parse(dir,,,"DIRECTORY")
$ set default 'dir'
$ set default [-]
$ on error then goto exit
$ on control_y then goto exit
$ set on
$ zip -@ -u socketshr_src
[.ucx]BUILD.COM
[.ucx]DESCRIP.MMS
[.ucx]MAKE_SRC_ZIP.COM
[.ucx]SETUP.COM
[.ucx]SOCKET_UCX.C
[.ucx]SOCKETSHR_UCX_ALPHA.OPT
[.ucx]SOCKETSHR_UCX_VAX.OPT
[.ucx]UCX_IOCTL.C
$ exit:
$ set default 'default'
