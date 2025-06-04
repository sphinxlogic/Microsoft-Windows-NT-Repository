$!# wav2au
$!sox -t wav $1 -U -r 8000 `basename $1 .wav`.au
$!
$ if (p1 .eqs. "") .or. (p2 .eqs. "") 
$ then
$  write sys$output "Usage:  @wav2au input_file output_file"
$  exit
$ endif
$ sox -t wav "''p1" "-U" -r 8000 "''p2'"
$ exit
