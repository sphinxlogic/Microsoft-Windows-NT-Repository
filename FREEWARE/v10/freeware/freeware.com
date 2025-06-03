$! Freeware.com - Digital's Freeware CD-ROM access procedure
$!
$! Copyright (c) 1994 Digital Equipment Corp.
$!
$ req_privs = "TMPMBX,NETMBX
$ old_priv = f$setprv(req_privs)
$
$ version=f$extract(1,3,f$getsyi("version"))-"."  ! makes V5.2 --> 52
$ arch = "VAX"
$ if f$getsyi("HW_MODEL") .gt. 1024 then arch = "AXP"
$ version = f$extract(0,1,arch) + version
$ 
$ a=f$environment("PROCEDURE")
$ dev    = f$parse("''a'",,,"DEVICE",)
$ devdir = f$parse("''a'",,,"DEVICE",) + f$parse("''a'",,,"DIRECTORY",)
$
$ define/process/nolog freeware$cd  'dev'
$ define/process/nolog freeware$loc 'devdir'
$
$ @'devdir'freeware_copyright.com   "Main Menu" "''req_privs'" "NO"
$ if freeware_status .eqs. "ERROR" then goto done
$! wait 00:00:05    ! let them read the menu stuff for a bit
$
$! A c t i v a t e   t h e   m e n u   s y s t e m
$
$ write sys$output "Loading freeware Menu System..."
$ @freeware$cd:[tti_intouch]intouch.com
$ define/user sys$input sys$command
$ set nocontrol=y
$ intouch/source freeware$loc:freeware_menu.int
$ set control=y
$
$ done:
$ z = f$setprv (old_priv)
$ exit
