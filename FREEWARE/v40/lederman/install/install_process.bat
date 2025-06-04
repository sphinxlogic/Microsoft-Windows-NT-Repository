$ SET NOON
$ SET PROC /PRIV = ALL
$! INSTALL :== $SYS$SYSTEM:INSTALL/COMMAND
$! INSTALL
$!LIST/FULL
$!
$ INSTALL LIST /FULL
$ EXIT
$!
$! This command file is intended to run as a Batch job to extract
$! information about installed images into a log file which can
$! then be processed to normalize the data.  If done as a SET HOST 0::/LOG
$! the program written to process the information won't work because
$! SET HOST deliberately inserts nulls at the beginning of each record.
$!
$! Since INSTALL is privileged, it must be submitted from a privileged
$! account (though the log file and processing program can be anywhere).
$!
$! B. Z. Lederman	29-Sep-1987	ITT World Communications
$!			21-Mar-1996	Digital Equipment Corp.
