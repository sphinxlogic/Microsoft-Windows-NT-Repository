$!------------------------------------------------------------------------------
$!  Compile and/or link JUMP under VAX or Alpha.
$!
$!  Optional P1 parameter value:
$!
$!	None	- if P1 is missing, compile and link with NO TRACEBACK.
$!	C	- if "C" is in P1, compile.
$!	L	- if "L" is in P1, link with NO TRACEBACK.
$!	T	- if "T" is in P1, link with TRACEBACK.
$!
$!  If P1 contains both "L" and "T", "T" is used.
$!
$!  Optional P2 parameter value:
$!
$!	None	- if P2 is missing, presume Pascal at particular version
$!		  based on architecture (see code below).
$!	N	- if "N" is in P2, use /USAGE=NOPERFORMANCE in the compile.
$!..............................................................................
$!  Author: Jonathan Ridler,
$!	    Information Technology Services,
$!	    The University of Melbourne,
$!	    Parkville, Victoria, AUSTRALIA, 3052.
$!
$!	    Internet: jonathan@unimelb.edu.au
$!..............................................................................
$!
$!  With new releases of OpenVMS, it may be necessary to re-link JUMP.
$!------------------------------------------------------------------------------
$
$ on ERROR  then  EXIT
$ say   := write SYS$OUTPUT
$ arch   = f$getsyi ("ARCH_NAME")			! "VAX" or "Alpha"
$ trace := /notraceback
$
$ p1    = f$edit (p1,"UPCASE,UNCOMMENT,COLLAPSE")
$ p1len = f$length (p1)
$ compile = (p1 .eqs. "") .or. (f$locate (p1,"C") .nes. p1len)
$ linking = (p1 .eqs. "") .or. (f$locate (p1,"L") .nes. p1len) .or. -
			       (f$locate (p1,"T") .nes. p1len)
$
$ p2 = f$edit (p2,"UPCASE,UNCOMMENT,COLLAPSE")
$ perform = (f$locate (p2,"N") .nes. f$length (p2)) .or. (arch .eqs. "Alpha")
$
$!  If the Pascal compiler is version 5.3 or later, add ",noperformance" to
$!  the usage_types below.  Presume Pascal version from architecture.  Allow
$!  ",noperformance" to be forced by p2.
$
$ if (arch .eqs. "VAX")
$ then
$   nowarning = ""
$ else
$   nowarning = "/warning=noinformation"
$ endif
$
$ if perform
$ then
$   usage_types = "all,noperformance"	! Assume Pascal >= v5.3 (for Alpha)
$ else
$   usage_types = "all"			! Assume Pascal <= v5.2 (for VAX)
$ endif
$
$ if compile
$ then
$   say "Compiling common JUMP Pascal source ..."
$   pascal jump /usage=('usage_types') /object=jump.obj_'arch'
$   say "Compiling ''arch' architecture-specific JUMP Pascal source ..."
$   pascal jump_'arch' /usage=('usage_types') /object=jump_'arch'.obj
$   say "Compiling JUMP Macro source ..."
$   macro jump_parse_id 'nowarning' /object=jump_parse_id.obj_'arch'
$ endif
$
$!  Compile each time to ensure the object is correct for the architecture.
$
$ say "Compiling JUMP CLD ..."
$ set command /object jump_cld
$
$ if linking
$ then
$   if (f$locate (p1,"T") .nes. p1len)  then  trace := /traceback
$   say "Linking JUMP ..."
$   if arch .eqs. "VAX"
$   then
$     link         'trace' jump.obj_vax,jump_vax.obj,jump_cld.obj, -
		jump_parse_id.obj_vax,jump.opt/option,		   -
		SYS$SYSTEM:SYS.STB/select,SYSDEF.STB/select
$   else
$     link /sysexe 'trace' jump.obj_alpha,jump_alpha.obj,jump_cld.obj, -
		jump_parse_id.obj_alpha,jump.opt/option,	       -
		SYS$LOADABLE_IMAGES:SYSDEF.STB/select
$   endif
$ endif
$
$ if linking
$ then
$   say "*** JUMP built successfully ***"
$   say "Don't forget to install the image with the correct privileges."
$ endif
$
$ EXIT 1
