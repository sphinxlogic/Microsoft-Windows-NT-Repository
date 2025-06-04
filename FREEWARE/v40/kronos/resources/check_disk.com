$ ! Check user disks for sufficient free space
$ !
$ file = 0
$ !
$ disk = "USER$DISK1"
$ size = 300000
$ gosub doit
$ !
$ disk = "USER$DISK2"
$ size = 300000
$ gosub doit
$ !
$ if file
$   then
$   close tf
$   mail/subject="Disk Space" kronos_root:[resources]temp.tmp system,operator
$   delete kronos_root:[resources]temp.tmp;*
$ endif
$ exit
$ !
$doit:
$ free = f$getdvi("''disk'","FREEBLOCKS")
$ if 'free' .lt. 'size'
$   then
$   if .not. file
$     then
$     file = 1
$     open/write tf kronos_root:[resources]temp.tmp
$   endif
$   write tf f$fao("Warning... disk !AS has less than !SL free blocks left.", -
                   disk,size)
$ endif
$ return
