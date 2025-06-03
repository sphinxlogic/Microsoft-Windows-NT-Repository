bflags = /list=lis$:/object=obj$:

default 	: exe$:kdump.exe exe$:jsy_kdump.cld exe$:jsy_kdump.hlp -
		  com$:kdump_files.dat
		library/compress/output=obj$: obj$:dump.olb
	        purge exe$:

exe$:kdump.exe 		: obj$:dump.olb
	link /map=lis$:kdump /full /cross /notrace /nouserlibrary -
	/exec=exe$:kdump -
	obj$:dump/include=(dump$main,dump$file,dump$header,dump$fao_line,-
	dumpmsg), -
	src$:f11aacp/include=makstr, src$:f11bxqp/include=chksum, -
	sys$library:starlet/include=(cli$interface,cli$globals,sys$p1_vector), -
	sys$system:SYS.STB/SELECTIVE

obj$:dump.olb 		: obj$:dump.obj obj$:dumpfile.obj obj$:dumpfaoln.obj -
	                  obj$:dumpheade.obj obj$:dumpmsg.obj
  	library/create obj$:dump.olb -
	    obj$:dump.obj,obj$:dumpfile.obj,obj$:dumpfaoln.obj,-
	    obj$:dumpheade.obj,obj$:dumpmsg.obj

obj$:dump.obj 		: src$:dump.b32 src$:dumpre.req

obj$:dumpfile.obj 	: src$:dumpfile.b32 src$:dumpre.req

obj$:dumpheade.obj 	: src$:dumpheade.b32 src$:dumpre.req

obj$:dumpfaoln.obj 	: src$:dumpfaoln.mar
	macro/obj=obj$:/list=lis$: src$:dumpfaoln.mar

obj$:dumpmsg.obj 	: src$:dumpmsg.msg
	message/object=obj$: src$:dumpmsg.msg
  
exe$:jsy_kdump.cld 	: src$:jsy_kdump.cld
        copy src$:jsy_kdump.cld exe$:

exe$:jsy_kdump.hlp 	: src$:jsy_kdump.hlp
        copy src$:jsy_kdump.hlp exe$:


! 	++ Required to build JVMS kit ++
com$:kdump_files.dat 	: src$:kdump_files.dat
        copy src$:kdump_files.dat com$:

