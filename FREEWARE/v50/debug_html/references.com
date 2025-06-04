$!references.com
$!totals and lists references for each file
$!
$!
$ on control_y then goto end
$ debug="!"
$ quote="""
$ qeq="="""
$
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$ if p2.eqs."" then p2=htmdir+"*.html"
$
$ open/write htm 'htmdir'references.htm
$ write htm "<HTML>"
$ write htm "<HEAD>"
$ write htm "<TITLE>References count by all files</TITLE>"
$ write htm "</HEAD>"
$ write htm "<BODY bgcolor=",quote,"#FFEECC",quote,">"
$ write htm "<H1>References count by all files ''f$time()'</H1>"
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "<P>"
$next:
$
$ file=f$search(p2,1)
$ if file.eqs."" then goto end
$ file=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ write htm "<a href=",quote,file,quote,">",file,"</a><br>"
$
$ search/nowarn 'p2' 'file'/wind=0/out=seatmp.tmp
$ filcount=0
$ open/read/error=nomore seafil seatmp.tmp
$nextfil:
$ read/error=nomore seafil data
$ nam=f$edit(f$parse(data,,,"name")+f$parse(data,,,"type"),"lowercase")
$ if nam.eqs."updated.html" then goto nextfil
$
$ write htm "--<a href=",quote,nam,quote,">",nam,"</a><br>"
$ filcount=filcount+1
$ goto nextfil
$nomore:
$ close seafil
$ deletex seatmp.tmp;*
$ if filcount.le.2 then write htm "<font color=",quote,"#ff0000",quote,">"
$ write htm "-- Total of ",filcount," <p>"
$ if filcount.le.2 then write htm "</font>"
$ goto next
$end:
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ exit
