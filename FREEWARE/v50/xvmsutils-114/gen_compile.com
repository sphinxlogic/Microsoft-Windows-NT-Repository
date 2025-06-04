$ open/write tmp compile.dat
$ single_apostrophe = "'"
$ compile_command = single_apostrophe + single_apostrophe + "cc_options" + single_apostrophe
$ loop:
$   file = f$search("*.c",1)
$   if file .eqs. "" then goto all_done
$   file = f$parse(file,,,"NAME")
$   if file .eqs. "" then exit
$   write tmp -
"$ CALL MAKE ''file'.OBJ  	""CC "+compile_command+" ''file'.C""	''file'.C"
$   goto loop
$ all_done:
$ close tmp
$ exit
