$ This_Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ Write Sys$Output "Setting up the Freeware Demo environment for PCal"
$ If F$GetSYI("Arch_Name") .eqs. "Alpha"
$   Then
$       pcal := $ 'This_Path'pcal.exe_alp
$   Else
$       pcal := $ 'This_Path'pcal.exe_vax
$ EndIf
$
$ pcal -o sys$login:pcal_calendar.ps
$
$ Type /NoPage Sys$Input

    A sample, default calendar in Postscript has been created in your
    login directory called PCAL_CALENDAR.PS

    At this time you can either view it with a PS viewer or print it on a
    PS printer.

$ Exit
