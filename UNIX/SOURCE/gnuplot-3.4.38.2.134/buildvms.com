$ ! buildvms.com  (Command file to compile/link gnuplot and doc2hlp)
$ CFLAGS = "/nowarn/NOOP/define=(NOGAMMA,MEMSET)"
$ TERMFLAGS = "/define=(X11,VMS)"
$! For DECwindows:
$ DEFINE X11 DECW$INCLUDE
$ set verify
$ cc 'CFLAGS' binary.c
$ cc 'CFLAGS' gnubin.c
$ cc 'CFLAGS' specfun.c
$ cc 'CFLAGS' bitmap.c
$ cc 'CFLAGS' command.c
$ cc 'CFLAGS' contour.c
$ cc 'CFLAGS' eval.c
$ cc 'CFLAGS' graphics.c
$ cc 'CFLAGS' graph3d.c
$ cc 'CFLAGS' internal.c
$ cc 'CFLAGS' 'TERMFLAGS' misc.c
$ cc 'CFLAGS' parse.c
$ cc 'CFLAGS' plot.c
$ cc 'CFLAGS' scanner.c
$ cc 'CFLAGS' setshow.c
$ cc 'CFLAGS' standard.c
$ cc 'CFLAGS' 'TERMFLAGS' term.c
$ cc 'CFLAGS' util.c
$ cc 'CFLAGS' version.c
$ link /exe=gnuplot -
   bitmap.obj,command.obj,contour.obj,eval.obj,graphics.obj,graph3d.obj, -
   binary,gnubin,specfun, -
   internal.obj,misc.obj,parse.obj,plot.obj,scanner.obj,setshow.obj, -
   standard.obj,term.obj,util.obj,version.obj ,linkopt.vms/opt
$ cc 'CFLAGS' bf_test.c
$ link /exe=bf_test bf_test.obj,binary.obj,linkopt.vms/opt
$ ren bf_test.exe [.demo]
$ CC/nowarn/DEFINE=VMS GPLT_X11
$ LINK /exe=GNUPLOT_X11 gplt_x11,SYS$INPUT:/OPT
SYS$SHARE:DECW$XLIBSHR/SHARE
$ cc [.docs]doc2hlp.c
$ link doc2hlp,linkopt.vms/opt
$ @[.docs]doc2hlp.com
$ library/create/help gnuplot.hlb gnuplot.hlp
$ run [.demo]bf_test
$ set noverify

