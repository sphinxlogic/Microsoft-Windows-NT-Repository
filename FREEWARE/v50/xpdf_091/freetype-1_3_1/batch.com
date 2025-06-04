$ Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ Set Default 'Path'
$
$ MMS /Description = [.lib.arch.vms]descrip.mms /Ignore = Warning
$
$ Exit
