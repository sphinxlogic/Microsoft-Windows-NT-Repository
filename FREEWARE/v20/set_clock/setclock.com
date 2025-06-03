$! SPRINGFOR.COM - Adjust cluster clocks for spring.
$!
$! 31-Mar-1990 G Altobello
$!
$! SPRINGFOR.EXE (author J. Hanrahan) is taken from the DECUServe VAXnotes
$! conferences.
$!
$! It diddles a couple of registers in VMS, causing the system clock to run
$! 20% fast for 5 hours, thereby adjusting the clock ahead an hour without
$! any sudden time changes.
$!
$! This command file is to be submitted to a *single* batch queue on the
$! cluster, /AFTER whenever is a good time to run it (probably real late at
$! night, I usually use /AFTER=<date> and let it run at midnight).
$!
$! When it starts up, it creates SPRINGFOR.EXE and runs it on all nodes, via
$! SYSMAN, then waits while the clocks move ahead.  After the wait, it executes
$! a $SET TIME command on all nodes which forces the current system time into
$! the TOY clock (if this isn't done, a crash will cause the time to be reset to
$! its previous value) (note that an orderly shutdown *will* properly set the
$! TOY clock).
$!
$! Note that SPRINGFOR.EXE and TIMESET.COM are specifically placed in the
$! SYS$COMMON:[SYSMGR] directory, since the input lines to SYSMAN cannot contain
$! symbols for translation.  This method insures the files are available to all
$! nodes in my cluster.  IF YOU HAVE A HETEROGENEOUS CLUSTER, you should change
$! all occurrences of "SYS$COMMON:[SYSMGR]" in this procedure to a directory
$! visible to all members of your cluster.
$!
$! Revision History
$! ================
$! 31-Mar-1990 GWA  - Initial edit.
$! 03-Mar-1994  Jc  - Alpha revisions
$!------------------------------------------------------------------------------
$ SET NOON
$ SET VERIFY
$ AXP_MACHINE = F$GETSYI("NODE_HWTYPE") .EQS. "ALPH"
$!
$! SYSMAN doesn't set local node privs, so do so here.
$!
$ SET PROC/PRIV=(OPER,SYSPRV,CMKRNL,LOG_IO)
$!
$! Remove any bogus copies of springfor before we start...
$!
$ IF F$SEARCH("SYS$LOGIN:SPRINGFOR.*") .NES. "" THEN DELETE/LOG SYS$LOGIN:SPRIN
$!
$! Create SPRINGFOR.MAR in our default directory, and assemble and link it.
$!
$ IF AXP_MACHINE
$ THEN
$    CREATE SPRINGFOR.MAR
$    DECK
     .TITLE    SPRINGFOR, Adjust OpenVMS clock to run 20% fast for five hours

     .LIBRARY \SYS$LIBRARY:LIB\
     .LINK \SYS$LOADABLE_IMAGES:SYSDEF.STB\

     .ENTRY    START, ^M<>
     $CMKRNL_S FIXIT
     MOVL #1, R0
     RET

     .ENTRY    FIXIT, ^M<>

     LOCK HWCLK
     MOVL #175770, G^EXE$GL_TIMEADJUST  ; five hours' worth of ticks
     MOVL #11718, G^EXE$GL_TICKLENGTH   ; normal is 9765
               ; change the preceding constant from 11718 to
               ; 7812 to make a `fallback' program
     UNLOCK    HWCLK

     MOVL #1, R0
     RET

     .END START
$    EOD
$    ALPHA = "/SYSEXE"
$ ELSE
$    CREATE SPRINGFOR.MAR
$    DECK
     .TITLE    SPRINGFOR, Adjust VMS clock to run 20% fast for five hours

     .LIBRARY \SYS$LIBRARY:LIB\
     .LINK \SYS$SYSTEM:SYS.STB\

     .ENTRY    START, ^M<>
     $CMKRNL_S FIXIT
     MOVL #1, R0
     RET

     .ENTRY    FIXIT, ^M<>

     LOCK HWCLK
     MOVL #1800000, G^EXE$GL_TIMEADJUST ; five hours' worth of ticks
     MOVL #120000, G^EXE$GL_TICKLENGTH  ; normal is 100000
               ; change the preceding constant from 120000 to
               ; 80000 to make a `fallback' program
     UNLOCK    HWCLK

     MOVL #1, R0
     RET

     .END START
$    EOD
$ ENDIF
$!
$ MACRO/NOLIST SPRINGFOR
$ LINK/NOMAP'ALPHA' SPRINGFOR
$ IF F$GETSYI("CLUSTER_MEMBER")
$ THEN
$    IF F$SEARCH("SYS$COMMON:[SYSMGR]SPRINGFOR.EXE") .EQS. "" THEN -
          RENAME SPRINGFOR.EXE SYS$COMMON:[SYSMGR]
$ ELSE
$    RENAME SPRINGFOR.EXE SYS$COMMON:[SYSMGR]
$ ENDIF
$!
$! Start time-shift on clocks on all cluster members.
$!
$ TEMPFILE = F$PARSE("SYS$LOGIN:.TMP",F$GETJPI(0,"PID"))
$ OPEN/WRITE OUTFILE 'TEMPFILE'
$ WRITE OUTFILE "$RUN SYS$SYSTEM:SYSMAN"
$ WRITE OUTFILE "SET ENVIRONMENT/CLUSTER"
$ WRITE OUTFILE "SET PROFILE/PRIV=(SYSPRV,CMKRNL)"
$ WRITE OUTFILE "DO IF F$GETDVI(""SYS$SYSDISK"",""DEVNAM"").EQS.""''F$GETDVI("S
$ WRITE OUTFILE " RUN SYS$COMMON:[SYSMGR]SPRINGFOR"
$ WRITE OUTFILE "$DELETE 'F$ENVIRO(""PROCEDURE"")'"
$ CLOSE OUTFILE
$ @'TEMPFILE'
$!
$! Now wait till the time-shift is done.
$! (This should be an apparent 5 hours from now (though it'll take 4 hours)).
$!
$ WAIT 05:00:00
$!
$! Now ensure that the TOY clocks on all systems get updated.
$! (This requires a special command file, so make one and execute that.)
$!
$ OPEN/WRITE TIMESET SYS$COMMON:[SYSMGR]TIMESET.COM                       !***
$ WRITE TIMESET "$IF F$GETDVI(""SYS$SYSDISK"",""DEVNAM"").EQS.""''F$GETDVI("SYS
$ WRITE TIMESET "$THEN"
$ WRITE TIMESET "$  TIME = F$TIME()
$ WRITE TIMESET "$  SET TIME=&TIME"
$ WRITE TIEMSET "$ENDIF
$ CLOSE TIMESET
$
$ RUN SYS$SYSTEM:SYSMAN
SET ENVIRONMENT/CLUSTER
SET PROFILE/PRIV=(OPER,SYSPRV,LOG_IO)
DO @SYS$COMMON:[SYSMGR]TIMESET                                  !***
DO DELETE SYS$COMMON:[SYSMGR]SPRINGFOR.EXE;*,TIMESET.COM;*
DO DELETE SPRINGFOR.MAR;*,SPRINGFOR.OBJ;*,SPRINGFOR.EXE;*
EXIT
$!
$! That's it, clean up...
$!
$ IF F$SEARCH("SYS$SHARE:NM_MAILSHR.EXE") .NES. ""
$ THEN
$    MAIL := MAIL/TRANSPORT=NM%
$ ELSE
$    MAIL := MAIL/NOTRANSPORT
$ ENDIF
$!
$ TMP = F$PARSE("SYS$LOGIN:.TMP",F$GETJ(0,"PID"))
$ SHOW PROCESS/ACCOUNT/OUT=&TMP
$ MAIL/SUBJ="Daylight Savings Time adjustment was successful." 'TMP' SYSTEM
$ IF F$SEARCH(TMP) .NES. "" THEN DELETE 'F$SEARCH(TMP)'
$ EXIT
