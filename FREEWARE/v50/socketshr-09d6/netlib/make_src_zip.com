$ default = f$environent("DEFAULT")
$ dir = f$environment("PROCEDURE")
$ dir = f$parse(dir,,,"DEVICE") + f$parse(dir,,,"DIRECTORY")
$ set default 'dir'
$ set default [-]
$ on error then goto exit
$ on control_y then goto exit
$ set on
$ zip -@ -u socketshr_src
[.netlib]BUILD.COM
[.netlib]DESCRIP.MMS
[.netlib]MAKE_SRC_ZIP.COM
[.netlib]NETLIB_ROUTINES.TXT
[.netlib]SETUP.COM
[.netlib]SI_NETLIB_SOCKET.C
[.netlib]SOCKETSHR_NETLIB1_ALPHA.OPT
[.netlib]SOCKETSHR_NETLIB1_VAX.OPT
[.netlib]SOCKET_NETLIB.C
[.netlib]SOCKET_NETLIB.H
$ exit:
$ set default 'default'
