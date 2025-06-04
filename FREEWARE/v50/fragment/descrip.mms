.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
.ENDIF

fragment$(exe) :	fragment$(obj)
	$(link)$(linkflags)/notrace $(mms$source)

fragment$(obj) :	fragment.mar
