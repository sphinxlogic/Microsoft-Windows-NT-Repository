$!link-html
$! will make links for all html files
$ on control_y then goto end
$ quote="""
$
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$ if p2.eqs."" then p2=htmdir+"*.html"
$
$ open/write htm 'htmdir'link-html.htm
$ write htm "<HTML>"
$ write htm "<HEAD>"
$ write htm "<TITLE>Links to all HTML</TITLE>"
$ write htm "</HEAD>"
$ write htm "<BODY bgcolor=",quote,"#FFEECC",quote,">"
$ write htm "<H1>Links to all HTML ''f$time()'</H1>"
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "<P>"
$next:
$
$ file=f$search(p2,1)
$ if file.eqs."" then goto end
$
$ fname=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$
$ write htm "<a href=",quote,fname,quote,">",fname," ",f$file(file,"cdt"),"</a><br>"
$ goto next
$
$last:
$end:
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ exit
