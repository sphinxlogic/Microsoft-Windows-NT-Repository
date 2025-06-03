$! intouch_demo.COM - INTOUCH Demo Procedure  ** distribution **
$! Copyright (c) 1994 Touch Technologies, Inc.
$!
$! After displaying the copyright notice, the main INTOUCH Demo
$! menu is started.
$!
$!
$ if  f$getsyi("HW_MODEL") .gt. 1024
$ then
$   define/job/nolog vms_environment AXP
$ else
$   define/job/nolog vms_environment VAX
$ endif
$!
$
$ a=f$environment("PROCEDURE")
$ devdir = f$parse("''a'",,,"DEVICE",) + f$parse("''a'",,,"DIRECTORY",)
$
$ rl = f$parse("rightslist","sys$system:rightslist.dat")
$
$ define/process TTI_INT_DEMO 'devdir'/nolog
$
$ int_demo_privs = ""
$
$ @tti_int_demo:int_demo_copyright "INTOUCH Demo" "''INT_DEMO_PRIVS'"
$ if int_demo_status .eqs. "ERROR" then goto done
$
$ int_demo_menu :==$intouch_image tti_int_demo:int_demo/image
$
$ write sys$output "Building INTOUCH 4GL demo environment..."
$
$ backup tti_run:vendor.dat/ignore=interlock -
         sys$scratch:tti_vendor.tmp/new_version
$ set file/prot=o:rwed sys$scratch:tti_vendor.tmp;*
$ purge sys$scratch:tti_vendor.tmp
$
$ if  f$mode() .eqs.  "BATCH"  then  goto batch
$
$ term = f$getdvi("tt:","devnam")
$
$ wrap_flag = f$getdvi("sys$output", "tt_wrap")
$ set terminal/wrap
$ define/user sys$input 'term'
$ define/user/nolog rightslist 'rl'
$ int_demo_menu
$
$ if f$search("sys$scratch:tti_vendor.tmp") .nes. ""  then -
    delete sys$scratch:tti_vendor.tmp;*
$
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
