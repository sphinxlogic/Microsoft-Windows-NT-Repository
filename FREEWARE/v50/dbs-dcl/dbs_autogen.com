$ ! Procedure:	DBS_AUTOGEN.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ saved_dir = f$environment("default")
$ set noon
$ on control_y then goto bail_out
$ ask = "@dbslibrary:sys_ask"
$ edit_first = f$edit(P1, "COLLAPSE,UPCASE")
$ editor = f$edit(P2, "COLLAPSE,UPCASE")
$ if (editor .eqs. "") then editor = "TECO"
$ editor_s = f$length(editor)
$ if (editor .eqs. f$extract(0,editor_s,"EDT"))
$   then editor = "EDT"
$ else
$ if (editor .eqs. f$extract(0,editor_s,"TECO"))
$   then editor = "TECO"
$ else
$ if (editor .eqs. f$extract(0,editor_s,"TPU"))
$   then editor = "TPU"
$ else
$ editor = ""
$ endif
$ endif
$ endif
$ interactive = (f$mode() .eqs. "INTERACTIVE")
$ if (interactive)
$   then
$ask_editor:
$   if (editor .eqs. "")
$     then 
$     ask answer "Which editor do you prefer to use (TECO,EDT or TPU)" -
			"TECO" DS^Z
$     if sys__backwards then goto bail_out
$     if sys__control_z then goto bail_out
$     if sys__control_y then goto bail_out
$     editor_s = f$length(answer)
$     if (answer .eqs. f$extract(0,editor_s,"EDT"))
$       then editor = "EDT"
$     else
$     if (answer .eqs. f$extract(0,editor_s,"TECO"))
$       then editor = "TECO"
$     else
$     if (answer .eqs. f$extract(0,editor_s,"TPU"))
$       then editor = "TPU"
$     else
$     editor = ""
$     endif
$     endif
$     endif
$   endif
$   if (editor .eqs. "") then goto ask_editor
$ endif !(interactive)
$ say = "write sys$output"
$ set default sys$system
$ if (interactive)
$   then
$   if (edit_first)
$     then define/user/nolog sys$input sys$command
$     editt/'editor' modparams.dat
$   endif
$ endif
$do_autogen:
$ @sys$update:autogen savparams genparams
$ if (interactive)
$   then define/user/nolog sys$input sys$command
$   list agen$params.report
$   ask answer "Edit MODPARAMS" "No" BD
$   if (sys__control_y) then goto bail_out
$   if (answer)
$	then define/user/nolog sys$input sys$command
$	editt/'editor' modparams.dat
$	ask go_again "AUTOGEN again" "Yes" BD
$	if (sys__control_y) then goto bail_out
$	if (go_again) then goto do_autogen ! yuk...
$   endif !(answer)
$   ask answer "Set parameters" "Yes" BD
$   if (sys__control_y) then goto bail_out
$ else
$ answer == "Y"	! default to set the parameters for a non-interactive run
$ endif !(interactive)
$ if (answer)
$	then
$	set process/privilege=bypass
$	@sys$update:autogen setparams setparams
$	set process/privilege=nobypass
$	say " "
$	purge sys$specific:[sysexe]
$	purge sys$specific:[sysmgr]
$ endif !(answer)
$bail_out:
$ say " "
$ set default 'saved_dir'
$ !'f$verify(__vfy_saved)'
$ exitt
$ !+==========================================================================
$ !
$ ! Procedure:	DBS_AUTOGEN.COM
$ !
$ ! Purpose:	This procedure will start by editing MODPARAMS.DAT the
$ !		invoking AUTOGEN to do the deed.
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		20-Sep-1990, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		20-Apr-1991, DBS; Version V1-002
$ !	002 -	New autogen report name.
$ !		06-Jan-1999, DBS; Version V1-003
$ !	003 -	Modified to allow P2 to select the editor.
$ !		21-Jul-1999, DBS; Version V1-004
$ !	004 -	Need to set BYPASS to do the setparams bit (V7.2?).
$ !-==========================================================================
