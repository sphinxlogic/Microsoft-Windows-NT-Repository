!
!
!   this is an MMS description file for building the SCAN runtime library
!
!

!
!
!   this section builds the library
!
!

robj$:scnrtl.exe : -
	robj$:scnanswer.obj, -
	robj$:scncheck.obj, -
	robj$:scnconvrt.obj, -
	robj$:scnbits.obj, -
	robj$:scndebug.obj, -
	robj$:scndbgext.obj, -
	robj$:scndbgrou.obj, -
	robj$:scntredbg.obj, -
	robj$:scndispat.obj, -
	robj$:scnepilog.obj, -
	robj$:scnio.obj, -
	robj$:scnmacro.obj, -
	robj$:scnmsg.obj, -
	robj$:scnmsgptr.obj, -
	robj$:scnoutput.obj, -
	robj$:scnprune.obj, -
	robj$:scnssintf.obj, -
	robj$:scnstart.obj, -
	robj$:scntoken.obj, -
	robj$:scntree.obj, -
	robj$:scntrefnd.obj, -
	robj$:scnutil.obj, -
	robj$:scnerror.obj, -
	robj$:scnerrint.obj, -
	robj$:scnvector.obj, -
	rsrc$:scnrtl.opt
		libr/compress/output=robj$:scnrtl.olb robj$:scnrtl.olb
		link/notrace/exe=robj$:scnrtl/share=scnrtl/map=rlis$:scnrtl/full/cross rsrc$:scnrtl/opt
		purge rtl$auto:*.*
		purge robj$:*.*
		purge rlis$:*.*


!
!   create the require files
!

!   machdef.sdl

rtl$auto:machdef.r32 :	-
	rsrc$:machdef.sdl
		sdl/language=(blissf=rtl$auto:machdef.r32) rsrc$:machdef

rtl$auto:scnlib.mlb($machdef) :	-
	rsrc$:machdef.sdl
		sdl/language=(macro=rtl$auto:machdef.mar) rsrc$:machdef
		lib/macro rtl$auto:scnlib.mlb rtl$auto:machdef.mar

rtl$auto:dstrecrds.l32 : -
	dobj$$:dstrecrds.l32
		copy dobj$$:dstrecrds.l32 rtl$auto:

rsrc$:scndef.req : -
	rtl$auto:dbgext.l32
                ! do nothing


rtl$auto:dbgext.l32 : -
	dobj$$:dbgext.l32
		copy dobj$$:dbgext.l32 rtl$auto:

!
!	compile the pattern recognition modules
!

robj$:scnssintf.obj :	-
	rsrc$:scnssintf.mar, -
	rtl$auto:scnlib.mlb($machdef)
		macro/obj=robj$:/lis=rlis$: rsrc$:scnssintf+rtl$auto:scnlib/lib
		library robj$:scnrtl.olb robj$:scnssintf.obj


robj$:scnerrint.obj :	-
	rsrc$:scnerrint.mar, -
	rtl$auto:scnlib.mlb($machdef)
		macro/obj=robj$:/lis=rlis$: rsrc$:scnerrint+rtl$auto:scnlib/lib
		library robj$:scnrtl.olb robj$:scnerrint.obj


robj$:scnvector.obj :	-
	rsrc$:scnvector.mar
		macro/obj=robj$:/lis=rlis$: rsrc$:scnvector


robj$:scnstart.obj :	-
	rsrc$:scnstart.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnstart
		library robj$:scnrtl.olb robj$:scnstart.obj


robj$:scntoken.obj :	-
	rsrc$:scntoken.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scntoken
		library robj$:scnrtl.olb robj$:scntoken.obj


robj$:scnanswer.obj :	-
	rsrc$:scnanswer.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnanswer
		library robj$:scnrtl.olb robj$:scnanswer.obj


robj$:scnmacro.obj :	-
	rsrc$:scnmacro.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnmacro
		library robj$:scnrtl.olb robj$:scnmacro.obj


robj$:scnerror.obj :	-
	rsrc$:scnerror.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnerror
		library robj$:scnrtl.olb robj$:scnerror.obj


robj$:scnio.obj :	-
	rsrc$:scnio.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnio
		library robj$:scnrtl.olb robj$:scnio.obj


robj$:scnoutput.obj :	-
	rsrc$:scnoutput.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnoutput
		library robj$:scnrtl.olb robj$:scnoutput.obj

robj$:scndbgext.obj :	-
	rsrc$:scndbgext.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32, -
	rtl$auto:dstrecrds.l32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scndbgext
		library robj$:scnrtl.olb robj$:scndbgext.obj

robj$:scntredbg.obj :	-
	rsrc$:scntredbg.b32, -
	rsrc$:scndef.req, -
	rsrc$:treedef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scntredbg
		library robj$:scnrtl.olb robj$:scntredbg.obj

robj$:scndbgrou.obj :	-
	rsrc$:scndbgrou.mar
		macro/obj=robj$:/lis=rlis$: rsrc$:scndbgrou
		library robj$:scnrtl.olb robj$:scndbgrou.obj

robj$:scndebug.obj :	-
	rsrc$:scndebug.b32, -
	rsrc$:scndef.req, -
	rsrc$:treedef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scndebug
		library robj$:scnrtl.olb robj$:scndebug.obj


robj$:scnbits.obj :	-
	rsrc$:scnbits.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnbits
		library robj$:scnrtl.olb robj$:scnbits.obj


robj$:scncheck.obj :	-
	rsrc$:scncheck.mar
		macro/obj=robj$:/lis=rlis$: rsrc$:scncheck
		library robj$:scnrtl.olb robj$:scncheck.obj


robj$:scnconvrt.obj :	-
	rsrc$:scnconvrt.mar
		macro/obj=robj$:/lis=rlis$: rsrc$:scnconvrt
		library robj$:scnrtl.olb robj$:scnconvrt.obj


robj$:scndispat.obj :	-
	rsrc$:scndispat.b32, -
	rsrc$:scndef.req, -
	rtl$auto:machdef.r32
		bliss/obj=robj$:/lis=rlis$: rsrc$:scndispat
		library robj$:scnrtl.olb robj$:scndispat.obj


robj$:scnutil.obj :	-
	rsrc$:scnutil.b32, -
	rsrc$:scndef.req
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnutil
		library robj$:scnrtl.olb robj$:scnutil.obj


robj$:scnepilog.obj :	-
	rsrc$:scnepilog.b32, -
	rsrc$:scndef.req
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnepilog
		library robj$:scnrtl.olb robj$:scnepilog.obj


robj$:scnprune.obj :	-
	rsrc$:scnprune.b32, -
	rsrc$:scndef.req, -
	rsrc$:treedef.req
		bliss/obj=robj$:/lis=rlis$: rsrc$:scnprune
		library robj$:scnrtl.olb robj$:scnprune.obj


robj$:scntree.obj :	-
	rsrc$:scntree.b32, -
	rsrc$:scndef.req, -
	rsrc$:treedef.req
		bliss/obj=robj$:/lis=rlis$: rsrc$:scntree
		library robj$:scnrtl.olb robj$:scntree.obj


robj$:scntrefnd.obj :	-
	rsrc$:scntrefnd.b32, -
	rsrc$:scndef.req, -
	rsrc$:treedef.req
		bliss/obj=robj$:/lis=rlis$: rsrc$:scntrefnd
		library robj$:scnrtl.olb robj$:scntrefnd.obj


robj$:scnmsg.obj :	-
	rsrc$:scnmsg.msg
		message/obj=robj$:/lis=rlis$: rsrc$:scnmsg
		link/exe=robj$:/share=scnmsg robj$:scnmsg


robj$:scnmsgptr.obj :	-
	rsrc$:scnmsg.msg
		message/obj=robj$:scnmsgptr/file=scnmsg/lis=rlis$:scnmsgptr rsrc$:scnmsg
		library robj$:scnrtl.olb robj$:scnmsgptr.obj


