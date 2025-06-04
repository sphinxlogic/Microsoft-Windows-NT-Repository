.IFDEF ALPHA
OPTIM		= /OPTIMIZE
CC		= GEMC/STANDARD=VAXC/PREFIX=ALL
LIBR		= LIBRARY/ALPHA
INCLUDE		= /INCLUDE=(SRC$, ALPHA$COMMON:[GEMC])
LINK		= LINK/ALPHA_OUT
.ENDIF

cflags = $(OPTIM)$(INCLUDE)/obj=obj$:/list=lis$:/machine

obj$:devilib.olb 	: obj$:devcommon.obj obj$:devtext.obj obj$:devsixel.obj
	if f$search("$(mms$target)") .eqs. "" then $(libr)/create $(mms$target)
	$(libr) $(librflangs) $(mms$target) $(mms$changed_list)

obj$:devcommon.obj 	: src$:devcommon.c src$:devildef.h

obj$:devtext.obj 	: src$:devtext.c src$:devildef.h

obj$:devsixel.obj 	: src$:devsixel.c src$:devildef.h

