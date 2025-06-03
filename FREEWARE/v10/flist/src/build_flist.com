$ alpha = 0
$ if f$search("SYS$LOADABLE_IMAGES:SYS$BASE_IMAGE.EXE").nes. "" then alpha = 1
$ write sys$output "Compiling FLIST...."
$ cc = "cc/nolist"
$ if alpha then cc = "cc/standard=vaxc/nolist"
$ 'cc' FLIST
$ write sys$output "Linking FLIST...."
$ if alpha
$ then	link/notrace flist
$ else	link/notrace FLIST,FLIST_VAX.OPT/options
$ endif
$ write sys$output "Compiling FLIST.TPU...."
$ edit/tpu/nosection/command=flist.tpu/nodisplay
$ write sys$output "FLIST build complete"
$ exit
