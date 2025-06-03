$
$	CREATE /NAME_TABLE /PARENT=LNM$SYSTEM_DIRECTORY -
		VWSBANNER /PROT=(S:RWED,G:RWED,O:RWED,W:RWED) 
$
$  DEFINE /TABLE=VWSBANNER BACKGROUND_REVERSE	"1"
$  DEFINE /TABLE=VWSBANNER POSITION_TOP		"1"
$  DEFINE /TABLE=VWSBANNER POSITION_RIGHT	"1"
$  DEFINE /TABLE=VWSBANNER TEXT_LINES		"6.2"
$! DEFINE /TABLE=VWSBANNER SCREEN_WIDTH		"0.60"
$  DEFINE /TABLE=VWSBANNER CUBE			"0"
$  DEFINE /TABLE=VWSBANNER PAGE_IO_MONITOR	"1"
$  DEFINE /TABLE=VWSBANNER PAGE_IO_UPDATE	"1"
$  DEFINE /TABLE=VWSBANNER PAGE_IO_LNM_UPDATE	"60"
$  IF F$GETSYI ("CLUSTER_MEMBER") THEN -
   DEFINE /TABLE=VWSBANNER PE_MONITOR		"1"
$  DEFINE /TABLE=VWSBANNER PE_UPDATE		"1"
$  DEFINE /TABLE=VWSBANNER MODE_UPDATE		"1"
$  DEFINE /TABLE=VWSBANNER CPU_UPDATE		"5"
$  DEFINE /TABLE=VWSBANNER CPU_COUNT		"60"
$  DEFINE /TABLE=VWSBANNER INCLUDE_BANNER_CPU	"0"
$  DEFINE /TABLE=VWSBANNER PRIORITY		"14"
$  IF F$GETSYI ("CLUSTER_MEMBER") THEN -
   DEFINE /TABLE=VWSBANNER REMOTE_DISK		"1"	! Occupies "DEVICE_0" slot
$
$!  DEFINE /TABLE=VWSBANNER DEVICE_1_NAME	"SYS$SYSDEVICE:","FOLKD$:"
$!  DEFINE /TABLE=VWSBANNER DEVICE_1_TITLE	"Sys/Folk"
$!  DEFINE /TABLE=VWSBANNER DEVICE_2_NAME	"nodename$DUAn:"
$!  DEFINE /TABLE=VWSBANNER DEVICE_2_TITLE	"PageDisk"
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_NAME	"ESA!UL:"	!for 2000-series machines
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_NAME	"XQA!UL:"	!for QNA/LQA machines
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_COUNT	"6"		!do XQA0: to XQA5:...
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_TITLE	"DECnet"
$
$ run sys$manager:vwsbanner -
	/process_name="VWS Banner" -
	/priv=(nosame,altpri,prmmbx,cmkrnl,sysprv) -
	/priority=14 -
	/uic='f$user()' -
	/maximum_working_set=150 -
	/extent=300 -
	/noswap -
	/output=wta0: -
	/error=sys$manager:banner_error.log
$exit
$!+
$!  VWSBANNER for workstation
$!
$!	    Parameters for the banner can be set by defining logical 
$!	    names.  In all cases, if a name is not defined it is
$!	    interpreted as a "0" or "".
$!
$!	    Possible types:
$!
$!		Boolean		- value is "0" or "1" (low bit of integer)
$!		Integer		- value is a whole number, such as "0" or
$!				  "5"
$!		Floating	- value is a floating point number, such
$!				  as "3" or "8.75"
$!		String		- value is a character string, such as
$!				  "TIGNES - your corner VAXstation"
$!
$!-
$
$ ! All VWSBANNER logical names are in a separate shareable logical
$ ! name table.  Create that table.
$ !
$	CREATE /NAME_TABLE /PARENT=LNM$SYSTEM_DIRECTORY -
		VWSBANNER /PROT=(S:RWED,G:RWED,O:RWED,W:RWED) 
$
$
$ ! Set banner background
$ !
$ !	BACKGROUND_REVERSE		(switch, set to either "1" or "0")
$ !
$ !		To force a background for the banner which is the
$ !		reverse of the normal background, set this logical
$ !		name to "1".
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER BACKGROUND_REVERSE "1"
$ !
$
$
$ ! Set placement of banner
$ !
$ !   Vertical switches		(switches, set to either "1" or "0")
$ !	POSITION_BOTTOM
$ !	POSITION_TOP
$ !			Define POSITION_BOTTOM to 1 to place VWSBANNER at
$ !			bottom of screen, or POSITION_TOP to place it at
$ !			the top.  By default, the banner will be placed at
$ !			the top of the screen.
$ !
$ !   Horizontal switches
$ !	POSITION_CENTER
$ !	POSITION_LEFT
$ !	POSITION_RIGHT
$ !			Place the banner in the center of the screen, against
$ !			the left edge or against the right edge, respectively.
$ !			By default, the banner will be placed at the right
$ !			side of the screen.
$ !
$ !   Exact			(floating value, such as "10.5" or "2.553")
$ !	POSITION_X
$ !	POSITION_Y
$ !			Specify screen exact placement in physical coordinates
$ !			(cm), overrides any other placement parameters also
$ !			given.  The value zero disables exact placement, use
$ !			BANNER$LEFT or BANNER$BOTTOM to specify X=0.0 or Y=0.0.
$ !
$ !	    Defaults:	POSITION_RIGHT	1
$ !			POSITION_TOP	1
$ !			POSITION_CENTER	0
$ !			POSITION_BOTTOM	0
$ !			POSITION_LEFT	0
$ !			POSITION_X	0.0
$ !			POSITION_Y	0.0
$
$
$ ! Define size of banner
$ !
$ !	TEXT_LINES		(floating value, such as "4" or "8.4")
$ !
$ !			Floating point number which defines the height of the
$ !			banner, in number of text lines.  The default value
$ !			is 4, unless the banner is a CUBE_ONLY banner (in
$ !			which case the default is 2).  These defaults are
$ !			also the minimum values, if you specify a number
$ !			smaller than the default, the default will be used.
$ !
$ !			Note that more devices are displayed in the monitor
$ !			areas if more lines are requested.  More lines are
$ !			also displayed in the message region.
$ !
$ !			If a fractional part of a line is specified, the
$ !			spacing between the lines will be increased.
$ !
$ !	    Type:  Floating	Default:  4 (or 2 if CUBE_ONLY=1)
$ !
$!  DEFINE /TABLE=VWSBANNER TEXT_LINES "6.5"
$
$
$ !	SCREEN_WIDTH		(Floating value between 0 and 1, such as ".75")
$ !
$ !			Maximum portion of the width of physical screen to
$ !			use. The default for SCREEN_WIDTH is 0.80, or 80% of
$ !			the screen width.
$ !			SCREEN_WIDTH is ignored if MESSAGE_WINDOW is 0, use
$ !			the parameter TEXT_LINES to adjust the size of
$ !			banners without message windows.
$ !
$ !	    Type:  Floating	Default:  0.80
$ !
$!  DEFINE /TABLE=VWSBANNER SCREEN_WIDTH "0.60"
$
$
$ !	CLOCK
$ !			If set to "1", an analog clock will be displayed,
$ !			along with the day, date and a digital clock.
$ !
$ !			If TEXT_LINES is less than or equal to 6.0, the
$ !			day, date and digital clock will appear beside
$ !			the analog clock.  If TEXT_LINES is greater than
$ !			6.0, the analog clock will be above the text lines.
$ !
$ !			Note that CLOCK_UPDATE defines the base timekeeping
$ !			for VWSBANNER even when CLOCK = "0".
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER CLOCK "0"
$ !
$
$
$ !	CUBE
$ !			If set to "1", a rotating cube will be displayed.
$ !			The rotating cube is a visual indicator of system
$ !			activity.  When the cube momentarily stops rotating,
$ !			it means that the µVAX processor is busy with a more
$ !			important task.
$ !
$ !			The cube normally runs at priority 0.  The cube will
$ !			consume spare processor capacity which normally is
$ !			discarded in the idle loop.  The cube will spin
$ !			when VMS is waiting for I/O or for the user -- it will
$ !			stop spinning when CPU activity is required.
$ !
$ !			Note that having the cube spinning will make the
$ !			MODE_MONITOR less useful - much of the time the mode
$ !			monitor will end up displaying which modes
$ !			the cpu used to spin the cube.  Spinning the cube
$ !			also forces one to run the banner at low priority,
$ !			which can cause the other displays to update slowly
$ !			during busy times.
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER CUBE "0"
$ !
$
$
$ !	CUBE_ONLY
$ !			When this is set to "1", the VWSBANNER displays only
$ !			the spinning cube.  This can be useful if one desires
$ !			reliable updates of the clock or monitors while still
$ !			having the cube.
$ !
$ !			This is accomplished by running VWSBANNER in two 
$ !			separate processes.  In one process (CUBE_ONLY = 1),
$ !			the cube spins at priority 0.  In the other process
$ !			(NOCUBE = 1), the clock and monitors are updated at
$ !			high priority.
$ !
$ !			An example setup for two-process VWSBANNER appears
$ !			at the end of this file.
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER CUBE_ONLY "1"
$ !
$
$
$ !	BORDER
$ !			If set to "1", the banner will have the standard multi-
$ !			line window border.  By default, the banner has a
$ !			thin outline.
$ !
$ !			Note that if the banner does not have a border, it is
$ !			not possible to move the banner with the cursor
$ !			(there is nothing to "grab" with the pointer)..
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER BORDER "1"
$ !
$
$ !	EDGE_LINES
$ !			If set to "1", the VWSBANNER will be outlined with
$ !			a single thin line.
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER EDGE_LINES "0"
$ !
$
$
$ !	SEPARATOR_LINES
$ !			If set to "1", narrow lines will be used to
$ !			separate the message area from the timestamp area and
$ !			the clock area (and title area, if one is present).
$ !			If set to "0", no lines will separate these areas.
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER SEPARATOR_LINES "0"
$ !
$
$
$ !	NO_RIGHT_EDGE
$ !			If set to "1", the line on the right edge of the
$ !			window will not be drawn.
$ !			
$ !			This is only used to make a seamless display
$ !			of a two-process banner.  A CUBE_ONLY display
$ !			(with NO_RIGHT_EDGE = "1") can be carefully
$ !			placed touching a NOCUBE display.  The left
$ !			edge of the NOCUBE display will appear as a
$ !			separator line between the two windows.  (This
$ !			avoids the need to overlap the two windows,
$ !			which can slow down some graphics displays.)
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER NO_RIGHT_EDGE "1"
$ !
$
$
$ !	MESSAGE_WINDOW
$ !			If set to "1", a message text area will appear.  The
$ !			message area can be used by programs and command
$ !			procedures to leave status and informational messages.
$ !			For example, a command procedure could use
$ !
$ !				$ open /write mbx banner$message_mailbox
$ !				$ write mbx "Call your mother"
$ !				$ close mbx
$ !
$ !			This would cause the message "Call your mother" to
$ !			be written into the text area.
$ !
$ !			"BANNER$MESSAGE_MAILBOX" is the name of the mailbox
$ !			used by banner for the message area.
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER MESSAGE_WINDOW "1"
$ !
$
$
$ !	TIMESTAMP_MESSAGES
$ !			If this is set to "1", VWSBANNER prints the current
$ !			time along with any message written to the banner
$ !			message area.  If "0" (the default), the timestamp
$ !			will be omitted.
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER TIMESTAMP_MESSAGES "1"
$ !
$
$
$ !	CLOCK_UPDATE
$ !			This tells VWSBANNER how often to update the face of the
$ !			clock.  This is normally done each second.  If this
$ !			is more than 30 seconds, the second hand of the clock
$ !			will not be drawn.
$ !
$ !			Note that if the µVAX is quite busy, the clock will
$ !			not be updated as frequently as requested.  This is
$ !			especially true when running with the cube at priority
$ !			zero.
$ !
$ !			For more accurate updating, run at higher priority
$ !			either without the cube, or with the cube in a separate
$ !			process.
$ !
$ !			Note that CLOCK_UPDATE defines the base timekeeping
$ !			for VWSBANNER even when CLOCK = "0".
$ !
$ !	    Type:  Integer	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER CLOCK_UPDATE "5"
$ !
$
$
$ !	PRIORITY
$ !			Sets the process priority for BANNER, defaults to
$ !			0 if CUBE = 1, to 14 if CUBE = 0.
$ !
$ !			NOTE - setting priority higher with the cube
$ !			spinning can slow things down immensely!
$ !
$ !	    Type:  Integer	Default:  14 (if CUBE = 0)
$ !					   0 (if CUBE = 1)
$ !
$!  DEFINE /TABLE=VWSBANNER PRIORITY "4"
$ !
$
$
$ !	PAGE_IO_MONITOR
$ !			If set to "1", a window containing paging and I/O
$ !			statistics will be displayed.  Up to 8 lines of data
$ !			are possible.
$ !
$ !			The first four lines of data always display paging
$ !			and memory statistics.  The last four lines display
$ !			data for particular devices or groups of devices.
$ !			See the "DEVICE_*" parameters next.
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER PAGE_IO_MONITOR "0"
$ !
$
$
$ !	PAGE_IO_UPDATE
$ !			This tells VWSBANNER how often to update the statistics
$ !			in the PAGE_IO monitor window, in multiples of
$ !			CLOCK_UPDATE.
$ !
$ !			For example, if CLOCK_UPDATE = "3", and MONITOR_UPDATE
$ !			= "2", then the monitor window will be updated every
$ !			6 seconds.
$ !
$ !	    Type:  Integer	Default:  3
$ !
$!  DEFINE /TABLE=VWSBANNER PAGE_IO_UPDATE "5"
$ !
$
$
$ !	DEVICE_?_NAME
$ !	DEVICE_?_TITLE
$ !	DEVICE_?_COUNT
$ !			The four lines of device statistics in the
$ !			PAGE_IO_MONITOR display must be defined by the
$ !			user.  The four lines are numbered 0 to 3.
$ !			
$ !			For each line, three logical names can define the
$ !			device(s) to be displayed for that line.  The line
$ !			number is included as part of the name, for example
$ !			the device for line 2 is defined by the name
$ !			DEVICE_2_NAME.
$ !
$ !			These logical names can be redefined while VWSBANNER
$ !			is running, and VWSBANNER will change to use the new
$ !			definition at the next update.
$ !			
$ !			Note that the REMOTE_DISK feature uses the DEVICE_0
$ !			line.  If you request REMOTE_DISK statistics, you
$ !			cannot use DEVICE_0 -- any attempt to define DEVICE_0
$ !			names will be ignored.
$ !			
$ !		DEVICE_?_NAME
$ !			
$ !			This name defines a device name for one of the
$ !			positions.  This can be a simple name, e.g.:
$ !
$ !				$ DEF/TAB=VWSBANNER DEVICE_0_NAME "DUA0:"
$ !			
$ !			The device name logical can also be a search list
$ !			of logical names.  The data displayed is the sum
$ !			of the activity on all of the devices in the list,
$ !			for example
$ !
$ !				$ DEF/TAB=VWSBANNER -
$ !					DEVICE_1_NAME "DUA0:","DUA1:","DUA2:"
$ !			
$ !			Finally, this name can be an FAO template (look up the
$ !			system service $FAO for more info) of the device name.
$ !			For example,
$ !			
$ !				$ DEF/TAB=VWSBANNER DEVICE_0_NAME "DUA!UL:"
$ !			
$ !			Template names are used in conjunction with the
$ !			DEVICE_?_COUNT logical names, described later.
$ !			Note that the FAO directive must be in upper case!
$ !			
$ !		DEVICE_?_TITLE
$ !			
$ !			This name defines a title for the device line.  The
$ !			title can be 1 to 8 letters long, upper or lower case.
$ !			If titles aren't set, VWSBANNER will come up with
$ !			the names "DEVICE_0_NAME", etc.
$ !
$ !				$ DEF/TAB=VWSBANNER DEVICE_2_TITLE "Modem"
$ !			
$ !		DEVICE_?_COUNT
$ !			
$ !			This name sets a device count to be used in conjunction
$ !			with a template device name.  VWSBANNER will examine
$ !			the first COUNT devices, and display those statistics.
$ !
$ !			For example, if you define:
$ !
$ !				$ DEF/TAB=VWSBANNER DEVICE_3_NAME "XQA!UL:"
$ !				$ DEF/TAB=VWSBANNER DEVICE_3_COUNT  "10"
$ !
$ !			VWSBANNER will sum the counts for the first 10 XQA
$ !			devices XQA0:, XQA1:, XQA2:, XQA3:, to XQA9:.  If
$ !			a device in the range does not exist, it is skipped
$ !			and no error occurs.
$ !			
$ !			
$ !
$ !	    Type:  String	DEVICE_?_NAME, DEVICE_?_TITLE
$ !		   Integer	DEVICE_?_COUNT
$ !
$ !	    Default:   None
$ !
$!  DEFINE /TABLE=VWSBANNER DEVICE_1_NAME	"SYS$SYSDEVICE:","FOLKD$:"
$!  DEFINE /TABLE=VWSBANNER DEVICE_1_TITLE	"Sys/Folk"
$!  DEFINE /TABLE=VWSBANNER DEVICE_2_NAME	"nodename$DUA1:"
$!  DEFINE /TABLE=VWSBANNER DEVICE_2_TITLE	"PageDisk"
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_NAME	"ESA!UL:"	!for 2000-series machines
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_NAME	"XQA!UL:"	!for QNA/LQA machines
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_COUNT	"6"		!do XQA0: to XQA5:...
$!  DEFINE /TABLE=VWSBANNER DEVICE_3_TITLE	"Ethernet"
$ !
$
$
$ !	REMOTE_DISK
$ !			If set to "1", the DEVICE_0 line will be used to
$ !			display statistics for disk devices connected to
$ !			other nodes in the VAXcluster.
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$ !     Note:  The following makes the "default" a "1" if in a cluster...
$ !
$   IF F$GETSYI ("CLUSTER_MEMBER") THEN DEFINE /TABLE=VWSBANNER REMOTE_DISK "1"
$
$
$ !	PE_MONITOR
$ !			If set to "1", a window containing Local Area VAXcluster
$ !			statistics (over PEA0) will be displayed.  Up to 8
$ !			lines of data are possible.
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$ !     Note:  The following makes the "default" a "1" if in a cluster...
$ !
$   IF F$GETSYI ("CLUSTER_MEMBER") THEN DEFINE /TABLE=VWSBANNER PE_MONITOR "1"
$
$
$ !	PE_UPDATE
$ !			This tells VWSBANNER how often to update the statistics
$ !			in the PE monitor window, in multiples of
$ !			CLOCK_UPDATE.
$ !
$ !			For example, if CLOCK_UPDATE = "3", and MONITOR_UPDATE
$ !			= "2", then the monitor window will be updated every
$ !			6 seconds.
$ !
$ !	    Type:  Integer	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER PE_UPDATE "5"
$ !
$
$
$ !	MODE_MONITOR
$ !			If set to "1", a window containing a histogram of the
$ !			time spent by the µVAX processor in various processor
$ !			modes is displayed.
$ !
$ !			The display has the legend "IKESUN", which is an
$ !			acronym for the processor modes:
$ !
$ !			  Interrupt  - kernel mode on the interrupt stack
$ !			  Kernel     - kernel mode on the kernel stack
$ !			  Executive  - executive mode
$ !			  Supervisor - supervisor mode
$ !			  User       - user mode
$ !			  Null       - time spent in idle loop
$ !
$ !			The null "mode" histogram is displayed in a different
$ !			color.
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER MONITOR "0"
$ !
$
$
$ !	MODE_UPDATE
$ !			This tells VWSBANNER how often to update the statistics
$ !			in the mode histogram, in multiples of CLOCK_UPDATE.
$ !			For example, if CLOCK_UPDATE = "2", and MODE_UPDATE
$ !			= "5", then the mode histogram window will be updated
$ !			every 10 seconds.
$ !
$ !	    Type:  Integer	Default:  2
$ !
$!  DEFINE /TABLE=VWSBANNER MODE_UPDATE "5"
$ !
$
$
$ !	CPU_MONITOR
$ !			If set to "1", enables the cpu monitor feature.  A
$ !			window displaying one or more cpu utilization bars
$ !			is shown.  The height of each bar represents the
$ !			amount of cpu spent doing "useful" work.
$ !
$ !	    Type:  Boolean	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER CPU_MONITOR "0"
$ !
$
$
$ !	CPU_UPDATE
$ !			Specifies the interval between new cpu bars, in multiples
$ !			of CLOCK_UPDATE.  (If the value of CPU_UPDATE = "5" and
$ !			CLOCK_UPDATE = "2", then a new bar will appear every 10
$ !			seconds.)  Note that if VWSBANNER is unable to draw a bar at
$ !			one or more ticks, it will "catch up" by drawing wider
$ !			bars at the next interval.  For example, if CPU use is so
$ !			heavy that BANNER misses two ticks, on the third tick BANNER
$ !			will draw a bar three times wider than normal, and average
$ !			the CPU use for the entire period.
$ !
$ !			Note that scrolling the CPU window is the most CPU-intensive
$ !			activity in VWSBANNER, especially for very large windows.
$ !			Reducing the frequency of updating is the best way to
$ !			lower the CPU use by VWSBANNER.
$ !
$ !	    Type:  Integer	Default:  2
$ !
$!  DEFINE /TABLE=VWSBANNER CPU_UPDATE "5"
$ !
$
$
$ !	CPU_COUNT
$ !			Tells the number of CPU bars to display in the CPU window.
$ !			Bars scroll to the left in the CPU window, giving a visual
$ !			history of recent CPU use.
$ !
$ !			The default of 30 bars, coupled with the default update
$ !			interval of 2 seconds, gives a 60 second history.
$ !
$ !	    Type:  Integer	Default:  30
$ !
$!  DEFINE /TABLE=VWSBANNER CPU_COUNT "300"
$ !
$
$
$ !	CPU_WIDTH
$ !			Set the width of a single cpu bar (in pixels).  This is
$ !			only used when CPU_COUNT = 1, so that a single wide bar
$ !			can be displayed.
$ !
$ !	    Type:  Integer	Default:  1
$ !
$!  DEFINE /TABLE=VWSBANNER CPU_WIDTH "5"
$ !
$
$
$ !	INCLUDE_BANNER_CPU
$ !			If set to "1", includes the CPU used by the VWSBANNER
$ !			process in the system CPU window.  Normally, the CPU
$ !			used by VWSBANNER itself is not shown (the window will
$ !			show 0% even while VWSBANNER is running).
$ !
$ !			When VWSBANNER time is included, the CPU window will
$ !			not show 0%  -- it will have a non-zero base which
$ !			represents the CPU used to run VWSBANNER.
$ !
$ !	    Type:  Boolean	Default:  0
$ !
$!  DEFINE /TABLE=VWSBANNER INCLUDE_BANNER_CPU "1"
$ !
$
$
$ !	MESSAGE_TITLE
$ !			This logical name defines a title which remains fixed
$ !			above the center of the message area.  Note that a
$ !			fixed title uses one of the message area text lines
$ !			(i.e. you get the title and TEXT_LINES-1 of scrolling
$ !			text).
$ !
$ !			A horizontal bar will separate the title area from the
$ !			message area if SEPARATOR_BARS = 1.
$ !
$ !	    Type:  String	Default:  no title
$ !
$ !   Note: The following line sets the "default" to SYS$ANNOUNCE:
$ !
$   DEFINE /TABLE=VWSBANNER MESSAGE_TITLE "''f$logical("SYS$ANNOUNCE")'"
$ !
$
$ !
$ ! Start banner program in separate process, assume that the banner image
$ ! is in the same directory as this command procedure.
$ !
$ proc = f$environment ("PROCEDURE")
$ dire = f$parse (proc,,,"DIRECTORY")
$ dev  = f$parse (proc,,,"DEVICE")
$ dev2 = dev - ":"
$ if f$logical (dev2) .nes. "" then dev = f$logical (dev2)
$
$ run 'dev''dire'vwsbanner -
	/process_name="VWSBanner" -
	/priv=(altpri,prmmbx,cmkrnl,sysprv) -
	/priority=4 -
	/uic='f$user()' -
	/maximum_working_set=150 -
	/extent=300 -
	/noswap -
	/output=wta0: -
	/error=sys$manager:banner_error.log
