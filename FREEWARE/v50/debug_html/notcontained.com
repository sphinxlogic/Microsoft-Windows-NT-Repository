$!files-not-referred.com
$!find which html files in directory p1 do not contained file passed in p2
$!used for index files, map files
$ on control_y then goto end
$ if p2.eqs."" then exit %x10
$ if f$search(p2).nes."" then goto carryon	! validate
$ exit %x1330
$!
$carryon:
$ p2=f$edit(p2,"lowercase")
$ debug="!"
$ quote="""
$ qeq="="""
$
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$
$ open/write htm 'htmdir'notcontained.htm
$ write htm "<HTML>"
$ write htm "<HEAD>"
$ write htm "<TITLE>Not containing ''p2'</TITLE>"
$ write htm "</HEAD>"
$ write htm "<BODY bgcolor=",quote,"#FFEECC",quote,">"
$ write htm "<H1>Files not containing ''p2' ''f$time()'</H1>"
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "<P>"
$next:
$
$ file=f$search("''htmdir'*.html",1)
$ if file.eqs."" then goto end
$ search/nooutput/nowarning 'file' "''p2'"
$ if $status.nes."%X08D78053" then goto next
$
$ file=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ write htm "<a href=",quote,file,quote,">",file,"</a><br>"
$
$ goto next
$end:
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ exit
