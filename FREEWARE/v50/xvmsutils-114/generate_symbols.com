$ SAVE_VERIFY = 'F$Verify(0)
$!
$!	Build the X11R4_Symbols.Com file from what *.EXEs exist in
$!	X11Bin:
$!
$ On Warning Then Goto Clean_Up
$ On Control_Y Then Goto Clean_Up
$!
$ open/write Tmp X11_Root:[000000]X11R4_Symbols.Com
$!
$! Output the header
$!
$ Write Tmp "$!"
$ Write Tmp "$!	Define the X11R4 Foreign Commands"
$ Write Tmp "$!"
$!
$! Start a search on X11Bin:*.Exe and build a foreign command for each image
$!
$ Exe_File = F$Search("X11Bin:*.Exe",1)
$!
$ Loop:		! come back to here, for the next *.Exe
$!
$	Exe_File_Name = F$Parse(Exe_File,,,"NAME")
$!
$	Write Tmp "$ ''Exe_File_Name'   :== $X11BIN:''Exe_File_Name'.EXE"
$!
$	Exe_File = F$Search("X11Bin:*.Exe",1)
$!
$	If Exe_File .Nes. "" Then Goto Loop
$!
$ Write Tmp "$!"
$ Write Tmp "$! All done, exit"
$ Write Tmp "$!"
$ Write Tmp "$ exit"
$!
$ Clean_Up:
$!
$ If F$TrnLnm("TMP") .Nes. "" Then Close Tmp
$!
$ exit
