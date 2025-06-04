$ !
$ !	VISETUP.COM  -  Set up to use the VI TPU interface.
$ !
$ !  Written by Gregg Wonderly JUN-30-1987
$ !
$ DEFINE/NOLOG/TRANS=CONCEAL 	VI$ROOT	MATH0:[PUBLIC.VI.]
$ DEFINE TPU$SECTION			VI$ROOT:[EXE]VI.GBL
$ DEFINE TPU$CALLUSER			VI$ROOT:[EXE]TPUSUBS.EXE
$ DEFINE/NOLOG VISECINI			VI$ROOT:[EXE]VI.GBL
$ DEFINE/NOLOG VI$CALLUSER		VI$ROOT:[EXE]TPUSUBS.EXE
$ DEFINE/NOLOG MAIL$EDIT		VI$ROOT:[EXE]MAILEDIT.COM
$!
$! This symbol gets you a NON-SPAWNED VI
$! Delete this if you are using VI.EXE and VI.CLD
$!
$ VI ==							"EDIT/TPU"
$! This symbol gets you a SPAWNED VI
$ VIS ==						"@VI$ROOT:[EXE]VI.COM"
$!
$! This gets you a readonly version of the file.
$!
$ IF F$TYPE (VI) .EQS. "" THEN VI="VI"
$ VIEW == "''VI'/READONLY"
$ SNU*FF == "@VI$ROOT:[EXE]SNUFFTPU.COM"
$ EXIT
