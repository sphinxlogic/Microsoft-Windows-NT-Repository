$!# voc2au
$!sox -t voc $1 -U -r 8000 `basename $1 .voc`.au
$!
$ if (p1 .eqs. "") .or. (p2 .eqs. "") 
$ then
$  write sys$output "Usage:  @voc2au input_file output_file"
$  exit
$ endif
$ sox -t voc "''p1'" "-U" -r 8000 "''p2'"
$ exit
