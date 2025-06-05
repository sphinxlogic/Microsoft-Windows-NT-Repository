$! Freeware.com - OpenVMS Freeware CD-ROM access procedure
$!
$! Copyright 2003 Hewlett-Packard Company
$! Copyright 2001 Compaq Computer Corporation
$! Copyright 1994 Digital Equipment Corporation
$!
$ Set NoOn
$
$ myname = f$environment("PROCEDURE")
$ dev    = f$parse(myname,,,"DEVICE")
$ dir    = f$parse(myname,,,"DIRECTORY") - "][" - "><"
$ devdir = dev + dir
$
$ define/process/nolog freeware$cd  'dev'
$ define/process/nolog freeware$loc 'devdir'
$
$!  A c t i v a t e   t h e   ( r e a l )  m e n u   s y s t e m
$!
$ @freeware$loc:freeware_menu.com
$ Exit
