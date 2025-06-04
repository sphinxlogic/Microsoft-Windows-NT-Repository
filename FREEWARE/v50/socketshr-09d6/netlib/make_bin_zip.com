$ default = f$environent("DEFAULT")
$ dir = f$environment("PROCEDURE")
$ dir = f$parse(dir,,,"DEVICE") + f$parse(dir,,,"DIRECTORY")
$ set default 'dir'
$ set default [-]
$ on error then goto exit
$ on control_y then goto exit
$ set on
$ zip -"V" -u socketshr_bin [.netlib]*.exe [.netlib]*.olb [.netlib]*.obj
$ exit:
$ set default 'default'
