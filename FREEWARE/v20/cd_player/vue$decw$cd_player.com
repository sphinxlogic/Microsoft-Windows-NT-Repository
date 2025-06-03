$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$!  Command procedure to run DECW$CD_PLAYER_xxx from the User Executive of
$!  DECwindows
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ vue$suppress_output_popup
$!
$! See if we should skip the dialog box
$!
$    vue$get_symbol vue$show_hidden_dialogs
$    vue$read show_hidden_dialogs
$
$    if show_hidden_dialogs then goto select_qualifiers
$	vue$get_qualifiers
$	goto do_mail
$
$select_qualifiers:
$	vue$popup_qualifiers
$
$do_mail:
$
$ vue$read vue$command
$
$ if "''vue$command'" .eqs. "DETACHED_APPLY" then goto select_qualifiers
$
$ 'vue$command
$!
$!	run the correct version for your CPU..
$!
$ if f$getsyi("ARCH_TYPE") .eq. 1
$ then
$ 	run sys$system:decw$cd_player_vax.exe
$ else
$	run sys$system:decw$cd_player_axp.exe
$ endif
$!
$ vue$check_verb_loop
$ vue$read loop
$ if "''loop'" .eqs. "TRUE" then goto select_qualifiers
