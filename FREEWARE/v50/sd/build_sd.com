$!------------------------------------------------------------------------------
$!  Compile and/or link SD under VAX or Alpha.
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
$!
$!..............................................................................
$!  SD has been successfully built and tested under OpenVMS VAX v6.2 and v7.1
$!  with Pascal v5.4, and OpenVMS Alpha v6.2 and v7.1 with Pascal v5.5.
$!
$!  With new releases of OpenVMS, it may be necessary to re-link SD.
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
$ perform = f$locate (p2,"N") .nes. f$length (p2)
$
$!  If the Pascal compiler is version 5.3 or later, add ",noperformance" to
$!  the usage_types below.  Presume Pascal version from architecture.  Allow
$!  ",noperformance" to be forced by p2.
$
$ if (arch .eqs. "VAX") .and. .not. perform
$ then
$   usage_types = "all"			! Assume Pascal <= v5.2 for VAX.
$ else
$   usage_types = "all,noperformance"	! Assume Pascal >= v5.3 for Alpha.
$ endif
$
$ if compile
$ then
$   say "Compiling SD source ..."
$   pascal sd /usage=('usage_types') /object=sd.obj_'arch'
$ endif
$
$ if linking
$ then
$   if (f$locate (p1,"T") .nes. p1len)  then  trace := /traceback
$   say "Linking SD ..."
$   if arch .eqs. "VAX"
$   then
$     link 'trace' sd.obj_vax,  sd.opt/option
$   else
$     link 'trace' sd.obj_alpha,sd.opt/option
$   endif
$ endif
$
$ if linking  then  say "*** SD built successfully ***"
$
$ EXIT 1
