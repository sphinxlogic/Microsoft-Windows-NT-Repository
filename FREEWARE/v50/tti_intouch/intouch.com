$! INTOUCH.COM - Set up INTOUCH logicals and symbols
$!
$! Copyright (c) 1987, 1988 Touch Technologies, Inc.
$!
$ a=f$environment("PROCEDURE")
$ devdir = f$parse("''a'",,,"DEVICE",) + f$parse("''a'",,,"DIRECTORY",)
$ demo_dir = devdir - "]" + ".DEMO]"
$ arch = "VAX"
$ if f$getsyi("HW_MODEL") .gt. 1024 then arch = "AXP"
$ exename = "EXE"
$ if arch .eqs. "AXP" then exename = "EXE_AXP"
$
$ imgname = "INT_IMG"
$ if arch .eqs. "AXP" then imgname = "INT_IMG_AXP"
$
$!
$ ASSIGN/PROCESS 'devdir'   TTI_HELP/nolog
$ ASSIGN/PROCESS 'devdir'   TTI_RUN/nolog
$
$ ASSIGN/PROCESS YES        INTOUCH_SETUP/nolog
$
$ ASSIGN/PROCESS 'demo_dir' INT_DEMO_DIR/nolog
$ INTOUCH_DEMO :==@INT_DEMO_DIR:INTOUCH_DEMO
$
$ ASSIGN/PROCESS 'devdir' TTI_TRAINING/nolog
$ ASSIGN/PROCESS 'devdir' TTI_TRAINING_TUTORIAL/nolog
$
$ @TTI_RUN:INT_ID.COM ! djs 12-may-95 always define the ids
$
$ if f$search("''devdir'intouch.''exename'") .nes. ""     then goto distribution
$ if f$search("''devdir'intouch_run.''exename'") .nes. "" then goto run_only
$
$ !
$ ! A demo
$ DEFINE/NOLOG/PROCESS INTOUCH_IMAGE "TTI_RUN:INTOUCH_DEMO.''exename'"
$! @TTI_RUN:INT_ID.COM  DJS always define the ids now
$ goto finish
$ !
$
$ ! Run only version
$ run_only:
$ DEFINE/NOLOG/PROCESS INTOUCH_IMAGE "TTI_RUN:INTOUCH_RUN.''exename'"
$ goto finish
$
$ !
$ ! A distribution
$ distribution:
$ DEFINE/NOLOG/PROCESS INTOUCH_IMAGE "TTI_RUN:INTOUCH.''exename'"
$ goto finish
$
$ finish:
$ intouch :== "$intouch_image:"
$
$ if f$search("tti_run:guide.''imgname'") .nes. "" then -
     guide :== "$intouch_image tti_run:guide/image"
$
$ if f$search("tti_run:maintain.''imgname'") .nes. "" then -
     maintain :== "$intouch_image tti_run:maintain/image"
$
$ if f$search("tti_run:training_system.''imgname'") .nes. "" then -
     training_system :== "$intouch_image tti_run:training_system/image"
$
$ exit
