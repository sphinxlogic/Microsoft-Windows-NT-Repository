$!-----------------------------------------------------
$!
$!            SCNPRT_REQUIRED Version 2.6
$!
$!          Check to see if QUEUE MANAGER is running
$!          Check for enough privileges
$!
$!-----------------------------------------------------
$!
$! Check to see if the queue manager is running by doing show command
$ on error then continue
$ define /user /nolog sys$error nl:
$ define /user /nolog sys$output nl:
$ show queue /form /output=nl:
$ s = $status
$ if s .eq. %x1004807a
$ then
$    write sys$error -
    "%SCNPRT-E-NOQUEMAN, The system queue manager is not running. Cannot proceed."
$    exit 's'
$ endif
$
$! Check to see if process has enough privs.
$ privs = "SYSNAM,SYSPRV,DETACH,OPER,TMPMBX,NETMBX"
$ 
$ if .not. f$privilege(privs) 
$ then
$     write sys$error "%SCNPRT-E-NOPRIVS, The following privs are need to run: ''privs'"
$     exit %x10000024				! SS$_NOPRIV, close enough
$ endif
$
$ exit
