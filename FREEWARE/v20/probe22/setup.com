$! SETUP.COM v2.2
$!
$ define/job sys_probe -
  'f$parse(f$environment("PROCEDURE"),,,"DEVICE")''f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")'
$!
$ set command sys_probe:probe
$!
$ exit
