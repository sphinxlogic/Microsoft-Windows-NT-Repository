$!
$	kitver:= 021-2
$!
$ on warning then goto error
$create/dir [.work]/protection=o:Rwed
$ if f$search ("[.work]*.*").nes."" then delete/log [.work]*.*;*
$open/read files mksrc.dat
$read:
$ read files line/end= done
$ line= f$edit (line,"TRIM,COMPRESS,UPCASE")
$ infile= f$element (0, ";", f$element (0, " ", line))
$ infile= f$parse (infile,"[.src];")
$ outfile= f$parse ("[.work];",,infile)
$ if f$element (1," ",line).eqs."N" then goto copy
$ type= f$parse (infile,,,"TYPE")-"."
$ line= f$element (0, ";", line)
$ goto parse_'type'
$parse_c:
$parse_h:
$ append/new [.src]header.c,'infile' 'outfile'
$ goto read
$parse_readme:
$parse_mms:
$parse_com:
$parse_cld:
$parse_opt:
$parse_mar:
$copy:
$ copy 'infile' 'outfile'
$ goto read
$done:
$ close files
$ on warning then stop
$ set def [.work]
$backup/block=2048 *.*; [-]emon_src'kitver'.bck/save
$ exit
$ delete *.*;/l
$ set def [-]
$ delete work.dir;
$ goto exit
$error: close/nolog files
$ exit
