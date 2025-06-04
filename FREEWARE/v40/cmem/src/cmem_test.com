$!++
$!  FACILITY:  CMEM  V1.1
$!
$!
$!  MODULE DESCRIPTION:
$!
$!	This is the command procedure which builds the CMEM test procedure.
$!	It will compile all the source code, link it together, and run the
$!	program.  If you desire, you can also use this program to compare
$!	a new run with an old run.
$!
$!
$!  FORMAL PARAMETERS:
$!
$!      P1:
$!	    Type:	String
$!	    Access:	Input
$!
$!	    If the user specifies P1 as "UPDATE", then we'll just generate a
$!	    new comparison file.
$!
$!	    If P1 is "COMPARE", then we'll compare the current run with a
$!	    previous "update" run.
$!
$!	    If P1 is a null string, then we just run the program.
$!
$!
$!  AUTHORS:
$!
$!      Brett Hunsaker (hunsaker@eisner.decus.org)
$!
$!
$!  CREATION DATE:  4 May 1995
$!
$!
$!  SIDE EFFECTS:
$!
$!      We will not delete the object files and executable.
$!
$!
$!  DESIGN ISSUES:
$!
$!	We'll assume that all our files are in the same directory as this
$!	command procedure.
$!
$!
$!  MODIFICATION HISTORY:
$!
$!      4-May-1995	B. Hunsaker	Initial implementation
$!
$!	Provide an example of how to use the routines.
$!
$!
$!--
$!
$!
$!      Get the directory of where this procedure lives so we can find the
$!	source code.  Also, get the name of the procedure for error messages.
$!
$       Procedure_Name = F$Environment( "Procedure" )
$	Source_Directory = -
		F$Parse( Procedure_Name, , , "Device", "Syntax_Only" ) + -
		F$Parse( Procedure_Name, , , "Directory", "Syntax_Only" )
$       Procedure_Name = F$Parse( Procedure_Name, , , "Name", "Syntax_Only" )
$!
$!
$!	Check the P1 parameter to make sure it is valid.
$!
$	If ( P1 .Nes. "" ) -
		.And. ( P1 .Nes. "UPDATE" ) -
		.And. ( P1 .Nes. "COMPARE" )
$	Then
$	  Write Sys$Error "%''Procedure_Name'-I-P1MUSTBE, ", -
	  	"P1 must be specified as null, ""UPDATE"", or ""COMPARE"""
$	  Exit
$	EndIf
$!
$!
$!	Try to figure out which C compiler we'll be using.
$!
$	Compiler_Type = "DEC C"
$	If F$GetSyi( "Arch_Type" ) .Eq. 1	! Is this a VAX?
$	Then
$	  If ( F$TrnLnm( "DECC$CC_DEFAULT" ) .Eqs. "/VAXC" ) -
		.Or. ( F$TrnLnm( "DECC$CC_DEFAULT" ) .Eqs. "" )
$	  Then
$	    Compiler_Type = "VAX C"
$	  EndIf
$	EndIf
$!
$!
$!	Compile the source modules
$!
$	FileName = "''Source_Directory'CMEM_TEST"
$	Call Compare_File_Dates 'FileName'.C 'FileName'.OBJ
$	If CMem___Update_File
$	Then
$	  Write Sys$Error "%''Procedure_Name'-I-COMPILE, ", -
	    "compiling ''Compiler_Type' source ''FileName'.C"
$	  CC/Debug/NoOptimize/Object='FileName' 'FileName'
$	EndIf
$!
$	FileName = "''Source_Directory'CMEM"
$	Call Compare_File_Dates 'FileName'.C 'FileName'.OBJ
$	If CMem___Update_File
$	Then
$	  Write Sys$Error "%''Procedure_Name'-I-COMPILE, ", -
	    "compiling ''Compiler_Type' source ''FileName'.C"
$	  CC/Debug/NoOptimize/Object='FileName' 'FileName'
$	EndIf
$!
$	FileName = "''Source_Directory'CMEM_TRACEBACK"
$	Call Compare_File_Dates 'FileName'.C 'FileName'.OBJ
$	If CMem___Update_File
$	Then
$	  Write Sys$Error "%''Procedure_Name'-I-COMPILE, ", -
	    "compiling ''Compiler_Type' source ''FileName'.C"
$	  CC/Debug/NoOptimize/Object='FileName' 'FileName'
$	EndIf
$!
$	FileName = "''Source_Directory'CMEM_MESSAGE"
$	Call Compare_File_Dates 'FileName'.MSG 'FileName'.OBJ
$	If CMem___Update_File
$	Then
$	  Write Sys$Error "%''Procedure_Name'-I-COMPILE, ", -
	    "compiling message source ''FileName'.MSG"
$	  Message/Object='FileName' 'FileName'
$	EndIf
$!
$!
$!	Now link the image as appropriate for the various systems.  We don't
$!	use the CMEM.OPT file as it looks in the default directory for the
$!	object files.  (Yes, we could have changed directories...)
$!
$!	Also, we'll display the "multiply defined symbol messages" if linking
$!	with DEC C.  We could get rid of these, but it is probably best to
$!	let the user know that these are to be expected.
$!
$	Modified_Count = 0
$	Executable_File = "''Source_Directory'CMEM_TEST.EXE"
$!
$	FileName = "''Source_Directory'CMEM_TEST.OBJ"
$	Call Compare_File_Dates 'FileName' 'Executable_File'
$	If CMem___Update_File Then Goto Link_It
$!
$	FileName = "''Source_Directory'CMEM.OBJ"
$	Call Compare_File_Dates 'FileName' 'Executable_File'
$	If CMem___Update_File Then Goto Link_It
$!
$	FileName = "''Source_Directory'CMEM_TRACEBACK.OBJ"
$	Call Compare_File_Dates 'FileName' 'Executable_File'
$	If CMem___Update_File Then Goto Link_It
$!
$	FileName = "''Source_Directory'CMEM_MESSAGE.OBJ"
$	Call Compare_File_Dates 'FileName' 'Executable_File'
$	If CMem___Update_File Then Goto Link_It
$!
$	Goto Run_It
$!
$!
$ Link_It:
$!
$	Write Sys$Error "%''Procedure_Name'-I-LINK, ", -
	    "linking ''Compiler_Type' object files in ''Source_Directory'"
$!
$	If Compiler_Type .Eqs. "DEC C"
$	Then
$	  Write Sys$Error "-''Procedure_Name'-I-EXPMULDEF, ", -
		"expect 4 multiply defined symbol messages"
$	  Link -
		/NoDebug -
		/Traceback -
		/Executable='Executable_File' -
		'Source_Directory'CMEM_TEST.OBJ, -
		'Source_Directory'CMEM.OBJ, -
		'Source_Directory'CMEM_TRACEBACK.OBJ, -
		'Source_Directory'CMEM_MESSAGE.OBJ
$	Else
$	  Link -
		/NoDebug -
		/Traceback -
		/Executable='Executable_File' -
		'Source_Directory'CMEM_TEST.OBJ, -
		'Source_Directory'CMEM.OBJ, -
		'Source_Directory'CMEM_TRACEBACK.OBJ, -
		'Source_Directory'CMEM_MESSAGE.OBJ, -
		SYS$LIBRARY:VAXCRTL.OLB/Library
$	EndIf
$!
$!
$!	Run the program and let's see what we get.  We make sure the format
$!	of VMS messages are the same between runs.
$!
$ Run_It:
$!
$	Write Sys$Error "%''Procedure_Name'-I-RUN, ", -
	    "running ''Executable_File'"
$!
$	Set NoOn
$	Old_Message = F$Environment( "Message" )
$	Set Message/Facility/Severity/Identification/Text
$!
$	If ( P1 .Eqs. "UPDATE" ) .Or. ( P1 .Eqs. "COMPARE" )
$	Then
$	  Source_Out_File = "''Source_Directory'CMEM_TEST_OUT.DAT"
$	  Source_Err_File = "''Source_Directory'CMEM_TEST_ERR.DAT"
$	  Temp_Out_File = "''Source_Directory'CMEM_TEST_OUT.TMP"
$	  Temp_Err_File = "''Source_Directory'CMEM_TEST_ERR.TMP"
$!
$	  Define/User Sys$Output "''Temp_Out_File'"
$	  Define/User Sys$Error "''Temp_Err_File'"
$	EndIf
$!
$	Run 'Executable_File'
$	Set Message 'Old_Message'
$!
$!
$!	If the user want us to update the source file, we'll do that.  Other
$!	wise compare the output we just got with the source file.
$!
$	If P1 .Eqs. "UPDATE"
$	Then
$	  Rename/NoLog 'Temp_Out_File' 'Source_Out_File'
$	  Rename/NoLog 'Temp_Err_File' 'Source_Err_File'
$	  Write Sys$Error "%''Procedure_Name'-I-UPDATE, ", -
		"updated CMEM_TEST source data files"
$	EndIf
$!
$	If P1 .Eqs. "COMPARE"
$	Then
$	  Dif$_FilAreDif = %X006C8013
$	  Differences/Output=NL: 'Temp_Out_File' 'Source_Out_File'
$	  Status1 = $Status
$	  Differences/Output=NL: 'Temp_Err_File' 'Source_Err_File'
$	  Status2 = $Status
$	  If ( Status1 .Eq. Dif$_FilAreDif ) .Or. -
			( Status2 .Eq. Dif$_FilAreDif )
$	  Then
$	    Write Sys$Error "%''Procedure_Name'-W-OUTNOMATCH, ", -
		"output from CMEM_TEST does NOT match data files"
$	  Else
$	    Write Sys$Error "%''Procedure_Name'-S-OUTMATCH, ", -
		"output from CMEM_TEST matches data files"
$	  EndIf
$	EndIf
$!
$!
$!	Clean up any temporary files.
$!
$	If F$Type( Temp_Out_File ) .Nes. ""
$	Then
$	  If F$Search( Temp_Out_File ) .Nes. "" Then -
	    Delete/NoLog 'Temp_Out_File';
$	  If F$Search( Temp_Err_File ) .Nes. "" Then -
	    Delete/NoLog 'Temp_Err_File';
$	EndIf
$!
$	Exit
$!
$!
$!	--------------------------------------------------------------------
$!
$ Compare_File_Dates:
$	Subroutine
$!
$	If F$Search( P2 ) .Eqs. ""
$	Then
$	  CMem___Update_File == "True"
$	Else
$	  Source_Date = F$File_Attributes( P1, "RDt" )
$	  Target_Date = F$File_Attributes( P2, "RDt" )
$!
$!
$!	If the source has been modified since the target was updated, tell
$!	the user to update the target.
$!
$	  If F$CvTime( Source_Date ) .Gts. F$CvTime( Target_Date )
$	  Then
$	    CMem___Update_File == "True"
$	  Else
$	    CMem___Update_File == "False"
$	  EndIf
$	EndIf
$!
$	Exit
$	  
$	EndSubroutine
