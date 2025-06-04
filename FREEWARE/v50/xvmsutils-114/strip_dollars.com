$ set noon
$ loop:
$    file = f$search("*.*;*", 1)
$    if file .eqs. "" then exit
$    file = f$parse(file,,,"NAME") + f$parse(file,,,"TYPE")
$    OldFile = file
$    if f$loc("$",OldFile) .eq. f$len(OldFile) then goto loop
$    write sys$output "Converting File ''file'"
$ loop2:
$    if f$loc("$",file) .ne. f$len(file)
$    then
$       file = file - "$"
$    else
$       rename 'Oldfile' 'file'
$       goto loop
$    endif
$    goto loop2
