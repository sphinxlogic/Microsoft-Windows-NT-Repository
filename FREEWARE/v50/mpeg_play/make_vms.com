$! MAKE_VMS.COM
$! 05-SEP-2000, David Mathog, Biology Division, Caltech
$! New port of version 2.4, using 
$!  Compaq C V6.2-007 on OpenVMS Alpha V7.2-1
$!
$!
$ mycc :== cc/standard=relaxed/prefix=all/define=(NDEBUG,SIG_ONE_PARAM)/arch=host
$!
$ mycc  util.c
$ mycc  video.c
$ mycc  parseblock.c
$ mycc  motionvector.c
$ mycc  decoders.c
$ mycc  fs2.c
$ mycc  fs2fast.c
$ mycc  fs4.c
$ mycc  hybrid.c
$ mycc  hybriderr.c
$ mycc  2x2.c
$ mycc  floatdct.c
$ mycc  gdith.c
$ mycc  gray.c
$ mycc  mono.c
$ mycc  jrevdct.c
$ mycc  16bit.c
$ mycc  util32.c
$ mycc  ordered.c
$ mycc  ordered2.c
$ mycc  mb_ordered.c
$ mycc  readfile.c
$ mycc  ctrlbar.c
$ lib/object/create mpegplay.olb *.obj
$ mycc  main.c
$ link/exe=mpeg_play main.obj,mpegplay.olb/lib,mpeg_play.opt/opt
$ delete *.obj.*
$ delete *.olb.*
$ set file/prot=w:re *.*
$ write sys$output "All Done"
