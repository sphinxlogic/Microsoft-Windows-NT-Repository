.FIRST
	@ write sys$output "    *** STARTING RWMBX BUILD ***"
        @ write sys$output ""

.LAST
        @ write sys$output ""
	@ write sys$output "    *** RWMBX BUILD COMPLETE ***"

ALL DEPENDS_ON RWMBX.EXE
        @ write sys$output ""
	@ write sys$output "	WELCOME TO THE WACKY WORLD OF MAILBOXes"

RWMBX.EXE DEPENDS_ON  RWMBX.OBJ
	LINK/SYSEXE RWMBX

RWMBX.OBJ DEPENDS_ON RWMBX.C
	CC/LIST/NOWARNING/prefix_library_entries=ALL_ENTRIES RWMBX+ALPHA$LIBRARY:SYS$LIB_C/LIBRARY
