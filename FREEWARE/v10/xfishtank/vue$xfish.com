$!  Copyright (c) 1988 Digital Equipment Corporation.  All rights reserved.
$!
$!  Command procedure to run the XFish application from
$!  the User Executive in DECwindows
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
$	goto do_XFish
$
$select_qualifiers:
$	vue$popup_qualifiers
$
$do_XFish:
$
$ vue$read vue$command
$
$ 'vue$command
$
$ xfish :== $decw$examples:xfish
$ vue$get_next_selection
$ vue$read selection
$ set proc/prior=0
$ xfish -r 0.444
$
$ vue$check_verb_loop
$ vue$read loop
$ if "''loop'" .eqs. "TRUE" then goto select_qualifiers
