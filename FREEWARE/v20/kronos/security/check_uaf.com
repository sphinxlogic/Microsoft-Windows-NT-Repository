$ !
$ !             ***  CHECK_UAF.COM   ***
$ !
$ ON ERROR THEN GOTO OOPS
$ SET PROCESS/PRIV=EXQUOTA
$ SET DEF KRONOS_ROOT:[SECURITY]
$ !
$ !  The CHECK_UAF program extracts all records from
$ !   SYSUAF.DAT and compares them to previous run.
$ !
$ RUN CHECK_UAF
$ IF $STATUS .GT. 1 THEN GOTO OOPS
$ !
$ !  If there were changes, let them know.
$ !
$ FILE = F$SEARCH ("CHANGES.DAT")
$ IF "''FILE'" .NES. "" 
$ THEN
$    MAIL/subject="SYSUAF Changes" 'FILE' system,operator
$    DELETE 'FILE'
$ ENDIF
$ PURGE/KEEP=2 CHECK_UAF.DAT
$ !
$ !  Check Automated Login Facility
$ !
$ FILE = F$PARSE ("SYSALF","SYS$SYSTEM:SYSALF.DAT")
$ FILE = F$SEARCH ("''FILE'")
$ IF "''FILE'" .NES. "" 
$ THEN
$   mess = "Someone has created an Automated Login File !!!"
$   gosub mailit
$ ENDIF
$ !
$ !  Check for Network Proxy accounts
$ !
$ FILE = F$PARSE ("NETPROXY","SYS$SYSTEM:NETPROXY.DAT")
$ FILE = F$SEARCH ("''FILE'")
$ IF "''FILE'" .NES. "" 
$ THEN
$   mess = "Someone has created a Network proxy account !!!"
$   gosub mailit
$ ENDIF
$ !
$ ! Normal exit
$ !
$ purge/keep=2 check_uaf.log
$ EXIT
$ !
$ !  Unexpected error exit
$ !
$OOPS:
$ mess = "Unexpected error in CHECK_UAF job; check CHECK_UAF.LOG."
$ gosub mailit
$ EXIT
$ !
$ !  Message writing/mailing subprogram
$ !
$mailit:
$ OPEN/WRITE TEMP_FILE TEMP.FILE
$ FILE = F$SEARCH ("TEMP.FILE")
$ WRITE TEMP_FILE " "
$ WRITE TEMP_FILE " ''mess'"
$ WRITE TEMP_FILE " "
$ CLOSE TEMP_FILE
$ MAIL/SUBJECT="Problems in CHECK_UAF job." 'FILE' system,operator
$ DELETE 'FILE'
$ return
