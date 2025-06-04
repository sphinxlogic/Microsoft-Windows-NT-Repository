$	Old_Def	= F$Environment("DEFAULT")
$	New_Dev	= F$Parse(F$Environment("PROCEDURE"),,,"DEVICE")
$	New_Dir	= F$Parse(F$Environment("PROCEDURE"),,,"DIRECTORY")
$	Set Default 'New_Dev''New_Dir'
$	Total	= 0
$Loop:
$	File	= F$Search("[...]*.EXE")
$	If (File .EQS. "") Then Goto Done
$	Name	= F$Parse(File,,,"NAME")
$	Dev	= F$Parse(File,,,"DEVICE")
$	Dir	= F$Parse(File,,,"DIRECTORY")
$	'Name	:== $'Dev''Dir''Name'
$	Total	= Total + 1
$	Goto Loop
$
$Done:
$	Write Sys$Output "''Total' PBMPLUS symbols defined"
$	Set Default 'Old_Def'
$	Exit

