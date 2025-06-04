$ set verify
$ set noon
$!
$! Example of how to put a device into mountverification
$!
$ ld create/nobackup tempdisk.dsk
$ ld connect/log/symbol tempdisk.dsk
$ init/nohigh/system lda'ld_unit': test
$ ld trace lda'ld_unit':
$ mount lda'ld_unit': test
$ ld watch lda'ld_unit': 1/action=error=%x84/function=code=%x0808
$ ld watch lda'ld_unit': 10/action=error=%x84/function=read
$ ld watch lda'ld_unit': 10/action=opcom/function=read
$ ld show/watch lda'ld_unit':
$ reply/enable/temp
$ spawn/nowait/input=nl: dump lda'ld_unit':/block=(start=10,count=1)
$ inquire dummy "Press return to continue
$ ld nowatch lda'ld_unit':
$ ld show/trace lda'ld_unit':
$ reply/disable
$ dism lda'ld_unit':
$ ld disc lda'ld_unit':
$ delete/nolog/noconf tempdisk.dsk;
