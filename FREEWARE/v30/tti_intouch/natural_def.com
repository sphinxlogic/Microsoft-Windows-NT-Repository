$! natural_def.com - command file to generate a report listing for a 
$!                   Natural data definition
$! parameters        p1 = Natural definition file to report
$!                   p2 = report filename to create
$!
$ define sys$output "nla0:"
$ nat stack=<logon sysddm;menu r,'p1':w,'p2':.;fin>
