$ SetUp MMK
$ Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ Set Default 'Path'
$
$ @ [.Command]SetUp Compile Distrib Static
$ XMake All
$
$ Exit
