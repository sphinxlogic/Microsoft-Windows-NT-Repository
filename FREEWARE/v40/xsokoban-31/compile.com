$
$ LCC := "CC/DECC/PREFIX=ALL/EXTERN=COMMON/SHARE/include=[]"
$ call  COMPILE DISPLAY
$ call	COMPILE	MAIN
$ call	COMPILE	PLAY
$ call	COMPILE	RESOURCES
$ call	COMPILE	SAVE				! /define=DEBUG
$ call	COMPILE	SCORE /DEFINE=(MAXPATHLEN=256)
$ call	COMPILE	SCOREDISP /NOWARNING
$ call	COMPILE	SCREEN
$ call  COMPILE VMS_STRDUP
$ call  COMPILE VMS_UNLINK
$exit
$
$ compile: subroutine
$ 	if f$search("''P1'.OBJ").eqs.""
$ 	then
$ 		write sys$output "Compiling ''P1'"
$	 	if P2.eqs.""
$ 		then	'LCC'/DEFINE=VMS 'P1'.C
$	 	else	'LCC'/DEFINE=(VMS)'P2' 'P1'.C
$	 	endif
$ 	else	write sys$output "Skipping ''P1'"
$ 	endif
$ endsubroutine
