$!*****************************************************************************
$!*                                                                           *
$!*                                                                           *
$!*  This version is fixed to correctly re-set the time.                      *
$!*  It is intended for distribution on the FreeWare CD-ROM with OpenVMS V7.3.*
$!*                                                                           *
$!*  See changes to values set in TIME$ADJ at approximately lines 270-280.    *
$!*                                                                           *
$!*                                                                           *
$!*****************************************************************************
$!*                                                                           *
$!*  SYS$EXAMPLES:DAYLIGHT_SAVINGS.COM                                        *
$!*                                                                           *
$!* Copyright 2000 Compaq Computer Corporation                                *
$!*                                                                           *
$!* Compaq and the Compaq logo Registered in U.S. Patent and Trademark Office.*
$!*                                                                           *
$!* Confidential computer software. Valid license from Compaq required for    *
$!* possession, use or copying. Consistent with FAR 12.211 and 12.212,        *
$!* Commercial Computer Software, Computer Software Documentation, and        *
$!* Technical Data for Commercial Items are licensed to the U.S. Government   *
$!* under vendors standard commercial license.                                *
$!*                                                                           *
$!*****************************************************************************
$!
$!  The following $DECK/$EOD prevents the comments/instructions
$!  from being removed if this procedure is DCLDIETed.
$   goto after_deck
$DECK
$!  This procedure is useful to change between Daylight Savings Time
$!  and Standard Time when automatic OpenVMS DST/STD changing and
$!  DTSS are not in use.
$!
$!  It will either ADD or SUBTRACT one hour from the system time.
$!
$!  It will re-set the system Time Differential Factor,
$!      including
$!          the logical name, SYS$TIMEZONE_DIFFERENTIAL,
$!          the system cell, EXE$GQ_TDF, and
$!          the file SYS$SYSTEM:SYS$TIMEZONE.DAT.
$!
$!  The following logical names are NOT [re]set and will be deassigned
$!  if they are present (They are not present on VAX and pre 7.3 Alpha
$!  OpenVMS systems unless DTSS is in use.)
$!
$!      SYS$TIMEZONE_DAYLIGHT_SAVING
$!      SYS$TIMEZONE_NAME
$!      SYS$TIMEZONE_RULE
$!
$!  (If you need these, please use automatic OpenVMS DST/STD changing or DTSS)
$!
$!  This procedure creates a command procedure, DST$CHANGE.COM, in the
$!  current directory.  The created procedure can execute on the current
$!  node only, or SYSMAN can be used to execute on all nodes in the cluster.
$!  If you choose to use SYSMAN, a second procedure, DST$SYSMAN.COM is
$!  created to execute DST$CHANGE.COM by using a SYSMAN DO command.
$!
$!  You can choose to execute immediately; to queue a job for future execution;
$!  or to save for possible modification.  If you choose to queue a job,
$!  it will be submitted to the SYS$BACH queue; if this is NOT satisfactory,
$!  save the procedure(s) and manually submit to the correct queue.
$!
$!  INPUT PARAMETERS
$!
$!      All parameters are optional.
$!      If parameters are not provided, input will be promoted
$!      and read from SYS$OUTPUT (normally the terminal).
$!
$!      P1  DAYLIGHT or STANDARD -- which you are changing _to_
$!      P2  NODE or CLUSTER -- changing this NODE only or the entire CLUSTER
$!      P3  EXECUTE, QUEUE or SAVE -- EXECUTE now, QUEUE for later
$!                                    or just SAVE the sysman procedure(s)
$!      P4  <date-time> -- time to run if QUEUED; unused otherwise
$!
$!  To execute this command procedure interactively, enter:
$!
$!          $ @SYS$EXAMPLES:DAYLIGHT_SAVINGS
$!
$!  and respond to the prompts.
$!
$!  To execute this command procedure with input provided by parameters,
$!  enter:
$!
$!          $ @SYS$EXAMPLES:DAYLIGHT_SAVINGS -
$!          _$      [ DAYLIGHT | STANDARD ] -
$!          _$      [ NODE | CLUSTER ] -
$!          _$      [ EXECUTE | QUEUE <time> | SAVE ]
$!
$!  The above example is shown on multiple lines for clarity; the command
$!  may be entered on a single line.  Parameters (except time) may be
$!  shortened -- only the first letter of each parameter is required.
$!
$EOD
$AFTER_DECK:
$!
$!
$!  Make sure DCL verbs aren't unexpected symbols
$!
$   set = "set"     ! in case "SET" is a symbol
$   set symbol /scope=(nolocal,noglobal)
$!
$   on control_y then goto y_exit
$   on warning then goto err_exit
$!
$   Say := write sys$output
$!
$!
$!  Warn if DTSS is in use
$!
$!  Look for process DTSS$CLERK or DTSS$SERVER to determine if
$!  DTSS is present and active.
$!
$!  If the following PIPE command succeeds in locating DTSS$...
$!      then  $STATUS will be %X10000001.
$!      For "no strings matched", $STATUS will be %X18D78053
$!
$   define sys$output nl:
$   set noon
$   pipe show system | search sys$input "DTSS$CLERK", "DTSS$SERVER"
$   dtss$status = $status
$   set on
$   deassign sys$output
$   if dtss$status .eq. %X10000001
$   then                                        ! DTSS found
$       say ""
$       say "DTSS is in use."
$       say "DTSS will make changess between DST and STD time."
$       say ""
$       say "The procedure(s) created by this procedure WILL NOT"
$       say "make any changess on nodes on which DTSS is in use."
$       say ""
$       say "(Press Ctrl-Y to abort.)"
$       say ""
$   endif
$!
$!
$!  Warn if AUTO_DLIGHT_SAV is set
$!
$   if f$getsyi("ARCH_NAME") .eqs. "Alpha"
$   then    ! check for a minimumum version of 7.3"
$       define sys$output nl:
$       set noon
$       product show product VMS /span_version=minimum=A7.3
$       sav$status = $status
$       set on
$       deassign sys$output
$       if sav$status .eqs. "1"
$       then
$           if f$getsyi("AUTO_DLIGHT_SAV")
$           then                            ! AUTO_DLIGHT_SAV set
$               say ""
$               say "The system parameter AUTO_DLIGHT_SAV is set."
$               say "OpenVMS will make changess between DST and STD time."
$               say ""
$               say "The procedure(s) created by this procedure WILL NOT"
$               say "make any changes on nodes on which AUTO_DLIGHT_SAV is set."
$               say ""
$               say "(Press Ctrl-Y to abort.)"
$               say ""
$           endif
$       endif
$   endif
$!
$!
$ASK_DST_STD:
$!
$   dst$std = "X"
$   if p1 .nes. ""
$   then
$       say "Changing to ''p1'"
$       ans = p1
$       goto p1_supplied
$   endif
$   say ""
$   read sys$output ans /prompt= -
        "Are you changing to DAYLIGHT or to STANDARD time? (DAYLIGHT/STANDARD): "
$   if ans .eqs. "^" then exit
$   if ans .eqs. "" then ans = "#"
$P1_SUPPLIED:
$   ans = f$edit(ans,"TRIM,UPCASE")
$   if f$locate(ans,"DAYLIGHT/STANDARD") .eq. 0 then dst$std = "D"
$   if f$locate(ans,"DAYLIGHT/STANDARD") .eq. 9 then dst$std = "S"
$   if ( (dst$std .nes. "D") .and. (dst$std .nes. "S") )
$   then
$       say ""
$       say "*** You must answer DST or STD!"
$       say ""
$       p1 = ""
$       goto ask_dst_std
$   endif
$!
$ASK_LOCAL_CLU:
$!
$   loc$clu = "X"
$   if p2 .nes. ""
$   then
$       say "Change will be made on ''p2'"
$       ans = p2
$       goto p2_supplied
$   endif
$   say ""
$   read sys$output ans /prompt= -
        "Will the sysman be on this NODE only or CLUSTER wide? (NODE/CLUSTER): "
$   if ans .eqs. "^" then goto ask_dst_std
$   if ans .eqs. "" then ans = "#"
$P2_SUPPLIED:
$   ans = f$edit(ans,"TRIM,UPCASE")
$   if f$locate(ans,"NODE/CLUSTER") .eq. 0 then loc$clu = "N"
$   if f$locate(ans,"NODE/CLUSTER") .eq. 5 then loc$clu = "C"
$   if ( (loc$clu .nes. "N") .and. (loc$clu .nes. "C") )
$   then
$       say ""
$       say "*** You must answer NODE or CLUSTER!"
$       say ""
$       p2 = ""
$       goto ask_local_clu
$   endif
$!
$ASK_EX_Q_SAV:
$!
$   ex$q$sav = "X"
$   if p3 .nes. ""
$   then
$       say "Procedure action is ''p3'"
$       ans = p3
$       goto p3_supplied
$   endif
$   say ""
$   say "If you choose to SAVE the procedures, you can modify"
$   say "the SYSMAN environment, or make other changess before executing."
$   say ""
$   say "Do you wish to EXECUTE the change now, QUEUE it for future execution,"
$   say "or SAVE the created procedures without executing?"
$   read sys$output ans /prompt="    (EXECUTE/QUEUE/SAVE): "
$   if ans .eqs. "^" then goto ask_local_clu
$   if ans .eqs. "" then ans = "#"
$P3_SUPPLIED:
$   ans = f$edit(ans,"TRIM,UPCASE")
$   if f$locate(ans,"EXECUTE/QUEUE/SAVE") .eq. 0 then ex$q$sav = "X"
$   if f$locate(ans,"EXECUTE/QUEUE/SAVE") .eq. 8 then ex$q$sav = "Q"
$   if f$locate(ans,"EXECUTE/QUEUE/SAVE") .eq. 14 then ex$q$sav = "S"
$   if  ( (EX$Q$SAV .nes. "X") -
    .and. (EX$Q$SAV .nes. "Q") -
    .and. (EX$Q$SAV .nes. "S") )
$   then
$       say ""
$       say "*** You must answer EXECUTE, QUEUE or SAVE!"
$       say ""
$       p3 = ""
$       goto ask_ex_q_sav
$   endif
$!
$!
$!  Create the DST$CHANGE procedure.
$!
$!  Get the current TDF in seconds
$   log$tdf = f$integer(f$trnlnm("SYS$TIMEZONE_DIFFERENTIAL"))
$!  convert to minutes
$   new$tdf = log$tdf / 60
$!  Check that it is an integral numer of minutes
$   if new$tdf * 60 .ne. log$tdf
$   then
$       say ""
$       say "*** The value of the logical name SYS$TIMEZONE_DIFFERENTIAL"
$       say "*** is not an integral multiple of 60 (seconds per minute)!"
$       say ""
$       exit
$   endif
$!
$!  Set the new TDF and the time adjustment
$   if dst$std .eqs. "D"
$   then
$       new$tdf = new$tdf + 60
$       time$adj = +60
$   endif
$   if dst$std .eqs. "S"
$   then
$       new$tdf = new$tdf - 60
$       time$adj = -60                                  
$   endif                                                          
$!                    
$!
$!  Write DST$CHANGE.COM, the procedure that will make the time change.
$!
$   close/error=open_change dst$change      ! Make sure it wasn't left open
$open_change:
$!
$!  Use CREATE to get desired record format
$   create DST$CHANGE.COM
$!
$   open/append dst$change DST$CHANGE.COM
$!
$   if dst$std .eqs. "D"
$   then
$       write dst$change -
    "$!  This procedure changes from Standard time to Daylight Saving time."
$   endif
$!
$   if dst$std .eqs. "S"
$   then
$       write dst$change -
    "$!  This procedure changess from Daylight Saving time to Standard time."
$   endif
$!
$   write dst$change -
    "$!      Created by DAYLIGHT_SAVINGS.COM ''f$time()'"
$!
$   close dst$change
$!
$   append sys$input dst$change.com
$DECK
$!  This procedure can be executed directly, or by a SYSMAN DO command
$!
$   Say := write sys$output
$!
$   arch$ = " "     ! Make sure the symbol is defined
$!
$   if f$getsyi("ARCH_NAME") .eqs. "Alpha"
$   then    ! check for a minimumum version of 7.3
$       define sys$output nl:
$       set noon
$       product show product VMS /span_version=minimum=A7.3
$       if $status .eqs. "1"
$       then
$           arch$ = "NEW_ALPHA"
$       else
$           arch$ = "OLD_ALPHA"
$       endif
$       set on
$       deassign sys$output
$   endif
$!
$   if f$getsyi("ARCH_NAME") .eqs. "VAX"
$   then
$       arch$ = "VAX"
$   endif
$!
$   define sys$output nl:
$   set noon
$   pipe show system | search sys$input "DTSS$CLERK", "DTSS$SERVER"
$   dtss$status = $status
$   set on
$   deassign sys$output
$   if dtss$status .eq. %X10000001
$   then                                        ! DTSS found
$       say ""
$       say "DTSS is in use."
$       say "DTSS will make changes between DST and STD time."
$       say ""
$       say "This procedure will exit without making any changes."
$       say ""
$       exit
$   endif
$!
$   if f$getsyi("ARCH_NAME") .eqs. "Alpha"
$   then    ! check for a minimumum version of 7.3"
$       define sys$output nl:
$       set noon
$       product show product VMS /span_version=minimum=A7.3
$       if $status .eqs. "1"
$       then
$           if f$getsyi("AUTO_DLIGHT_SAV")
$           then                            ! AUTO_DLIGHT_SAV set
$               say ""
$               say "The system parameter AUTO_DLIGHT_SAV is set."
$               say "OpenVMS will make changes between DST and STD time."
$               say ""
$               say "This procedure will exit without making any changes."
$               say ""
$               deassign sys$output
$               exit
$           endif
$       endif
$       set on
$       deassign sys$output
$   endif
$!
$   if  ( (arch$ .nes. "NEW_ALPHA") -
    .and. (arch$ .nes. "OLD_ALPHA") -
    .and. (arch$ .nes. "VAX") )
$   then
$       say ""
$       say "*** Error selecting architecture (Alpha/VAX)"
$       say "*** Standard to/from Daylight time change not made"
$       say ""
$       exit
$   endif
$!
$   if arch$ .eqs. "NEW_ALPHA"
$   then -
$EOD
$!
$!
$   open/append dst$change  DST$CHANGE.COM
$   write dst$change -
"        @sys$manager:utc$time_setup.com """" ""TDF"" ''new$tdf' ''time$adj'"
$   close dst$change
$!
$!
$   append sys$input dst$change.com
$DECK
$   endif
$!
$   if (  (arch$ .eqs. "VAX") -
    .or.  (arch$ .eqs. "OLD_ALPHA") )
$   then -
$EOD
$!
$!
$   open/append dst$change DST$CHANGE.COM
$   write dst$change -
"        @sys$manager:utc$configure_tdf.com ""SET"" ''new$tdf' ''time$adj'"
$   close dst$change
$!
$!
$   append sys$input dst$change.com
$DECK
$   endif
$!
$!  The following logical names are NOT [re]set and will be deassigned
$!  if they are present
$!
$!      SYS$TIMEZONE_DAYLIGHT_SAVING
$!      SYS$TIMEZONE_NAME
$!      SYS$TIMEZONE_RULE
$!
$   if f$trnlnm("SYS$TIMEZONE_DAYLIGHT_SAVING","LNM$SYSTEM","EXECUTIVE") .gts. "" then -
        deassign /system /executive SYS$TIMEZONE_DAYLIGHT_SAVING
$   if f$trnlnm("SYS$TIMEZONE_NAME","LNM$SYSTEM","EXECUTIVE") .gts. "" then -
        deassign /system /executive SYS$TIMEZONE_NAME
$   if f$trnlnm("SYS$TIMEZONE_RULE","LNM$SYSTEM","EXECUTIVE") .gts. "" then -
        deassign /system /executive SYS$TIMEZONE_RULE
$!
$   exit
$EOD
$!
$!
$   say "Created file ''f$parse("DST$CHANGE.COM")'"
$!
$!
$!  Write DST$SYSMAN.COM, if this is being done cluster wide."
$!  This procedure will use SYSMAN to exeucte DST$CHANGE.COM"
$!
$   if loc$clu .nes. "C" then goto after_write_sysman
$!
$   close/error=open_sysman dst$sysman      ! Make sure it wasn't left open
$open_sysman:
$!  Use CREATE to get desired record format
$   create DST$SYSMAN.COM
$!
$   open/append dst$sysman dst$sysman.com
$!
$   write dst$sysman -
    "$!  This procedure executes DST$CHANGE.COM using a SYSMAN DO command."
$!
$   write dst$sysman -
    "$!      Created by DAYLIGHT_SAVINGS.COM ''f$time()'"
$   write dst$sysman -
    "$   mcr sysman"
$   write dst$sysman -
    "$DECK"
$   write dst$sysman -
    "        set environment /cluster"
$   write dst$sysman -
    "        do @''f$parse("DST$CHANGE.COM")'"
$   write dst$sysman -
    "        exit"
$   write dst$sysman -
    "$EOD"
$   write dst$sysman -
    "$   exit"
$!
$   close dst$sysman
$!
$   say "Created file ''f$parse("DST$SYSMAN.COM")'"
$!
$!
$after_write_sysman:
$!
$   if ex$q$sav .nes. "Q" then goto after_q
$!
$ask_q_time:
$   if p4 .nes. ""
$   then
$       say "Procedure will be queuend after ''p4'"
$       ans = p4
$       goto p4_supplied
$   endif
$   say "Enter the time at which the queued procedure will execute."
$   say "(Default is 2:00 AM tomorrow.)"
$   read sys$output ans /prompt= "    (DD-MMM-YYYY HH:MM): "
$   if ans .eqs. "^" then goto ask_ex_q_sav
$   if ans .eqs. "" then ans = "TOMORROW+2:00"
$P4_SUPPLIED:
$   set noon
$   after$time = "/AFTER=""''f$cvtime(ans,"ABSOLUTE")'"""
$   after$severity = $severity
$   set on
$   if .not. after$severity
$   then
$       p4 = ""
$       goto ask_q_time
$   endif
$!
$   if loc$clu .eqs. "N"
$   then                        ! Queue DST$CHANGE.COM
$       submit DST$CHANGE.COM 'after$time'
$   endif
$!
$   if loc$clu .eqs. "C"
$   then                        ! Queue DST$SYSMAN.COM
$       submit DST$SYSMAN.COM 'after$time'
$   endif
$!
$after_q:
$!
$!
$   if ex$q$sav .nes. "X" then goto after_x
$!
$ask_x_ok:
$!
$!  Skip the O.K. question if all params were input and valid
$   if ( (p1 .gts. "") .and. (p2 .gts. "") .and. (p3 .gts. "") ) -
        then goto after_ask_ok
$!
$   say ""
$   read sys$output ans /prompt = -
        "Is it O.K. to execute the time sysman now? (Yes/No): "
$   if ans .eqs. "^" then goto ask_ex_q_sav
$   ans = f$edit(ans,"TRIM,UPCASE")
$!
$   if f$locate(ans,"YES/NO") .eq. 4
$   then
$       say ""
$       say "The created procedure(s) will be saved but not executed."
$       say ""
$       exit
$   endif
$!
$   if f$locate(ans,"YES/NO") .ne. 0
$   then
$       say ""
$       say "*** You must enter YES or NO!"
$       say ""
$       goto ask_x_ok
$   endif
$!
$AFTER_ASK_OK:
$!
$   if loc$clu .eqs. "N"
$   then                        ! Execute DST$CHANGE.COM
$       @DST$CHANGE.COM
$   endif
$!
$   if loc$clu .eqs. "C"
$   then                        ! Execute DST$SYSMAN.COM
$       @DST$SYSMAN.COM
$   endif
$!
$after_x:
$!
$!
$   if ex$q$sav .nes. "S" then goto after_s
$!
$   say ""
$   say "The created procedure(s) will be saved."
$   say ""
$!
$after_s:
$!
$!
$   goto common_exit
$!
$!  ------------------------------------------------------------
$!  Exit routines
$!
$y_exit:        ! Ctrl_y exit routine
$!
$!  Display Ctrl_y message
$!
$   write sys$output "Exiting due to Ctrl_y entry"
$!
$   goto common_exit
$!
$err_exit:      ! error/warning exit routine
$!
$   sav_status = $status
$   write sys$output f$message(sav_status)
$   goto common_exit
$!
$common_exit:   ! common exit
$!
$!  Disable control_y and error handling
$   on control_y then continue
$   on warning then continue
$!
$!  Close any file left open...
$!
$   close/error=y_1 dst$change
$y_1:
$   close/error=y_2 dst$sysman
$y_2:
$!
$   exit
