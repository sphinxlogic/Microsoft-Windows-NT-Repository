.ifdef ALPHA
opt = opt-axp
cflags = /obj=$(MMS$TARGET_NAME).$(obj)/noopt/debug/nolist/include_path=inc$/standard=vaxc/nomember_aligmnent
.else
opt = opt
cflags = /obj=$(MMS$TARGET_NAME).$(obj)/noopt/debug/nolist/include_path=inc$
.endif

.ifdef obj
.else
obj = obj
.endif

.ifdef exe
.else
exe = exe
.endif

build :					executables,-
					documents
	write sys$output "Build complete"

freeware :				directories,-
					build,-
					[.SDCL]freeware_release.txt,-
					[.SDCL]freeware_readme.txt,-
					[.SDCL]freeware_demo.txt,-
					source,-
					objects,-
					[.SDCL.alpha_images]sdcl.exe,-
					[.SDCL.alpha_images]sdcl-debug.exe,-
					zip
	write sys$output "Freeware complete"

directories :
	if f$parse("[.SDCL]") .eqs. "" then create/dir [.SDCL]
	if f$parse("[.SDCL.alpha_images]") .eqs. "" then create/dir [.SDCL.alpha_images]
	if f$parse("[.SDCL.src]") .eqs. "" then create/dir [.SDCL.src]
	if f$parse("[.SDCL.vax_images]") .eqs. "" then create/dir [.SDCL.vax_images]
	
[.SDCL]freeware_release.txt :	src$:freeware_release.txt
	copy $(mms$source) $(mms$target)

[.SDCL]freeware_readme.txt :	src$:aaareadme.txt
	copy $(mms$source) $(mms$target)

[.SDCL]freeware_demo.txt :		exe$:sdcl.mem
	copy $(mms$source) $(mms$target)

source :
	copy [-.ref-lib]*.* [.SDCL.src]
	purge [.SDCL.src]

objects :
	copy src$:*.$(obj) [.SDCL.src]*.obj
	purge [.SDCL.src]

[.SDCL.alpha_images]sdcl.exe :	exe$:sdcl.$(exe)
	copy $(mms$source) $(mms$target)

[.SDCL.alpha_images]sdcl-debug.exe : exe$:sdcl.$(exe)
	copy $(mms$source) $(mms$target)

zip :
	zip www_root:[download]sdcl.zip [.SDCL...]*.* "-rujvV"

documents :				exe$:sdcl.mem
	write sys$output "Documents complete"

executables :				exe$:sdcl.$(exe),-
					exe$:sdcl-debug.$(exe)
	write sys$output "Executables complete"

exe$:sdcl.$(exe) :			obj$:sdcl.$(obj),-
					obj$:lex.$(obj),-
					obj$:output.$(obj),-
					obj$:stack.$(obj),-
					obj$:stmt.$(obj),-
					src$:sdcl.$(opt)
	link/exec=$(mms$target)/notrace -
		$(mms$source),-
		obj$:lex.$(obj),-
		obj$:output.$(obj),-
		obj$:stack.$(obj),-
		obj$:stmt.$(obj),-
		src$:sdcl.$(opt)/options

exe$:sdcl-debug.$(exe) :			obj$:sdcl.$(obj),-
					obj$:lex.$(obj),-
					obj$:output.$(obj),-
					obj$:stack.$(obj),-
					obj$:stmt.$(obj),-
					src$:sdcl.$(opt)
	link/exec=$(mms$target)/debug -
		$(mms$source),-
		obj$:lex.$(obj),-
		obj$:output.$(obj),-
		obj$:stack.$(obj),-
		obj$:stmt.$(obj),-
		src$:sdcl.$(opt)/options


obj$:lex.$(obj) :				src$:lex.c,-
					inc$:tcodes.h,-
					inc$:defs.h
	$(CC) $(CFLAGS) $(MMS$SOURCE)

obj$:output.$(obj) :			src$:output.c,-
					inc$:defs.h
	$(CC) $(CFLAGS) $(MMS$SOURCE)

obj$:sdcl.$(obj) :				src$:sdcl.c,-
					inc$:defs.h
	$(CC) $(CFLAGS) $(MMS$SOURCE)

obj$:stack.$(obj) :			src$:stack.c,-
					inc$:defs.h
	$(CC) $(CFLAGS) $(MMS$SOURCE)

obj$:stmt.$(obj) :				src$:stmt.c,-
					inc$:tcodes.h,-
					inc$:defs.h
	$(CC) $(CFLAGS) $(MMS$SOURCE)

exe$:sdcl.mem :				src$:sdcl.rno
	RUNOFF $(MMS$SOURCE)/OUTPUT=$(MMS$TARGET)
