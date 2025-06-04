$!# snd2au
$!
$ if (p3 .eqs. "") .or. (p2 .eqs. "") 
$ then
$  write sys$output "Usage:  @snd2au RATE input_file output_file"
$  write sys$output "        where RATE is how the file was recorded."
$  write sys$output "        (usually 11025 or 22050)"
$  exit
$ endif
$ sox -t .ub  "-U" -r "''p1'" "''p2'" "-U" -r 8000 "''p3'" 
$ exit
