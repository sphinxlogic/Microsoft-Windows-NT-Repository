$ DEFINE SRC$ SYS$DISK:[]
$ DEFINE INC$ SRC$:
$ DEFINE OBJ$ SYS$DISK:[]
$ DEFINE EXE$ SYS$DISK:[]
$ ALPHA = F$GETSYI("CPU") .GT. 127
$ IF ALPHA
$ THEN	
$	CFLAGS = "/NOOPT/DEBUG/NOLIST/INCLUDE_PATH=INC$/STANDARD=VAXC/NOMEMBER_ALIGMNENT"
$	OPT = "OPT-AXP"
$ ELSE	
$	CFLAGS = "/NOOPT/DEBUG/NOLIST/INCLUDE_PATH=INC$"
$	OPT = "OPT"
$ ENDIF
$ CC /obj=OBJ$:SDCL.obj 'CFLAGS' SRC$:SDCL.C
$ CC /obj=OBJ$:LEX.obj 'CFLAGS' SRC$:LEX.C
$ CC /obj=OBJ$:OUTPUT.obj 'CFLAGS' SRC$:OUTPUT.C
$ CC /obj=OBJ$:STACK.obj 'CFLAGS' SRC$:STACK.C
$ CC /obj=OBJ$:STMT.obj 'CFLAGS' SRC$:STMT.C
$ link/exec=EXE$:SDCL.EXE/notrace 		OBJ$:SDCL.OBJ,		obj$:lex.obj,		obj$:output.obj,		obj$:stack.obj,		obj$:stmt.obj,		src$:sdcl.'OPT'/options
$ link/exec=EXE$:SDCL-DEBUG.EXE/debug 		OBJ$:SDCL.OBJ,		obj$:lex.obj,		obj$:output.obj,		obj$:stack.obj,		obj$:stmt.obj,		src$:sdcl.'OPT'/options
$ write sys$output "Executables complete"
$ write sys$output "Build complete"
