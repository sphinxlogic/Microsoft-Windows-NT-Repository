$ verify = 'f$verify(0)'
$!------------------------------------------------------------------------------
$! Continuously SHOW something (assume VT100-type display).
$! Exit with CONTROL_C, CONTROL_Y, CONTROL-Z, or if any errors.
$!
$! Press RETURN at any time to completely refresh the screen.
$!
$!  P1 = option for SHOW command (required), or if first character is "#" then
$!       the command is not a show command.  If the first character is "_" then
$!	 disable error handling for the command and check for "#".
$!  P2 = optional time between updates (in seconds); default is 5 seconds.
$!  P3 = optional display formatting character:
$!	   if absent (default), home cursor before each re-display;
$!	   if a "C" (for "Continue"), print a blank line before each re-display;
$!	   if any other character, always clear screen before each re-display.
$!------------------------------------------------------------------------------
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	14-Nov-1994	JER	Original version.
$!------------------------------------------------------------------------------
$
$ esc[0,8] = 27
$ home  = esc + "[H"
$ clear = home + esc + "[J"
$ use_show = 1
$ no_fail  = 0
$
$ say  := write SYS$OUTPUT
$ ask  := read SYS$COMMAND /error=DONE /end=DONE /prompt=
$ ask2 := read SYS$COMMAND /error=SKIP /end=DONE /prompt=""""""
$
$ on control_y  then  goto  DONE
$ASK_IT:
$ if p1 .eqs. ""  then  ask "Show what? " p1
$ if p1 .eqs. ""  then  goto  ASK_IT
$ p1 = f$edit (p1,"COMPRESS,TRIM,UPCASE")
$GET_IT:
$ a1 = f$extract (0,1,p1)
$ a2 = f$extract (0,2,p1)
$ if a1 .eqs. "_"
$ then
$   p1 = p1 - "_"
$   no_fail = 1
$   goto  GET_IT
$ endif
$ if a1 .eqs. "#"
$ then
$   p1 = p1 - "#"
$   use_show = 0
$ endif
$ if a2 .eqs. "SH"  then  use_show = 0		! Fudge it for SH*OW.
$
$ if p2 .le.  1  then  p2 =  5
$ if p2 .gt. 60  then  p2 = 60
$
$ zot = "home"
$ if p3 .nes. ""   then  zot = "clear"
$ if p3 .eqs. "C"  then  zot = """"""
$ say clear
$
$LOOP:
$ on warning  then  goto  DONE
$ if no_fail  then  set noon
$ if use_show
$ then
$   show 'p1'
$ else
$   'p1'
$ endif
$ if no_fail  then  set on
$ ask2 /timeout='p2' junk
$ say clear
$SKIP:
$ say 'zot'
$ goto  LOOP
$
$DONE:
$ EXIT 1 + 0 * f$verify (verify)
