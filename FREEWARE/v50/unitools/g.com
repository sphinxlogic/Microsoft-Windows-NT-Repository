$ verify = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!  G . C O M
$!  =========
$!
$!  This DCL command procedure textually searches output from DCL commands,
$!  specifically SHOW commands, but also other commands that produce output.
$!  The DCL command and the consequent SEARCH command may both optionally
$!  have qualifiers.
$!
$!  Usage:  $ @g  P1  P2  [P3]  [P4]  [P5]
$!   where  P1 = a valid DCL SHOW command option (e.g. LOGICAL or SYMBOL);
$!		 P1 may be prepended with a "#" to indicate that the command
$!		   is standalone (i.e. does not require "SHOW" to be used)  OR
$!		 P1 may be prepended with a "^" to indicate that the command
$!		   does not support the /OUTPUT qualifier  OR
$!		 P1 may be prepended with a "=" to indicate the same as
$!		   prepending the command with both "#" and "^".
$!		 
$!	    P2 = translation text(s) to be found (comma-separated list);
$!          P3 = qualifiers and/or parameters for SHOW (or other ("#")) command;
$!          P4 = qualifiers for SEARCH command;
$!          P5 = optional - if present, do not delete output from command.
$!
$!  Various commands are handled specifically in this procedure according to
$!  their syntactic ability to use the /OUTPUT qualifier, and if they are not
$!  SHOW commands.  For some specific non-SHOW commands, the "#" is optional.
$!
$!  Example usage:
$!     $ glnm :== @disk:[directory]G LOGICAL
$!     $ gsym :== @disk:[directory]G SYMBOL
$!
$!     $ glnm my_fancy_logical_translation
$!       "FRED" = "MY_FANCY_LOGICAL_TRANSLATION"
$!       "JACK" = "INCLUDED_HERE_IS_MY_FANCY_LOGICAL_TRANSLATION_TOO"
$!     $ glnm TRANS1,TRANS2,".JAZZY]" /table=*
$!	 "JAZZ" = "D_J:[BOPP.JAZZY]"
$!       "JILL" = "SEE_TRANS1_ONLY"
$!       "JOAN" = "SEE_TRANS2_ONLY_HERE"
$!     $ glnm concealed /full
$!       "DISK$SYS_0" [exec] = "$1$DUS0:" [concealed,terminal]
$!       "DISK$SYS_1" [exec] = "$1$DUS10:" [concealed,terminal]
$!
$!     $ gsym sd
$!	 POTSDAM == "@SYS$LOGIN:ZOT"
$!	 SD == "$SYS_SYSTEM:SD"
$!	 ZOT == "@SYS$LOGIN:POTSDAM"
$!
$!     $ @g "#ncp show known nodes" mynode
$!       42.12 (MYNODE)   reachable     4     0    MNA-0      42.12 (MYNODE)
$!
$!     $ @g "sysgen show /all" shadow
$!       SHADOWING             3          0         0         3 Coded-valu
$!       SHADOW_SYS_DISK       0          0         0         1 Boolean
$!       SHADOW_SYS_UNIT       0          0         0      9999 Unit
$!       SHADOW_MAX_COPY       3          4         0        42 Threads    D
$!       SHADOW_MBR_TMO       20         20         0     65535 Seconds    D
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	03-Jan-1993	JER	Original version.
$!------------------------------------------------------------------------------
$
$ on warning    then  goto  ESCAPE
$ on control_y  then  goto  ESCAPE
$
$ say := write SYS$OUTPUT
$ ask := read SYS$COMMAND /error=ESCAPE /end=ESCAPE /prompt=
$ Pid  = f$getjpi ("","PID")
$ Fyl  = "G_''PID'.TMP"
$ output_ok  = 1
$ use_show   = 1
$ delete_fyl = 1
$
$GET_P1:
$ if p1 .eqs. ""  then  ask "Show (or do) what? " p1
$ if p1 .eqs. ""  then  goto  GET_P1
$ p1 = f$edit (p1,"UPCASE,COMPRESS")
$
$GET_P2:
$ if p2 .eqs. ""  then  ask "String(s) to find? " p2
$ if p2 .eqs. ""  then  goto  GET_P2
$
$ if p5 .nes. ""  then  delete_fyl = 0
$
$ a1 = f$extract (0,1,p1)
$ a2 = f$extract (0,2,p1)
$ a3 = f$extract (0,3,p1)
$ a4 = f$extract (0,4,p1)
$ if a1 .eqs. "#"  then  p1 = p1 - "#"
$ if a1 .eqs. "="  then  p1 = p1 - "="
$ if (a1 .eqs. "#") .or. (a1 .eqs. "=")  then  use_show  = 0
$ if  a1 .eqs. "="  then  output_ok = 0
$
$ if a2 .eqs. "SH"  then  use_show = 0		! Fudge it for SH*OW.
$
$ if p1 .eqs. "ACL"  then  output_ok = 0
$ if p1 .eqs. "KEY"
$ then
$   output_ok = 0
$   p1 = "KEY /ALL "
$ endif
$ if p1 .eqs. "LOGICAL"  then  p1 = "LOGICAL * "
$ if p1 .eqs. "SYMBOL"
$ then
$   output_ok = 0
$   p1 = "SYMBOL /ALL /GLOBAL "
$ endif
$
$ if a2 .eqs. "AN"  then  use_show = 0		! Some form of ANALYZE.
$
$ if a3 .eqs. "NCP"				! NCP.
$ then
$   output_ok = 0
$   use_show  = 0
$ endif
$
$ if (a4 .eqs. "SYSG") .or. -	! SYSGEN or
     (a4 .eqs. "AUTH") .or. -	! AUTHORIZE or
     (a4 .eqs. "MCR ")		! MCR something - make assumptions about
$ then				!  the MCR target program.
$   output_ok = 0
$   use_show  = 0
$ endif
$
$!  Do the DCL command.
$
$ if output_ok
$ then
$   if use_show
$   then
$     show 'p1' /output='Fyl' 'p3'
$   else
$          'p1' /output='Fyl' 'p3'
$   endif
$ else
$   define SYS$OUTPUT 'Fyl'
$   if use_show
$   then
$     show 'p1' 'p3'
$   else
$     'p1' 'p3'
$   endif
$   deassign SYS$OUTPUT
$ endif
$
$ search 'Fyl' 'p2' 'p4'
$
$ESCAPE:
$ set noon
$ if delete_fyl
$ then
$   if f$search (Fyl) .nes. ""  then  delete /nolog 'Fyl';*
$ else
$   say "%Output from command remains in file ''f$parse(Fyl)'"
$ endif
$ if f$trnlnm ("SYS$OUTPUT") .eqs. Fyl  then  deassign SYS$OUTPUT
$ set on
$
$ EXIT 1 + 0 * f$verify (verify)
