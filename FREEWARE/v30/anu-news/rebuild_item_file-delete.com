$ inquire/nopunctuation file "Enter rebuild_item_file logfile name: "
$ open/read input1 'file
$!
$ loop:
$   read/end=closeit input1 line
$   if f$extract(0,14,line) .eqs. "Error adding: "
$   then
$     line = line - f$extract(0,14,line)
$     p1 = f$locate("<",line)
$     p2 = f$locate(":",line)
$     p3 = f$locate(">",line)
$     newsgroup = f$extract(p1+1,p2-p1-1,line)
$     itemnum = f$extract(p2+1,p3-p2-1,line)
$     write sys$output "$ delete/log news_device:[''newsgroup']''itemnum'.itm;0"
$     delete/log news_device:['newsgroup']'itemnum'.itm;0
$   endif
$   goto loop
$!
$ closeit:
$   close input1
$   exit
