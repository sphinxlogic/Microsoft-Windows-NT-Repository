$! SDLLNK.COM
$!
$! This file is invoked from SDLBLD.COM to link all the images.
$!
$! This procedure is called with the following parameters:
$!
$!	p1 = Required image name
$!	p2 = /DEBUG qualifier or null
$!
$if f$trnlnm("alpha_tool") .nes. ""
$ then EXE_NAME = "ALPHA_''p1'"
$ else EXE_NAME = "''p1'"
$ endif
$
$	goto 'p1'
$!
$ SDL:
$	link 'p2' /map=map$:'EXE_NAME' /exe=exe$:'EXE_NAME' /full /cross -
		obj$:sdlmain, lib$:sdl/lib,	-
		src$:sdlversion.opt/options
$	goto common
$!
$ SDLNPARSE:
$	link 'p2' /map=map$:'EXE_NAME' /exe=exe$:'EXE_NAME' /full /cross -
		obj$:sdlmain2, lib$:sdl/lib,-
		src$:sdlversion.opt/options
$	goto common
$!
$ SDLBASIC:
$	link 'p2' /map=map$:'EXE_NAME' /share=exe$:'EXE_NAME' /full /cross -
		obj$:sdlbasic, lib$:sdlbaslib/lib, lib$:sdl/lib, -
		src$:sdlbasic.opt/options, -
		src$:sdlversion.opt/options
$	goto common
$!
$ SDLADA:
$ SDLBLISS:
$ SDLBLISS64:
$ SDLBLISSF:
$ SDLCC:
$ SDLDTR:
$ SDLEPASCAL:
$ SDLFORTRAN:
$ SDLFORTV3:
$ SDLLISP:
$ SDLMACRO:
$ SDLPASCAL:
$ SDLPLI:
$ SDLSDML:
$ SDLTPU:
$ SDLUIL:
$	link 'p2' /map=map$:'EXE_NAME' /share=exe$:'EXE_NAME' /full /cross -
		obj$:'p1', lib$:sdl/lib,	-
		src$:sdllang.opt/options,	-
		src$:sdlversion.opt/options
$ 
$ common:
$	sts = $status
$	exit sts
