$ Write Sys$Output "Setting up the Freeware Demo environment for GhostView (GV)"
$ On Warning Then GoTo No_GS
$ Show Symbol GS
$ GoTo Have_GS
$
$No_GS:
$
$   Write Sys$Output ""
$   Write Sys$Output "      WARNING: GhostView (GV) needs GhostScript (GS) to work!"
$   Write Sys$Output "      You will need to find it from the Freeware disc"
$   Write Sys$Output "      and set it up before trying to use this GV demo."
$   Write Sys$Output ""
$   Exit
$
$Have_GS:
$
$   VMS_Version := 'F$GetSYI (""Version"")'
$   Length = F$Length (VMS_Version)
$   Bin_Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + ".GhostView-3_5_8]"
$   Root_Path = F$Element (0, "]", F$Environment ("PROCEDURE")) + ".GhostView-3_5_8.]"
$   Define X11_Root 'Root_Path'
$
$   If ("''F$GetSYI (""Arch_Name"")'" .eqs. "Alpha")
$       Then
$           If (F$Locate ("V6.2", VMS_Version) .le. Length) Then Version = "ALP62"
$           If (F$Locate ("V7.", VMS_Version) .le. Length) Then Version = "ALP71"
$       Else
$           If (F$Locate ("V6.2", VMS_Version) .le. Length) Then Version = "VAX62"
$   EndIf
$
$   GV :== $ 'Bin_Path'GV.'Version'_Exe
$   GV tiger.ps
$
$   Exit
