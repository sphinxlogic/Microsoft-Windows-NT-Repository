$! INTOUCH_BUILD.COM - Build INTOUCH end-user utilities
$!
$! Copyright (c) 1994 Touch Technologies, Inc.
$!
$! p1 = YES  if just use standard INTOUCH (no Oracle special .EXE)
$
$ a=f$environment("PROCEDURE")
$ devdir = f$parse("''a'",,,"DEVICE",) + f$parse("''a'",,,"DIRECTORY",)
$ @'devdir'intouch.com
$ arch = "VAX"
$ if f$getsyi("HW_MODEL") .gt. 1024 then arch = "AXP"
$ exename = "EXE"
$ if arch .eqs. "AXP" then exename = "EXE_AXP"
$
$
$ done_engines:
$ write sys$output ""
$ write sys$output "INTOUCH Utility Build Started"
$
$ !write sys$output "  building Guided Query Language (GQL)..."
$ !intouch/compile guide.int
$
$ write sys$output "  building SETUP - Structure Set up..."
$ intouch/compile setup.int
$
$ write sys$output "  building MAINTAIN - Structure Maintenance..."
$ intouch/compile maintain.int
$
$ write sys$output "  building TRAINING_SYSTEM - INTOUCH Training Package..."
$ intouch/compile training_system.int
$
$ write sys$output "  building INTOUCH_DEMO - INTOUCH DEMO system..."
$ gosub do_build_demo
$
$ done_building:
$ set file *.int_img*/prot=w:re
$ purge tti_run:*.int_img*
$
$ write sys$output "INTOUCH Utility Build Completed"
$ write sys$output ""
$ write sys$output "INTOUCH Build Completed"
$ @'devdir'intouch.com  ! set up final symbols
$ exit
$
$! D O   B U I L D   D E M O
$!
$! Build the INTOUCH demo system
$ do_build_demo:
$   if f$search("tti_run:demo.bck") .eqs. "" then return  ! nothing to do
$
$   if f$search("[]demo.dir;1") .eqs. "" then gosub create_demo_dir
$
$   set default [.demo]
$   backup tti_run:demo.bck/save *.*/new_version
$   intouch/compile int_demo.int
$   @int_demo_link.com
$   purge *.*
$   set file *.*;/prot=w:re
$   set default tti_run:
$
$   return
$
$ create_demo_dir:
$   create/dir/version=3 [.demo]
$   set file tti_run:demo.dir/prot=w:re
$   return
