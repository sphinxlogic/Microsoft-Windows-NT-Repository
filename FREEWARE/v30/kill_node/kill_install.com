$ arch = f$getsyi ("ARCH_NAME")
$ maj= f$extract (1,80,f$element (0,".",f$getsyi ("version")))
$ ext= ""
$ sh sym maj
$ if arch.eqs."Alpha" .and. maj.lts."7" then ext:= _v6
$ copy [.'arch']*driver'ext'.exe sys$common:[sys$ldr]/log
$ copy [.'arch']kill.exe sys$common:[sysexe]/log
