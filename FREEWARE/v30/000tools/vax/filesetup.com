$set command sys$system:file
$file:==
$echo:==$echo
$echo "Just defined FILE command"
$!echo "Attempting to execute FILE command..retry if nothing happens"
$!ass/user sys$command sys$input
$!file 'p1' 'p2' 'p3' 'p4'
