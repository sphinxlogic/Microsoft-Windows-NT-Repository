$! kill any /CGI processes
$!
$!      @kill_tentacles             just for the user
$!      @kill_tentacles all         kill all tentacles (requires WORLD)
$!
$ ctx = ""
$ doall = f$edit(p1,"trim,upcase") .eqs. "ALL"
$ if (.not. doall)
$ then
$   u = f$edit(f$getjpi("","username"),"trim")
$   x = f$context("PROCESS",ctx,"USERNAME",u,"EQL")
$ else
$   oprv = f$setprv("WORLD")
$ endif
$!
$ x = f$context("PROCESS",ctx,"MODE","OTHER","EQL")
$ loop:
$   pid = f$pid(ctx)
$   if pid .eqs. "" then goto done
$   prog = ""
$   det  = 0
$   name = ""
$   prog = F$PARSE(f$getjpi(pid,"IMAGNAME"),,,"NAME")
$   if prog .nes. "TENTACLE" then goto loop                 !! tentacle
$   det = (f$getjpi(pid,"creprc_flags") .and. %x200) .eq. %x200
$   if .not. det then goto loop                             !! detached
$   u = f$edit(f$getjpi(pid,"username"),"TRIM")             !! user/CGIxx
$   name = f$edit(f$getjpi(pid,"PRCNAM"),"TRIM")
$   j = f$locate("/CGI",name)
$   if j .ge. f$length(name) then goto loop
$   if f$extract(0,j,name) .eqs. f$extract(0,j,u)
$   then
$       write sys$output "Stopping proc ''pid'  ''name'"
$       stop/id='pid'
$   endif
$ goto loop
$!
$ done:
$   if doall then x = f$setprv(oprv)
$ exit
