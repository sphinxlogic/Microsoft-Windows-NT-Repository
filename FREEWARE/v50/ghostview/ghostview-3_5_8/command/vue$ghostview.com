$ vue$suppress_output_popup
$!
$! See if we should skip the dialog box
$!
$    vue$get_symbol vue$show_hidden_dialogs
$    vue$read show_hidden_dialogs
$
$    if show_hidden_dialogs then goto select_qualifiers
$	vue$get_qualifiers
$	goto do_ghostview
$
$select_qualifiers:
$	vue$popup_qualifiers
$
$do_ghostview:
$
$ vue$read vue$command
$
$ 'vue$command
$
$ vue$get_next_selection
$ vue$read selection
$ gv 'selection
$ exit
