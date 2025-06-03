$if p1.eqs."" then $inquire p1 "image file name"
$open/read/write file 'f$parse(p1,".exe")
$offset = 80 !Alpha. We'll assume exe matches current system.
$if f$getsyi("arch_name").eqs."VAX" then offset = 32
$read file header
$old_state = "OFF"
$new_state = "OFF"
$old_bit = f$cvsi(offset*8,1,header)
$new_bit = 0
$if old_bit then old_state = "ON"
$if p2.eqs."" then p2 = .not.old_bit
$if p2 then new_bit = 1
$if p2 then new_state = "ON"
$header[offset*8,1] = new_bit
$write/update/symbol file header
$close file
$write sys$output "Debug bit was ", old_state, ". Now ", new_state, "."
