$ on error then goto error_exit
$ if p1 .eqs. "" then goto help
$ if p2 .nes. "" .and. p3 .nes. ""
$ then
$  tdate = f$cvtime("","ABSOLUTE","DATE")
$  tdnm = f$edit(f$extract(0,3,f$cvtime(tdate,,"WEEKDAY")),"UPCASE")
$  tdow = f$locate(tdnm,"SUN,MON,TUE,WED,THU,FRI,SAT,") / 4
$  rdow = f$locate(f$edit(p2,"UPCASE"),"SUN,MON,TUE,WED,THU,FRI,SAT,") / 4
$  if rdow .eq. 7 then goto help
$  reday = rdow - tdow
$  if reday .le. 0 then reday = 7 + reday
$  submit /noprinter /queue=os$sys /priority=1 -
 /after="''tdate'+''reday'-''p3'" nist_set /par=('p1','p2','p3')
$ endif
$ all 'p1'
$ if $severity .ne. 1 then exit $status
$ set ter 'p1' /pasthru /nohostsync /nottsync /altypeahd -
 /nobroadcast /noecho /nowrap /nodialup
$ ass/use 'p1' lu_term
$ ass/use nist_set_dial.par lu_dial
$ def/use df_settime "Y"
$ ass/use nist_set_hist.lis lu_hist
$ ass/use nist_set_log.lis for$print
$ run nist_set_a
$error_exit:
$ save_status = $status
$ on error then continue
$ deall 'p1'
$ sho sym save_status
$ exit save_status
$help:
$ write sys$output "  P1 = terminal name, required."
$ write sys$output "  P2 = resubmit day of week, optional."
$ write sys$output "  P3 = resubmit time of day, optional."
$ write sys$output "  If P2 and P3 given, the com file resubmits itself ..."
$ write sys$output "    at that time on that day, with parameters P1, P2, P3."
$ exit %X18008022
