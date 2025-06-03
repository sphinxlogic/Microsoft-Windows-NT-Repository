kpri.exe	: kprilib.olb	kpri.opt	jsy$devil:devilib.olb
	ass/user sys$share:librtl.exe librtl
	link/map=kpri/full/cross/notrace/nouserlibrary/exec=$(mms$target_name) -
		jsy$kpri:kprilib/inc=kpri$kprint/lib, jsy$devil:devilib/lib, -
		jsy$kpri:jsylib/lib, jsy$kpri:kpri/option

debug		: kprilib.olb	kpri.opt	jsy$devil:devilib.olb
	link/map=kpri/full/cross/trace/nouserlibrary/exec=kpri/debug -
		kprilib/inc=kpri$kprint/lib, jsy$devil:devilib/lib, jsy$library:jsylib/lib

kprilib.olb	: kprint.obj	kprirms.obj	kpriutil.obj	kpriparse.obj-
		  kprimsg.obj	fexconv.obj	preconv.obj
	if f$search("$(mms$target)") .eqs. "" then $(libr)/create $(mms$target)
	$(libr) $(librflangs) $(mms$target) $(mms$changed_list)

kprint.obj	: kprint.c	kpridef.h	jsy$devil:devildef.h

kprirms.obj	: kprirms.c	kpridef.h

kpriutil.obj	: kpriutil.c	kpridef.h

kpriparse.obj	: kpriparse.c	kpridef.h

fexconv.obj	: fexconv.c	fextbl.h

preconv.obj	: preconv.c

kprimsg.obj	: kprimsg.msg
