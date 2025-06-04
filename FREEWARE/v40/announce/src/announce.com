$! File: SYS$MANAGER:ANNOUNCE.COM
$
$ SET NOON
$
$LOOP:
$
$ RUN SYS$MANAGER:ANNOUNCE
$
$ REQUEST "SYS$ANNOUNCE terminated, $STATUS=''$status', restarting..."
$
$ GOTO LOOP
