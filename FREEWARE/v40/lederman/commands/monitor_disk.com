$! This simple command file turns SHOW DEVICE into
$! a repeating screen display.
$!
$! Works with VT100/200/300 and similar devices.  Doesn't check
$! to see if you have the right device.
$!
$! B. Z. Lederman	System Resources Corp.
$!
$ ON CONTROL_Y THEN EXIT
$ WRITE SYS$OUTPUT "[H[J"
$!
$ loop:
$ WRITE SYS$OUTPUT "[H"
$ SHOW DEVICE D
$ WAIT 00:00:01
$ GOTO loop
