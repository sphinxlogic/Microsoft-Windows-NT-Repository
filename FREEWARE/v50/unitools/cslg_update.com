$ verify = 'f$verify (0)'
$!------------------------------------------------------------------------------
$!
$!  Replace a CSLG PAK.  Presumes the name and form of a CSLG licence
$!  distribution DCL procedure from Digital.
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	03-Feb-1997	JER	Original version.
$!	18-Feb-1998	JER	Allow never before used products to be added.
$!------------------------------------------------------------------------------
$
$ on warning    then  goto  EXIT_DOOR
$ on control_y  then  goto  EXIT_DOOR
$
$ say = "Write SYS$OUTPUT"
$ ask = "Read SYS$COMMAND /end=EXIT_DOOR /error=ERROR_DOOR /prompt="
$ pid = f$getjpi("","PID")
$
$ tmp := PAK_UPDATE_'pid'.TMP
$ lst := PAK_LIST_'pid'.TMP
$ all_paks  = 0
$ list_open = 0
$ new_pak   = 0
$
$ pak = f$edit (p1,"UNCOMMENT,COLLAPSE")
$ new_pak = f$edit (p2,"UNCOMMENT,COLLAPSE") .nes. ""
$
$ type SYS$INPUT

  CSLG PAK Update procedure syntax:

  $ @SYS_SYSTEM:CSLG_UPDATE  pak_name  [new_pak]

  where pak_name is the name of a single PAK or "?" to indicate to replace
        all currently installed PAKs *with confirmation* to do so per PAK;

    and new_pak is an optional parameter which can be any character - if
        there is a character present, the procedure presumes this product
        does not have a current PAK loaded.

$
$ASK_PAK:
$ if pak .eqs. ""
$ then
$   say ""
$   ask "PAK to replace? " pak
$   pak = f$edit (pak,"COMPRESS,UNCOMMENT,TRIM")
$   if pak .eqs. ""  then  goto  ASK_PAK
$   if f$element (1," ",pak) .nes. " "
$   then
$     new_pak = 1
$     pak = f$element (0," ",pak)
$   endif
$   say ""
$ endif
$
$ if pak .eqs. "?"
$ then
$   all_paks = 1
$   show license /output='lst'
$ endif
$
$GET_PAK:
$ if all_paks
$ then
$   if .not. list_open
$   then
$     open paks 'lst' /read
$     read paks next /error=EXIT_DOOR /end=EXIT_DOOR
$     read paks next /error=EXIT_DOOR /end=EXIT_DOOR
$     read paks next /error=EXIT_DOOR /end=EXIT_DOOR
$     read paks next /error=EXIT_DOOR /end=EXIT_DOOR
$     read paks next /error=EXIT_DOOR /end=EXIT_DOOR
$   endif
$   list_open = 1
$NEXT_PAK:
$   read paks next /error=EXIT_DOOR /end=EXIT_DOOR
$   pak = f$element (0," ",next)
$   if f$locate ("%",pak) .ne. f$length (pak)  then  goto  EXIT_DOOR
$   say ""
$   say next
$   say ""
$   say ""
$   ask "Replace ''pak'? (Yes/No) [Yes] " ans
$   ans = f$edit (ans,"UPCASE,COLLAPSE,UNCOMMENT")
$   if ans .eqs. ""  then ans = "Yes"
$   if .not. ans  then  goto  NEXT_PAK
$   say ""
$ endif
$
$ if .not. new_pak
$ then
$   license list /full 'pak' /output='tmp'
$   type 'tmp'
$   show license 'pak'
$   say ""
$   say ""
$   ask "Is this the correct PAK to replace? (Yes/No) [Yes] " ans
$   ans = f$edit (ans,"UPCASE,COLLAPSE,UNCOMMENT")
$   if ans .eqs. ""  then ans = "Yes"
$   if .not. ans
$   then
$     pak = ""
$     if all_paks
$     then
$       goto  GET_PAK
$     else
$       new_pak = 0
$       goto  ASK_PAK
$     endif
$   endif
$ endif
$
$!  Filename assumed here - change as necessary ...
$
$ search SYS_SYSTEM:CSLG_PAKS_1998.VMS "license register ''pak' " -
	/window=(7,9) /output='tmp'
$ if $status .eq. %X08D78053		! %SEARCH-I-NOMATCHES
$ then
$   say ""
$   say "PAK not found in CSLG!"
$   pak = ""
$   if all_paks
$   then
$     goto  GET_PAK
$   else
$     new_pak = 0
$     goto  ASK_PAK
$   endif
$ endif
$
$ pak = f$edit (pak,"UPCASE")
$
$ say ""
$ say "Commands to be executed:"
$ say ""
$ if .not. new_pak
$ then
$   say "$ SET NOON"
$   say "$ LICENSE DELETE ''pak'"
$   say "$ SET ON"
$ endif
$ type 'tmp'
$ if .not. new_pak
$ then
$   say "$ SET NOON"
$   say "$ LICENSE UNLOAD ''pak'"
$   say "$ SET ON"
$ endif
$ say "$ LICENSE LOAD   ''pak'"
$ say ""
$
$ ask "Execute these commands? (Yes/No) [Yes] " ans
$ ans = f$edit (ans,"UPCASE,COLLAPSE,UNCOMMENT")
$ if ans .eqs. ""  then ans = "Yes"
$ if .not. ans
$ then
$   pak = ""
$   new_pak = 0
$   goto  ASK_PAK
$ endif
$
$ say ""
$ if .not. new_pak
$ then
$   set noon
$   license delete 'pak'
$   set on
$ endif
$ @'tmp'
$ set noon
$ if .not. new_pak  then  LICENSE UNLOAD 'pak'
$ set on
$ LICENSE LOAD   'pak'
$ show license   'pak'
$
$ if all_paks
$ then
$   say ""
$   say "*********************"
$   goto  GET_PAK
$ endif
$!..............................................................................
$EXIT_DOOR:
$
$ if f$search (tmp) .nes. ""  then  delete /nolog 'tmp';*
$ if f$search (lst) .nes. ""  then  delete /nolog 'lst';*
$ close /nolog paks
$
$ EXIT 1 + 0 * f$verify (verify)
