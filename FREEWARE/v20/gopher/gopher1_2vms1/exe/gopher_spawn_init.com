$ V = 'F$VERIFY(0)'
$ ON ERROR THEN CONTINUE
$!
$!****************************************************************************
$!*									     *
$!*                          GOPHER_SPAWN_INIT.COM                           *
$!*        (For passing symbols/logicals to Gopher server subprocesses)      *
$!*                   Foteos Macrides (MACRIDES@SCI.WFEB.EDU)                *
$!*									     *
$!****************************************************************************
$!
$!  This command procedure should be executed by the sys$creprc()-created
$!  subprocesses when vms_system() is used under Inetd/MULTINET_SERVER by
$!  the Gopher server in VMS.  Such processes have FMODE() .eqs. "OTHER",
$!  do not (and should not) execute SYS$MANAGER:SYLOGIN.COM, and therefore
$!  need to acquire foreign command symbols from an alternate command file.
$!  It can also be used to pass logicals to the subprocesses.
$!
$!  It's location and name should be defined for the 'SpawnInit:' field of the
$!  VMSGopherServer's configuration file.  
$!
$ COP*Y   :== COPY/LOG
$ DEL*ETE :== DELETE/LOG
$ GAWK    :== $device:[directory]GAWK
$ EGR*EP  :== $device:[directory]EGREP
$ PUR*GE  :== PURGE/LOG
$ REN*AME :== RENAME/LOG
$ PRI*NT  :== PRINT/NOFLAG
$!
$ EXIT
