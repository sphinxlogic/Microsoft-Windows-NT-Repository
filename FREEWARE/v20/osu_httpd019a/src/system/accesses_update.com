$!
$! Manually update entry in accesses.dat file
$! Usage:
$!    @update key [new-count|DELETE]
$!
$! Note that P1 (key) must be in quotes if it starts with a "/".
$!
$ if p1 .eqs. ""
$ then
$   write sys$Output "Missing P1: name of entry"
$   exit
$ else
$  p1 = f$edit(p1,"upcase")
$ endif
$ if p2 .eqs. "" then p2 = "0"
$ if p2 .eqs. "DELETE" then p2 = "-999"
$ new_count = f$integer(p2)
$ open/read/write/share=write acc www_root:[000000]accesses.dat
$ on error then goto cleanup
$ on control_y then goto cleanup
$!
$! Read record and rewrite.  Format of record:
$!    bytes 1-100		Index key, path of file being counted
$!    bytes 101-110		Number of accesses (portion rewritten)
$!    bytes 111-120		Counter incarnation number (version).
$!
$ key = f$fao("!100<!AS!>",p1)		! pad p1 to 100 chars.
$ show symbol new_count
$ if new_count .ne. -999
$ then
$    read/key="''key'" acc rec
$    write sys$output "Old value: ", f$extract(100,10,rec)
$    write/update acc key,f$fao("!10ZL",new_count),f$extract(110,10,rec)
$ else
$    read/delete/key="''key'" acc rec
$    write sys$output "Deleted record, old value: ", f$extract(100,10,rec)
$ endif
$!
$ cleanup:
$ close acc
