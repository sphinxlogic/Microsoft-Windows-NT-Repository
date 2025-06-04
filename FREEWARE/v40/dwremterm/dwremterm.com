$ saved_verify = f$verify(0)
$ set noon
$ DWremterm_version := V031
$!******************************************************************************
$!
$! AUTHOR: kingsmill@ripper.stl.dec.com
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
$!
$! This command procedure is designed to allow users flexability in using 
$! DECwindows across a large number of machines connected together by a 
$! Local Area Network. It allows users to create LAT or DECnet windows locally
$! or to create remote DECwindow sessions. The decwindows commands RDT, RFV and
$! RDW are actually network process's and therefore do not count as an
$! interactive login.
$! 
$! DWremterm consists of two files:
$! SYS$COMMON:[VUE$LIBRARY.USER]DWREMTERM.COM 
$! SYS$COMMON:[SYSHLP]DWREMTERM.HLB
$!
$! This file DWREMTERM.COM needs to exist on both the local and remote nodes 
$! and requires the user to either specify node access control or for the 
$! remote account to be set up for proxy access. DWremterm can simply reside 
$! in sys$login on the remote node, which requires each user to have their own
$! copy -or- DWremterm can be defined as a DECnet object, requiring each 
$! system to have only one copy.
$!
$! To install DWremterm as a network object the system manager should enter the
$! following commands:
$!  $ MCR NCP
$!  NCP> DEFINE OBJECT DWREMTERM NUMBER 0 FILE -
$!       SYS$COMMON:[VUE$LIBRARY.USER]DWREMTERM.COM PROXY BOTH
$!  NCP> SET OBJECT DWREMTERM ALL
$!  NCP> EXIT
$!  $
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
$! LATmaster must be installed to use the LAT command then. This software is 
$! optional under VMS 5.4-x and has been integrated into VMS 5.5. If 
$! LATmaster is installed on VMS 5.4-x then CSCPAT_0511 patch should be also 
$! be installed. If LATmaster is not installed then the LAT command cannot 
$! be used.
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
$! DWremterm can be run in two modes:
$!   MENU MODE: commands are entered from a Small Menu Window.
$!   COMMAND MODE: commands are entered from the DCL command line.
$!
$! Valid command are:- HELP, DWHELP, LAT, DECNET, RDT, RFV, RDW, DWRT and $.
$! Parameters listed are [] brackets is optional. 
$! HELP: Gives details on how to receive Brief or Expanded Help
$! DWHELP: Provide detailed Help on DWremterm
$! LAT:    Creates a Local DECterm Window using a SET HOST/LAT command.
$!         Only valid in Menu mode.
$!         LAT nodename
$! DECNET: Creates a Local DECterm Window using a SET HOST command.
$!         Only valid in Menu mode.
$!         DECNET nodename
$! RDT:    Creates a Remote Decterm Window.
$!         RDT nodename [init_state] [title] [dcl_command]
$!                      [display] [transport] [x:y]
$! RFV:    Creates a Remote Fileview Window.
$!         RFV nodename [init_state] [display] [transport]
$! RDW:    Runs a Remote Remote Decwindows Application.
$!         RDW nodename command [display] [transport]
$! DWRT:   Starts a second DWremterm Window.
$!         DWRT nodename [init_state] [display] [transport]
$! $:      Executes a local DCL command
$! For further details refer to sys$library:dwremterm.hlb.
$! 
$! A unique window title and icon name is chosen by referencing the user's
$! process name. If the login sequence changes the process name then duplicate
$! window and icon names may occur.
$! 
$! Refer to SYS$HELP:DWREMTERM.HLP for further help by typing:-
$! @VUE$LIBRARY:DWREMTERM DWHELP.
$!
$! CONFIGURATION SETUPS:
$!   The following logicals can be used to alter default DWremterm behaviour.
$!   DWREMTERM_INHIBIT_COPY to prevent a user being asked whether a new version
$!     of DWremterm should be copied.
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
$!
$! TROUBLESHOOTING HINTS:
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
$! o DWREMTERM_'pid'*.COM temporary file on the remote node shows the created
$!     command file.
$! o DWREMTERM.LOG temporary file on on the remote node showing the output
$!     from dwremterm_'pid'.com
$! o If you have problems creating DECterms then get the latest DECterm patch
$!     from your CSC.
$!
$!******************************************************************************
$! First set up a few symbols.
$
$ define/nolog dwremterm_inhibit_copy true
$ if f$trnlnm("''f$parse(f$env(""procedure""),,,""name"")'_Verify") .or. -
     f$trnlnm("Verify_All") then set verify
$ set symbol/scope=(nolocal)
$
$ ! verify can be enabled by defining dwremterm_verify. This will also enable
$ ! verbose mode
$ verbose := false 
$ if f$trnlnm("dwremterm_verify")  then verbose := true
$
$ ! verbose can be enabled by defining dwremterm_verbose or dwremterm_verify
$ ! or prefixing the version with an "X"
$ if f$trnlnm("dwremterm_verbose") then verbose := true
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
$!   DWremterm 
$!
$ netobj = f$extract(f$length(f$trnlnm("sys$net"))-11,10,f$trnlnm("sys$net"))
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
$ IF p1 .eqs. "" .or. p1 .eqs. "DWREMTERM" 
$ THEN
$   ! if we are in verbose mode then bring up full sized window by 
$   ! over-riding the window size specified in the resource file.
$   window_size = ""
$   if verbose then window_size = ",column=80,row=24"
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
$   create/term/wait/reso='resource_file' -
          /wind=(title="''DWnodename'""''dwusername'"" DWremterm V''version'",-
                 icon="DWremterm ''DWnodename'""''dwusername'""" -
                 'window_size''window_position''window_state')-
          @'f$envi("PROCEDURE") MENU_MODE         !re-execute this program
$   status = $status
$   restore_verify = f$verify(saved_verify)
$   exit 'status'
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
$ call names "DWremterm" 
$ ! we should now have our process name
$ if verbose then say Dnodename," Setting process name to: ",dprcnam
$ if sprcnam .nes. "" then set process'sprcnam'
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
$ IF verb-"LAT"-"RDW"-"RDT"-"RFV"-"DWRT"-"DWHELP"-"HELP"-"EXIT"-"DECNET"-"$" -
     .nes. ""
$ THEN
$   SAY "Invalid Command: Valid Commands are - "
$   SAY "LAT,DECNET,RDT,RFV,RDW,DWRT,$, HELP and DWHELP"
$   if MENU_MODE then goto menu_loop
$   restore_verify = f$verify(saved_verify)
$   exit
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
$   say "If you EXIT, then windows created by the LAT and DECNET"
$   say "commands will be terminated. If you really want to exit"
$   say "then use the EXIT option from the FILE Pull-Down-Menu"
$   if MENU_MODE then goto menu_loop
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "HELP"
$ THEN
$   say "Valid Commmands are: "
$   say "LAT,DECNET,RDT,RFV,RDW,DWRT,$, HELP and DWHELP."
$   say "Enter Command only for Brief Help or DWHELP for Full Help."
$   if MENU_MODE then goto menu_loop
$   restore_verify = f$verify(saved_verify)
$   exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "DWHELP"
$ THEN
$   create/term/nowait/window= -
          (title="DWremterm Help",icon="DWHelp",rows=24,columns=80) -
          help/nouser/library=sys$help:dwremterm.hlb DWremterm
$   if MENU_MODE then goto menu_loop
$   restore_verify = f$verify(saved_verify)
$   exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "LAT" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command creates a LAT window to a remote system."
$     say "It is only valid in MENU mode."
$     say "LAT nodename"
$     if MENU_MODE then goto menu_loop
$     restore_verify = f$verify(saved_verify)
$     exit
$   ELSE
$     nodename == f$extract(0,f$locate("""",nodename),nodename)
$     bnodename == f$extract(0,f$locate("""",bnodename),bnodename)
$   ENDIF
$
$   IF .not. MENU_MODE
$   THEN
$     say "The LAT command is only valid in MENU MODE." 
$     restore_verify = f$verify(saved_verify)
$     exit
$   ENDIF
$
$   IF bnodename .eqs. dwnodename
$   THEN
$     IF f$search("sys$system:ltpad_v2.exe") .eqs. ""
$     THEN 
$       say "You cannot use LATmaster to connect to your own node." 
$       say "Use DECterm for local windows."
$       if MENU_MODE then goto menu_loop
$       restore_verify = f$verify(saved_verify)
$       exit
$     ENDIF
$   ENDIF
$
$   ! tell user to use the DECNET command if LATMASTER is not running
$   IF .not. f$getdvi("LTA0:","exists")
$   THEN
$     Say "The LAT software has not been started on this node."
$     Say "Use the DWremterm DECnet command."
$     if MENU_MODE then goto menu_loop
$     restore_verify = f$verify(saved_verify)
$     exit
$   ENDIF
$
$   ! tell user to use the DECNET command if LATMASTER is not installed.
$   define/user/nolog sys$error nl:
$   define/user/nolog sys$output nl:
$   set host/lat/noautoprompt
$   status = $status
$   deassign/user sys$error
$   deassign/user sys$output
$   IF status .eqs. "%X00038240"
$   THEN
$     Say "LATmaster is not installed on this node."
$     Say "Use the DWremterm DECnet command."
$     if MENU_MODE then goto menu_loop
$     restore_verify = f$verify(saved_verify)
$     exit
$   ENDIF
$   
$   call names 'nodename' "LAT"
$   disc_char = ""
$   break_char = ""
$   if f$trnlnm("LAT$DISCONNECT_CHARACTER") .nes. "" then -
      disc_char = "/DISC="+f$extr(0,1,f$trnlnm("LAT$DISCONNECT_CHARACTER"))
$   if f$trnlnm("LAT$BREAK_CHARACTER") .nes. "" then -
      break_char = "/BREAK="+f$extr(0,1,f$trnlnm("LAT$BREAK_CHARACTER"))
$   create/term/nowait'prcnam' -
          /window=(title="''name'",icon="''name'") -
          SET HOST/LAT/AUTOCONNECT'disc_char''break_char' 'bnodename'
$   if MENU_MODE then goto menu_loop
$   restore_verify = f$verify(saved_verify)
$   exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ IF verb .eqs. "DECNET" 
$ THEN
$   IF nodename .eqs. "" 
$   THEN
$     say "This command creates a DECNET window to a remote system."
$     say "It is only valid in MENU mode."
$     say "DECNET nodename"
$     if MENU_MODE then goto menu_loop
$     restore_verify = f$verify(saved_verify)
$     exit
$   ELSE
$     nodename == f$extract(0,f$locate("""",nodename),nodename)
$   ENDIF
$
$   IF .not. MENU_MODE
$   THEN
$     say "The DECNET command is only valid in MENU MODE."  
$     restore_verify = f$verify(saved_verify)
$     exit
$   ENDIF
$
$   call names 'nodename' "DECNET"
$   create/term/nowait'prcnam' -
          /window=(title="''name'",icon="''name'") -
          SET HOST 'nodename' 
$   if MENU_MODE then goto menu_loop
$   restore_verify = f$verify(saved_verify)
$   exit
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
$     '$_command'
$   ENDIF
$   if MENU_MODE then goto menu_loop
$   restore_verify = f$verify(saved_verify)
$   exit
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
$   restore_verify = f$verify(saved_verify)
$   exit
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
$   restore_verify = f$verify(saved_verify)
$   exit
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
$   restore_verify = f$verify(saved_verify)
$   exit
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
$   restore_verify = f$verify(saved_verify)
$   exit
$ ENDIF
$
$!------------------------------------------------------------------------------
$ goto menu_loop	!catchall, should never get here
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
$ ELSE
$   Command    := 'P1	       !(Required)
$ ENDIF
$   Nodename    = 'P2	       !(Required)
$!
$! REMOTE DECTERM INPUTS
$!
$ IF verb .eqs. "RDT"
$ THEN 
$   Init_State = 'P3	       !(Optional, Default = window) icon/window
$   title      = 'P4	       !(Optional, Default ='nodename':R'count':'trans')
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
$ !set decw$display_* symbols for current window
$ define/user/nolog sys$error  nl:
$ define/user/nolog sys$output nl:
$ show display/symbol		! undocumented command
$
$ ! if a DECwindow display is not currently defined or specified then exit
$ IF display .eqs. "" .and  f$type(decw$display_node) .eqs. ""
$ THEN
$   say "ERROR: Decwindows display not defined or specified."
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
$       goto open_server
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
$     ! if transport type node defined then assume decnet
$     if f$type(decw$display_transport) .eqs. "" then -
         decw$display_transport := DECNET
$     decw$display_transport = f$edit(decw$display_transport,"upcase,trim")
$     ! LOCAL transport uses a Global Section which is more efficient than
$     ! network transports
$     if decw$display_node .eqs. bnodename then -
         decw$display_transport := LOCAL
$     if decw$display_node .nes. bnodename .and. -
         decw$display_transport .eqs. "LOCAL" then -
         decw$display_transport := DECNET
$     if f$extract(0,3,decw$display_node) .eqs. "LAT" then -
         decw$display_transport := LAT
$   ELSE
$     decw$display_transport = transport
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
$   window_size := ,x_position='x_pos',y_position='y_pos'
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
$
$!------------------------------------------------------------------------------
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
$ pa == f$edit('sb'p1,"upcase")
$ pb == f$edit('sb'p2,"upcase")
$ pc == f$edit('sb'p3,"upcase")
$ pd == f$edit('sb'p4,"upcase")
$ pe == f$edit('sb'p5,"upcase")
$ pf == f$edit('sb'p6,"upcase")
$ pg == f$edit('sb'p7,"upcase")
$ ph == f$edit('sb'p8,"upcase")
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
$ ! ensure nodename is suffixed with double a double colon 
$ if nodename .nes. "" .and. -
     f$extract(f$length(nodename)-2,2,nodename) .nes. "::" then -
     nodename == nodename + "::"
$ ! ensure bnodename is not suffixed with double colon 
$ if nodename .nes. "" .and. -
     f$extract(f$length(nodename)-2,2,nodename) .eqs. "::" then -
     bnodename == f$extract(0,f$length(nodename)-2,nodename)
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
$ ! DWremterm_inhibit_copy
$ if f$trnlnm("dwremterm_inhibit_copy") then goto copy_file_exit
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
$! THIS SUBROUTINE IS USED TO GENERATE A UNIQUE PROCESS NAME, WINDOW TITLE
$! AND ICON NAME.
$!
$NAMES: SUBROUTINE
$! determine a unique process name and use this to set the window title
$! and icon name. Note: for this to work the user's login.com should not
$! manually set their process name.
$! The process name is of the form:-
$! DWremterm:L'count' 		!for DWremterm menu window
$! nodename:L'count':verb	!for local  proccess's
$! nodename:R'count':verb	!for remote proccess's
$! The verb suffix is omitted on long nodenames such as VT1200 LAT nodenames.
$!
$ set noon
$ prefix = p1
$ suffix = p2
$ index = 0
$ state := L					!Local process
$ if f$mode() .eqs. "NETWORK" then state := R	!Remote process
$ saved_prcnam = f$getjpi("","prcnam")
$
$proc_loop:
$ index = index + 1
$ ! trap out if we are in a endless loop
$ if index .gt. 100 then index = "?"
$
$ ! shorten LAT nodenames
$ if f$edit(f$extract(0,4,prefix),"upcase,trim") .eqs. "LAT_" then -
     prefix := LAT'f$extract(10,6,prefix)
$
$ ! shorten ADSP MACINTOSH node names
$ if f$length(prefix) .ne. f$loc(":",prefix) then -
      prefix = f$extract(0,f$loc(":",prefix),prefix)
$ 
$ IF f$locate("LAT",prefix) .ne. f$length(prefix) .and. -
     suffix .eqs. "LAT" .or. suffix .eqs. ""
$ THEN
$    ! omit suffix if a LAT nodename
$    name == "''prefix':''state'''index'"
$ ELSE
$    name == "''prefix':''state'''index':''suffix'"
$ ENDIF
$
$ !don't allow us to re-use our current process name
$ if saved_prcnam .eqs. name then goto proc_loop
$ if index .eqs. "?" then goto got_name
$
$ ! if process name is already in use the return and try another process name.
$ on error then goto proc_loop
$ define/user/nolog sys$error nl:
$ define/user/nolog sys$output nl:
$ set process/name="''f$extract(0,15,name)'"
$ !we are here because we have a unique process name
$ set noon	
$ !return process name to the default value.
$ set process/name="''saved_prcnam'"
$
$got_name:
$ set noon
$ IF index .eqs. "?"
$ THEN
$   ! if a unique process name was not found in 100 passes the return blank.
$   dprcnam == ""  !name for verbose message
$   sprcnam == ""  !name for set proc/name
$   prcnam  == ""  !name for create/term/prcnam
$ ELSE
$   dprcnam == f$extract(0,15,name)
$   sprcnam == "/name=""''f$extract(0,15,name)'"""
$   prcnam  == "/process=""''f$extract(0,15,name)'"""
$ ENDIF
$
$ exit 1
$ ENDSUBROUTINE        !end of names subroutine
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
$ IF DWusername .eqs. "DECNET"
$ THEN 
$   IF rem_command .nes. "DECTERM" .or. rem_dcl_command .nes. ""
$   THEN
$     Write_Source -
          "ERROR: You are not Authorized to use the DECNET account..."
$     Say "ERROR: You are not Authorized to use the DECNET account..."
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
$ Say "Received Title is: ",Rem_title
$! ensure lowercase characters are maintained
$ if f$locate(" ",rem_title) .ne. f$length(rem_title) .and. - 
     f$extract(0,1,rem_title) .nes. """" then rem_title = """''rem_title'"""
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
$   Say "Received Window Location is: ",rem_position,boff
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
$ ! default process name is nodename:Rcount:transport
$ call names 'rem_node' 'verb'
$
$! temp bug workaround
$ ctx = ""
$ tmp = f$cont("PROCESS",ctx,"PRCNAM",dwusername,"EQL")
$ IF f$pid(ctx) .eqs. ""
$ THEN 
$   ! there is a bug whereby decterms will not correctly set the process name
$   ! if a process name containing the username does not already exist.
$   if verbose then Write_Source bon,Dnodename,-
     " DECterm bug found - setting network proccess name to overcome error",boff
$   Say -
      "DECterm bug found - setting network proccess name to overcome error"
$   set noon
$   define/user sys$error nl:
$   define/user sys$output nl:
$   set proc/name='dwusername'
$   on warning then goto remote_error	!error handler
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
$   ! default icon and title names is of the form nodename:Rcount:transport
$   if rem_title .eqs. "" then rem_title := -
      'DWnodename':'f$ext(f$loc(f$elem(1,":",name),name),2,name):'rem_transport'
$   if prcnam .eqs. "" then prcnam = """-""!"
$! It would be a security hole to allow a remote user to log on as DECnet.
$! Therefore the only command we allow to execute from the DECnet account
$! is an RDT command, but with the /nologged_in qualifier. This will
$! prompt the user for a username and password.
$   IF DWusername .eqs. "DECNET" 
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
$     Write_Source bon,-
      "  $ Create/Terminal''dwwait'''detached'''loggedin'",prcnam,"-"
$     Write_Source -
      "          /Window_att=(title=",rem_title,"-"
$     Write_Source -
      "                       Icon_name=",Rem_title,"-"
$     Write_Source -
      "                       Initial_State=",Rem_Init_State,"-"
$     Write_Source -
      "                       ''Rem_Position') ",Rem_Dcl_Command,boff
$   Endif
$
$   Say -
      "Window Created with the following commands:-"
$   Say -
      "  $ Create/Terminal''dwwait'''detached'''loggedin'",prcnam,"-"
$   Say -
      "          /Window_att=(title=",rem_title,"-"
$   Say -
      "                       Icon_name=",Rem_title,"-"
$   Say -
      "                       Initial_State=",Rem_Init_State,"-"
$   Say -
      "                       ''Rem_Position') ",Rem_Dcl_Command
$
$   IF Rem_Dcl_Command .eqs. "" 
$   THEN
$     !due to a bug in DECwindows V2.x a failure in /DETACHED will not return
$     !the error in $STATUS. This is fixed in DECwindows MOTIF.
$     Create/Terminal'dwwait''Detached''loggedin''prcnam' -
            /Window_Att=(Title='Rem_title',Icon_name='Rem_title', -
                         Initial_State='Rem_Init_State''rem_position') 
$     goto network_close
$   ELSE
$     execute_command  = -
        "Create/Terminal''dwwait'''Detached'''loggedin'" + prcnam +-
        "/Window_Att=(Title=" + Rem_title + ",Icon_name=" + Rem_title +-
        ",Initial_State=" + Rem_Init_State + rem_position + ")" + " " +-
         Rem_Dcl_Command
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
$! this creates a command procedure which is read by the detached process
$! The detached job is done for two reasons:-
$!  - to prevent the remove job tying up the local interactive users process
$!  - to allow jobs that issue a spawn to work 
$!       (spawn jobs do not work from a network process)
$!
$ i = 0
$REM_FILENAME_LOOP:
$ i = i+1
$ outfile := -
    'f$trnlnm("sys$login")'dwremterm_'f$ext(4,4,f$getj("","pid"))'_'i'.tmp;1
$ if f$search(outfile,1) .nes. "" then goto rem_filename_loop
$ open/write out 'outfile'
$ write out "$!*** this is a temporary file created by dwremterm.com ***"
$ write out "$ set noverify"
$ write out "$ set noon"
$ write out "$! set process name"
$ write out "$ i = 0
$ write out "$nameloop:
$ write out "$ i = i + 1
$ write out "$ define/user/nolog sys$error nl:
$ write out "$ define/user/nolog sys$output nl:
$ write out "$ set process/name="+-
        "''f$edit(f$extr(0,7,f$getj("","username")),"trim,lowercase")'_dwrt_'i'"
$ write out "$ if .not. $status then goto nameloop
$ write out "$! we are here because we have a unique process name
$ write out "$ Set Display/Create/Node=",Rem_Node,"/Trans=",Rem_Transport,-
            "/Screen=",Rem_Screen,"/Server=",Rem_Server
$ write out "$ write sys$output ""Starting Command"""
$ write out "$ type sys$input"
$ write out "$ deck/dollars=""^z"""
$ write out "$ ",execute_command
$ write out "^z"
$ write out "$ ",execute_command
$ write out "$ write sys$output ""Finished Command"""
$ write out "$loop:"
$ write out "$! this loop prevents the program from exiting if a subprocess "+-
            "exists"
$ write out "$ wait 00:00:15"
$ write out "$ if f$getjpi("""",""prccnt"") .ne. 0 then goto loop"
$ write out "$ delete/nolog 'f$env(""procedure"")'"
$ write out "$ purge/keep=2/nolog sys$login:dwremterm.log"
$ write out "$ exit"
$ close out
$!
$ if verbose then Write_Source bon,Dnodename,"Starting Detached Process",boff
$ Say "Starting Detached Process"
$ Run/detached/authorize sys$system:loginout.exe -
     /input='outfile'/output='f$trnlnm("sys$login")'dwremterm.log 
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
$   Write_Source/error=send_mail -
        "Remote Application Error Occured, Status: ",Status
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
