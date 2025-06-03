*2D
 $	link/NODEB/notrace /exe=VMI$KWD:DECW$banner.exe sys$input /options 
 VMI$KWD:banner.olb/inc=banner
 VMI$KWD:banner.olb/inc=bannersubs
 VMI$KWD:banner.olb/inc=bannerclock
 VMI$KWD:banner.olb/inc=bannerglobe
 VMI$KWD:banner.olb/inc=bannerhanoi
 VMI$KWD:banner.olb/inc=bannercpu
 VMI$KWD:banner.olb/inc=bannermonitor
 VMI$KWD:banner.olb/inc=bannermessage
 VMI$KWD:banner.olb/inc=bannercube
 VMI$KWD:banner.olb/inc=bannerprocess
 VMI$KWD:banner.olb/inc=bannerlck
 VMI$KWD:banner.olb/inc=bannercwps
 VMI$KWD:banner.olb/inc=bannerpe
 VMI$KWD:banner.olb/inc=bannerremote
 SYS$SYSTEM:sys.stb/sel
 sys$share:vaxcrtl/share
 sys$library:decw$dwtlibshr/share
 sys$library:decw$xlibshr/share
*2E
*2I
*4D
 $	link/NODEB/notrace /exe=VMI$KWD:DECW$banner.exe VMI$KWD:BANNER.OPT /options 
*4E
*2E
*4I
*5D
 $	link/NODEB/notrace"''banner$debug'"/exe=VMI$KWD:DECW$banner.exe VMI$KWD:BANNER.OPT /options 
*5E
*4E
*5I
 $	if "''banner$debug'" .eqs. "" then banner$debug := "/NODEB/NOTRACE"
 $	link'banner$debug'/exe=VMI$KWD:DECW$banner.exe VMI$KWD:BANNER.OPT /options 
*5E
