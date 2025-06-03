$!+
$!	BANNER_CW.COM	Startup banner for workstation
$!-
$
$ node = f$getsyi ("NODENAME")
$ vers := 'f$getsyi ("VERSION")
$ title = f$logical ("SYS$ANNOUNCE") + " (VMS " + vers - "X4.4" + ")"
$ define /system BANNER$TITLE  "''title'"
$
$ define /system BANNER$REVERSE		"1"	! Black background
$ define /system BANNER$TOP		"1"	! Against top of screen
$ define /system BANNER$RIGHT		"0"	! Right side of screen
$ define /system BANNER$EDGE_LINES	"1"	! Little lines around the edges
$ define /system BANNER$WIDTH	     "0.90"	! Use part of the screen
$ define /system BANNER$NOBARS		"0"	! Separator bars
$ define /system BANNER$NOBORDER	"1"	! No window border
$ define /system BANNER$NOMESSAGE	"1"	! No message window
$ define /system BANNER$NOTIMESTAMP	"1"	! No timestamp at the left edge
$ define /system BANNER$UPDATE		"1"	! Tick every second
$ define /system BANNER$CPU_WINDOW	"1"	! Yes, see a cpu window
$ define /system BANNER$CPU_UPDATE	"2"	! CPU count every other second
$ define /system BANNER$CPU_COUNT	"90"	! Save a 180 second (3 minute) window
$!
$! For a 132 column wide screen
$!
$! define /system BANNER$CPU_COUNT	"405"	! Save a large window (20 mins)  !405 @ 3 seconds makes it 132 wide...!
$! define /system BANNER$X_POS	   "6.0"	! Right here
$
$
$ !
$ ! Start the banner cube and the banner program in separate processes, so
$ ! that the banner can run at high priority.
$
$ define /system BANNER$X_POS	      "12.6"	! Right here
$ define /system BANNER$LINES	      "5.6"
$ define /system BANNER$NO_RIGHT_EDGE "0"
$ define /system BANNER$CUBE_ONLY     "1"
$ define /system BANNER$NOCUBE	      "0"
$ define /system BANNER$PE_MONITOR    "0"
$ define /system BANNER$PRIORITY      "1"	! Base prio of 1 (N.B. only raise priority if NOCUBE is 1)
$ run sys$manager:banner -
	/process_name="Cube" -
	/priority=6 -
	/uic='f$user()' -
	/maximum_working_set=150 -
	/extent=300 -
	/noswap -
	/output=wta0: -
	/error=sys$manager:banner_error.log
$
$ wait 0:0:6
$ deassi /system BANNER$X_POS
$ define /system BANNER$RIGHT	      "1"	! Right side of screen
$ define /system BANNER$LINES	      "8.0"
$ define /system BANNER$NO_RIGHT_EDGE "0"
$ define /system BANNER$CUBE_ONLY     "0"
$ define /system BANNER$NOCUBE	      "1"
$ define /system BANNER$PE_MONITOR    "1"
$ define /system BANNER$PRIORITY      "14"	! Base prio of 1 (N.B. only raise priority if NOCUBE is 1)
$ run sys$manager:banner -
	/process_name="Banner" -
	/priority=6 -
	/uic='f$user()' -
	/maximum_working_set=150 -
	/extent=300 -
	/noswap -
	/output=wta0: -
	/error=sys$manager:banner_error.log
