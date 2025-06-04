$! Procedure to generate MPACK.EXE and MUNPACK.EXE from object files
$
$ set noon
$
$ if f$getsyi("CPU") .gt. 127
$ then arch = "Alpha"
$ else arch = "VAX"
$ endif
$
$ self = f$environment("procedure")
$ archdir = f$parse(self,,,"device") + -
           (f$parse(self,,,"directory") - "]" + ".''arch']")
$
$ orgdir = f$environment("default")
$ set default 'archdir'
$ on control_y then goto fin
$
$ if arch .eqs. "VAX"
$ then crtl_opt = ",[-.src]vaxcrtl.opt/option"
$ else crtl_opt = ""
$ endif
$
$ link /executable=mpack.exe -
	vmspk.obj, encode.obj, magic.obj, mpacklib/library 'crtl_opt'
$ if $status then -
	write sys$output "''archdir'MPACK.EXE linked"
$ link /executable=munpack.exe -
	vmsunpk.obj, decode.obj, uudecode.obj, part.obj, mpacklib/library 'crtl_opt'
$ if $status then -
	write sys$output "''archdir'MUNPACK.EXE linked"
$
$fin:
$ set default 'orgdir'
$ exit
