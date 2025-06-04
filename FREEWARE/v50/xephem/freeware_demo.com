$ This_Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ XEphem_Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + ".SRC.XEphem.]"
$ Write Sys$Output "Setting up the Freeware Demo environment for XEphem"
$
$ If ("''F$GetSYI (""Arch_Name"")'" .nes. "Alpha")
$   Then
$       Write Sys$Output "Demo only supported for Alpha"
$       Exit
$ EndIf
$
$ Define /User_Mode XEphem_Dir 'XEphem_Path'
$ MCR 'This_Path'XEphem.exe_alp
$
$ Exit
