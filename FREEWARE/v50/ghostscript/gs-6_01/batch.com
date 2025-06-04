$ SetUp MMK
$ Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ Set Default 'Path'
$
$ mmk /Ignore = Warning /Descrip = [.src]openvms.mmk /Macro = ("DECWINDOWS1_2=1")
$
$ Exit
