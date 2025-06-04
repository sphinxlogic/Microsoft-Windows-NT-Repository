$!# 8svx2au
$!sox -t 8svx $1 -U -r 8000 `basename $1 .8svx`.au
$!
$ if (p1 .eqs. "") .or. (p2 .eqs. "") 
$ then
$  write sys$output "Usage:  @8svx2au input_file output_file"
$  exit
$ endif
$ sox -t 8svx "''p1'" "-U" -r 8000 "''p2'"
$ exit
