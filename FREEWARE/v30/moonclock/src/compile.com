$! Compile under VMS
$ def sys sys$library:
$ CC/define=VMS CLOCK
$ CC/define=VMS DTIME
$ CC/define=VMS MOON
$ CC/define=VMS OCLOCK
$ CC/define=VMS PHASE
$ CC/define=VMS TRANSFORM
$! Link
$ @link
