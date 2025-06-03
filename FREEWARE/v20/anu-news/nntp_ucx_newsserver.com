$ ! Create the NNTP detached server process for
$ ! VMS/Ultrix Connection (UCX) TCP/IP
$ !
$ ! This version of NNTP_TCPUCXM creates a driver log file called
$ ! NEWS_MANAGER_DEV:[NEWS_LOG]NNTP_SERVER_DRVR.LOG.  This may be
$ ! changed or removed by changing the line
$ !
$ ! #define drvrlogfile NEWS_MANAGER_DEV:[NEWS_LOG]NNTP_SERVER_DRVR.LOG
$ !
$ ! (found in NNTP_TCPUCXM.C) to something else (#define drvrlogfile TT:
$ ! comes to mind; I should really change this to a name suitable for
$ ! a logical).  This will eventually be removed, I suspect (see the code).
$ !
$ !
$ ! manage back copies of the log file (modify or delete if you've redefined
$ ! drvrlogfile)
$ !
$ purge/keep=4 NEWS_MANAGER_DEV:[NEWS_LOG]NNTP_SERVER_DRVR.LOG
$ create NEWS_MANAGER_DEV:[NEWS_LOG]NNTP_SERVER_DRVR.LOG
$ !
$ ! run the server as a detached process
$ !
$ run/detach -
     /output = NLA0: -
     /error = NLA0: -
     /input = NLA0: -
     /process_name = "UCX/NNTP server" -
     NEWS_MANAGER_DEV:[NEWS_DIST]NNTP_TCPUCXM.EXE
$ exit
