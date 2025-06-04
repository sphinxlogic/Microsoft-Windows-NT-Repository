$!notreferenced.com
$!find which html files do not contained file passed in p2
$!used for index files, map files
$ on control_y then goto end
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$
$ if p2.eqs."" then exit %x10
$ if f$search("''htmdir'''p2'").nes."" then goto carryon
$ exit %x1330
$!
$carryon:
$ p2=f$edit(p2,"lowercase")
$ p3=htmdir+"*.html"		! search these files
$ debug="!"
$ quote="""
$ qeq="="""
$
$ open/write htm 'htmdir'notreferenced.htm
$ WRITE htm "<HTML>"
$ WRITE htm "<HEAD>"
$ WRITE htm "<TITLE>Not referenced by ''p2'</TITLE>"
$ WRITE htm "</HEAD>"
$ WRITE htm "<BODY bgcolor=",quote,"#FFEECC",quote,">"
$ WRITE htm "<H1>Files not referenced by ''p2' ''f$time()'</H1>"
$ WRITE htm "<P>"
$ WRITE htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ WRITE htm "<P>"
$next:
$
$ file=f$search(p3,1)
$ if file.eqs."" then goto end
$ file=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ search/nooutput/nowarning 'htmdir''p2' "''file'"
$ if $status.nes."%X08D78053" then goto next
$
$ write htm "<a href=",quote,file,quote,">",file,"</a><br>"
$
$ goto next
$end:
$ WRITE htm "<P>"
$ WRITE htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ exit
