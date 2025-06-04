$!link-images.com
$!
$!makes links to all images
$ on control_y then goto finished
$
$ alphabet="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
$ quote="""
$
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$ if p2.eqs."" then p2=htmdir+"*.*"
$
$ open/write images 'htmdir'IMAGE-LINKS.HTM
$ write images "<html>"
$ write images "<head>"
$ write images "<title>Image Links</title>"
$ write images "</head>"
$ write images "<body bgcolor=",quote,"#FFEECC",quote,">"
$ write images "<h1>Image links ''f$time()'</H1>"
$ write images "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write images "<p>"
$ initial="0-9"
$ gosub letters
$next:
$ image=f$search(p2)
$ if image.eqs."" then goto finished
$ if f$locate(f$parse(image,,,"type"),".GIF.JPG").eq.8 then goto next
$ if initial.nes.f$extract(0,1,f$parse(image,,,"name")) then gosub newletter
$ fnam=f$edit(f$parse(image,,,"name")+f$parse(image,,,"type"),"lowercase")
$ write images "<a href=",quote,-
	fnam,quote,"> ",fnam,"</a> - ",-
	(f$file(image,"EOF")*512)/1000,"k - ",f$file(image,"cdt"),"<br>"
$ goto next
$finished:
$ gosub letters
$ write images "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write images "</html>"
$ close images
$ exit
$newletter:
$ initial=f$extract(0,1,f$parse(image,,,"name"))
$letters:
$ write images "<a href=",quote,"#0-9",quote,">0-9</a>"
$ write images "<a href=",quote,"#A",quote,">A</a>"
$ write images "<a href=",quote,"#B",quote,">B</a>"
$ write images "<a href=",quote,"#C",quote,">C</a>"
$ write images "<a href=",quote,"#D",quote,">D</a>"
$ write images "<a href=",quote,"#E",quote,">E</a>"
$ write images "<a href=",quote,"#F",quote,">F</a>"
$ write images "<a href=",quote,"#G",quote,">G</a>"
$ write images "<a href=",quote,"#H",quote,">H</a>"
$ write images "<a href=",quote,"#I",quote,">I</a>"
$ write images "<a href=",quote,"#J",quote,">J</a>"
$ write images "<a href=",quote,"#K",quote,">K</a>"
$ write images "<a href=",quote,"#L",quote,">L</a>"
$ write images "<a href=",quote,"#M",quote,">M</a>"
$ write images "<a href=",quote,"#N",quote,">N</a>"
$ write images "<a href=",quote,"#O",quote,">O</a>"
$ write images "<a href=",quote,"#P",quote,">P</a>"
$ write images "<a href=",quote,"#Q",quote,">Q</a>"
$ write images "<a href=",quote,"#R",quote,">R</a>"
$ write images "<a href=",quote,"#S",quote,">S</a>"
$ write images "<a href=",quote,"#T",quote,">T</a>"
$ write images "<a href=",quote,"#U",quote,">U</a>"
$ write images "<a href=",quote,"#V",quote,">V</a>"
$ write images "<a href=",quote,"#W",quote,">W</a>"
$ write images "<a href=",quote,"#X",quote,">X</a>"
$ write images "<a href=",quote,"#Y",quote,">Y</a>"
$ write images "<a href=",quote,"#Z",quote,">Z</a>"
$ write images "<P>"
$ write images "<a name=",quote,initial,quote,"></a>"
$ return
