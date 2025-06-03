$ SET DEF KRONOS_ROOT:[resources]
$ RUN CHECK_PROCESS
$ FILE = F$SEARCH ("CHECK_PROCESS.OUT")
$ IF "''FILE'" .NES. "" 
$ THEN
$    MAIL/subject="Kronos alert" 'FILE' SYSTEM,OPERATOR
$    DELETE check_process.out;*
$ ENDIF
