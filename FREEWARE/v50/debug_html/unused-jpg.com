$! search out for unused jpg's in the directory
$
$ set noon
$ on control_y then goto end
$ linkonly:=false	! boolean links only or images
$ quote="""
$
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
        f$parse("''p1'",f$envir("default"),,"directory")
$ if p2.eqs."" then p2=htmdir+"*.jpg"
$
$ open/write htm 'htmdir'unused-jpg.htm
$ write htm "<HTML>"
$ write htm "<HEAD>"
$ write htm "<TITLE>Unused jpg</TITLE>"
$ write htm "</HEAD>"
$ write htm "<BODY bgcolor=",quote,"#FFEECC",quote,">"
$ write htm "<H1>Unused jpg ''f$time()'</H1>"
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "<P>"
$next:
$ file=f$search(p2)
$ if file.eqs."" then goto end
$ name=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ search/nooutput/nowarning 'htmdir'*.html "''name'"
$ if $status.eqs."%X08D78053" then goto notfound
$ goto next
$notfound:
$ if linkonly
$ then
$ write htm "<a href=",quote,-
        name,-
        quote,"> ",-
        name,-
        " ",(f$file(file,"EOF")*512)/1024,"k</a> ",f$file(file,"cdt"),"<br>"
$ else
$ write htm "<img src=",quote,-
        name,-
        quote,"><br>",-
        name,-
        " ",(f$file(file,"EOF")*512)/1024,"k ",f$file(file,"cdt"),"<br>"
$ endif
$ goto next
$end:
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ exit
