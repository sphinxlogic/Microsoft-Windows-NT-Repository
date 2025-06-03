$ dirname = f$environment("PROCEDURE")
$ gosub parse_dir
$ src = dirname
$ goal = f$parse(dirname-"]"+".-]A.;0") - "A.;0"
$ rename 'src'temacs.exe 'goal'emacs.exe
$ rename 'src'temacs.dump 'goal'emacs.dump
$ exit
$
$
$parse_dir:
$   dirname=f$parse("A.;0",dirname) - "A.;0"
$   dirname=dirname - ".][000000" - ".><000000" - ".>[000000" - ".]<000000"
$   dirname=dirname - "][" - "><" - ">[" - "]<"
$   if f$element(1,"<",dirname) .nes. "<" then -
	dirname = f$element(0,"<",dirname) + "[" + f$element(1,"<",dirname)
$   if f$element(1,">",dirname) .nes. ">" then -
	dirname = f$element(0,">",dirname) + "]" + f$element(1,">",dirname)
$ ! write sys$output "''dirname'
$   return
