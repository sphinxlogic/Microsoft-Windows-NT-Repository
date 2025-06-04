$ ! ASKFORDFLTOPT.COM
$loop1:
$ write sys$output "Default dcc option(s) ?  For example:"
$ write sys$output " -zgpr        (less checking of ""good programming practices"")"
$ write sys$output " +zrhw -zlvl2 (report only errors or highest level warnings, and prevent"
$ write sys$output "               compilation if level >= 2)"
$ write sys$output "See 'dcreadme.pod' for options. If several options, separate them by spaces."
$ read/prompt=": " sys$command defaults
$loop2:
$ inquire answer "OK ? (y/n)"
$ if answer .eqs. "N" then goto loop1
$ if answer .nes. "Y" then goto loop2
$ if defaults .eqs. ""
$ then
$   copy dccFiles.mng locDcFil.mng
$   exit
$ endif
$ defaults = defaults + " "
$ open/read in dccFiles.mng
$ open/write out locDcFil.mng
$loop3:
$ read/end=endFile in line
$ if F$LOCATE("PrefixDCCDFLTS", line) .NE. F$LENGTH(line)
$ then
$   temp = F$LOCATE("""", line) + 1
$   line = F$EXTRACT(0, temp, line) + defaults + F$EXTRACT(temp, 1000, line)
$ endif
$ write out line
$ goto loop3
$endFile:
$ close in
$ close out
$ ! End ASKFORDFLTOPT.COM
