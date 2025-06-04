$! msmtcp.com (20-Oct-1994 lww) Start MiniSm on a remote tcpip display
$ set ver
$ @sys$login:login
$ set proc/priv=all
$ ! see if user specified target display in P1
$ If P1 .nes. ""
$  Then
$    dispnode = "''P1'"
$  Else
$    remote = f$getdvi("TT", "TT_ACCPORNAM")
$!    sho symbol remote
$    startloc = f$locate(" ", remote)+1
$!    sho symbol startloc
$    str = f$extract(startloc,16,remote)
$!    sho symbol str
$    dispnode = f$element(0," ",str)
$!    sho symbol dispnode
$!   dispnode = "222.0.1.132" ! wolfie
$ Endif
$ If dispnode .eqs. ""
$  Then
$    write sys$output "Unable to determine Remote Display Node, Exiting..."
$    exit
$ Endif
$ sho time
$ set display/create/super/transp=tcpip/node="''dispnode'"
$ sho log decw$display
$ rem_disp = f$trnlnm("decw$display")-"_"
$ Open /Share=write 'rem_disp DECW_CHAR.DAT
$ show dev/full 'rem_disp
$ define MSM_DETACHED "decterm%fta"
$! run  usr$tools:minism.exe /output='rem_disp
$ run  usr$tools:minism.exe
$ exit
