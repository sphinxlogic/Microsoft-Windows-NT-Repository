$! int_demo.COM - INTOUCH Demo Procedure  ** distribution **
$! Copyright (c) 1989 Touch Technologies, Inc.
$!
$! After displaying the copyright notice, the main INTOUCH Demo
$! menu is started.
$!
$
$ a=f$environment("PROCEDURE")
$ devdir = f$parse("''a'",,,"DEVICE",) + f$parse("''a'",,,"DIRECTORY",)
$
$ rl = f$parse("rightslist","sys$system:rightslist.dat")
$
$ a = devdir - ".demo"
$ @'a'intouch.com
$
$ define/process/nolog TTI_INT_DEMO 'devdir'/nolog
$
$ int_demo_privs = ""
$
$ @tti_int_demo:int_demo_copyright "INTOUCH Demo" "''INT_DEMO_PRIVS'"
$ if int_demo_status .eqs. "ERROR" then goto done
$
$ int_demo_menu :==$intouch_image tti_int_demo:int_demo/image
$
$ if  f$mode() .eqs.  "BATCH"  then  goto batch
$
$ wrap_flag = f$getdvi("sys$output", "tt_wrap")
$ set terminal/wrap
$ define/user sys$input tt:
$ define/user/nolog rightslist 'rl'
$ int_demo_menu
$ goto done
$
$ batch:
$ define/user sys$input sys$command
$ define/user/nolog rightslist 'rl'
$ int_demo_menu
$ goto done
$
$ other:
$ int_demo_menu
$ goto done
$
$ done:
$ if  wrap_flag .eqs. "FALSE"  then $set terminal/nowrap
$ exit
$
