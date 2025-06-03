$ arch = f$getsyi ("ARCH_NAME")
$ copy [.'arch']*driver.exe sys$common:[sys$ldr]/log
$ copy [.'arch']kill.exe sys$common:[sysexe]/log
