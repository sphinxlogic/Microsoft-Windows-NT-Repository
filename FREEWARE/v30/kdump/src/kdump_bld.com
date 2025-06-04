$!
$! Build procedure for KDUMP utility
$!
$! To build KDUMP utility, you need VAX BLISS-32 and Macro compiler.
$! VAX BLISS compiler is also available from the OpenVMS Freeware CD-ROM.
$!
$! Written for OpenVMS Freeware CD-ROM V2.0 by r.miyabi, 2-Nov-1995
$!
$ cur_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE","SYNTAX_ONLY")
$ cur_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY","SYNTAX_ONLY")
$ define/process src$ 'cur_dev''cur_dir'
$ BLISS /list/object src$:DUMP.B32
$ BLISS /list/object src$:DUMPFILE.B32
$ macro/obj/list src$:dumpfaoln.mar
$ BLISS /list/object src$:DUMPHEADE.B32
$ message/object src$:dumpmsg.msg
$ library/create dump.olb -
  dump.obj,dumpfile,dumpfaoln.obj,dumpheade.obj,dumpmsg.obj
$ link /map=kdump /full /cross /notrace /nouserlibrary 	/exec=kdump -
  dump/include=(dump$main,dump$file,dump$header,dump$fao_line,dumpmsg), -
  src$:f11aacp/include=makstr, src$:f11bxqp/include=chksum, -
  sys$library:starlet/include=(cli$interface,cli$globals,sys$p1_vector), -
  sys$system:SYS.STB/SELECTIVE
$ library/compress dump.olb
$ deasss/process src$
$ exit
