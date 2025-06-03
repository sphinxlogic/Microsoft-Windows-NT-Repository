$! freeware_demo.com - VToolkit DEMO
$!
$!*****************************************************************************
$! VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
$! (c) Steve Klein							      *
$! Unpublished rights reserved under the copyright laws of the United States. *
$!*****************************************************************************
$!
$ olddef = f$environment("DEFAULT")
$ here = f$environment("PROCEDURE")
$ define/nolog here 'f$parse(here,,,"DEVICE",)''f$parse(here,,,"DIRECTORY",)
$ t = f$edit(f$getsyi("ARCH_NAME"),"UPCASE")	! platform
$ set def here
$ set def [.'t'.exe]
$
$ on severe then continue
$
$ spawn/nowait run vcolorizer
$ mcr sys$disk:[]vcountdown 10 "Starting cool VToolkit demo"
$ run testvhist
$ run testvlist
$ run testtlist
$ run simpletestvlist
$ run testvdrag
$ mcr sys$disk:[]vcountdown 10 "Thanks for trying VToolkit!"
$
$ set def 'olddef'
