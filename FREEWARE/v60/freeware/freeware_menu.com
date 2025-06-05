$! Freeware_Menu.com - OpenVMS Freeware CD-ROM menu access procedure
$!
$! Copyright 2003 Hewlett-Packard Company
$! Copyright 2001 Compaq Computer Corporation
$! Copyright 1994 Digital Equipment Corporation
$!
$ req_privs = "TMPMBX,NETMBX"
$ old_priv = f$setprv(req_privs)
$
$ myname = f$environment("PROCEDURE")
$ dev    = f$parse(myname,,,"DEVICE")
$ dir    = f$parse(myname,,,"DIRECTORY") - "][" - "><"
$ devdir = dev + dir
$
$ define/process/nolog freeware$cd  'dev'
$ define/process/nolog freeware$loc 'devdir'
$
$ @'devdir'freeware_copyright.com   "Main Menu" "''req_privs'" "FALSE"
$ if freeware_status .eqs. "ERROR" then goto main_exit
$! wait 00:00:05    ! allow the user to read the display
$
$! A c t i v a t e   t h e   m e n u   s y s t e m
$!
$ write sys$output " "
$ write sys$output "Loading the OpenVMS Freeware Menu System..."
$ write sys$output " "
$ @freeware$cd:[tti_intouch]intouch.com
$ define/user sys$input sys$command
$ set nocontrol=y
$ intouch/source freeware$loc:freeware_menu.int
$ set control=y
$
$main_exit:
$ garbage = f$setprv(old_priv)
$ exit
