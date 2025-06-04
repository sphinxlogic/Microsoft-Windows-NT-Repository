$! SETUP.COM
$!
$! This file is called to set up the PROBE command and the required logical
$! name. This file must be located in the same directory as the executable
$! (.EXE) files and the command definition (.CLD) file.
$!
$ this_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE")
$ this_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")
$!
$ define/job sys_probe 'this_dev''this_dir'
$!
$ set command sys_probe:probe
$!
$ exit
