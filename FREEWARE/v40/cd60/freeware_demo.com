$!x='f$ver(0)'  !Force verify off
$ set noon
$!Quick demo of CD
$ sysver = f$edit(f$getsyi("version"),"upcase,collapse")
$ maj = f$element(0,".",sysver)
$ majch = f$extract(0,1,maj)
$ if (majch.ges."A").and.(majch.les."Z") then maj = maj - majch
$ maj = f$int(maj)
$ min=f$element(0,"-",f$element(1,".",sysver))
$ min = f$int(min)
$ target="62"
$ if (maj.gt.7).or.((maj.eq.7).and.(min.ge.2)) then target="72"
$ arch="VAX"
$ if f$getsyi("hw_model").ge.1024 then arch="ALPHA"
$!
$ this=f$envi("default")
$ cddir = f$parse(this,,,"NODE")+f$parse(this,,,"DEVICE")+f$parse(this,,,"DIRECTORY")
$ cdimage = cddir + "CD_VMS''target'_''arch'.EXE"
$ write sys$output ""
$ write sys$output "Defining test CD symbol:"
$ write sys$output ""
$ write sys$output "  $ CD:==$''cdimage'/INH/LOG"
$ write sys$output ""
$! Make this a --LOCAL-- definition!
$ CD="$''cdimage'/INH/LOG"
$ write sys$output "Executing a few test commands:"
$ write sys$output ""
$ if target.eqs."72" then call test_72
$ if target.nes."72" then call test_62
$ set def 'this'
$ write sys$output ""
$ write sys$output "Tests concluded... Pausing briefly...."
$ wait 0:0:10
$ exit
$!
$test_62: subroutine
$ set verify
$ CD		!Show current directory
$ CD ~		!Change to SYS$LOGIN
$ CD $		!Change back to previous directory
$ CD ..		!Change up one level
$ CD ??		!Show CD version
$!x='f$ver(0)'  !Force verify back off
$ endsubroutine
$!
$test_72: subroutine
$ set verify
$ CD		!Show current directory
$ CD ~		!Change to SYS$LOGIN
$ CD $		!Change back to previous directory
$ CD .		!DID the current directory
$ CD ...	!Force change directory up one level
$ CD .		!unDID the current directory
$ CD ??		!Show CD version
$!x='f$ver(0)'  !Force verify back off
$ endsubroutine
$!
