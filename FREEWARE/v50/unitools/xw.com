$ verify = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!  X W . C O M
$!
$!  From a VMS DECwindows workstation, run a DECwindows application program
$!  on a remote node.
$!
$!  Assumptions:
$!	- For execution on a remote server from a VAXstation, the user has a
$!	  proxy login enabled on the remote server node.
$!	- By default, the image to be executed has a name of the form
$!	  SYS$SYSTEM:DECW$xx.EXE where "xx" is the image identification passed
$!	  to this procedure (e.g. CALENDAR, PAINT, CALC, etc.).
$!	- Because of a 12 character limit to the length of the string passed to
$!	  the DECNET TASK object, the image name (or image pseudonym) can have
$!	  a maximum length of 10 characters or less, depending on the length of
$!	  the name of the NCP TASK which executes this DCL procedure.
$!	  If no NCP TASK is defined, then the length of the parameter to the
$!	  task will be (11 - length_of_proc_name).
$!	  For example, if this procedure is in a file called DWR.COM, the
$!	  maximum image identification is 8 characters.
$!	- For a non-DECnet invocation, the image/command parameter has no
$!	  imposed minimum length limit.
$!
$!  Usage: @XW p1 [p2] [p3] [p4]
$!   where p1 is an image identification / command string; (required)
$!         p2 is a client on which to display; (optional)
$!	   p3 is a server on which to run; (optional)
$!	   p4 is a parameter/qualifier set for the SET DISPLAY cmd. (optional)
$!
$!  Examples:
$!	- To run the DECwindows calculator on node BIGVAX from
$!	  a VMS VAXstation, do  $ @XW  CALC  ""  BIGVAX
$!	- To run the DECwindows bookreader to display on node 128.250.9.9 from
$!	  a telnet session on a VAXcluster node, do  $ @ XW  BOOK  128.250.9.9
$!
$!------------------------------------------------------------------------------
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!           Thanks to Chris Chaundy for some ideas here.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	12-Jun-1991	JER	Original version.
$!------------------------------------------------------------------------------
$
$ on error  then  goto  END
$ say := write SYS$OUTPUT
$
$ network = f$mode() .eqs. "NETWORK"
$ node    = f$getsyi ("NODENAME")
$ remote  = f$trnlnm ("SYS$REM_NODE") - "::"
$ display = f$trnlnm ("DECW$DISPLAY")
$ VAXstation   = (f$getsyi ("WINDOW_SYSTEM") .eq. 1) .and. (display .nes. "")
$ temp_display = 0
$
$ screen = 0				! default
$ xport  = "TCPIP"			! default
$
$ cmd    = p1
$ client = p2
$ server = p3
$ if network
$ then
$   client = remote
$   xport  = "DECNET"
$ endif
$
$ if .not. network		! ... then it's the initiating process.
$ then
$   on control_y  then  goto  END
$   set control = (Y,T)
$
$   if cmd .eqs. ""
$   then
$     say "%XW-ERROR, A DECwindows application or command must be specified."
$     goto  END
$   endif
$
$   if f$locate (".",client) .eq. f$length (client)  then  xport = "DECNET"
$
$   if VAXstation
$   then
$     if client .eqs. ""    then  client = node
$     if client .eqs. node  then  xport  = "LOCAL"
$     if server .nes. ""	! Fire it off, then terminate.
$     then
$	if client .eqs. node
$	then
$	  open  net1 'server'::"task=XW ''cmd'"
$	  close net1
$	else
$	  say "%XW-ERROR, Execution and display not on client is not supported."
$	endif
$	goto  END		! End of VAXstation initiation of application
$     endif
$   else			! It's something other than a VMS VAXstation.
$     if client .eqs. ""
$     then
$	if remote .nes. ""
$	then
$	  client = remote
$	else
$	  say "%XW-ERROR, A remote node name on which to display is required."
$	  goto  END
$	endif
$     endif
$     colon  = f$locate (":",client)
$     client = f$extract (0,colon,client)
$     screen = f$extract (colon+1,f$length(client)-colon-1,client)
$     if screen .eqs. ""  then  screen = 0
$   endif
$ endif
$
$! Run application:
$! Make it easier to use standard DECwindows applications ...
$
$ if cmd .eqs. "BOOK"  then  cmd = "BOOKREADER"
$ if cmd .eqs. "CARD"  then  cmd = "CARDFILER"
$
$ if (cmd .eqs. "BOOKREADER") .or. -
     (cmd .eqs. "CALC") .or. -
     (cmd .eqs. "CALENDAR") .or. -
     (cmd .eqs. "CARDFILER") .or. -
     (cmd .eqs. "CLOCK") .or. -
     (cmd .eqs. "MAIL") .or. -
     (cmd .eqs. "NOTEPAD") .or. -
     (cmd .eqs. "PAINT") .or. -
     (cmd .eqs. "PUZZLE")  then  cmd = "run SYS$SYSTEM:DECW$''cmd'"
$
$! Include pseudonym handling for non-standard commands here ...
$
$ if cmd .eqs. "DECTERM"  then  cmd = "create /terminal=decterm /detach /nologg"
$ if cmd .eqs. "DECTERMN" then  cmd = "create /terminal=decterm /detach"
$ if cmd .eqs. "DECTERMP" then  cmd = "create /terminal=decterm /wait"
$ if cmd .eqs. "EVE"  then -
     cmd = "edit /tpu /display=decwindows /section=TPU$SECTION"
$ if (cmd .eqs. "FILEVIEW") .or. (cmd .eqs. "VUE")  then -
     cmd = "run SYS$SYSTEM:VUE$MASTER"
$ if cmd .eqs. "VIEW" then  cmd = "view /select=x_display"
$
$ if cmd .eqs. ""  then  goto  END
$
$ if network
$ then
$   open  net2 /read /write SYS$NET
$   close net2
$ endif
$
$! If the display doesn't already exist, create it ... then do the command!
$
$ if (display .eqs. "") .or. (client .nes. node)
$ then
$   set display /create /node='client' /screen='screen' /transport='xport' 'p4'
$   temp_display = 1
$ endif
$
$ 'cmd'
$
$END:
$ if temp_display  then  set display /delete
$ if network  then  EOJ
$
$ EXIT 1 + 0 * f$verify(verify)
