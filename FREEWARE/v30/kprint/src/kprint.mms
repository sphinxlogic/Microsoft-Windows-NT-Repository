.IFDEF ALPHA
OPTIM		= /NOOPTIMIZE
CC		= GEMC/STANDARD=VAXC/PREFIX=ALL
LIBR		= LIBRARY/ALPHA
INCLUDE		= /INCLUDE=(SRC$)
LINK		= LINK/ALPHA_OUT
.else
OPTIM		= /OPTIMIZE
.ENDIF

cflags = $(OPTIM)$(INCLUDE)/obj=obj$:/list=lis$:

default 	: exe$:kpri.exe exe$:jsy_kprint.cld exe$:jsy_kprint.hlp -
		  com$:kprint_files.dat
        purge exe$:

exe$:kpri.exe 	: obj$:kprilib.olb obj$:devilib.olb
        $(link) /map=lis$:kpri /full /cross /notrace /nouserlibrary -
		/exec=$(mms$target_name) obj$:kprilib/inc=kpri$kprint/lib, -
		obj$:devilib/lib, kpri$opt/option

obj$:kprilib.olb 	: obj$:kprint.obj obj$:kprirms.obj obj$:kpriutil.obj -
			  obj$:kpriparse.obj obj$:kprimsg.obj -
			  obj$:fexconv.obj obj$:preconv.obj
        if f$search("$(mms$target)") .eqs. "" then $(libr)/create $(mms$target)
        $(libr) $(librflangs) $(mms$target) $(mms$changed_list)

obj$:kprint.obj 	: src$:kprint.c src$:kpridef.h src$:devildef.h

obj$:kprirms.obj 	: src$:kprirms.c src$:kpridef.h

obj$:kpriutil.obj 	: src$:kpriutil.c src$:kpridef.h

obj$:kpriparse.obj 	: src$:kpriparse.c src$:kpridef.h

obj$:fexconv.obj 	: src$:fexconv.c src$:fextbl.h

obj$:preconv.obj 	: src$:preconv.c

obj$:kprimsg.obj 	: src$:kprimsg.msg
.ifdef ALPHA
	message/alpha/obj=obj$:/list=lis$: src$:kprimsg.msg 
.else
	message/obj=obj$:/list=lis$: src$:kprimsg.msg 
.endif

.ifdef ALPHA
exe$:jsy_kprint.cld 		: src$:jsy_kprint_alpha.cld
        copy src$:jsy_kprint_alpha.cld exe$:jsy_kprint.cld
.else
exe$:jsy_kprint.cld 		: src$:jsy_kprint.cld
        copy src$:jsy_kprint.cld exe$:
.endif

.ifdef ALPHA
exe$:jsy_kprint.hlp 		: src$:jsy_kprint_alpha.hlp
        copy src$:jsy_kprint_alpha.hlp exe$:jsy_kprint.hlp
.else
exe$:jsy_kprint.hlp 		: src$:jsy_kprint.hlp
        copy src$:jsy_kprint.hlp exe$:
.endif                               
                            
! 	++ Required to build JVMS kit ++
com$:kprint_files.dat 	: src$:kprint_files.dat
        copy src$:kprint_files.dat com$:
