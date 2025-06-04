$! This simple command file turns MONITOR MEMORY into
$! a repeating screen display.  Unfortunately, you can't
$! get all information into a 24 line screen, so you have
$! to change the SHOW MEMORY command to have the qualifiers
$! you want in any particular instance.
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
$ SHOW MEM /PHY/POOL/FILE
$ WAIT 00:00:02
$ GOTO loop
