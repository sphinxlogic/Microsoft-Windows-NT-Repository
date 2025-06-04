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
$ IF VAX    !  If target architecture is VAX (cross-tool)
$   THEN SDLLANG = "SDLLANG_VAX"
$   ELSE SDLLANG = "SDLLANG_ALPHA"
$   ENDIF
$
$	goto 'p1'
$!
$ SDL:
$	link 'p2' /map=map$:'EXE_NAME' /exe=exe$:'EXE_NAME' /full /cross -
							OBJ$:SDLMAIN.OBJ, -
							OBJ$:LIBRECDEP.OBJ, -
							OBJ$:SDLACTION.OBJ, -
							OBJ$:SDLDATA.OBJ, -
							OBJ$:SDLDUMP.OBJ, -
							OBJ$:SDLGETLIN.OBJ, -
							OBJ$:SDLINTREE.OBJ, -
							OBJ$:SDLIMGAC2.OBJ, -
							OBJ$:SDLIMGACT.OBJ, -
							OBJ$:SDLLEX.OBJ, -
							OBJ$:SDLOLDINTREE.OBJ, -
							OBJ$:SDLOUTTRE.OBJ, -
							OBJ$:SDLREADFILE.OBJ, -
							LIB$:SDL.OLB/LIBRARY, -
							LIB$:PAT.OLB/LIBRARY, -
							COM$:SDLMAIN.OPT/OPTIONS, -
							COM$:SDLVERSION.OPT/OPTIONS
$	goto common
$!
$ SDLNPARSE:
$	link 'p2' /map=map$:'EXE_NAME' /exe=exe$:'EXE_NAME' /full /cross -
							OBJ$:SDLNPARSE.OBJ, -
							OBJ$:LIBRECDEP.OBJ, -
							OBJ$:SDLACTION.OBJ, -
							OBJ$:SDLDATA.OBJ, -
							OBJ$:SDLDUMP.OBJ, -
							OBJ$:SDLGETLIN.OBJ, -
							OBJ$:SDLINTREE.OBJ, -
							OBJ$:SDLIMGAC2.OBJ, -
							OBJ$:SDLIMGACT.OBJ, -
							OBJ$:SDLLEX.OBJ, -
							OBJ$:SDLOLDINTREE.OBJ, -
							OBJ$:SDLOUTTRE.OBJ, -
							OBJ$:SDLREADFILE.OBJ, -
							LIB$:SDL.OLB/LIBRARY, -
							COM$:SDLMAIN.OPT/OPTIONS, -
							COM$:SDLVERSION.OPT/OPTIONS
$	goto common
$!
$ SDLBASIC:
$	link 'p2' /map=map$:'EXE_NAME' /share=exe$:'EXE_NAME' /full /cross -
							OBJ$:SDLBASIC.OBJ, -
							OBJ$:BASADD_COMMENTS.OBJ, -
							OBJ$:BASDO_BITFIELDS.OBJ, -
							OBJ$:BASDO_CHILDREN.OBJ, -
							OBJ$:BASDO_COM_NODE.OBJ, -
							OBJ$:BASDO_CONDITIONAL.OBJ, -
							OBJ$:BASDO_CONST.OBJ, -
							OBJ$:BASDO_ENTRY.OBJ, -
							OBJ$:BASDO_GLOB_COM.OBJ, -
							OBJ$:BASDO_ITEM.OBJ, -
							OBJ$:BASDO_LITERAL.OBJ, -
							OBJ$:BASDO_MODULE.OBJ, -
							OBJ$:BASDO_PARAM.OBJ, -
							OBJ$:BASDO_STRUCTURE.OBJ, -
							OBJ$:BASDO_TOP_LEVEL_DIM.OBJ, -
							OBJ$:BASDO_UNION.OBJ, -
							OBJ$:BASDO_VAR_STRING.OBJ, -
							OBJ$:BASGET_TYPE.OBJ, -
							OBJ$:BASOUTPUTNODE.OBJ, -
							OBJ$:BASOUTPUT_BUF.OBJ, -
							OBJ$:BASPUT_CHAR_VAR_RECS.OBJ, -
							OBJ$:BASPUT_TYPES.OBJ, -
							OBJ$:BASSPECIAL_CHECKS.OBJ, -
							OBJ$:BASTABS.OBJ, -
							LIB$:SDL.OLB/LIBRARY, -
							COM$:SDLBASIC.OPT/OPTIONS, -
							COM$:'SDLLANG'.OPT/OPTIONS, -
							COM$:SDLVERSION.OPT/OPTIONS
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
							OBJ$:'P1', -
							LIB$:SDL.OLB/LIBRARY, -
							COM$:'SDLLANG'.OPT/OPTIONS, -
							COM$:SDLVERSION.OPT/OPTIONS
$ 
$ common:
$	sts = $status
$	exit sts
