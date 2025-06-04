$! Install script for Physical Backup Reader
$! Assumes privs, will fail if it hasn't got enough.
$! Run in the directory with the software.
$set noon
$! A warning error is normal that there's no transfer
$! address here.
$write sys$output "Assembling DTdriver"
$ macro dtdriver
$write sys$output "Linking DTdriver. A warning there is no"
$write sys$output "transfer address is normal here."
$ link/notrace dtdriver+sys$system:sys.stb/sel+sys$input/opt
base=0
$set on
$link/notrace dthstimg+sys$system:sys.stb/sel
$! Now put things into sys$system.
$copy dtdriver.exe sys$common:[sysexe]
$copy dthstimg.exe sys$common:[sysexe]
$copy dthstimg.cld sys$common:[sysexe]
$copy dthost_start.com sys$common:[sysexe]
$! if we got all the way through, that's it.
$write sys$output "Done. To use type @sys$system:dthost_start."
$bugout:
