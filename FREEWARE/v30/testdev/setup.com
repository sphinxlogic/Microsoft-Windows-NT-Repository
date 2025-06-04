$! SETUP.COM v2.1
$!
$ define/job sys_tools -
  'f$parse(f$environment("PROCEDURE"),,,"DEVICE")''f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")'
$!
$ set command sys_tools:testdev
$!
$ exit
