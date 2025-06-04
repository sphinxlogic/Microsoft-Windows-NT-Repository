$!check-ref
$! will look for SRC and HREF and make sure the files exist
$ on control_y then goto end
$ debug="!"
$ quote="""
$ qeq="="""
$ htmdir=f$parse("''p1'",f$envir("default"),,"device")+ -
	f$parse("''p1'",f$envir("default"),,"directory")
$ htmdi=htmdir-"]"		! for subdirectories
$ if p2.eqs."" then p2=htmdir+"*.html"
$
$ open/write htm 'htmdir'check-ref.htm
$ write htm "<html>"
$ write htm "<head>"
$ write htm "<title>Check references</title>"
$ write htm "</HEAD>"
$ write htm "<body bgcolor=",quote,"#FFEECC",quote,">"
$ write htm "<H1>Check references ''f$time()'</H1>"
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "<P>"
$next:
$
$ file=f$search(p2,1)
$ if file.eqs."" then goto end
$
$ open/read data 'file'
$ linecount=0
$nextrec:
$ read/error=last data record
$ linecount=linecount+1
$nextref:
$ chop=0
$ if f$locate("src=",record).ne.f$length(record) then goto src
$ if f$locate("src =",record).ne.f$length(record) then goto src
$ if f$locate("href=",record).ne.f$length(record) then goto href
$ if f$locate("SRC=",record).ne.f$length(record) then goto src
$ if f$locate("HREF=",record).ne.f$length(record) then goto href
$ goto nextrec
$src:
$ chop=f$locate("src",record)+3
$ if chop.eq.0 then chop=f$locate("SRC",record)+3
$href:
$! if chop.eq.0 then chop=f$locate("href",f$edit(record,"UPCASE"))+4
$ if chop.eq.0 then chop=f$locate("href",record)+4
$ if chop.eq.0 then chop=f$locate("HREF",record)+4
$ 'debug' sh sym chop
$ 'debug' sh sym record
$ firstquote=f$locate(quote,record)
$ secondquote=f$locate(quote,f$extract(firstquote+1,f$length(record),record))
$ 'debug' sh sym firstquote
$ 'debug' sh sym secondquote
$ if firstquote.eq.f$length(record) then goto missingquote
$ if secondquote.eq.f$length(f$extract(firstquote+1,f$length(record),record)) then goto missingquote
$! refer=f$element(1,quote,record)
$ refn=f$element(0,quote,f$extract(f$locate(qeq,record)+2,f$length(record),record))
$ if refn.eqs."mailto:nic@python.demon.co.uk" then goto nextrec
$! common to all pages so ignore
$'debug' sh sym refn
$ fname=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ refer=f$extract(0,f$locate("#",refn),refn)
$'debug' sh sym refer
$ if refer.eqs."" then goto nulled
$ if refer.eqs."http://webcam.bolton.ac.uk/htbin/cam3/" then goto nextrec
$! common to all pages so ignore
$ if refer.nes.f$edit(refer,"lowercase")
$ then
$ write htm fname," MIXED CASE -----> <a href=",quote,refer,quote,">",-
	refer,"</a><br>"
$ endif
$ find=f$search("''htmdir'''refer'",2)
$ subdir=f$element(0,"/",refer)
$'debug' sh sym subdir
$ if f$search("''htmdir'''subdir'.dir",3).nes."" then find = -
	f$search("''htmdi'.''f$element(0,"/",refer)']''f$element(1,"/",refer)'",2)
$'debug' sh sym find
$ if find.eqs."" then goto badref
$ record = f$extract(chop,f$length(record),record)
$ 'debug' sh sym record
$ goto nextref
$
$badref:
$ prefix=" REF NOT FOUND ---> "
$ if f$locate("http://",refer).ne.f$length(refer) then prefix=-
         " EXTERNAL REF ----> "
$ write htm fname,prefix,"<a href=",quote,refer,quote,">",refer,"</a><br>"
$ goto nextrec
$nulled:
$ if f$length(refn).ne.0.and.f$locate("#",refn).ne.f$length(refn) then goto nextrec
$ fname=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ write htm fname," null ref on line number ",linecount,"<br>"
$ goto nextrec
$
$missingquote:
$ fname=f$edit(f$parse(file,,,"name")+f$parse(file,,,"type"),"lowercase")
$ write htm fname," missing quote(s) on line ",linecount," ",record,"<br>"
$ goto nextrec
$
$
$last:
$ close data
$ goto next
$end:
$ write htm "<P>"
$ write htm "<a href=",quote,"debug.htm",quote,">Back to debug main</a>"
$ write htm "</html>"
$ close htm
$ if f$trnlnm("data","lnm$process").nes."" then close data
$ exit
