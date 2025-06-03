.IFDEF __ALPHA__
MFLAGS = $(MFLAGS)/NOWARNINGS		!There are unaligned memory references
.ENDIF

sd$(exe) :	sd$(obj)
	$(link)$(linkflags)/notrace $(mms$source)

sd$(obj) :	sd.mar
