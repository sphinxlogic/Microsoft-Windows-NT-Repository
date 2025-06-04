$!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
$!
$! INSTALL_HELP.COM
$!
$! For addition of available online help.  Code adapted from setup.com for xv.
$! by Rick Dyson (dyson@IowaSP.Physics.UIowa.EDU) 19-AUG-1994
$! 10-95 Sheldon Bishov -- p1 = input .hlb file to make procedure more generic.
$!
$!  Put the help library into the next available help library slot
$!  p1 = full path for help library file
$!
$!------------------------------------------------------------------------------
$!
$ CurrLibLogical = "Hlp$Library"
$ CurrLibValue = F$Trnlnm (CurrLibLogical, "Lnm$Process")
$ If CurrLibValue .eqs. "" Then GoTo InsertLib
$ If CurrLibValue .eqs. p1 -
     Then GoTo Done
$ baselib = CurrLibLogical + "_"
$ LibCount = 1
$!
$NextLib:
$   CurrLibLogical := 'baselib''LibCount'
$   CurrLibValue = F$Trnlnm (CurrLibLogical, "Lnm$Process")
$   If CurrLibValue .eqs. "" Then GoTo InsertLib
$   If CurrLibValue .eqs. p1 -
      Then GoTo Done
$   LibCount = LibCount + 1
$   GoTo NextLib
$!
$InsertLib:
$   assign 'p1' 'CurrLibLogical'
$!
$Done: 
$   exit
