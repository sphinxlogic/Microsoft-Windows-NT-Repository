$ saved_verify = 'f$verify(0)'
$ DWremterm_version := V041
$ set noon
$!******************************************************************************
$! AUTHOR: Geoff.Kingsmill@compaq.com
$!
$! Geoff Kingsmill - 21-OCT-1991 - created
$! Geoff Kingsmill - 09-NOV-1991 - fixed error caused by a user setting his
$!                                 process name. The caused FPD to fail.
$! Geoff Kingsmill - 06-DEC-1991 - improved command verification
$! Geoff Kingsmill - 11-JAN-1992 - combined network routines into this procedure
$!				   and added RDT, RFV, RDW and DWRT commands
$! Geoff Kingsmill - 18-FEB-1992 - bugfixes.
$! Geoff Kingsmill - 17-MAR-1992 - provided user with more error information.
$! Geoff Kingsmill - 26-MAR-1992 - provided user with warning if LAT not running
$! Geoff Kingsmill - 15-APR-1992 - added DISCONNECT_LINK to provide user with 
$!                                 even more error information. Added option for
$!                                 users to override the DWRT resource file
$!                                 window position by way of logical names.
$! Geoff Kingsmill - 27-AUG-1992 - removed menu item from display and fixed
$!                                 a timing loop problem when setting process
$!                                 name
$! Geoff Kingsmill - 29-SEP-1992 - if you issue an RDT and you proxied into the
$!                                 DECNET account then you will now be prompted
$!                                 for the a username and password rather than
$!                                 being denied access.
$!				 - fixed up DECNET and LAT commands so that
$!                                 nodename access control information is
$!                                 stripped off ie mvax"user pass" becomes mvax
$!				 - when an error occurs on a remote node,
$!                                 non-privileged users will now be mailed with
$!                                 an error message stating why the failure
$!                                 occurred. Also stopped sending a mail
$!				   when the source node closed the network link.
$! Geoff Kingsmill - 02-FEB-1993 - Modified LAT command to support LAT V2
$!				   functionality. This allows a set host/lat
$!				   to the same node and /noautoprompt.
$! Geoff Kingsmill - 27-MAR-1993 - Modified error message when DWremterm object
$!				   access fails.
$! Geoff Kingsmill - 17-MAY-1993 - Added temporary workaround for DECwindows
$!				   Motif V1.1 bug where the WSA display device
$!				   gets deleted on DECterm creation. This 
$!				   workaround only works for users with SYSNAM 
$!				   privilege.
$! Geoff Kingsmill - 28-JUN-1993 - Fixed Duplicate Process Name problem
$! Geoff Kingsmill - 28-SEP-1993 - Added support for Parthworks (PCX$SERVER)
$! Geoff Kingsmill - 06-DEC-1993 - Added $set host/lat/AUTOCONNECT
$! Geoff Kingsmill - 06-APR-1994 - Added X/Y location to the RDT command
$! Geoff Kingsmill - 30-NOV-1994 - Fix DWremterm startup when being brought up
$!				   by TCP/IP RSH.
$! Geoff Kingsmill - 12-DEC-1994 - DWremterm window would abort if special
$!				   characters such as ?,^ were entered whilst
$!				   in MENU_MODE.
$! Geoff Kingsmill - 29-APR-1995 - Preserve lowercase characters 
$!				 - RDW and "RDT command" commands are now
$!				   run from a detached process. This was done
$!				   to support DECnet/OSI and remote spawn
$!                                 commands.
$!                               - disable autoupdate
$!				 - updated to version V3.1
$! Geoff Kingsmill - 23-JAN-1996 - Added /nobroadcast to lat and decnet commands
$! Geoff Kingsmill - 31-JAN-1996 - LAT and DECNET commands now run as a detached
$!				   job and can therefore be run in command mode.
$! Geoff Kingsmill -  1-FEB-1996 - added TELNET and RSH commands.
$! Geoff Kingsmill - 24-APR-1996 - modified RDW to allow command to specify
$!				   display eg. mcr decw$clock -display maxps1:0
$! Geoff Kingsmill - 24-MAY-1996 - fixed blank process name when creating
$!                                 detached process. All DWremterm commands
$!				   now run in detached mode.
$! Geoff Kingsmill - 10-AUG-1996 - added LDT command which does a local
$!                                 create/term followed by an rlogin.
$!				   Update to V4.0
$! Geoff Kingsmill - 03-SEP-1996 - prevent non-privileged users from logging 
$!				   in whilst logins are disabled.
$! Geoff Kingsmill - 10-FEB-1997 - fixed so dwremterm menu mode could be 
$!				   started from eXcursion using TCPIP transport.
$!				   Changed dwremterm menu mode so resource file
$!				   was specifically specified rather than
$!				   relying on the decw$user_defaults logical
$! Geoff Kingsmill - 13-FEB-1997 - fixed menu_mode window size under CDE
$! Geoff Kingsmill -  6-MAY-1997 - make tcpip the default transport
$! Geoff Kingsmill - 16-JUN-1997 - fix problem where password would fail to be
$!                                 passed to detached process via mailbox
$!				 - fix dwremterm window size after restarting
$!				   cde
$! Geoff Kingsmill - 13-Aug-1998 - modified rdt with a command file so that
$!                                 it returns to the dcl prompt when finished
$!                                 rather than exiting. This required dwremterm
$!                                 to manually invoke sys$sylogin/sylogin.com 
$!				   and login.com. Note that this procedure will
$!                                 NOT execute a non-default sys$login:login.com
$!                                 procedure as defined in authorize (lgicmd).
$!
$! This command procedure is designed to allow users flexibility in using 
$! DECwindows across a large number of machines connected together by a 
$! Local Area Network. It allows users to create LAT, DECnet, Telnet and LDT 
$! windows locally and connect to remote nodes or to create remote DECwindow 
$! sessions using the RDT, RFV, RDW, DWRT and RSH commands. The remote 
$! commands are actually network process's and therefore do not count as an 
$! interactive login.
$! 
$! DWremterm consists of two files:
$! SYS$COMMON:[VUE$LIBRARY.USER]DWREMTERM.COM 
$! SYS$COMMON:[SYSHLP]DWREMTERM.HLB
$!
$! The RDT, RDW, DWRT and RFV commands require DWREMTERM.COM to exist on the 
$! both the local and remote node and requires the user to either specify node
$! access control or for the remote account to be set up for proxy access. 
$! DWremterm can simply reside in sys$login on the remote node, which requires 
$! each user to have their own copy -or- DWremterm can be defined as a DECnet 
$! object, requiring each system to have only one copy.
$!
$!DECNET:
$! To install DWremterm as a network object the system manager should enter the
$! following commands:
$!DECnet/IV:
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT DWREMTERM NUMBER 0 FILE -
$!       SYS$COMMON:[VUE$LIBRARY.USER]DWREMTERM.COM PROXY BOTH
$!  NCP> SET OBJECT DWREMTERM ALL
$!  NCP> EXIT
$!  $
$!DECnet/OSI:
$!  $@sys$startup:net$configure application_add dwremterm "" -
$!  "{name=dwremterm}||vue$library:dwremterm.com|true|true|true|true|true||||1" 
$!
$! If DWremterm is not installed as a network object then by default no other 
$! setups are required. However if the DECnet object task has been modified to 
$! disable proxy access then DWremterm will fail. In this case the system 
$! manager should enter the following commands:
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT TASK PROXY BOTH
$!  NCP> SET OBJECT TASK ALL
$!  NCP> EXIT
$!  $
$!
$! PROXY accounts allow one user direct access to another users account
$!  without having to specify username and password. The system manager
$!  can set up proxy account access from within AUTHORIZE. 
$!  Example:  $ MCR AUTHORIZE>
$!            UAF> ADD/PROXY MVAX::KINGSMILL KINGSMILL/DEFAULT
$!            UAF> ADD/PROXY MVAX::KINGSMILL OLSENK
$!            UAF> EXIT
$!            $
$!  This examples allows user KINGSMILL on node MVAX into the local KINGSMILL 
$!  and OLSENK account. The default switch enables you to specify just the 
$!  nodename when addressing this node:-
$!    eg. $ DIRECTORY TVAX::
$!  If Default access is not specified then you need to specify the 
$!  nodename and username when addressing this node:-
$!    eg. $ DIRECTORY TVAX"OLSENK"::
$!  If you do not have proxy access then you need to specify the
$!  nodename, username and password when addressing a remote node:-
$!    eg. $ DIRECTORY TVAX"SYSTEM password"::
$!  
$!LAT:
$! VMS V5.5-2 is required for outgoing LAT. This is enabled as follows:-
$! MCR LATCP> SET NODE/CONNECTION=BOTH.
$!
$!TCPIP:
$! To prevent the need to use passwords on the RSH command proxies can be
$! set up to authorize remote user access. To set up remote user access
$! on a UNIX machine, or TCPWARE/MULTINET under VMS you specify the remote
$! node and username in the .rhosts file which resides in the users home
$! directory. With UCX you use the "UCX> ADD PROXY command"
$! In addition, if you don't want to specify a password on RSH and RLOGIN 
$! you may need to add the following:-
$! UCX> set service rsh /flag=(proxy,case)
$! UCX> set service rlogin /flag=(proxy,case)
$!
$!DISPLAY SECURITY:
$! The Factory Defaults prohibit a remote users from displaying their
$! output to your workstation. To authorize other users or remote
$! nodes to use your workstation display you should:
$!   1. Choose Security... from Session Manager's Options menu.
$!   2. Type the nodename, the username, and the method of transport,
$!      of the user you want to authorize. Entering * * * will allow
$!      any user on any node with any transport to display output on
$!      your workstaton.
$!   3. Click on the Add button and the Clock OK.
$! Refer to the DECwindows user guide for further information.
$!
$!DWREMTERM COMMANDS:
$! DWremterm can be run in two modes:
$!   MENU MODE: commands are entered from a Small Menu Window.
$!   COMMAND MODE: commands are entered from the DCL command line.
$!
$! Valid command are:- 
$!   HELP, DWHELP, LAT, DECNET, TELNET, LDR, RDT, RFV, RDW, RSH, DWRT and $.
$! Parameters listed are [] brackets is optional. 
$! HELP: Gives details on how to receive Brief or Expanded Help
$! DWHELP: Provide detailed Help on DWremterm
$! LAT:    Creates a Local DECterm Window using a SET HOST/LAT command.
$!         LAT nodename
$! DECNET: Creates a Local DECterm Window using a SET HOST command.
$!         DECNET nodename
$! TELNET: Creates a Local DECterm Window using a TELNET command.
$!         TELNET nodename
$! LDT:	   Creates a local decterm and then does an rlogin
$!         LDT nodename [init_state] [title] 
$! RDT:    Creates a Remote Decterm Window.
$!         RDT nodename [init_state] [title] [dcl_command]
$!                      [display] [transport] [x:y]
$! RFV:    Creates a Remote Fileview Window.
$!         RFV nodename [init_state] [display] [transport]
$! RDW:    Runs a Remote Remote Decwindows Application.
$!         RDW nodename command [display] [transport]
$! RSH:	   Runs RSH commands on a remote node.
$!         RSH nodename command 
$! DWRT:   Starts a second DWremterm Window.
$!         DWRT nodename [init_state] [display] [transport]
$! $:      Executes a local DCL command
$! For further details refer to sys$library:dwremterm.hlb.
$! 
$! A unique window title and icon name is chosen based on the command, remote
$! nodename and username.
$! 
$! Refer to SYS$HELP:DWREMTERM.HLP for further help by typing:-
$! @VUE$LIBRARY:DWREMTERM DWHELP.
$!
$!CONFIGURATION SETUPS:
$!   The following logicals can be used to alter default DWremterm behaviour.
$!   DWREMTERM_ENABLE_COPY gives the user the option of copying the latest
$!     version of DWREMTERM should there be a mismatch in versions between
$!     the local and remote node.
$!   The WINDOW POSITION and INITIAL STATE can be altered from the default
$!     by editing the resource file. Details on how to do this is given within 
$!     the following resource files. 
$!       Default DWremterm window:
$!       - decw$user_defaults:dwremterm$default_v*.dat 
$!       DWRT DWremterm window:
$!       - decw$user_defaults:dwremterm$dwrt_default_v*.dat 
$!         This is on the remote node/users account.
$!       You can also override the DWRT resource file window position by
$!       defining the logical names DWREMTERM_DWRT_X and DWREMTERM_DWRT_Y
$!       under your local user account. These values are then passed to
$!       the remote node/users account.
$!   LAT$DISCONNECT_CHARACTER defines the character that you can use to 
$!     disconnect from a remote session. Default character is Ctrl+\.
$!   SYS$SCRATCH - temporary files are written to sys$scratch. If you don't
$!     want the temporary files written to sys$login then define sys$scratch.
$!
$!TROUBLESHOOTING HINTS:
$! o Ensure that Security has been set up to enable a remote node to
$!   display output on your workstation. See installation section above
$!   for further details.
$! o The following logicals can be used to aid in troubleshooting
$!   These can be defined before starting DWremterm or during a DWremterm 
$!   Session by using the $ command.
$!     DWREMTERM_VERBOSE to display the progress of both local and remote 
$!       events. If verbose is enabled then a full sized menu window will 
$!       be displayed. This can also be enabled by prefixing the version 
$!       with an "X".
$!     DWREMTERM_VERIFY to turn on verify and verbose.
$! o NETSERVER.LOG on the remote node may also aid in troubleshooting.
$! o DWREMTERM_'pid'_*.TMP temporary file on the remote node shows the created
$!     command file.
$! o DWREMTERM_'pid'_*.LOG temporary file on on the remote node showing the 
$!     output from dwremterm_'pid'_*.TMP
$! o If you have problems creating DECterms then get the latest DECterm patch
$!     from your CSC.
$!
$!TIPS:
$! o By default you cannot shutdown VMS from an RDT window when running
$!   DECnet/OSI or DECnet/Plus because the shutdown procedure stops DECnet,
$!   which aborts the DWremterm session and the shutdown procedure itself. 
$!   You can workaround this by doing the following:-
$!   $ DEFINE NET$STARTUP_STATUS SHUTDOWN
$!   $ @SYS$SYSTEM:SHUTDOWN
$!
$!******************************************************************************
$! First set up a few symbols.
$ if p1 .nes. "RSH_DETACHED_JOB" then set symbol/scope=(nolocal)
$
$ if f$trnlnm("DWremterm_Verify") .or. f$trnlnm("Verify_All") then set verify
$
$ ! verify can be enabled by defining dwremterm_verify. This will also enable
$ ! verbose mode
$ verbose := false 
$ if f$trnlnm("DWremterm_verify")  then verbose := true
$
$ ! verbose can be enabled by defining dwremterm_verbose or dwremterm_verify
$ ! or prefixing the version with an "X"
$ if f$trnlnm("DWremterm_verbose") then verbose := true
$ if f$extract(0,1,DWremterm_version) .eqs. "X" then verbose := true
$
$! suppress printing DECterm debug messages
$ if f$trnlnm("decterm_diag") .nes. "" then deassign decterm_diag
$
$ lf[0,8]  = %xa
$ cr[0,8]  = %xd
$ nl       = cr+lf
$ esc[0,8] = %x1b
$ bon  = esc+"[1m"	!turn on  bold printing
$ boff = esc+"[0m"	!turn off bold printing
$ say = "write sys$output"
$ pid = f$getjpi("","PID")
$ version = f$extract(1,255,DWremterm_version)
$ DWUsername = f$edit(F$Getjpi("","Username"),"collapse")
$ DWNodename = F$Getsyi("Nodename")				!scsnode
$ If DWNodename .eqs. "" Then DWNodename = F$Trnlnm("Sys$node") !decnet
$ DWnodename = f$edit(DWnodename,"trim,upcase,compress")
$ If f$extract(f$length(DWnodename)-2,2,DWnodename) .eqs. "::" then -
     DWnodename = f$extract(0,f$length(DWnodename)-2,DWnodename)
$ Dnodename = DWnodename + "::"
$ loc_procedure = f$ext(0,f$loc(";",f$env("procedure")),f$env("procedure"))
$!
$! Only run the network portion of this code if directly called from
$!   DWremterm. ie. only at initial Network link creation time.
$!
$! Only goto NETWORK during initial network link creation time.
$ netobj = ""
$ if f$locate("PCX$SERVER",f$trnlnm("sys$net")) .nes. -
     f$length(f$trnlnm("sys$net")) then netobj = "PCX$SERVER"
$ if f$mode() .eqs. "NETWORK" .and. -
     p1 .eqs. "" .and. -
     f$trnlnm("sys$net") .nes. "" .and. -
     netobj .nes. "PCX$SERVER" then goto network
$!
$!******************************************************************************
$! THIS SECTION OF CODE IS USED TO GENERATE THE COMMAND WINDOW
$!
$! If we don't have P1 then we are in menu mode. Therefore create a small menu
$! window and call this procedure with p1 = MENUMODE.
$! P1 should only be blank during the first pass.
$!
$ IF p1 .eqs. "" 
$ THEN
$   ! create resource file if not found
$   resource_file := decw$user_defaults:DWremterm$default_V'version'.dat
$   IF f$search(resource_file) .eqs. "" 
$   THEN
$     ! delete any old versions of the resource file
$     IF f$search("decw$user_defaults:DWremterm$default*.dat") .nes. "" 
$     THEN 
$       set prot=w:rwed decw$user_defaults:dwremterm$default*.dat;*
$       delete decw$user_defaults:dwremterm$default*.dat;*
$     ENDIF
$     create/prot=o:rwe 'resource_file'
$     deck
DECW$TERMINAL.borderWidth:	1
DECW$TERMINAL.decWmValueMask:	1
DECW$TERMINAL.*decterm_mb*height: 1
DECW$TERMINAL.*decterm_mb*resizeHeight: false
DECW$TERMINAL.*decterm_mb*resizeWidth: false
DECW$TERMINAL.title: 	DWremterm 
DECW$TERMINAL.iconName: DWremterm
DECW$TERMINAL.main.terminal.rows:       4
DECW$TERMINAL.main.terminal.columns:    62
DECW$TERMINAL.main.terminal.bitPlanes:	4
DECW$TERMINAL.main.terminal.height:	86
DECW$TERMINAL.main.terminal.width:	285
DECW$TERMINAL.main.terminal.displayHeight:	6
DECW$TERMINAL.main.terminal.displayHeightInc:	13
DECW$TERMINAL.main.terminal.transcriptSize:	20
DECW$TERMINAL.main.terminal.displayWidth:	285
DECW$TERMINAL.main.terminal.fontUsed:	-DEC-Terminal-Medium-R-Narrow--14-140-75-75-C-6-ISO8859-1
DECW$TERMINAL.main.terminal.condensedFont:	on
DECW$TERMINAL.main.terminal.adjustFontSizes:	off
DECW$TERMINAL.main.terminal.fontSetSelection:   1
DECW$TERMINAL.main.terminal.defaultTitle: 	DWremterm 
DECW$TERMINAL.main.terminal.defaultIconName:	DWremterm
DECW$TERMINAL.main.terminal.defaultTitleName:	DWremterm
DECW$TERMINAL.main.terminal.borderColor:	#000000000000
DECW$TERMINAL.main.terminal.autoResizeTerminal:	on
#-------------------------------------------------------------------------------
# modify the x value to place the window at a different location.
# suggest setting the x location to 10.
DECW$TERMINAL.x:        10
#
#-------------------------------------------------------------------------------
# modify the y value to place the window at a different location.
# suggest setting the y location to 115 for 75 DPI terminals.
# suggest setting the y location to 122 for 100 DPI terminals (VT1200).
DECW$TERMINAL.y:        125	
#
#-------------------------------------------------------------------------------
#
# change initialState paramater to startup state:
#   Window on startup: 1
#   Icon on startup: 3
DECW$TERMINAL.initialState: 1
#
#-------------------------------------------------------------------------------
$     eod
$   ENDIF
$ ENDIF
$
$! If P1 contains DWREMTERM then we are to create a second DWRT 
$! DWremterm window. Therefore create a small menu window and call this 
$! procedure with p1 = MENUMODE. P1 should only be blank during the first pass.
$
$ IF p1 .eqs. "DWREMTERM" 
$ THEN
$   ! create DWRT resource file if not found
$   resource_file := decw$user_defaults:DWremterm$dwrt_default_V'version'.dat
$   IF f$search(resource_file) .eqs. "" 
$   THEN
$     ! delete any old versions of the resource file
$     if f$search("decw$user_defaults:DWremterm$dwrt_default*.dat") .nes. "" -
         then delete decw$user_defaults:dwremterm$dwrt_default*.dat;*
$     create/prot=o:rwe 'resource_file'
$     deck
DECW$TERMINAL.borderWidth:	1
DECW$TERMINAL.decWmValueMask:	1
DECW$TERMINAL.title: DWremterm_DWRT 
DECW$TERMINAL.iconName: DWremterm_DWRT
DECW$TERMINAL.*decterm_mb*height: 1
DECW$TERMINAL.*decterm_mb*resizeHeight: false
DECW$TERMINAL.*decterm_mb*resizeWidth: false
DECW$TERMINAL.main.terminal.rows:       4
DECW$TERMINAL.main.terminal.columns:    62
DECW$TERMINAL.main.terminal.bitPlanes:	4
DECW$TERMINAL.main.terminal.height:	86
DECW$TERMINAL.main.terminal.width:	285
DECW$TERMINAL.main.terminal.displayHeight:	6
DECW$TERMINAL.main.terminal.displayHeightInc:	13
DECW$TERMINAL.main.terminal.transcriptSize:	20
DECW$TERMINAL.main.terminal.displayWidth:	285
DECW$TERMINAL.main.terminal.fontUsed:	-DEC-Terminal-Medium-R-Narrow--14-140-75-75-C-6-ISO8859-1
DECW$TERMINAL.main.terminal.autoResizeTerminal:	on
DECW$TERMINAL.main.terminal.condensedFont:	on
DECW$TERMINAL.main.terminal.adjustFontSizes:	off
DECW$TERMINAL.main.terminal.fontSetSelection:   1
DECW$TERMINAL.main.terminal.defaultTitle: 	DWremterm_DWRT
DECW$TERMINAL.main.terminal.defaultIconName:	DWremterm_DWRT
DECW$TERMINAL.main.terminal.defaultTitleName:	DWremterm_DWRT
DECW$TERMINAL.main.terminal.borderColor:	#000000000000
#-------------------------------------------------------------------------------
#
# modify the x value to place the window at a different location.
# suggest setting the x location to 10.
DECW$TERMINAL.x:        10
#
#-------------------------------------------------------------------------------
# modify the y value to place the window at a different location.
# suggest setting the y location to 265 for 75 DPI terminals.
# suggest setting the y location to 275 for 100 DPI terminals (VT1200).
DECW$TERMINAL.y:        265	
#
#-------------------------------------------------------------------------------
#
# change initialState paramater to startup state:
#   Window on startup: 1
#   Icon on startup: 3
DECW$TERMINAL.initialState: 1
#
#-------------------------------------------------------------------------------
$     eod
$   ENDIF
$ ENDIF
$
$! If an RSH job with password is supplied then DWremterm calls itself as a
$! spawned job to create a DCL mailbox, start the detached job and
$! transfer the password to the detached job via the DCL mailbox.
$! This is done this way to prevent the mailbox creation from failing when
$! a mailbox already exists and is only a problem because of the the
$! crude method used to create the mailbox.
$ IF p1 .eqs. "RSH_DETACHED_JOB" 
$ THEN 
$   gosub create_mbx
$   gosub rsh_detached_job
$   exit
$ ENDIF
$ 
$! dwremterm calls itself using P1 "CREATE_MBX" to create Dwremterm mailbox
$! which is used to pass the user's password to a detached process.
$ if p1 .eqs. "CREATE_MBX" then goto create_mbx_noinit
$!
$ IF p1 .eqs. "" .or. p1 .eqs. "DWREMTERM" 
$ THEN
$   ! if we are in verbose mode then bring up full sized window by 
$   ! over-riding the window size specified in the resource file.
$   window_size = ""
$   if verbose then window_size = ",col=80,row=24"
$   ! allow the remote DWREMTERM DWRT user to override the resource file's
$   ! x and y positions.
$   IF p1 .eqs. "DWREMTERM"
$   THEN
$     window_position = p2
$     window_state = p3
$   ELSE
$     window_position = ""
$     window_state = ""
$   ENDIF
$! resolve decw$user_defaults logical name to ensure detached job finds 
$! resource file
$   resfile = f$search(resource_file)
$! make sure we don't fail if resource file not found
$   if resfile .eqs. "" then -
       resfile := decw$user_defaults:DWremterm$default_V'version'.dat
$   execute_command = -
      "cre/term/wait/nobr/res='f$trnl(""resfile"")'/win=" +-
      "(tit=""''DWnodename'""""''dwusername'"""" DWremterm V''version'""," +-
      "ico=""DWremterm ''DWnodename'""""''dwusername'""""""" +-
      "''window_size'''window_position'''window_state') "+-
      "@''f$environment("procedure")' menu_mode"
$   execute_p1_command = ""
$   display_required = 1
$   password_required = 0
$   gosub create_detach 
$   if verbose then say Dnodename," DWREMTERM Detach Command is:- ",-
       execute_command
$   if verbose then say Dnodename," DWREMTERM Execute Command is:- ",-
       execute_p1_command
$   set noon
$   i = 0
$DWrt_loop:
$   if i .le. 99 
$   then
$     dprcnam = "DWrt-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
         /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto DWrt_loop
$   else
$     Say "Error: Unable to create detached process"
$   endif
$   set on
$   restore_verify = f$verify(saved_verify)
$   exit 1
$ ENDIF
$!
$!******************************************************************************
$! THIS SECTION OF CODE DETERMINES WHAT MODE WE ARE RUNNING IN
$!
$ IF p1 .eqs. "MENU_MODE" 
$ THEN  
$   !  we are in menu mode therefore the input commands will come from a small
$   !  window menu
$   menu_mode :== true
$   p1 = ""   !reset p1
$ ELSE
$   ! we are in command mode, therefore the input was supplied on entry to this
$   ! procedure.
$   menu_mode :== false 
$   goto check_for_keywords
$ ENDIF
$!
$!******************************************************************************
$! THIS SECTION OF CODE IS USED TO PROMPT AND VALIDATE TERMINAL INPUT COMMANDS
$!
$ !first set up the terminal environment
$ if .not. verbose then set nocontrol=y
$ set control=t
$ set term/broadcast
$ set broadcast=(none,dcl,mail)
$
$ ! let's get a process name, window title name and icon name
$
$ say "For help, type HELP at the DWremterm> prompt"
$ say "Please report problems to Geoff Kingsmill, DEC."
$ say ""
$menu_loop:
$ ! check verbose each pass. This allows you to turn on verbose from a
$ ! "DWremterm> $ define dwremterm_verbose 1"  command.
$
$ ! set window size large if verify or verbose enabled, otherwise window size
$ ! should be small.
$ IF f$trnlnm("dwremterm_verbose") .or. f$environment("verify_procedure")
$ THEN 
$   verbose := true
$   if f$getdvi("tt:","tt_page")   .ne. 24 then set term/page=24
$   wait 00:00:01	!fix up timing problem with X-terminals
$   if f$getdvi("tt:","devbufsiz") .ne. 80 then set term/width=80
$ ENDIF
$ IF .not. f$trnlnm("dwremterm_verbose") .and. -
     .not. f$environment("verify_procedure")
$ THEN 
$   verbose := false
$   if f$getdvi("tt:","tt_page")   .ne.  4 then set term/page=4
$   wait 00:00:01	!fix up timing problem with X-terminals
$   if f$getdvi("tt:","devbufsiz") .ne. 62 then set term/width=62
$ ENDIF
$
$ ! get command
$ read/end_of_file=menu_loop/error=menu_loop/prompt="DWremterm> " -
       sys$command params_all
$ if f$edit(params_all,"collapse,trim") .eqs. "?" then params_all := HELP
$ if params_all .eqs. "" then goto menu_loop
$
$check_for_keywords:
$ copy_pass == 0	!reset file copy attempts at the start of each command
$ IF .not. menu_mode
$ THEN
$   ! we are in command mode
$   pa == p1
$   pb == p2
$   pc == p3
$   pd == p4
$   pe == p5
$   pf == p6
$   pg == p7
$   ph == p8
$   ! the odd ""params_all syntax is used to overcome a call failure when 
$   !   special characters such as ?,^ are entered
$   call command_parser ""pa pb pc pd pe pf pg ph
$ ELSE
$   ! we are in menu mode
$   ! the odd ""params_all syntax is used to overcome a call failure when 
$   !   special characters such as ?,^ are entered
$   call command_parser ""'params_all
$ ENDIF
$
$! validate command
$ IF verb-"LAT"-"RDW"-"RDT"-"RFV"-"DWRT"-"DWHELP"-"HELP"-"EXIT"-"DECNET"- -
     "TELNET"-"RSH"-"LDT"-"$" .nes. ""
$ THEN
$   SAY "Invalid Command: Valid Commands are - "
$   SAY "LAT,DECNET,TELNET,LDT,RDT,RFV,RDW,RSH,DWRT,HELP,DWHELP and $"
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$ IF verbose
$ THEN
$   say Dnodename," Decoded Verb is: ",verb
$   say Dnodename," Decoded Nodename is: ",sec_nodename
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "EXIT"
$ THEN
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "HELP"
$ THEN
$   say "Valid Commmands are: "
$   say "LAT,DECNET,TELNET,LDT,RDT,RFV,RDW,RSH,DWRT,$, HELP and DWHELP."
$   say "Enter Command only for Brief Help or DWHELP for Full Help."
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "DWHELP"
$ THEN
$   execute_command = "create/term/wait/nobroadcast/window=" +-
          "(title=""DWremterm Help"",icon=""DWHelp"", rows=24,column=80)" +-
          " @'f$environment(""procedure"")' execute_cmd"
$   execute_p1_command = -
          "help/nouser/library=sys$help:dwremterm.hlb DWremterm"
$   display_required = 1
$   password_required = 0
$   gosub create_detach 
$   if verbose then say Dnodename," DWHELP Detach Command is:- ",-
       execute_command
$   if verbose then say Dnodename," DWHELP Execute Command is:- ",-
       execute_p1_command
$   set noon
$   i = 0
$DWhelp_loop:
$   if i .le. 99 
$   then
$     dprcnam = "DWhelp-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
         /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto DWhelp_loop
$   else
$     Say "Error: Unable to create detached process"
$   endif
$   set on
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "LAT" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command creates a LAT window to a remote system."
$     say "LAT nodename"
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ELSE
$     nodename == f$extract(0,f$locate("""",nodename),nodename)
$     bnodename == f$extract(0,f$locate("""",bnodename),bnodename)
$   ENDIF
$
$   IF bnodename .eqs. dwnodename
$   THEN
$     IF f$search("sys$system:ltpad_v2.exe") .eqs. ""
$     THEN 
$       say "You cannot use LATmaster to connect to your own node." 
$       say "Use DECterm for local windows."
$       if MENU_MODE then goto menu_loop
$       goto local_exit
$     ENDIF
$   ENDIF
$
$   ! tell user to use the DECNET or TELNET command if LAT is not running
$   IF .not. f$getdvi("LTA0:","exists")
$   THEN
$     Say "The LAT software has not been started on this node."
$     Say "Use the DWremterm DECNET or TELNET command."
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ENDIF
$
$   ! tell user to use the DECNET or TELNET command if LATMASTER not installed.
$   define/user/nolog sys$error nl:
$   define/user/nolog sys$output nl:
$   set host/lat/noautoprompt
$   status = $status
$   deassign/user sys$error
$   deassign/user sys$output
$   IF status .eqs. "%X00038240"
$   THEN
$     Say "LATmaster is not installed on this node."
$     Say "Use the DWremterm DECnet or TELNET command."
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ENDIF
$   
$   !window title name
$   tname == f$edit(nodename-"::"-":","lowercase") 
$   !window icon name
$   iname == f$extract(0,f$locate(".",tname),tname) 
$   disc_char = ""
$   break_char = ""
$   if f$trnlnm("LAT$DISCONNECT_CHARACTER") .nes. "" then -
      disc_char = "/disc="+f$extr(0,1,f$trnlnm("LAT$DISCONNECT_CHARACTER"))
$   if f$trnlnm("LAT$BREAK_CHARACTER") .nes. "" then -
      break_char = "/break="+f$extr(0,1,f$trnlnm("LAT$BREAK_CHARACTER"))
$   execute_command = "create/term/wait/nobroadcast" +-
          "/window=(title=""" + tname + """,icon=""" + iname + """)" +-
          " @'f$environment(""procedure"")' execute_cmd"
$   execute_p1_command = "set host/lat" + disc_char +-
           break_char + " " + f$edit(bnodename,"lowercase")
$   display_required = 1
$   password_required = 0
$   gosub create_detach 
$   if verbose then say Dnodename," LAT Detach Command is:- ",execute_command
$   if verbose then say Dnodename," LAT Execute Command is:- ",-
       execute_p1_command
$   set noon
$   i = 0
$DWlat_loop:
$   if i .le. 99 
$   then
$     dprcnam = "DWlat-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
       /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto DWlat_loop
$   else
$     Say "Error: Unable to create detached process"
$   endif
$   set on
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "DECNET" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command creates a DECNET window to a remote system."
$     say "DECNET nodename"
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ELSE
$     nodename == f$extract(0,f$locate("""",nodename),nodename)
$     bnodename == f$extract(0,f$locate("""",bnodename),bnodename)
$   ENDIF
$
$   ! tell user to use the LAT or TELNET command if DECNET is not running
$   IF .not. f$getdvi("RTA0:","exists")
$   THEN
$     Say "The DECNET software has not been started on this node."
$     Say "Use the DWremterm LAT or TELNET command."
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ENDIF
$!
$   !window title name
$   tname == f$edit(nodename-"::"-":","lowercase") 
$   !window icon name
$   iname == f$extract(0,f$locate(".",tname),tname) 
$   execute_command = "create/term/wait/nobroadcast" +-
          "/window=(title=""" + tname + """,icon=""" + iname + """)" +-
          " @'f$environment(""procedure"")' execute_cmd"
$   execute_p1_command = "set host " + f$edit(bnodename,"lowercase")
$   display_required = 1
$   password_required = 0
$   gosub create_detach 
$   if verbose then say Dnodename," DECNET Detach Command is:- ",-
       execute_command
$   if verbose then say Dnodename," DECNET Execute Command is:- ",-
       execute_p1_command
$   set noon
$   i = 0
$DWdnet_loop:
$   if i .le. 99 
$   then
$     dprcnam = "DWdnet-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
       /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto DWdnet_loop
$   else
$     Say "Error: Unable to create detached process"
$   endif
$   set on
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "TELNET" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command creates a TELNET window to a remote system."
$     say "TELNET nodename"
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ELSE
$     nodename  == f$extract(0,f$locate("""",nodename),nodename)
$     bnodename == f$extract(0,f$locate("""",bnodename),bnodename)
$   ENDIF
$
$   ! tell user to use the LAT or DECNET command if TCPIP is not running
$   IF .not. f$getdvi("NTA0:","exists") .and. -   !tcpware
       .not. f$getdvi("TNA0:","exists") .and. -   !ucx
       .not. f$getdvi("NTY0:","exists")           !multinet
$   THEN
$     Say "The TCPIP software has not been started on this node."
$     Say "Use the DWremterm LAT or DECNET command."
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ENDIF
$
$   !window title name
$   tname == f$edit(nodename-"::"-":","lowercase") 
$   !window icon name
$   iname == f$extract(0,f$locate(".",tname),tname) 
$   execute_command = "create/term/wait/nobroadcast" +-
          "/window=(title=""" + tname + """,icon=""" + iname + """)" +-
          " @'f$environment(""procedure"")' execute_cmd"
$   execute_p1_command = "telnet " + f$edit(bnodename,"lowercase") 
$   display_required = 1
$   password_required = 0
$   gosub create_detach 
$   if verbose then say Dnodename," TELNET Detach Command is:- ",-
       execute_command
$   if verbose then say Dnodename," TELNET Execute Command is:- ",-
       execute_p1_command
$   set noon
$   i = 0
$DWtnet_loop:
$   if i .le. 99 
$   then
$     dprcnam = "DWtnet-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
       /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto DWtnet_loop
$   else
$     Say "Error: Unable to create detached process"
$   endif
$   set on
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "LDT" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command creates a local DECTERM window and then does an rlogin"
$     say "LDT nodename [init_state] [title]"
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ELSE
$     nodename == f$extract(0,f$locate("""",nodename),nodename)
$     bnodename == f$extract(0,f$locate("""",bnodename),bnodename)
$   ENDIF
$
$   ! tell user to use the LAT or DECNET command if TCPIP is not running
$   IF .not. f$getdvi("NTA0:","exists") .and. -   !tcpware
       .not. f$getdvi("TNA0:","exists") .and. -   !ucx
       .not. f$getdvi("NTY0:","exists")           !multinet
$   THEN
$     Say "The TCPIP software has not been started on this node."
$     Say "Use the DWremterm LAT or DECNET command."
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ENDIF
$
$   ! break out remote hostname and username for the rlogin command
$   ! from this set the window title and icon name 
$   !window title name
$   tname == f$edit(nodename-"::"-":","lowercase") 
$   !window icon name
$   iname == f$extract(0,f$locate(".",tname),tname) 
$   if f$length(p2) .ne. f$locate("""",p2)
$   then
$     lnode = f$edit(f$extract(0,f$loc("""",p2),p2),"lowercase") - ":" - ":"
$     if lnode .eqs. "0" then lnode = f$edit(nodename-"::"-":","lowercase")
$     luser = f$extract(f$loc("""",p2),255,p2) - "::" -":"
$     luser = f$extract(1,f$loc(" ",luser)-1,luser) - """" - """"
$     if luser .eqs. "" 
$     then
$       execute_p1_command = "rlogin ''lnode'"
$     else
$       !window title name
$       tname == lnode - "::" - ":" + " " +  luser
$       !window icon name
$       iname == f$extract(0,f$locate(".",tname),tname) + " " + luser 
$       execute_p1_command = "rlogin/user=''luser' ''lnode'"
$     endif
$   else
$     tname == f$edit(nodename-"::"-":","lowercase") 
$     ename = p2
$     if p2 .eqs. "" then ename = tname
$     if p2-"::"-":" .eqs. "0" then ename = tname
$     execute_p1_command = "rlogin " + ename - "::" -":"
$   endif
$!
$   !should window start as an icon?
$   init_state = "window"
$   if p3 .nes. "" 
$   then
$     if f$extract(0,1,f$edit(p3,"upcase")) .eqs. "I" then init_state = "icon"
$   endif 
$!
$   !has a window title and icon name been specified
$   if p4 .nes. "" 
$   then 
$     tname = p4
$     iname = p4
$   endif
$!
$   execute_command = "create/term/wait/nobroadcast" +-
          "/window=(title=""" + tname + """,icon=""" + iname + """" +-
                   ",initial_state=" + init_state + ")" +-
          " @'f$environment(""procedure"")' execute_cmd"
$   display_required = 1
$   password_required = 0
$   gosub create_detach 
$   if verbose then say Dnodename," LDT Detach Command is:- ",-
       execute_command
$   if verbose then say Dnodename," LDT Execute Command is:- ",-
       execute_p1_command
$   set noon
$   i = 0
$LDTnet_loop:
$   if i .le. 99 
$   then
$     dprcnam = "LDTnet-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
       /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto LDTnet_loop
$   else
$     Say "Error: Unable to create detached process"
$   endif
$   set on
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF f$extract(0,1,verb) .eqs. "$" 
$ THEN
$   IF $_command .eqs. ""
$   THEN
$     say "This command will execute a local DCL command"
$     say "$ dcl_command"
$   ELSE
$     if verbose then say Dnodename," $ Command is:- ",$_command
$     '$_command'
$   ENDIF
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "RSH" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command issues an RSH command on a remote system."
$     say "RSH nodename command"
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ELSE
$     nodename == f$extract(0,f$locate("""",nodename),nodename)
$   ENDIF
$
$   IF rsh_command .eqs. "" 
$   THEN
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   ENDIF
$
$   execute_command = rsh_command
$   execute_p1_command = ""
$   display_required = 0
$   password_required = passwd_specified
$   if passwd_specified
$   then
$     ! create dwremterm mailbox so we can pass the password to the 
$     !  detached process
$     password_required = 1
$     ! close any existing dwremterm mailboxes
$     if f$trnlnm("dwremterm_mbx") .nes. "" then close dwremterm_mbx
$     ! create mailbox. This is done as a spawned job to prevent
$     !   any existing mailbox's causing problems with this crude
$     !   method of creating a mailbox. 
$     spawn/nolog/wait/process=crembx_'f$getjpi("","pid")' -
             @'f$env("procedure")' RSH_DETACHED_JOB
$   else
$     password_required = 0
$     gosub rsh_detached_job
$   endif
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$RSH_DETACHED_JOB:
$ IF verb .eqs. "RSH" 
$ THEN
$   gosub create_detach 
$   if verbose then say Dnodename," RSH Command is: ",execute_command
$   set noon
$   i = 0
$DWrsh_loop:
$   if i .le. 99 
$   then
$     dprcnam = "DWrsh-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$     i = i + 1
$     define/user sys$error nl:
$     define/user sys$output nl:
$     Run/detached/authorize sys$system:loginout.exe -
         /input='outfile'/output='outlog'/process_name="''dprcnam'"
$     if .not. $status then goto DWrsh_loop
$   else
$     set on
$     Say "Error: Unable to create detached process"
$     if MENU_MODE then goto menu_loop
$     goto local_exit
$   endif
$   set on
$   if password_required 
$   then 
$     ! pass the password to the detached process via a Dwremterm Mailbox
$     count = 0
$read_mbx_loop:
$     wait 0:0:1
$     count = count + 1
$     if count .eq. 20 
$     then
$       if verbose then say Dnodename, -
           " ERROR: Detached process did not write to ''mbxdev'
$     else
$       if verbose .and. count .ne. 1 then say Dnodename," ...waiting "+-
           "for Detached process to write to DWremterm Mailbox ''mbxdev'"
$       read/time=0/error=read_mbx_loop dwremterm_mbx pwdreq
$       if verbose then say Dnodename," Read """,pwdreq, -
           """ from DWremterm Mailbox "
$       write dwremterm_mbx passwd
$       if verbose then say Dnodename, -
           " Wrote password to DWremterm Mailbox ''mbxdev'"
$     endif
$     if verbose then say Dnodename," Closing DWremterm Mailbox "
$     close dwremterm_mbx
$     wait 0:0:1
$     if verbose .and. f$getdvi(mbxdev,"exists") then say Dnodename, -
         " ERROR: DWremterm Mailbox ''mbxdev' was not deleted"
$   endif
$   RETURN     ! end rsh_detached_job subroutine
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "RDT"
$ THEN
$   IF nodename .eqs. "" 
$   THEN 
$     say "This command creates a Remote Decterm Window:"
$     say "RDT nodename [init_state] [title] [dcl_command]"
$     say "             [display] [transport] [X:Y]"
$   ELSE
$     call send DECTERM nodename pc pd pe pf pg ph
$   ENDIF
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "RFV"
$ THEN
$   IF nodename .eqs. "" 
$   THEN 
$     say "This command runs a Remote Fileview Window:"
$     say "RFV nodename [init_state] [display] [transport]"
$   ELSE
$     call send FILEVIEW nodename pc pd pe
$   ENDIF
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "RDW"
$ THEN
$   IF nodename .eqs. "" 
$   THEN 
$     say "This command will run a Remote Decwindows Application:"
$     say "RDW nodename command [display] [transport]"
$   ELSE
$     call send pc nodename pd pe 
$   ENDIF
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "DWRT"
$ THEN
$   IF nodename .eqs. "" 
$   THEN 
$     say "This command will start a second DWremterm Window:"
$     say "DWRT nodename [init_state] [display] [transport]"
$   ELSE
$     call send DWREMTERM nodename pc pd pe
$   ENDIF
$   if MENU_MODE then goto menu_loop
$   goto local_exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ goto menu_loop	!catchall, should never get here
$
$local_exit:
$! ensure global symbols containing passwords are deleted
$ if f$type(pb) .nes. "" then delete/sym/global pb
$ if f$type(passwd) .nes. "" then delete/sym/global passwd
$ if f$type(nodename) .nes. "" then delete/sym/global nodename
$ if f$type(param_all) .nes. "" then delete/sym/global param_all
$ if f$type(bnodename) .nes. "" then delete/sym/global bnodename
$ exit !'f$verify(saved_verify)'
$!
$!******************************************************************************
$! THIS SUBROUTINE SENDS THE COMMAND TO THE REMOTE NODE
$!
$SEND: SUBROUTINE 
$ set noon
$ Write_Destination = "write link_destination"
$ Read_Destination  = "read/error=open_failure  link_destination"
$
$ init_state = ""
$ title      = ""
$ dcl_command= ""
$ display    = ""
$ x_y_loc    = ""
$!
$! COMMON INPUTS
$!
$ IF verb .eqs. "RDW"
$ THEN 
$   Command     = 'P1	       !(Required)
$   if command .eqs. ""
$   then
$     say "ERROR: No command to execute, the RDW command syntax is"
$     say "RDW nodename command [display] [transport]"
$     exit
$   endif
$ ELSE
$   Command    := 'P1	       !(Required)
$ ENDIF
$ Nodename      = 'P2	       !(Required)
$! if the nodename is of the format nodename::user then look in
$! the proxy database to find the username
$ if f$length(nodename-"::") .ne. f$length(nodename-"::"-"::")
$ then
$   pnodename = f$extract(0,f$length(nodename)-2,nodename)
$   if f$extract(f$length(pnodename)-1,1,pnodename) .eqs. ":" then -
       pnodename = f$extract(0,f$length(pnodename)-1,pnodename)
$   define/process sys$error nl:
$   define/process sys$output nl:
$   proxy 'pnodename'/logical=pnode
$   status = $status
$   if f$trnlnm("sys$error","lnm$process_table") .nes. "" then -
       deass sys$error
$   if f$trnlnm("sys$output","lnm$process_table") .nes. "" then -
       deass sys$output
$   if status
$   then
$     nodename = f$trnlnm("pnode")
$     deassign pnode
$     if verbose then -
         say Dnodename," New nodename from Proxy Database:- ",nodename
$   else
$     if status .eqs. "%X00038090" 
$     then 
$       say "ERROR: proxy utility not found"
$       exit
$     endif
$     if status .eqs. "%X100388B2" 
$     then 
$       say "ERROR: proxy utility not found"
$       exit
$     endif
$     if f$search(f$trnlnm("proxy$database")) .eqs. ""
$     then
$       say "ERROR: proxy database does not exist"
$       exit
$     endif
$     say "ERROR: ",pnodename," does not exist in your proxy database" 
$   endif
$ endif
$!
$! REMOTE DECTERM INPUTS
$!
$ IF verb .eqs. "RDT"
$ THEN 
$   Init_State = 'P3	       !(Optional, Default = window) icon/window
$   title      = 'P4	       !(Optional, Default ='nodename' 'username')
$   DCL_command= 'P5	       !(Optional)
$   Display    = 'P6	       !(Optional, Default = 0::0.0)
$   Transport  = 'P7	       !(Optional, Default = current)
$   x_y_loc    = 'P8	       !(Optional)
$ ENDIF
$!
$! REMOTE FILEVIEW INPUTS
$!
$ IF verb .eqs. "RFV"
$ THEN 
$   Init_State = 'P3	       !(Optional, Default = window) icon/window
$   Display    = 'P4 	       !(Optional, Default = 0::0.0)
$   Transport  = 'P5	       !(Optional, Default = current)
$ ENDIF
$!
$! REMOTE DECWINDOWS INPUTS
$!
$ IF verb .eqs. "RDW" 
$ THEN 
$   Display    = 'P3	       !(Optional, Default = 0::0.0)
$   Transport  = 'P4	       !(Optional, Default = current)
$ ENDIF
$!
$! DWREMTERM INPUTS
$!
$ IF verb .eqs. "DWRT" 
$ THEN 
$   Init_State = 'P3	       !(Optional, Default = window) icon/window
$   Display    = 'P4	       !(Optional, Default = 0::0.0)
$   Transport  = 'P5	       !(Optional, Default = current)
$ ENDIF
$
$ IF verbose 
$ THEN
$   Say Dnodename," Decoded commands are: "
$   Say "  Verb: ",verb
$   Say "  Command: ",command
$   Say "  Nodename: ",nodename
$   Say "  Display: ",display
$   Say "  Initial State: ",init_state
$   Say "  Transport: ",transport
$   Say "  DCL Command: ",dcl_command
$   Say "  Title: ",title
$   Say "  Position: ",x_y_loc
$ ENDIF
$
$ if verbose then say dnodename,-
     " Testing availability of remote node: ",sec_nodename
$ define/user/nolog sys$error nl:
$ define/user/nolog sys$output nl:
$ ! the file lexical is used as it is the only way I could get a definitive
$ ! status code.
$ test = f$file(nodename,"eof")
$ status = $status
$ deassign/user sys$error
$ deassign/user sys$output
$
$ IF status .eqs. "%X0000028C" .or. status .eqs. "%X000185F4"
$ THEN
$   Say "Node Unknown..."
$   goto send_exit
$ ENDIF
$
$ IF status .eqs. "%X0000209C"
$ THEN
$   Say "Node Login Information Invalid..."
$   goto send_exit
$ ENDIF
$
$ IF status .eqs. "%X00002094"
$ THEN
$   Say "Node Unreachable..."
$   goto send_exit
$ ENDIF
$
$ if verbose then say dnodename," Remote node ",sec_nodename," is reachable"
$ 
$! see if display was specified as part of the RDW command
$! eg. dwremterm rdw mvax "mcr decw$clock -display maxps1:0"
$ if verb .eqs. "RDW" .and. f$length(command) .ne. f$length(command - " -d") 
$ then
$   display = f$edit(f$extract(f$loc(" -d",command),255,command),"trim")
$   display = f$edit(f$extract(f$loc(" ",display),255,display),"trim")
$   display = f$edit(f$extract(0,f$loc(" ",display),display),"trim")
$ endif
$
$ !set decw$display_* symbols for current window
$ if f$type(decw$display_node) .nes. "" then -
     delete/symbol/global decw$display_node
$ define/user/nolog sys$error  nl:
$ define/user/nolog sys$output nl:
$ show display/symbol		! undocumented command
$
$ ! if a DECwindow display is not currently defined or specified then exit
$ IF display .eqs. "" .and. f$type(decw$display_node) .eqs. ""
$ THEN
$   Say "%DECW-E-CANT_OPEN_DISPL, Display not supplied"
$   goto send_exit
$ ENDIF
$
$ IF verbose 
$ THEN 
$   IF f$type(decw$display_node)
$   THEN 
$      say Dnodename,-
         " Current Display is:-",nl,-
         "  Display Node is: ",decw$display_node,nl,-
         "  Display Server is: ",decw$display_server,nl,-
         "  Display Screen is: ",decw$display_screen,nl,-
         "  Display Transport is: ",decw$display_transport
$   ELSE
$     Say Dnodename,-
        " No DECwindows display currently active"
$   ENDIF
$ ENDIF
$
$ shutdown_pending = 0
$ on warning then goto open_failure	!exit handler
$Open_Server:
$!
$! Open network connection
$!
$ IF f$trnlnm("link_destination",,,,,"terminal") 
$ THEN 
$   if verbose then say Dnodename," Closing the DWremterm link"
$   close link_destination
$ ENDIF
$ if verbose then say Dnodename,-
     " Opening DWremterm task on node: ",sec_nodename
$ open/read/write/error=open_server_failed -
       link_destination -
       'nodename'"TASK=''f$parse(f$environment(""procedure""),,,""NAME"")'"
$ if verbose then say Dnodename,-
     " DWremterm Task sucessfully opened on node: ",sec_nodename
$!
$!
$!------------------------------------------------------------------------------
$! THIS SECTION OF CODE PASSES THE PARAMETERS BETWEEN THE LOCAL AND REMOTE NODES
$!
$flush_output:
$!
$! Get command from remote node
$!
$ Read_Destination rem_record
$
$ IF rem_record .eqs. "%VERBOSE"
$ THEN
$   if verbose then say Dnodename,-
       " Reqesting remote node to display verbose messages"
$   Write_Destination verbose
$   Goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%PROCEDURE"
$ THEN
$   if verbose then say Dnodename," Requesting Remote Procedure name"
$   Read_Destination rem_procedure
$   goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%VERSION"
$ THEN
$   if verbose then say Dnodename," Requesting Remote DWremterm Version"
$   Read_Destination rem_version
$   if verbose then say Dnodename," Local  version is: V",version
$   if verbose then Read_Destination rem_version_message
$   if verbose then Say rem_version_message
$   IF copy_pass .eq. 0 
$   THEN
$     IF version .nes. rem_version
$     THEN
$       ! Version Mismatch. Go and ask if a new version should be copied
$       call copy_file version_mismatch 'nodename' 'rem_version' 'rem_procedure'
$       IF f$trnlnm("dwremterm_enable_copy")
$       THEN
$         goto open_server
$       ELSE
$         goto flush_output
$       ENDIF
$     ENDIF
$   ENDIF
$   goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%COMMAND" 
$ THEN 
$   ! Send Command
$   if verbose then say Dnodename," Sending command"
$   Write_Destination Command
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%DCL_COMMAND" 
$ THEN 
$   ! Send DCL command if supplied
$   if verbose then say Dnodename," Sending DCL comamnd"
$   Write_Destination DCL_command
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%USERNAME"
$ THEN
$   if verbose then say Dnodename," Requesting Remote Username"
$   Read_Destination rem_username
$   goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%NODENAME"
$ THEN
$   if verbose then say Dnodename," Requesting Remote Nodename"
$   Read_Destination rem_nodename
$   goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%PID"
$ THEN
$   if verbose then say Dnodename," Requesting Remote Process ID"
$   Read_Destination rem_pid
$   goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%PRCNAM"
$ THEN
$   if verbose then say Dnodename," Requesting Remote Process Name"
$   Read_Destination rem_prcnam
$   goto flush_output
$ ENDIF
$
$ IF rem_record .eqs. "%DISPLAY_NODE" 
$ THEN 
$   ! DEFINE DISPLAY NODE - decw$display_node
$   !   obtained from show display/symbol
$   IF display .eqs. ""
$   THEN
$     ! if display not defined then assume decnet node name
$     if f$type(decw$display_node) .eqs. "" then -
         decw$display_node = DWnodename
$     if decw$display_node .eqs. "0" then -
         decw$display_node = DWnodename
$     ! if transport type is LAT then strip off LTA port
$     decw$display_node := -
        'f$extract(0,f$locate("=",decw$display_node),decw$display_node)'
$   ELSE
$     display = f$edit(display,"compress,trim")
$!
$! The user supplied display is in the format of nodename::server.screen
$! We must now break out each of these fields. As the nodename syntax vary's 
$! from transport to transport, we decode from right to left (screen to
$! server to nodename) for ease of decoding different transport specifications.
$!
$     IF f$locate(":",display) .eq. f$length(display)
$     THEN
$       ! the user has supplied the nodename only
$       decw$display_node = display
$       display = ""
$     ELSE
$       ! the user has supplied the nodename and the server.screen
$       display_counter = f$length(display)
$display_server_loop:
$       ! extract server.screen
$       IF f$extract(display_counter-1,1,display) .nes. ":" 
$       THEN
$         display_counter = display_counter-1
$         goto display_server_loop
$       ELSE
$         node = f$extract(0,display_counter,display)
$         display = f$extract(display_counter,255,display)
$       ENDIF
$       ! if transport parameter not defined then try and get it from
$       ! then node specifier eg: node:0, node::0
$       IF transport .eqs. ""
$       THEN
$          if f$extract(display_counter-2,1,node) .eqs. ":" then -
              transport = "TCPIP"
$          if f$extract(display_counter-2,2,node) .eqs. "::" then -
              transport = "DECNET"
$       ENDIF
$display_node_loop:
$       ! extract nodename
$       IF f$extract(display_counter-1,1,node) .eqs. ":" 
$       THEN
$         display_counter = display_counter-1
$         goto display_node_loop
$       ELSE
$         decw$display_node = f$extract(0,display_counter,node)
$       ENDIF
$     ENDIF
$   ENDIF
$   if verbose then say Dnodename," Sending Display node"
$   Write_Destination decw$display_node
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%DISPLAY_SERVER" 
$ THEN 
$   ! DEFINE DISPLAY SERVER - decw$display_server 
$   !   obtained from show display/symbol
$   IF display .eqs. ""
$   THEN
$     if f$type(decw$display_server) .eqs. "" then -
         decw$display_server = 0
$   ELSE
$     decw$display_server = f$extract(0,f$locate(".",display),display)
$     display = f$extract(f$locate(".",display)+1,255,display)
$   ENDIF
$   if verbose then say Dnodename," Sending Display server"
$   Write_Destination decw$display_server
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%DISPLAY_SCREEN" 
$ THEN 
$   ! DEFINE DISPLAY SCREEN - decw$display_screen
$   !   obtained from show display/symbol
$   IF display .eqs. ""
$   THEN
$     if f$type(decw$display_screen) .eqs. "" then -
         decw$display_screen = 0
$   ELSE
$     decw$display_screen = display
$   ENDIF
$   if verbose then say Dnodename," Sending Display screen"
$   Write_Destination decw$display_screen
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%DISPLAY_TRANSPORT" 
$ THEN 
$   IF Transport .eqs. ""
$   THEN
$     ! DEFINE DISPLAY TRANSPORT - decw$display_transport
$     !   obtained from show display/symbol
$     ! if transport type node defined then assume tcpip
$     if f$type(decw$display_transport) .eqs. "" then -
         decw$display_transport := TCPIP
$     decw$display_transport = f$edit(decw$display_transport,"upcase,trim")
$     ! LOCAL transport uses a Global Section which is more efficient than
$     ! network transports
$     if decw$display_node .eqs. bnodename then -
         decw$display_transport := LOCAL
$     if decw$display_node .nes. bnodename .and. -
         decw$display_transport .eqs. "LOCAL" then -
         decw$display_transport := TCPIP
$     if f$extract(0,3,decw$display_node) .eqs. "LAT" then -
         decw$display_transport := LAT
$   ELSE
$     decw$display_transport = f$edit(transport,"upcase")
$   ENDIF
$     if verbose then say Dnodename," Sending Display transport"
$     Write_Destination decw$display_transport
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%TITLE" 
$ THEN 
$   ! Send the Window Title and Icon Name if supplied.
$   if verbose then say Dnodename," Sending Window Title and Icon Name"
$   Write_Destination title
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%INIT_STATE" 
$ THEN 
$   ! Send the Window Initial State - default is window.
$   IF f$edit(f$extract(0,1,init_state),"collapse,upcase") .eqs. "I" 
$   THEN 
$     init_state := icon
$   ELSE
$     init_state := window       !default
$   ENDIF
$   if verbose then say Dnodename," Sending Window Initial Startup State"
$   Write_Destination Init_State
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%POSITION" 
$ THEN 
$   ! Send desired X and Y location
$   if command .eqs. "DECTERM"
$   then
$     x_pos = f$extract(0,f$locate(":",x_y_loc),x_y_loc)
$     y_pos = f$extract(f$locate(":",x_y_loc)+1,255,x_y_loc)
$   else
$     x_pos = f$trnlnm("dwremterm_dwrt_x")
$     y_pos = f$trnlnm("dwremterm_dwrt_y")
$   endif
$   window_size := ,x_p='x_pos',y_p='y_pos'
$   if x_pos .eqs. "" .and. y_pos .eqs. "" then window_size = ""
$   if x_pos .nes. "" .and. y_pos .eqs. "" then -
       window_size := ,x_position='x_pos'
$   if x_pos .eqs. "" .and. y_pos .nes. "" then -
       window_size := ,y_position='y_pos'
$   if verbose then say Dnodename," Sending Desired Window Location"
$   Write_Destination Window_size
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%MESSAGE" 
$ THEN 
$   ! Do not display informational messages if started automatically from the 
$   ! Session Manager
$   Read_Destination Rem_message
$   If f$trnlnm("vue$input") .eqs. "" then Say rem_message
$   Goto Flush_Output
$ ENDIF
$
$ IF rem_record .eqs. "%FINISHED" 
$ THEN 
$   ! Ending Session
$   Goto Send_exit
$ ENDIF
$
$ IF rem_record .eqs. "%SHUTDOWN" 
$ THEN 
$   ! Suppress -SYSTEM-F-THIRDPARTY messages caused by network link killer
$   Shutdown_Pending = 1
$   Goto Flush_Output
$ ENDIF
$
$ SAY rem_record	!catchall
$ goto flush_output
$!
$!******************************************************************************
$! THIS SECTION OF CODE IS THE ERROR AND EXIT HANDLERS
$!
$open_server_failed:
$ if verbose then say Dnodename," Remote Task DWREMTERM failed"
$ ! copy a new file if we've not done so already
$ if copy_pass .ne. 0 then goto open_failure
$ call copy_file object_not_found 'nodename'
$ !if an error occurred in the call copy_file subroutine then on returning 
$ ! we will branch to open_failure as on warning is still in effect.
$ goto open_server
$
$open_failure:
$! 
$! do not display an error message if
$!  a. FILE NOT FOUND (already displayed)
$!  b. SYSTEM-F-THIRDPARTY (DISCONNECT_LINK has terminated link)
$!
$ status = $status
$ IF f$extract(7,3,status) .ne. 910 .and. -
     (.not. shutdown_pending .and. f$extract(7,5,status) .ne. %x1C0F4) then -
     say "Failed connection, Return Status: ", $Status
$!
$send_exit:
$ IF f$trnlnm("link_destination",,,,,"terminal") 
$ THEN 
$   if verbose then say Dnodename," Closing the DWremterm link"
$   close link_destination
$ ENDIF
$ Exit 1
$ ENDSUBROUTINE	!end of SEND Subroutine
$!
$!******************************************************************************
$! THIS SUBROUTINE IS THE COMMAND PARSER THAT DECODES AND VALIDATES INPUT CMDS.
$!
$command_parser: subroutine
$ sb = "'"			! parameter was passed as a symbol, command mode
$ if menu_mode then sb = "" 	! parameter was passed as a value , menu mode
$ pa == 'sb'p1
$ pb == 'sb'p2
$ pc == 'sb'p3
$ pd == 'sb'p4
$ pe == 'sb'p5
$ pf == 'sb'p6
$ pg == 'sb'p7
$ ph == 'sb'p8
$ verb     == f$edit(pa,"compress,trim,upcase")
$ nodename == f$edit(pb,"compress,trim,upcase")
$ !decode node 0 as the local nodename
$ if f$extract(0,1,nodename) .eqs. "0" then -
     nodename == dwnodename+f$extract(1,256,nodename)
$ bnodename == "" 
$
$ ! combine all parameters
$ param_all == f$edit(pa+" "+pb+" "+pc+" "+pd+" "+pe+" "+-
                      pf+" "+pg+" "+ph,"compress,trim")
$
$ ! break out the DCL command
$ IF f$extract(0,1,verb) .eqs. "$" 
$ THEN
$   $_command == f$extract(1,255,param_all)
$   nodename == ""
$   verb :== $
$ ENDIF
$
$ ! break out the RSH command
$ IF verb .eqs. "RSH" 
$ THEN
$   ! if the nodename is of the format nodename::user then look in
$   ! the proxy database to find the username
$   ! strip off trailing colons
$   if f$extract(f$length(nodename)-2,2,nodename) .eqs. "::" then -
       nodename == f$extract(0,f$length(nodename)-2,nodename) 
$   if f$extract(f$length(nodename)-1,1,nodename) .eqs. ":" then -
       nodename == f$extract(0,f$length(nodename)-1,nodename) 
$   if f$length(nodename) .ne. f$length(nodename-"::")
$   then
$     pnodename = nodename
$     define/process sys$error nl:
$     define/process sys$output nl:
$     proxy 'pnodename'/logical=pnode
$     status = $status
$     if f$trnlnm("sys$error","lnm$process_table") .nes. "" then -
         deass sys$error
$     if f$trnlnm("sys$output","lnm$process_table") .nes. "" then -
         deass sys$output
$     if status
$     then
$       nodename = f$trnlnm("pnode")
$       nodename = f$extract(0,f$length(nodename)-2,nodename)
$       deassign pnode
$       if verbose then -
           say Dnodename," New nodename from Proxy Database:- ",nodename
$     else
$       if status .eqs. "%X00038090" 
$       then 
$         say "ERROR: proxy utility not found"
$         exit
$       endif
$       if status .eqs. "%X100388B2" 
$       then 
$         say "ERROR: proxy utility not found"
$         exit
$       endif
$       if f$search(f$trnlnm("proxy$database")) .eqs. ""
$       then
$         say "ERROR: proxy database does not exist"
$         exit
$       endif
$       say "ERROR: ",pnodename," does not exist in your proxy database" 
$     endif
$   endif
$   ! here we are building the rsh command rsh/user=x/pass=w node command
$   ! break out node
$   node = f$extract(0,f$locate("""",nodename),nodename)
$   ! break out user
$   user = f$extr(1,f$loca(" ",nodename-node)-1,nodename-node) - """" - """" 
$   ! break out password
$   passwd == nodename - node - user - " "
$   ! rsh
$   rsh_command == "rsh"
$   ! rsh/user
$   if user .nes. "" then -
       rsh_command == rsh_command + "/user=" + """" + user + """"
$   ! rsh/user/password
$   if (passwd - """" - """") .nes. "" 
$   then 
$     passwd_specified == 1
$     rsh_command == rsh_command + "/password='hidden'"
$   else
$     passwd_specified == 0
$   endif
$   ! rsh/user/password node command
$   ! determine if required command parameter was supplied
$   if f$edit(f$extract(3,255,param_all)-pb,"trim") .eqs. ""
$   then
$     say "ERROR: No command to execute, the RSH command syntax is"
$     say "RSH nodename command"
$     rsh_command == ""
$     exit
$   endif
$   ! determine if case sensitivity is required to support UNIX
$   if f$edit(f$extract(3,255,param_all)-pb,"trim") .eqs. -
       f$edit(f$extract(3,255,param_all)-pb,"trim,upcase")
$   then
$     !command was uppercase, so ignore quotes and hope rsh/lower works
$     rsh_command == rsh_command + " " + node + " " +-
                     f$edit(f$extract(3,255,param_all)-pb,"trim")
$   else
$     !command was lowercase, so preserve by adding quotes
$     rsh_command == rsh_command + " " + node + " " + """" +-
                     f$edit(f$extract(3,255,param_all)-pb,"trim") + """"
$   endif
$ ENDIF
$
$ if nodename .nes. ""
$ then
$   ! ensure nodename is suffixed with double colon 
$   if f$extract(f$length(nodename)-1,1,nodename) .eqs. ":" then -
      nodename == f$extract(0,f$length(nodename)-1,nodename)
$   if f$extract(f$length(nodename)-1,1,nodename) .eqs. ":" then -
      nodename == f$extract(0,f$length(nodename)-1,nodename)
$   if f$extract(f$length(nodename)-1,1,nodename) .eqs. ":" then -
      nodename == f$extract(0,f$length(nodename)-1,nodename)
$   ! ensure bnodename is not suffixed with double colon 
$   bnodename == nodename
$   nodename  == nodename + "::"
$ endif
$ ! hide password.
$ sec_nodename == f$parse(nodename,,,"node")
$
$ exit
$ ENDSUBROUTINE		!end of command_parser subroutine
$!
$!******************************************************************************
$! THIS SUBROUTINE WILL COPY A NEW VERSION OF DWREMTERM TO THE REMOTE NODE
$!
$COPY_FILE: SUBROUTINE
$ set noon
$ status = 1
$ file_copied = 0
$ copy_reason   = p1
$ nodename      = p2
$ rem_version   = p3
$ rem_procedure = p4
$ ! conceal password
$ sec_nodename  = f$parse(nodename,,,"node")
$ copy_pass == copy_pass+1
$
$ IF copy_reason .eqs. "VERSION_MISMATCH"
$ THEN
$   say bon,"WARNING, DWREMTERM version mismatch",boff
$   say bon,"Local ",Dnodename," V",Version,"     ",-
            "Remote ",Sec_nodename," V",Rem_Version,boff
$ ENDIF
$
$ IF copy_reason .eqs. "OBJECT_NOT_FOUND"
$ THEN
$   status := %X00000910	!return failure code if new file not copied
$   say -
"DECWindows is not installed, DWREMTERM object not defined or login error"
$   goto copy_file_exit
$ ENDIF
$ !
$ ! the DWremterm copy facility can be disabled by defining the logical
$ ! DWremterm_enable_copy
$ if .not. f$trnlnm("dwremterm_enable_copy") then goto copy_file_exit
$
$ Read/Prompt="Do you want to copy the latest version? Y/(N) " -
      /end_of_file=copy_file_exit sys$command response
$
$ IF response 
$ THEN
$   IF .not. verbose 
$   THEN 
$     ! don't display error messages. We'll just return the error code
$     define/user/nolog sys$error nl:
$     define/user/nolog sys$output nl:
$   ENDIF
$   !
$   ! copy latest version of DWremterm file to or from remote node.
$   !
$   IF copy_reason .eqs. "VERSION_MISMATCH" .and. -
       rem_version .ges. version
$   THEN
$     ! copy from remote node to local node
$     Copy/log/protection=w:rwe 'nodename''rem_procedure' 'loc_procedure'
$     status = $status
$     IF status
$     THEN
$       SAY "Copied file from ",sec_nodename," to ",dnodename
$     ELSE
$       IF copy_reason .nes. "OBJECT_NOT_FOUND"
$       THEN
$       say "No privilege to copy DWremterm to ",dnodename," - continuing..."
$       status = 1
$       ENDIF
$     ENDIF
$     file_copied := true
$   ENDIF
$
$   IF .not. file_copied
$   THEN
$     ! copy from local node to remote node
$     Copy/log/protection=w:rwe 'loc_procedure' 'nodename''rem_procedure'
$     status = $status
$     IF status
$     THEN
$       SAY "Copied file from ",dnodename," to ",sec_nodename
$     ELSE
$       IF copy_reason .nes. "OBJECT_NOT_FOUND"
$       THEN
$       say "No privilege to copy DWremterm to ",sec_nodename," - continuing..."
$       status = 1
$       ENDIF
$     ENDIF
$   ENDIF
$ ENDIF
$copy_file_exit:
$ exit status+%x10000000
$ ENDSUBROUTINE		!end of copy_file subroutine
$!
$!******************************************************************************
$CREATE_DETACH: 
$! this creates a command procedure which is read by the detached process
$! The detached job is done for two reasons:-
$!  - to prevent the remove job tying up the local interactive users process
$!  - to allow jobs that issue a spawn to work 
$!       (spawn jobs do not work from a network process)
$!
$! the RSH command does not require a local display
$! the DWHELP, LAT, DECNET, TELNET and RDW do require a display
$ if display_required 
$ then
$   if f$trnlnm("decw$display") .eqs. "" .or. -
       .not. f$getdvi("decw$display","exists")
$   then
$     Say "%DECW-E-CANT_OPEN_DISPL, Can't open display"
$     exit 
$   endif
$   define/user/nolog sys$error nl:
$   define/user/nolog sys$output nl:
$   ! get current display
$   show display/symbol
$   if .not. $status then exit
$   if f$type(rem_node) .eqs. "" then rem_node = decw$display_node
$   if f$type(rem_screen) .eqs. "" then rem_screen = decw$display_screen
$   if f$type(rem_server) .eqs. "" then rem_server = decw$display_server
$   if f$type(rem_transport) .eqs. "" then -
              rem_transport = decw$display_transport
$ endif
$
$ i = 0
$CREATE_DETACH_LOOP:
$ i = i+1
$ outfile := -
    'f$trnlnm("sys$scratch")'dwremterm_'f$ext(4,4,f$getj("","pid"))'_'i'.tmp;1
$ if f$search(outfile,1) .nes. "" then goto create_detach_loop
$ outlog := -
    'f$trnlnm("sys$scratch")'dwremterm_'f$ext(4,4,f$getj("","pid"))'_'i'.log;1
$ if f$search(outlog,1) .nes. "" then goto create_detach_loop
$ if verbose then say Dnodename," Creating detached command file: ",outfile
$ open/write out 'outfile'
$ write out "$!'f$verify(0)'
$ write out "$!*** this is a temporary file created by dwremterm.com ***"
$ write out "$ set noon"
$! set verify if in debug mode
$ if verbose 
$ then
$   write out "$ set verify"
$   write out "$ write sys$output ""Current PID is ''f$getjpi("","pid")'"""
$ else
$   write out "$ set noverify"
$ endif
$ write out "$ if p1 .eqs. ""RDT_CMD"" then goto rdt_cmd
$ write out "$ existing_subprocesses = f$getjpi("""",""prccnt"")"
$ write out "$ fatal_error = 0"
$ write out "$ if f$mode() .eqs. ""INTERACTIVE"" then set terminal/noecho"
$ write out "$ if f$trnlnm(""tcpware"") .nes. """" "
$ write out "$ then"
$ if verbose then -
  write out "$   write sys$output ""This node is running TCPWARE"""
$ write out "$   if f$type(rsh) .eqs. """" then rsh := $tcpware:rsh"
$ write out "$   if f$type(telnet) .eqs. """" then telnet := $tcpware:telnet"
$ write out "$   if f$type(rlogin) .eqs. """" then rlogin := $tcpware:rlogin"
$ write out "$ endif
$ write out "$ if f$trnlnm(""multinet"") .nes. """" "
$ write out "$ then"
$ if verbose then -
  write out "$   write sys$output ""This node is running MULTINET"""
$ write out "$   if f$type(rsh) .eqs. """" then rsh := multinet rsh"
$ write out "$   if f$type(telnet) .eqs. """" then telnet := multinet telnet"
$ write out "$   if f$type(rlogin) .eqs. """" then rlogin := multinet rlogin"
$ write out "$ endif
$ if verbose .and. f$trnlnm("ucx$host") .nes. "" then -
  write out "$ write sys$output ""This node is running UCX"""
$! The DWHELP, LAT, DECNET and TELNET command itself with a P1 parameter of 
$!   EXECUTE_CMD. This is done so that the user can see why a command fails.
$! The RSH and RDW commands do not call itself.
$ if execute_p1_command .nes. ""
$ then
$   write out "$ if p1 .eqs. ""EXECUTE_CMD"""
$   write out "$ then
$   if verb .nes. "LDT" 
$   then -
      write out "$   write sys$output "+-
                """Enter <RETURN> to connect to ''bnodename' via ''verb'"""
$     write out "$   inquire/nopunc x """""
$ endif
$   write out "$ if f$mode() .eqs. ""INTERACTIVE"" then set terminal/echo"
$   write out "$   define/nolog sys$input sys$command"
$   write out "$retry:"
$   write out "$   set verify"
$   write out "$ ",execute_p1_command"
$   if .not. verbose then write out "$! 'f$verify(0)'"
$   write out "$   if .not. $status"
$   write out "$   then" 
$   write out "$     set noverify
$   write out "$     write sys$output ""
$   write out "$     inquire/nopunc x ""<CR> to retry or QUIT to exit: """
$   write out "$     if f$edit(f$extract(0,1,x),""trim,upcase"") .nes. ""Q"" then goto retry"
$   write out "$   else"
$   write out "$     wait 0:0:5"
$   write out "$   endif
$   write out "$ exit"
$   write out "$ endif"
$ endif
$
$! display required on commands except RSH
$ if display_required 
$ then 
$   write out "$ Set Display/Create/Node=",Rem_Node,"/Trans=",Rem_Transport,-
                           "/Screen=",Rem_Screen,"/Server=",Rem_Server
$   write out "$ show display"
$ endif
$
$! A password is only required for the RSH command.
$! A DWremterm mailbox is used to pass the password to the detached RSH command.
$! This overcomes the security problem of writing the password into the 
$!   command file.
$ if password_required
$ then
$   write out "$ write sys$output ""Getting password from " +-
              "''f$trnlnm("dwremterm_mbx")':"""
$   write out "$ open/read/write/error=pwdmbx pwdmbx ",f$trnlnm("dwremterm_mbx"),":"
$   write out "$ write/error=pwdmbx pwdmbx ""please_give_me_password"""
$   write out "$ read/error=pwdmbx pwdmbx hidden"
$   write out "$pwdmbx:"
$   write out "$ if .not. $status"
$   write out "$ then"
$   write out "$   write sys$output ""ERROR: Unable to get password from Mailbox" +-
              "''f$trnlnm("dwremterm_mbx")':"""
$   write out "$   close pwdmbx"
$   write out "$   fatal_error = 1"
$   write out "$   goto loop"
$   write out "$ endif"
$   write out "$ 
$   write out "$ close pwdmbx"
$   write out "$ wait 0:0:1"
$   write out "$ if f$getdvi(""",f$trnlnm("dwremterm_mbx"),""",""exists"")"+-
              " then write sys$output ""WARNING: Mailbox """, -
              f$trnlnm("dwremterm_mbx"),""" was not deleted"""
$ endif
$
$! if the resfile symbol exists then we are creating a dwremterm window in
$! menu mode. We define resfile to prevent the execute_command symbol 
$! from overflowing.
$ if f$type(resfile) .nes. ""
$ then
$   write out "$ define/nolog resfile ''resfile'"
$   delete/symbol resfile
$ endif
$
$ write out "$ write sys$output ""-- Starting Command --"""
$ if f$extract(0,31,execute_command) .eqs. "Create/Term/noproc/define=dtdev"
$ then
$   ! normally we just execute the command however if this was an RDT command
$   ! and a dcl-command was supplied then we need to create the decterm
$   ! in a special way so that when the dcl-command finishes the user is
$   ! returned to the dcl prompt rather than the default where the DECterm
$   ! window is closed.
$   if .not. verbose then -
    write out "$ set verify !'f$verify(0)'"
$   ! create decterm window 
$   write out "$ ",execute_command
$   write out "$ if .not. $status then goto finish
$   ! allocate decwindows device to ensure DECwindows display is not deleted
$   write out "$ dtdev = f$trnlnm(""dtdev"")
$   write out "$ allocate/nolog 'dtdev'
$   ! we must now turn off verify otherwise verify will be enabled in the 
$   ! decterm window we just created. So the command appears in the log files
$   ! we write out the command we are executing and then turn off verify.
$   write out "$ set noverify !'f$verify(0)'
$   write out "$ write sys$output ""$ spawn/wait/nolog/input="",dtdev,""/output="",dtdev,"" @"",f$env(""procedure""),"" rdt_cmd"""
$   write out "$ write sys$output ""Execute command is:- $ ",rem_dcl_command,"""
$   ! now run sylogin.com, login.com and direct the users command to the decterm window
$   write out "$ spawn/wait/nolog/input='dtdev'/output='dtdev' @'f$env(""procedure"")' rdt_cmd"
$   write out "$ deallocate 'dtdev' !'f$verify(1)'
$   if .not. verbose then -
    write out "$!'f$verify(0)'"
$ else
$   ! all other commands
$   if verbose 
$   then 
$     ! preserve dwremterm_verbose. Needed for commands like "$dwremterm"
$     ! so that the window remains 24x80 rather than just the normal
$     ! small dwremterm window
$     write out "$ define/job dwremterm_verbose true"
$   else
$     ! the command is written out and verify disabled to ensure that the
$     ! password is not written into any log files.
$     write out "$!'f$verify(0)'
$     write out "$ type sys$input"
$     write out "$ deck/dollars=""^z"""
$     write out "$ ",execute_command
$     write out "^z"
$   endif
$   ! now execute the command
$   write out "$ ",execute_command
$ endif
$ ! Write out the p1 command into the logfile as this would otherwise not
$ ! be seen as its executed in the DECterm window.  
$ if execute_p1_command .nes. "" then -
  write out "$ write sys$output ""Execute command is:- $ ",execute_p1_command,"""
$ write out "$finish:
$ write out "$ write sys$output ""-- Finished Command --"""
$ write out "$loop:"
$ write out "$ if f$getjpi("""",""prccnt"")-existing_subprocesses .le. 0 .or. fatal_error .eq. 1"
$ write out "$ then"
$! clean up temporary files
$ write out "$   define/user sys$output nl:"
$ write out "$   define/user sys$error nl:"
$ write out "$   delete/before=""-0:5:0"" ''f$trn("sys$scratch")'dwremterm_*_%.log;*"
$ write out "$   define/user sys$output nl:"
$ write out "$   define/user sys$error nl:"
$ write out "$   delete/before=""-0:5:0"" ''f$trn("sys$scratch")'dwremterm_*_%.tmp;*"
$ if .not. verbose then write out -
            "$   if f$search(f$env(""procedure"")) .nes. """" then delete 'f$env(""procedure"")'"
$ write out "$   exit"
$ write out "$ else"
$! this loop prevents the program from exiting if a subprocess exists
$ write out "$   wait 00:00:15"
$ write out "$   goto loop"
$ write out "$ endif"
$ write out "$ exit"
$ write out "$!"
$! If an RDT job and a DCL command was supplied then we need to emulate the
$! login by running sylogin.com and login.com followed by the actual command
$ if f$extract(0,31,execute_command) .eqs. "Create/Term/noproc/define=dtdev"
$ then
$   write out "$rdt_cmd:"
$   write out "$ if f$trnlnm(""sys$sylogin"") .nes. """" .and. -"
$   write out "     f$search(""sys$sylogin"") .nes. """
$   write out "$ then"
$   write out "$   write sys$output ""Executing ''f$edit(f$trnl("sys$sylogin"),"trim,lowercase")'"
$   write out "$   @sys$sylogin"
$   write out "$ else"
$   write out "$   if f$search(""sys$manager:sylogin.com"") .nes. """""
$   write out "$   then"
$   write out "$     write sys$output ""Executing sys$manager:sylogin.com"""
$   write out "$     @sys$manager:sylogin"
$   write out "$   else"
$   write out "$     write sys$output ""Cannot locate the sylogin procedure"""
$   write out "$   endif"
$   write out "$ endif"
$   write out "$ if f$search(""sys$login:login.com"") .nes. """""
$   write out "$ then"
$   write out "$   write sys$output ""Executing sys$login:login.com"""
$   write out "$   @sys$login:login"
$   write out "$ else"
$   write out "$   write sys$output ""Cannot localte the users login procedure"""
$   write out "$ endif"
$   write out "$ write sys$output ""Executing supplied command"""
$   write out "$ write sys$output ""$ ''rem_dcl_command'"""
$   write out "$ define/nolog sys$input sys$command"
$   write out "$ ",rem_dcl_command
$   write out "$ write sys$output ""Returning to the DCL command prompt"""
$ endif
$ close out
$ RETURN    !end of create_detach gosub routine
$!
$!******************************************************************************
$! This subroutine generates a DWremterm Mailbox so we can pass the password 
$!  without having to write it into the command file
$!
$CREATE_MBX:
$ p1 = ""
$ p2 = ""
$ p3 = ""
$CREATE_MBX_NOINIT:
$ vfy = f$verify(0)
$ my_pid = f$getjpi("","pid")
$ if f$leng(p2) .gt. 0 then goto create_mbx_subprocess
$ count = ""
$! allow for multiple dwremterm mailboxes to be created (not to exceed 32 total)
$ sanity_count = 0
$CREATE_MBX_LOOP:
$ sanity_count = sanity_count + 1
$ if sanity_count .ge. 32 then exit 4
$ if f$trnlnm("dwremterm_mbx''count'") .eqs. "" then goto spawn
$ count = f$stri(f$inte(count)+1)
$ goto create_mbx_loop
$spawn:
$ dwremterm_mbx = "dwremterm_mbx" + count		! logical name used
$ procedure = f$envi("procedure")
$! spawn a subprocess creating a dwremterm mailbox...
$ spawn/nolog/nowait/process=crembx_'my_pid' -
  @'procedure' CREATE_MBX 'my_pid' 'count'
$ set process/suspend/id=0
$ mailbox = "mba" + f$trnlnm(dwremterm_mbx)
$ open/read/write 'dwremterm_mbx' 'mailbox':		! grab mailbox
$ deassign/job 'dwremterm_mbx'				! remove job logical
$ stop crembx_'my_pid'					! kill subprocess
$ if verbose then say Dnodename," ''f$fao("%CREMBX-I-CREATED, "+-
    "Mailbox !AS !AS: created.",dwremterm_mbx,f$trnlnm(dwremterm_mbx))
$ mbxdev = f$trnlnm("dwremterm_mbx")
$ pipe'count' == f$fao("spawn/nowait/nolog/output=!AS:",f$trnlnm(dwremterm_mbx))
$ grep'count' == f$fao("search !AS:",f$trnlnm(dwremterm_mbx))
$ RETURN 1+0*f$verify(vfy)	 !end of create_mbx subroutine
$!
$CREATE_MBX_SUBPROCESS:
$ mailbox = f$getjpi("","tmbu")
$ define/nolog/job dwremterm_mbx'p3' 'mailbox'		! tell parent mbx name
$CREATE_MBX_SUBPROCESS_LOOP:
$ set process/resume/id='p2'
$ wait 00:00:01.50
$ if f$extr(0,3,f$getjpi(p2,"state")) .eqs. "SUS"
$  then
$    sanity_check = f$inte("''sanity_check'")+1
$    if sanity_check .gt. 256 then exit %x4
$    goto create_mbx_subprocess_loop
$  endif
$ set process/suspend/id=0
$ RETURN 1+0*f$verify(vfy)	 !end of create_mbx subroutine
$!
$!******************************************************************************
$! THIS SECTION OF CODE IS EXECUTED ON THE REMOTE NODE
$!
$NETWORK:
$!
$! Write status information to NETSERVER.LOG and the originating node if
$! Verbose is enabled.
$!
$ errors_not_reported :== 0
$ define/process/nolog netserver$timeout "0 00:00:00"
$
$ Rem_command = ""
$ Write_Source = "write link_source"
$ Read_Source  = "read  link_source"
$
$ on warning then goto remote_error	!error handler
$
$ open/read/write link_source sys$net
$
$ Write_Source "%VERBOSE"		!Request Verbose Status
$ Read_Source verbose
$ IF verbose 
$ THEN 
$   Write_Source bon,Dnodename," Verbose Messaging is: On",boff
$   Say "Verbose Messaging is: On"
$   define/nolog sys$error link_source 
$ ELSE
$   Say "Verbose Messaging is: Off"
$ ENDIF
$
$ Write_Source "%PROCEDURE"
$ Write_Source loc_procedure
$ if verbose then Write_Source bon,Dnodename,-
     " Remote Procedure Name is: ",f$env("procedure"),boff
$ Say "Current Procedure Name is: ",f$env("procedure")
$ 
$ Write_Source "%VERSION"		!Supply Version
$ Write_Source  version
$ if verbose then Write_Source bon,Dnodename,-
     " Remote version is: V",version,boff
$ Say "Current version is: V",version
$
$ Write_Source "%COMMAND"		!request a command to issue
$ Read_Source Rem_Command
$ if verbose then Write_Source bon,Dnodename, -
     " Received DECwindows Command is: ",rem_command,boff
$ Say "Received DECwindows Command is: ",rem_command
$
$ Write_Source "%DCL_COMMAND" 		!request DCL command
$ Read_Source Rem_Dcl_Command
$ if verbose then Write_Source bon,Dnodename, -
     " Received DCL command is: ",rem_dcl_command,boff
$ Say "Received DCL command is: ",rem_dcl_command
$
$ Write_Source "%USERNAME"		!Supply Username
$ Write_Source DWusername
$ if verbose then Write_Source bon,Dnodename,-
     " Remote Username is: ",DWusername,boff
$ Say "Current Username is: ",DWusername
$! It would be a security hole to allow a remote user to log on as DECnet.
$! Therefore the only command we allow to execute from the DECnet account
$! is an RDT command, but with the /nologged_in qualifier. This will
$! prompt the user for a username and password.
$ IF DWusername .eqs. "DECNET" .or. DWusername .eqs. "FAL$SERVER"
$ THEN 
$   IF rem_command .nes. "DECTERM" .or. rem_dcl_command .nes. ""
$   THEN
$     Write_Source -
          "ERROR: You are not Authorized to use the default DECNET account..."
$     Say "ERROR: You are not Authorized to use the default DECNET account..."
$     goto network_close
$   ENDIF
$ ENDIF
$!
$! disallow login if logins are disabled and the user does not have system
$! privileges
$ IF f$getsyi("ijoblim") .eq. 0 
$ THEN 
$   IF f$len(f$getj("","authpriv")) .eq. -
       f$len(f$getj("","authpriv")-"OPER"-"SETPRV")
$   THEN
$     Write_Source "Logins are currently disabled"
$     Say "Logins are currently disabled"
$     goto network_close
$   ENDIF
$ ENDIF
$
$ Write_Source "%NODENAME"		!supply nodename
$ Write_Source Dnodename
$ if verbose then Write_Source bon,dnodename," Nodename is: ",dnodename,boff
$ Say "Nodename is: ",dnodename
$
$ Write_Source "%PID"			!supply Process ID
$ Write_Source f$getjpi("","pid")
$ if verbose then Write_Source bon,Dnodename,-
     " Remote PID is: ",f$getjpi("","pid"),boff
$ Say "Current PID is: ",f$getjpi("","pid")
$
$ Write_Source "%PRCNAM"		!supply process name
$ Write_Source f$getjpi("","prcnam")
$ if verbose then Write_Source bon,Dnodename,-
     " Remote Process Name is: ",f$getjpi("","prcnam"),boff
$ Say "Current Process Name is: ",f$getjpi("","prcnam")
$
$ Write_Source "%DISPLAY_NODE"	  	!request nodename
$ Read_Source Rem_node
$ if verbose then Write_Source bon,Dnodename,-
     " Received Display Node is: ",Rem_node,boff
$ Say "Received Display Node is: ",Rem_node
$ rem_node = """"+rem_node+""""
$
$ Write_Source "%DISPLAY_SERVER"  	!request server
$ Read_Source Rem_Server
$ if verbose then Write_Source bon,Dnodename,-
     " Received Display Server is: ",Rem_server,boff
$ Say "Received Display Server is: ",Rem_server
$
$ Write_Source "%DISPLAY_SCREEN"	!request screen number
$ Read_Source Rem_Screen
$ if verbose then Write_Source bon,Dnodename,-
     " Received Display Screen is: ",Rem_screen,boff
$ Say "Received Display Screen is: ",Rem_screen
$
$ Write_Source "%DISPLAY_TRANSPORT"	 !request transport type
$ Read_Source Rem_Transport
$ if verbose then Write_Source bon,Dnodename,-
     " Received Display Transport is: ",Rem_transport,boff
$ Say "Received Display Transport is: ",Rem_transport
$
$ Write_Source "%TITLE"  		!request icon and title name
$ Read_Source Rem_title
$ if verbose then Write_Source bon,Dnodename,-
     " Received Window Title and Icon Name is: ",Rem_title,boff
$ Say "Received Window Title and Icon Name is: ",Rem_title
$! ensure lowercase characters are maintained
$ BRem_title =  Rem_title
$ BRem_icon  =  Rem_title
$ Rem_title = """" + Rem_title + """"
$ Rem_icon = Rem_title
$
$ Write_Source "%INIT_STATE"  		!request initial state - window or icon
$ Read_Source Rem_Init_State
$ if verbose then Write_Source bon,Dnodename, -
     " Received Window Startup State is: ",rem_init_state,boff
$ Say "Received Window Startup State is: ",rem_init_state
$
$ IF Rem_Command .eqs. "DWREMTERM" .or. Rem_Command .eqs. "DECTERM"
$ THEN
$   Write_Source "%POSITION"  		!request DWRT window position
$   Read_Source Rem_Position
$   if verbose then Write_Source bon,Dnodename, -
       " Received Window Location is: ",rem_position,boff
$   Say "Received Window Location is: ",rem_position
$ ENDIF
$
$!------------------------------------------------------------------------------
$! the perm display is a temporary workaround to fix a Motif V1.1 bug.
$! this problem is fixed in Decwindows Motif V1.2
$ oldsysnam = f$setprv("sysnam")
$ if f$priv("sysnam") 
$ then
$   perm_display := "/Exec"
$   say "User has SYSNAM privilege - Creating permanent decwindows display "
$   say "     to overcome a DECwindows Motif V1.1 bug"
$ else
$   perm_display := ""
$   say "User does not have SYSNAM privilege - display may be deleted due "
$   say "     to a DECwindows Motif V1.1 bug"
$ endif
$ Set Display/Create/Node='Rem_Node''perm_display' -
     /Trans='Rem_Transport'/Screen='Rem_Screen'/Server='Rem_Server'
$ if verbose then Write_Source bon,-
     "Display Created with the following command:-",nl,-
     "  $ Set Display/Create/Node=",Rem_Node,"/Trans=",Rem_Transport,nl,-
     "        /Screen=",Rem_Screen,"/Server=",Rem_Server,perm_display,boff
$ Say "Display Created with the following command:-",nl,-
      "  $ Set Display/Create/Node=",Rem_Node,"/Trans=",Rem_Transport,nl,-
      "        /Screen=",Rem_Screen,"/Server=",Rem_Server,perm_display
$ set proc/priv='oldsysnam'
$!
$! Set up symbols for process name generation
$!
$ Show_command    = Rem_command
$ Execute_command = Rem_command
$
$ IF Rem_command .nes. "DECTERM" .and. -
     Rem_command .nes. "FILEVIEW" .and. -
     Rem_command .nes. "DWREMTERM"
$ THEN
$   rem_init_state = ""
$   verb := RDW
$ ELSE
$   verb := 'rem_transport'
$ ENDIF
$
$ IF Rem_command .eqs. "DWREMTERM"
$ THEN
$   verb := DWremterm
$   Execute_command := @'f$environment(""procedure"")' DWREMTERM -
                       'Rem_Position' ,initial_state='Rem_Init_State'
$ ENDIF
$
$ IF Rem_command .eqs. "FILEVIEW" 
$ THEN 
$   verb := RFV
$   ! note: a current bug in the DECwindows Toolkit prevents FILEVIEW from 
$   ! starting as an icon reliably - works fine with LOCAL transport but
$   ! is intermittent under other transports.
$   IF rem_init_state .eqs. "ICON" 
$   THEN 
$     rem_init_state := -iconic
$   ELSE
$     rem_init_state = ""
$   ENDIF
$   Show_command    := FILEVIEW 'rem_init_state'
$   Execute_command := MCR VUE$MASTER 'rem_init_state'
$ ENDIF
$
$ IF rem_transport .eqs. "DECNET"
$ THEN
$   node_delim = "::"
$ ELSE
$   node_delim = ":"
$ ENDIF
$
$ !tell us where we are going to display the window.
$ Write_Source "%MESSAGE"
$ Write_Source -
    "Remote `",f$edit(Show_Command+" "+rem_dcl_command,""trim""),"' is ",-
    "starting on ''Dnodename' and being ",-
    "displayed on ",Rem_Node,Node_delim,Rem_Server,".",-
    Rem_Screen," using the ",rem_transport," transport."
$ Say -
    "Remote `",f$edit(Show_Command+" "+rem_dcl_command,""trim""),"' is ",-
    "starting on ''Dnodename' and being ",-
    "displayed on ",Rem_Node,Node_delim,Rem_Server,".",-
    Rem_Screen," using the ",rem_transport," transport."
$
$ IF Rem_Command .eqs. "DECTERM"
$ THEN
$   wtiname = f$edit(dwnodename-"::","lowercase") + " " +-
              f$edit( dwusername,"lowercase")
$   if rem_title .eqs. """""" then rem_title = """" + wtiname + """"
$   if rem_icon  .eqs. """""" then rem_icon  = -
         """" + f$extra(0,f$loc(".",wtiname),wtiname) + """"
$   if BRem_title .eqs. "" then BRem_title = wtiname
$   if BRem_icon  .eqs. "" then BRem_icon  = -
         f$extra(0,f$loc(".",wtiname),wtiname) 
$! It would be a security hole to allow a remote user to log on as DECnet.
$! Therefore the only command we allow to execute from the DECnet account
$! is an RDT command, but with the /nologged_in qualifier. This will
$! prompt the user for a username and password.
$   IF DWusername .eqs. "DECNET" .or. DWusername .eqs. "FAL$SERVER"
$   THEN
$     loggedin = "/NoLogged_in"
$   ELSE
$     loggedin  = ""
$   ENDIF
$   IF Rem_Dcl_Command .eqs. "" 
$   THEN 
$     ! if no DCL command is supplied then run as a detached process
$     dwwait = ""
$     detached = "/Detached"
$   ELSE
$     ! if a DCL command is supplied then run as a subprocess
$     ! It is done this way as you cannot use run a command file as part of a
$     ! detached process startup.
$     dwwait = "/wait"
$     detached = "/Nodetached"
$     dwremterm_rdt := true
$   ENDIF 
$
$!------------------------------------------------------------------------------
$   If verbose then Write_Source bon,-
       "Window Created with the following commands:-",boff
$   If verbose 
$   Then 
$     If Rem_Dcl_Command .eqs. ""
$     Then
$       Write_Source bon,-
        "  $ Create/Terminal''dwwait'''detached'''loggedin'-"
$       Write_Source -
        "          /Window_att=(title=",rem_title,"-"
$       Write_Source -
        "                       Icon_name=",Rem_icon,"-"
$       Write_Source -
        "                       Initial_State=",Rem_Init_State,"-"
$       Write_Source -
        "                       ''Rem_Position') ",Rem_Dcl_Command,boff
$     Else
$       Write_Source bon,-
        "  $ Create/Term/noproc/define=dtdev-"
$       Write_Source -
        "          /Window_att=(title=",rem_title,"-"
$       Write_Source -
        "                       Icon_name=",Rem_icon,"-"
$       Write_Source -
        "                       Initial_State=",Rem_Init_State,"-"
$       Write_Source -
        "                       ''Rem_Position') 
$       Write_Source -
        "  $ dtdev = f$trnlnm("dtdev")
$       Write_Source -
        "  $ allocate/nolog 'dtdev'
$       Write_Source -
        "  $ spawn/wait/nolog/input='dtdev'/output='dtdev' ",Rem_Dcl_Command
$       Write_Source -
        "  $ deallocate 'dtdev'",boff
$     Endif
$   Endif
$
$   If Rem_Dcl_Command .eqs. ""
$   Then
$     Say -
        "Window Created with the following commands:-"
$     Say -
        "  $ Create/Terminal''dwwait'''detached'''loggedin'-"
$     Say -
        "          /Window_att=(title=",rem_title,"-"
$     Say -
        "                       Icon_name=",Rem_icon,"-"
$     Say -
        "                       Initial_State=",Rem_Init_State,"-"
$     Say -
        "                       ''Rem_Position') ",Rem_Dcl_Command
$   Else
$     Say -
          "  $ Create/Term/noproc/define=dtdev-"
$     Say -
          "          /Window_att=(title=",rem_title,"-"
$     Say -
          "                       Icon_name=",Rem_icon,"-"
$     Say -
          "                       Initial_State=",Rem_Init_State,"-"
$     Say -
          "                       ''Rem_Position') "
$     Say "  $ dtdev = f$trnlnm(""dtdev"")"
$     Say "  $ allocate/nolog 'dtdev'"
$     Say "  $ spawn/wait/nolog/input='dtdev'/output='dtdev' ",Rem_Dcl_Command
$     Say "  $ deallocate 'dtdev'"
$   Endif
$
$   IF Rem_Dcl_Command .eqs. "" 
$   THEN
$     !due to a bug in DECwindows V2.x a failure in /DETACHED will not return
$     !the error in $STATUS. This is fixed in DECwindows MOTIF.
$     Create/Terminal'dwwait''Detached''loggedin' -
            /Window_Att=(Title='Rem_title',Icon_name='Rem_icon', -
                         Initial_State='Rem_Init_State''rem_position') 
$     goto network_close
$   ELSE
$     ! This command will create the decterm window. The dcl-command will be 
$     ! passed by redefining sys$input. This is done on the remote node using 
$     ! the create_detach routine in DWremterm.
$     ! Normally we just execute the command however if this was an RDT command
$     ! and a dcl-command was supplied then we need to create the decterm
$     ! in a special way so that when the dcl-command finishes the user is
$     ! returned to the dcl prompt rather than the default where the DECterm
$     ! window is closed.
$     execute_command  = -
        "Create/Term/noproc/define=dtdev" +-
        "/Window_Att=(Title=""''BRem_title'"",Icon_name=""''BRem_icon'"","+-
        "Initial_State=" + Rem_Init_State + rem_position + ")" 
$   ENDIF
$ ENDIF
$   
$! all commands except detached decterms
$ if verbose .and. Rem_Command .nes. "DECTERM" then Write_Source -
     "Remote Command being executed is:-",nl,-
      "  $ ",Execute_Command
$ if Rem_Command .nes. "DECTERM" then Say -
      "Remote Command being executed is:-",nl,-
      "  $ ",Execute_Command
$!
$ execute_p1_command = ""
$ display_required = 1
$ password_required = 0
$ gosub create_detach 
$!
$ if verbose then Write_Source bon,Dnodename,"Starting Detached Process",boff
$ Say "Starting Detached Process"
$ set noon
$ i = 0
$DWterm_loop:
$ if i .le. 99 
$ then
$   dprcnam = "DWterm-" + f$extract(0,5,f$getjpi("","username")) + "-''i'"
$   i = i + 1
$   define/user sys$error nl:
$   define/user sys$output nl:
$   Run/detached/authorize sys$system:loginout.exe -
     /input='outfile'/output='outlog'/process_name="''dprcnam'"
$   if .not. $status then goto DWterm_loop
$ else
$   Say "Error: Unable to create detached process"
$ endif
$ goto network_close
$
$!------------------------------------------------------------------------------
$remote_error:
$ ! return completion status if link still open
$ Status = $Status
$ set noon
$! Status %X0001827A = RMS-E-EOF, end of file detected.
$! This occurs when the source node closes the network link. 
$! As this is an expected status (eg. copying a new version of DWremterm)
$! ignore the error
$ IF Status .eqs. "%X0001827A" 
$ THEN 
$   Say "Network Link Closed by Source Node ",f$trnlnm("sys$rem_node")
$   goto network_exit
$ ENDIF
$
$ IF .not. f$trnlnm("link_source",,,,,"terminal") 
$ THEN 
$   Say "Network already closed, all completed, exiting"
$   exit
$ ENDIF
$
$ IF Status 
$ THEN 
$!   Write_Source "%MESSAGE"
$!   Write_Source "Remote Application created sucessfully."
$   Say "Remote Application created sucessfully."
$ ELSE
$   set message sys$message:decw$terminalmsg.exe
$   Write_Source/error=send_mail -
        "ERROR: ",f$message(status)
$   Goto Network_Close
$send_mail:
$   file := sys$scratch:dwremterm.'f$getjpi("","pid")'
$   status_message = f$message(status) 
$   Open/write mailfile 'file'
$   write mailfile ""
$   write mailfile "DWremterm Failed. "
$   write mailfile "Error code: ",status,"  ",status_message
$   IF Rem_command .eqs. "DECTERM" 
$   THEN
$     write mailfile "DECterm command issued was: "
$     write mailfile "   ''Rem_DCL_Command'" 
$   ELSE
$     write mailfile "Command issued was:  "
$     write mailfile "   ''execute_command'" 
$   ENDIF
$   write mailfile "Refer to the file NETSERVER.LOG for details."
$   Say "Mailing off error message"
$   close mailfile
$   Mail/noself/subject="DWremterm Failure" -
        'file' 'f$trnlnm("sys$rem_node")''f$trnlnm("sys$rem_id")'
$   delete 'file';
$!   Write_Source "Ensure that Security for this user has been enabled..."
$   Say "Remote Application Error Occured, Status: ",Status
$ ENDIF
$
$network_close:
$ Write_Source/error=network_exit "%FINISHED"
$ Say "Closing Remote Link"
$ if f$trnlnm("link_source",,,,,"terminal") then close link_source
$network_exit:
$ Say "Remote Link is Closed"
$ set noverify
$ exit
$!
