$ IF (P1 .EQS. "") .OR. (P2 .EQS. "")
$ THEN
$	WRITE SYS$OUTPUT "Usage: @ANNOTATE <baselevel> <server-error-log>
$	EXIT
$ ENDIF
$ base = "DECW$SRESD:[''P1'"
$ OPEN/WRITE outfile SYS$SCRATCH:ANNOTATE.TMP
$ WRITE outfile base,".SERVER.LIS]DECW$SERVER_DIX.MAP"
$ WRITE outfile ""
$ WRITE outfile base,".DDXGPX.LIS]DECW$SERVER_DDX_GA.MAP"
$ WRITE outfile ""
$ WRITE outfile base,".DDXLEGSS.LIS]DECW$SERVER_DDX_GB.MAP"
$ WRITE outfile ""
$ WRITE outfile base,".DDXMFB.LIS]DECW$SERVER_DDX_GC.MAP"
$ WRITE outfile ""
$ WRITE outfile base,".DDXSCN.LIS]DECW$SERVER_DDX_GE.MAP"
$ WRITE outfile ""
$ WRITE outfile ""
$ WRITE outfile P2
$ CLOSE outfile
$ DEFINE/USER SYS$INPUT SYS$SCRATCH:ANNOTATE.TMP
$ RUN ANNOTATE
$ DELETE SYS$SCRATCH:ANNOTATE.TMP;*
$ EXIT
