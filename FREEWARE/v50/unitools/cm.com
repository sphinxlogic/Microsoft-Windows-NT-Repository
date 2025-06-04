$ verflg = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!
$!  Examine and (optionally) update the NEWMAIL counter for a VMS MAIL user.
$!
$!  Usage:   @CM [[p1] [p2]]
$!	        where  p1 = optional username,
$!		       p2 = optional new NEWMAIL count for given user.
$!
$! If no username is specified, the current user's username is used.
$! Without a new value for NEWMAIL, the current NEWMAIL count is displayed.
$! With a new value, the current NEWMAIL count may _optionally_ be changed.
$! (To update the current user's NEWMAIL count, specify the username explicitly)
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne.
$!           Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	03-Jan-1992	JER	Original version.
$!------------------------------------------------------------------------------
$
$ set control=(Y,T)
$ on control_y  then  goto  END
$ on error      then  goto  END
$
$ say := write SYS$OUTPUT
$ ask := read SYS$COMMAND /error=END /end=END /prompt=
$
$ if .not. f$privilege("SYSPRV")  then  exit (10548)	! SS$_NOSECURITY.
$ if p1 .eqs. ""  then  p1 = f$getjpi ("","USERNAME")
$ p1 = f$edit (p1,"COLLAPSE")
$ lock = "/nolock"
$ if p2 .nes. ""
$ then
$   p2 = f$edit (p2,"COLLAPSE")
$   lock = "/lock"
$ endif
$
$ close /nolog Mailfile
$ open /share=write /read  /write  Mailfile  VMSMAIL_PROFILE	! Use logical.
$ read /index=0 /key="''p1'" /error=NO_REC 'lock'  Mailfile  User_Rec
$ new_cnt = f$cvui (35*8,16,User_Rec)		! Determine NEWMAIL count.
$ if new_cnt .eq. 0
$ then
$   say "''p1' has no new MAIL messages."
$ else
$   say f$fao ("!AS has !SL new MAIL message!%S.",p1,new_cnt)
$ endif
$
$ if p2 .nes. ""
$ then
$   if f$type (p2) .eqs. "INTEGER"
$   then
$     p2i = f$integer (p2)
$     if (p2i .lt. 0) .or. (p2i .gt. 32000)
$     then
$       say "Invalid integer replacement NEW MAIL count."
$     else
$       ask "Change NEW MAIL count for ''p1' from ''new_cnt' to ''p2'? [No]: " -
	  ans
$	ans = f$edit (ans,"COLLAPSE")
$	if ans .eqs. ""  then  ans = "NO"
$       if ans
$       then
$         User_Rec[35*8,16] = p2i		! Update field.
$         write /update  Mailfile  User_Rec	! Replace record.
$	  say "NEW MAIL count for ''p1' updated to ''p2'."
$	else
$	  say "MAIL record for ''p1' NOT updated."
$       endif
$     endif
$   else
$     say "Invalid replacement NEW MAIL count - must be integer."
$   endif
$ endif
$
$ goto  END
$
$NO_REC:
$ say "No MAIL record for ''p1'."
$
$END:
$ close /nolog Mailfile
$ EXIT 1 + 0 * f$verify (verflg)
