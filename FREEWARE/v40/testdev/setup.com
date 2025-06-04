$! SETUP.COM
$!
$ this_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE")
$ this_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")
$!
$ define/job/nolog sys_tools 'this_dev''this_dir'
$!
$ set command sys_tools:testdev
$!
$ exit
