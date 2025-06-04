kpri.exe	: kprilib.olb	kpri.opt	jsy$devil:devilib.olb
	link/map=kpri/full/cross/notrace/nouserlibrary/exec=$(mms$target_name) -
		jsy$kpri:kprilib/inc=kpri$kprint/lib, jsy$devil:devilib/lib, -
		jsy$kpri:test/option

kprilib.olb	: kprint.obj	kprirms.obj	kpriutil.obj -
		  kpriparse.obj	kprimsg.obj
	if f$search("$(mms$target)") .eqs. "" then $(libr)/create $(mms$target)
	$(libr) $(librflangs) $(mms$target) $(mms$changed_list)

kprint.obj	: kprint.c	kpridef.h	jsy$devil:devildef.h

kprirms.obj	: kprirms.c	kpridef.h

kpriutil.obj	: kpriutil.c	kpridef.h

kpriparse.obj	: kpriparse.c	kpridef.h

kprimsg.obj	: kprimsg.msg
