$ write sys$output "Setting host to ''p1'"
$ set h 'p1'
#This is a key
$ write sys$output "Back to procedure"
