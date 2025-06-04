$! The idea is to get the same output from a DCL procedure as would
$! be obtained from LIB$INIT_TIMER and LIB$SHOW_TIMER, vis:
$!
$! ELAPSED:    0 00:00:06.59  CPU: 0:00:03.87  BUFIO: 23  DIRIO: 87  FAULTS: 1071 
$!
$ ctrlstr = -
 " ELAPSED:    0 !AS  CPU: 0:!2ZL:!2ZL.!2ZL  BUFIO: !SL  DIRIO: !SL  FAULTS: !SL"
$!
$! Save counts at beginning
$!
$ bt = F$CVTIME("", "ABSOLUTE", "TIME")
$ bp = F$GETJPI("", "PAGEFLTS")
$ bd = F$GETJPI("", "DIRIO")
$ bb = F$GETJPI("", "BUFIO")
$ bc = F$GETJPI("", "CPUTIM")
$!
$!!! thing to be timed here
$ dir/out=nl:
$!
$ ec = F$GETJPI("", "CPUTIM")
$ eb = F$GETJPI("", "BUFIO")
$ ed = F$GETJPI("", "DIRIO")
$ ep = F$GETJPI("", "PAGEFLTS")
$ et = F$CVTIME("", "ABSOLUTE", "DATETIME")
$!
$! Save counts at end (above), then calculate differences.
$!
$ dc = ec - bc
$ db = eb - bb
$ dd = ed - bd
$ dp = ep - bp
$!
$! Getting times to subtract calls for some manipulation
$!
$ dt = F$CVTIME(et + " -0-" + bt, "ABSOLUTE", "TIME")
$!
$! need to separate minutes, seconds, and hundreths.
$!
$ dcs = dc / 100
$ dcc = dc - (dcs * 100)
$ dcm = 0
$ IF dcs .GT. 60
$ THEN
$    dcm = dcs / 60
$    dcs = dcs - (60 * dcm)
$ ENDIF
$!
$ WRITE SYS$OUTPUT F$FAO(ctrlstr, dt, dcm, dcs, dcc, db, dd, dp)
$!
$ EXIT
