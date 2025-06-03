$set ver
$ define vmi$kwd []
$	link/DEB/notrace /exe=banner.exe  -
		banner.opt /options     'opts'
