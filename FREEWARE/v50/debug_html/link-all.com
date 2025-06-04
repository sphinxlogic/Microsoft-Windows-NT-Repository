$!link-all
$! will make links for all files
$ set symbol/scope=(nolocal,noglobal)
$ on control_y then goto end
$ quote="""
$ alphabet="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
$
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$ if p2.eqs."" then p2=htmdir+"*.*"
$
$ open/write htm 'htmdir'link-all.htm
$ write htm "<HTML>"
$ write htm "<HEAD>"
$ write htm "<TITLE>Links to all files</TITLE>"
$ write htm "</HEAD>"
$ write htm "<BODY bgcolor=",quote,"#FFEECC",quote,">"
$ write htm "<H1>Links to all files ''f$time()'</H1>"
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "<P>"
$ initial="0-9"
$ gosub letters
$next:
$
$ file=f$search(p2,1)
$ if file.eqs."" then goto end
$ if initial.nes.f$extract(0,1,f$parse(file,,,"name")) then gosub newletter
$
$ fname=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$
$ write htm "<a href=",quote,fname,quote,">",fname," ",f$file(file,"cdt"),"</a><br>"
$ goto next
$
$last:
$ gosub letters
$end:
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ exit
$newletter:
$ initial=f$extract(0,1,f$parse(file,,,"name"))
$letters:
$ write htm "<a href=",quote,"#0-9",quote,">0-9</a>"
$ write htm "<a href=",quote,"#A",quote,">A</a>"
$ write htm "<a href=",quote,"#B",quote,">B</a>"
$ write htm "<a href=",quote,"#C",quote,">C</a>"
$ write htm "<a href=",quote,"#D",quote,">D</a>"
$ write htm "<a href=",quote,"#E",quote,">E</a>"
$ write htm "<a href=",quote,"#F",quote,">F</a>"
$ write htm "<a href=",quote,"#G",quote,">G</a>"
$ write htm "<a href=",quote,"#H",quote,">H</a>"
$ write htm "<a href=",quote,"#I",quote,">I</a>"
$ write htm "<a href=",quote,"#J",quote,">J</a>"
$ write htm "<a href=",quote,"#K",quote,">K</a>"
$ write htm "<a href=",quote,"#L",quote,">L</a>"
$ write htm "<a href=",quote,"#M",quote,">M</a>"
$ write htm "<a href=",quote,"#N",quote,">N</a>"
$ write htm "<a href=",quote,"#O",quote,">O</a>"
$ write htm "<a href=",quote,"#P",quote,">P</a>"
$ write htm "<a href=",quote,"#Q",quote,">Q</a>"
$ write htm "<a href=",quote,"#R",quote,">R</a>"
$ write htm "<a href=",quote,"#S",quote,">S</a>"
$ write htm "<a href=",quote,"#T",quote,">T</a>"
$ write htm "<a href=",quote,"#U",quote,">U</a>"
$ write htm "<a href=",quote,"#V",quote,">V</a>"
$ write htm "<a href=",quote,"#W",quote,">W</a>"
$ write htm "<a href=",quote,"#X",quote,">X</a>"
$ write htm "<a href=",quote,"#Y",quote,">Y</a>"
$ write htm "<a href=",quote,"#Z",quote,">Z</a>"
$ write htm "<P>"
$ write htm "<a name=",quote,initial,quote,"></a>"
$ return
