$ foo = ""
$ if f$trnlnm("globe_data") .nes. "" then goto gotit
$ foo = f$parse("",,,"DEVICE")+f$directory()+"GLOBE.DAT"
$ define globe_data 'foo'
$gotit:
$ run rotate_globe
$ if foo .nes. "" then deassign globe_data
$ exit
