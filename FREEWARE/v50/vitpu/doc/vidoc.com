$ ! This command procedure builds copies of the various
$ ! VI documentation files
$ !
$ DO="@[-.EXE]DO"
$ IF F$LOGICAL ("VI$ROOT") .NES. "" THEN DO="@[EXE]DO"
$ !
$ RM=""
$ IF P2 .NES. "" THEN RM=P2
$ IF P1 .NES. "" THEN GOTO 'P1'
$ !
$ ! Build the printable manual by default
$ !
$ MANUAL:
$	VARS="MANUAL" ! Actually there is no such variant, but something is needed
$	EXT="MEM"
$	FILE="VI"
$	IF RM .EQS. "" THEN RM=8
$	GOTO PROCESS
$!
$! Build the tutorial
$!
$ TUTOR:
$	VARS="MANUAL"
$	EXT="MEM"
$	FILE="TUTOR"
$	IF RM .EQS. "" THEN RM=8
$	GOTO PROCESS
$ !
$ ! Build the on-line help manual
$ !
$ HELP:
$	VARS="SYSTEM"
$	EXT="HLP"
$	FILE="VI"
$	IF RM .EQS. "" THEN RM=0
$	GOTO FINAL
$ !
$ PROCESS:
$ !
$	DO RUNOFF/LOG/OUT=NLA0:/RIGHT='RM'/INTERMEDIATE/MESS=USER/VAR='VARS' -
           'FILE'.RNO
$ !
$ ! Do the table of contents
$ !
$	DO RUNOFF/CONTENTS/OUT='FILE' 'FILE'.BRN
$	DO RUNOFF/INDEX/OUT='FILE' 'FILE'.BRN
$ !
$ FINAL:
$	DO RUNOFF/LOG/OUT='FILE'.'EXT'/RIGHT='RM'/MESS=USER/VAR='VARS' 'FILE'.RNO
$	DO PURGE 'FILE'.'EXT','FILE'.RNT
$ !
$	DO DELETE 'FILE'.BRN;*
$ !
$	IF P1 .NES. "HELP" THEN EXIT
$ !
$ INSTALL_HELP:
$ !
$ ! Reinstall help file
$ !
$	WRITE SYS$OUTPUT " "
$   IF F$SEARCH("VI.HLB") .EQS. "" THEN DO LIB/LOG/CREATE/HELP VI
$	INQUIRE/NOPUNCT HELPLIB "Name of library to install HELP files in? "
$	DO LIB/REPLACE/HELP 'HELPLIB' VI.'EXT'
$	EXIT
$ !
$ ! Just install the HELP file
$ !
$ HELPINST:
$	EXT="HLP"
$	VARS="HELP"
$	GOTO INSTALL_HELP
