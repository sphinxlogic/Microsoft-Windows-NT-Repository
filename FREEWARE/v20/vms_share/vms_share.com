$ v='f$verify(f$trnlnm("SHARE_VERIFY"))'
$! Purpose:
$!	Package a series of files into a multi-part archive, especially encoded
$!	so that it can be mailed across multiple networks without undue damage
$!
$! Parameters:
$!  	P1 = List of files to scan
$!	P2 = name of share file to generate (will be suffixed by part no.)
$!
$! Qualifiers:
$!      /BACKUP			Specify BACKUP as the selection date criteria
$!	/BEFORE[=date]		Specify latest date for file consideration
$!	/[NO]COMPRESS[=n]	Try to do compression on files
$!	/[NO]CONFIRM		Require confirmation of selected files
$!	/CREATED		Specify CREATED as the selection date criteria
$!	/[NO]DEBUG[=n]		Turn on debugging code [at level n]
$!	/[NO]DIRECTORY		Preserve directory structure in archives
$!	/[NO]EXCLUDE=list	Exclude list of files
$!	/EXPIRED		Specify EXPIRED as the selection date criteria
$!	/[NO]LOG[=n]		Specify logging
$!	/[NO]LONGLINES		Allow long lines if using later TPU's
$!	/MODIFIED		Specify MODIFIED as the selection date criteria
$!	/[NO]NAME[=ident]	Specify an identifier for the package
$!	/[NO]PACKAGE_INDEX[=type] Write part list for file server package files
$!	/PART_SIZE=NNN		Specify maximum size of parts
$!	/[NO]SHARE		Request creation of a share file
$!	/SINCE[=date]		Specify earliest date for file consideration
$!	/[NO]SPACE_ENCODE	Specify whether spaces should be quoted
$!	/[NO]SPLIT[=n]		Process big files in smaller chunks
$!	/TEMPORARY=filename	Specify alternate temporary filename
$!	/[NO]VERSION		Preserve version numbers in archive
$!	/WORK[=file]		Specify workfile for TPU code
$!
$! Environment:
$!	Needs at least VMS 5.0
$!
$! Revision History:
$!	8.0	Andy Harper	September 1992	Extensive rewrite
$!	8.1	Andy Harper	September 1992	Bug fixes following release
$!						&  a few new features
$!	8.2	Andy Harper	December  1992	New features and bugfixes
$!	8.3	Andy Harper	January   1993  Support for alpha, [000000] bugfix
$!	8.4	Andy Harper	June      1993  Various new things added
$!
$! This software is copyright (C) Andy Harper, Kings College London and is
$! supplied free of charge with no warranties expressed or implied. Any
$! problems arising from its use are entirely the responsibility of the user
$! You are free to distribute this software to anyone, provided that all
$! copyright notices remain in place.
$
$! Protection
$ set="set"
$ set noon
$ set symbol/scope=(nolocal,noglobal)
$
$ Facility              = f$parse(f$environment("PROCEDURE"),,,"NAME")
$ em="write sys$error ""%''Facility'"","
$ SS$_NORMAL            = %X00000001
$ SS$_ABORT             = %X1000002C
$ status                = SS$_ABORT
$
$ Min_VMS               = "5.0"	! To allow block-IF stuff
$ IsVAX			= f$getsyi("CPU") .le. 127
$ IsAlpha		= f$getsyi("CPU") .gt. 127
$ VMS                   = f$getsyi("VERSION")
$ VMSx                  = f$extract(1,f$length(VMS)-1,VMS)
$
$! Check system:  Alpha: Any VMS is OK; VAX: Need at least 'Min_VMS'
$ if (IsAlpha) .or. (VMSx .ges. Min_VMS) then $ goto SYS_ok
$ em "-E-OLD, ", Facility, " not supported on this system"
$ em "-I-REQ, requires at least VMS ", Min_VMS, " on VAX, or any Alpha"
$ goto abort
$
$!
$! From here, we can use block-IF stuff as we are on the right version
$SYS_ok:
$ Version               = "8.4"
$ Max_Share_Length      = 79
$ Def_Part_Size         = 30
$
$ Min_VMS_For_TPU       = "4.4"	! To recognize TPU
$ Min_VMS_For_WorkFile  = "5.2"	! To allow use of /WORK qualifier
$ Min_VMS_For_LongLines = "5.4"	! To allow long records in TPU
$ Max_Line_OldTPU       = 960	! Max line lengths for older TPUs
$ Max_Line_NewTPU       = 65535 ! Max line lengths for newer TPUs
$
$ Current_Directory     = f$parse(f$environment("DEFAULT"),"[]") - ".;"
$ Cur_Device            = f$parse(Current_Directory,,,"DEVICE")
$ Cur_Directory         = f$parse(Current_Directory,,,"DIRECTORY")
$
$! Define the format for the name of sharefile parts
$ format1 = "!AS!#*n-OF-!#ZL"
$ format2 = "!AS!#ZL-OF-!#ZL"
$
$! Set debugging levels/commands
$ IfDebugRejected   = "if Debugging .ge. 2 then $ em"
$ IfDebugShowParams = "if Debugging .ge. 2 then $ em"
$
$! Set logging levels
$ IfLogIdent        = "if Logging .ge. 1 then $ em "
$ IfLogSelected     = "if Logging .ge. 1 then $ em "
$ IfLogRenameParts  = "if Logging .ge. 1 then $ em "
$ IfLogPackageIndex = "if Logging .ge. 1 then $ em "
$
$! Setup the system defaults
$ gosub Get_Available_Facilities
$ gosub Get_Default_Parameters
$
$! Set up abort handling
$ on control_y then $ goto abort
$
$! Parse parameters
$ gosub Parse_Command_Line
$ gosub Break_Out_Parameters
$ gosub Break_Out_Options
$ gosub Check_Parameters
$
$ IfLogIdent "-I-IDENT, ", f$fao("!AS !AS - !3AS, !17%D - !AS",Facility,Version,f$cvtime(,,"WEEKDAY"),0,EmailAddr)
$
$! Write parameters to the parameter file, so the packing code can pick them up
$ gosub Create_Parameter_File
$ if FileCount .eq. 0
$   then
$     em "-E-NOFILES, No files selected."
$     goto abort
$ endif
$ IfLogSelected "-I-TOTALCNT, ", f$fao("!8%T, !SL File!%S selected",0,FileCount)
$
$ gosub PackFiles			! Go pack the files into the archive
$ gosub Rename_Parts			! Generate part type of ".nnn-of-mmm"
$ gosub Create_Package_Index		! Create the xxx.$PACKAGE index file
$ 
$! Clean up and exit
$ status = ss$_normal
$abort:
$ if f$trnlnm("SHARE_PARAMS")  .nes. "" then $ close SHARE_PARAMS
$ if f$trnlnm("SHARE_PACKIDX") .nes. "" then $ close SHARE_PACKIDX
$ if f$search(Tempfile) .nes. "" then $ delete &TempFile;*
$ v=f$verify(v)
$ exit status
$
$
$!
$! ---------------------------------------------------------------------------
$! GET AVAILABLE FACILITIES
$!
$!   Various versions of TPU have additional facilities that can be used to
$!   improve things. Here we determine what facilities are available for
$!   use
$!
$! ---------------------------------------------------------------------------
$Get_Available_Facilities:
$ WorkFileAvailable  = "T"	! TPU recognizes /WORK qualifier
$ LongLinesAvailable = "T"	! TPU allows records > 960 bytes
$
$! Now check the version
$ if IsAlpha then $ return
$
$ if (VMSx .lts. Min_VMS_For_LongLines)
$   then
$      LongLinesAvailable = "F"
$ endif
$
$ if (VMSx .lts. Min_VMS_For_WorkFile)
$   then
$      WorkFileAvailable = "F"
$ endif
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! GET DEFAULT PARAMETERS
$!
$!   Defaults are set by logical names. If no logical exists, then the
$!   built-in system default is used.
$!
$! ---------------------------------------------------------------------------
$Get_Default_Parameters:
$! Get parameters from logical names/system; normally these will set defaults
$ Compression=f$edit(f$trnlnm("SHARE_COMPRESS"),      "TRIM")
$ Confirm   = f$edit(f$trnlnm("SHARE_CONFIRM"),       "TRIM")
$ Debugging = f$edit(f$trnlnm("SHARE_DEBUG"),         "TRIM")
$ DirKeep   = f$edit(f$trnlnm("SHARE_DIRECTORY"),     "TRIM")
$ Exclude   = f$edit(f$trnlnm("SHARE_EXCLUDE"),       "TRIM,UPCASE")
$ Logging   = f$edit(f$trnlnm("SHARE_LOG"),           "TRIM")
$ LongLines = f$edit(f$trnlnm("SHARE_LONGLINES"),     "TRIM")
$ Name      = f$edit(f$trnlnm("SHARE_NAME"),          "TRIM")
$ PackageIdx= f$edit(f$trnlnm("SHARE_PACKAGE_INDEX"), "TRIM")
$ Part_Size = f$edit(f$trnlnm("SHARE_PART_SIZE"),     "TRIM")
$ Real_Name = f$edit(f$trnlnm("SHARE_REAL_NAME"),     "TRIM")
$ Share     = f$edit(f$trnlnm("SHARE_SHARE"),         "TRIM")
$ SpaceEnc  = f$edit(f$trnlnm("SHARE_SPACE_ENCODE"),  "TRIM")
$ Split     = f$edit(f$trnlnm("SHARE_SPLIT"),         "TRIM")
$ VersKeep  = f$edit(f$trnlnm("SHARE_VERSION"),       "TRIM")
$ TempFile  = f$edit(f$trnlnm("SHARE_TEMP"),          "TRIM")
$ WorkFile  = f$edit(f$trnlnm("SHARE_WORK"),          "TRIM")
$
$
$! ------------------------------------------------------------------------
$! SET UP THE SHARE FILE IDENT LINE / EMAIL ADDRESS
$! ------------------------------------------------------------------------
$ gosub Get_Email_Address
$ gosub Build_Ident_Line
$
$! ------------------------------------------------------------------------
$! SET UP THE DEFAULT MAX LINE LENGTH AND MIN TPU VERSIONS ALLOWED FOR UNPACKING
$! ------------------------------------------------------------------------
$ Min_VMS_To_Unpack   = Min_VMS_For_TPU	! Default allows any version of TPU
$ Max_TPU_Line_Length = Max_Line_OldTPU ! ... provided we stick to short lines
$
$
$! ------------------------------------------------------------------------
$! DEFAULT FILE SELECTION DATES/CRITERIA
$! ------------------------------------------------------------------------
$ Since    = ""				! Earliest date is anything
$ Before   = ""				! Latest date is anything
$ Criteria = "RDT"			! Revision (modification) is default
$
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! PARSE COMMAND LINE
$!
$!    Parse the parameter list and extract qualifiers and parameters
$!
$! ---------------------------------------------------------------------------
$Parse_Command_Line:
$ Cmd_Line=f$edit(P1+" "+P2+" "+P3+" "+P4+" "+P5+" "+P6+" "+P7+" "+P8,"TRIM,COMPRESS")
$ P1 = ""
$ P2 = ""
$ P3 = ""
$ P4 = ""
$ P5 = ""
$ P6 = ""
$ P7 = ""
$ P8 = ""
$
$ N=0
$ Qualifiers = ""
$Next_Param:
$ if f$length(Cmd_Line) .ne. 0
$   then
$     String = f$extract(0,f$locate(" ",Cmd_Line),Cmd_Line)
$     Cmd_Line = Cmd_Line - String - " "
$     Param = f$extract(0,f$locate("/",String),String)
$     String = String - Param
$     if Param .nes. ""
$       then
$         N=N+1
$         P'N' = Param
$     endif
$     qualifiers = qualifiers + String
$     goto Next_Param
$ endif
$ 
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! BREAK OUT PARAMETERS
$!
$!    Check the parameters and break out those we want
$!
$! ---------------------------------------------------------------------------
$Break_Out_Parameters:
$ FileSpec = P1
$ ShareFile= P2
$
$get_filespec:
$ filespec = f$edit(filespec,"TRIM,UPCASE")
$ if filespec .eqs. ""
$   then
$     if f$mode() .nes. "INTERACTIVE"
$       then
$         em "-E-MISREQPAR, Missing required parameter - list of files"
$         goto abort
$     endif
$     read/prompt=".. Enter list of files to be packed : " sys$command filespec
$     goto get_filespec
$ endif
$
$get_share_file:
$ sharefile = f$edit(sharefile,"TRIM,UPCASE")
$ if sharefile .eqs. ""
$   then
$     if f$mode() .nes. "INTERACTIVE"
$       then
$         em "-E-MISREQPAR, Missing required parameter - output share file name"
$         goto abort
$     endif
$     read/prompt=".. Enter name of share file         : " sys$command sharefile
$     goto get_share_file
$ endif
$
$Check_Output_Dir:
$ if f$parse(sharefile) .eqs. ""
$   then
$     em "-E-INVOUT, Invalid output file or non-existent output directory"
$     goto abort
$ endif
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! BREAK OUT OPTIONS
$!
$!  Scan the qualifier string and set up the various options selected by the
$!  user. Default values for each option should already have been set up
$!  by the get_default_parameters routine.
$!  
$! ---------------------------------------------------------------------------
$Break_Out_Options:
$ if Qualifiers .nes. ""
$   then
$     ThisQual = Qualifiers - "/"
$     ThisQual = f$extract(0,f$locate("/",ThisQual),ThisQual)
$     Qualifiers = Qualifiers - "/" - ThisQual
$
$   ! First check for negated qualifiers and remember the status
$     if f$locate("NO",ThisQual) .eq. 0
$       then
$         ThisQual=ThisQual-"NO"
$         Value="FALSE"
$       else
$         Value="TRUE"
$     endif
$
$   ! Extract any equivalence value for parameters which expect them
$     Equiv    = f$element(1,"=",ThisQual)
$     ThisQual = f$element(0,"=",ThisQual)
$
$! Detect ambiguous qualifier /B
$     if f$locate(ThisQual,"B") .eq. 0 then $ goto BOO_AmbigQual
$
$! /BACKUP
$     if f$locate(f$extract(0,f$length("BACKUP"),ThisQual),"BACKUP") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         Criteria = "BDT"
$         goto Break_Out_Options
$     endif
$
$! /BEFORE[=date]
$     if f$locate(f$extract(0,f$length("BEFORE"),ThisQual),"BEFORE") .eq. 0
$       then
$         Before = ""
$         if Value .and. Equiv .nes. "=" then $ Before = Equiv
$         if Value .and. Equiv .eqs. "=" then $ Before = "TODAY"
$         goto Break_Out_Options
$     endif
$
$! Detect ambiguous qualifier /CO
$     if f$locate(ThisQual,"CO") .eq. 0 then $ goto BOO_AmbigQual
$
$! /COMPRESS[=n]
$     if f$locate(f$extract(0,f$length("COMPRESS"),ThisQual),"COMPRESS") .eq. 0
$       then
$         Compression = Value
$         if Compression .and. Equiv .nes. "=" then $ Compression = Equiv
$         goto Break_Out_Options
$     endif
$
$! /CONFIRM
$     if f$locate(f$extract(0,f$length("CONFIRM"),ThisQual),"CONFIRM") .eq. 0
$       then
$         Confirm = Value
$         goto Break_Out_Options
$     endif
$
$! /CREATED
$     if f$locate(f$extract(0,f$length("CREATED"),ThisQual),"CREATED") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         Criteria = "CDT"
$         goto Break_Out_Options
$     endif
$
$! Detect ambiguous qualifier /D
$     if f$locate(ThisQual,"D") .eq. 0 then $ goto BOO_AmbigQual
$
$! /DEBUG[=n]
$     if f$locate(f$extract(0,f$length("DEBUG"),ThisQual),"DEBUG") .eq. 0
$       then
$         Debugging = Value
$         if Debugging .and. Equiv .nes. "=" then $ Debugging = Equiv
$         goto Break_Out_Options
$     endif
$
$! /DIRECTORY
$     if f$locate(f$extract(0,f$length("DIRECTORY"),ThisQual),"DIRECTORY") .eq. 0
$       then
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         DirKeep = Value
$         goto Break_Out_Options
$     endif
$
$! Detect ambiguous qualifier /EX
$     if f$locate(ThisQual,"EX") .eq. 0 then $ goto BOO_AmbigQual
$
$! /EXCLUDE
$     if f$locate(f$extract(0,f$length("EXCLUDE"),ThisQual),"EXCLUDE") .eq. 0
$       then
$         Exclude = ""
$         if Value .and. Equiv .nes. "=" then $ Exclude = Equiv
$         goto Break_Out_Options
$     endif
$
$! /EXPIRED
$     if f$locate(f$extract(0,f$length("EXPIRED"),ThisQual),"EXPIRED") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         Criteria = "EDT"
$         goto Break_Out_Options
$     endif
$
$! Detect ambiguous qualifier /LO
$     if f$locate(ThisQual,"LO") .eq. 0 then $ goto BOO_AmbigQual
$
$! /LOG[=n]
$     if f$locate(f$extract(0,f$length("LOG"),ThisQual),"LOG") .eq. 0
$       then
$         Logging = Value
$         if Logging .and. Equiv .nes. "=" then $ Logging=Equiv
$         goto Break_Out_Options
$     endif
$
$! /LONGLINES
$     if f$locate(f$extract(0,f$length("LONGLINES"),ThisQual),"LONGLINES") .eq. 0
$       then
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         LongLines = Value
$         goto Break_Out_Options
$     endif
$
$! /MODIFIED
$     if f$locate(f$extract(0,f$length("MODIFIED"),ThisQual),"MODIFIED") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         Criteria = "RDT"
$         goto Break_Out_Options
$     endif
$
$! /NAME[=identifier]
$     if f$locate(f$extract(0,f$length("NAME"),ThisQual),"NAME") .eq. 0
$       then
$         Name = "="						! /NONAME
$         if Value .and. Equiv .eqs. "=" then $ Name = ""	! /NAME
$         if Value .and. Equiv .nes. "=" then $ Name = Equiv	! /NAME=xx
$         goto Break_Out_Options
$     endif
$
$! Detect ambiguous qualifier /PA
$     if f$locate(ThisQual,"PA") .eq. 0 then $ goto BOO_AmbigQual
$
$! /PACKAGE_INDEX
$     if f$locate(f$extract(0,f$length("PACKAGE_INDEX"),ThisQual),"PACKAGE_INDEX") .eq. 0
$       then
$         PackageIdx = Value
$         if PackageIdx .and. Equiv .nes. "=" then $ PackageIdx = Equiv
$         goto Break_Out_Options
$     endif
$
$! /PART_SIZE=nnn
$     if f$locate(f$extract(0,f$length("PART_SIZE"),ThisQual),"PART_SIZE") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if equiv .eqs. "=" then $ goto BOO_Qual_Val_Required
$         Part_Size = Equiv
$         goto Break_Out_Options
$     endif
$
$! Detect ambiguous qualifier /SP
$     if f$locate(ThisQual,"SP") .eq. 0 then $ goto BOO_AmbigQual
$
$! /SHARE
$     if f$locate(f$extract(0,f$length("SHARE"),ThisQual),"SHARE") .eq. 0
$       then
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         Share = Value
$         goto Break_Out_Options
$     endif
$
$
$! /SINCE[=date]
$     if f$locate(f$extract(0,f$length("SINCE"),ThisQual),"SINCE") .eq. 0
$       then
$         Since = ""
$         if Value .and. Equiv .nes. "=" then $ Since = Equiv
$         if Value .and. Equiv .eqs. "=" then $ Since = "TODAY"
$         goto Break_Out_Options
$     endif
$
$! /SPACE_ENCODE
$     if f$locate(f$extract(0,f$length("SPACE_ENCODE"),ThisQual),"SPACE_ENCODE") .eq. 0
$       then
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         SpaceEnc = Value
$         goto Break_Out_Options
$     endif
$
$
$! /SPLIT[=n]
$     if f$locate(f$extract(0,f$length("SPLIT"),ThisQual),"SPLIT") .eq. 0
$       then
$         if .not. Value                 then $ Split = "0"	! /NOSPLIT
$         if Value .and. Equiv .eqs. "=" then $ Split = ""	! /SPLIT
$         if Value .and. Equiv .nes. "=" then $ Split = Equiv	! /SPLIT=nnn
$         goto Break_Out_Options
$     endif
$
$! /TEMPORARY=Tempfile
$     if f$locate(f$extract(0,f$length("TEMPORARY"),ThisQual),"TEMPORARY") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if equiv .eqs. "=" then $ goto BOO_Qual_Val_Required
$         TempFile = Equiv
$         goto Break_Out_Options
$     endif
$
$! /VERSION
$     if f$locate(f$extract(0,f$length("VERSION"),ThisQual),"VERSION") .eq. 0
$       then
$         if Equiv .nes. "=" then $ goto BOO_QualNoValue
$         VersKeep = Value
$         goto Break_Out_Options
$     endif
$
$! /WORK[=filename]
$     if f$locate(f$extract(0,f$length("WORK"),ThisQual),"WORK") .eq. 0
$       then
$         if .not. Value then $ goto BOO_NonNegQual
$         if Value .and. Equiv .nes. "=" then $ WorkFile=Equiv
$         if Value .and. Equiv .eqs. "=" then $ WorkFile=""
$         goto Break_Out_Options
$     endif
$
$! Something else ..
$     em "-E-UNRECQUAL, Invalid qualifier specified"
$     if .not. Value then $ ThisQual = "NO" + ThisQual
$     write sys$error "  \", ThisQual, "\"
$     goto abort
$ endif
$ return
$
$
$BOO_AmbigQual:
$ if .not. value then $ ThisQual = "NO" + ThisQual
$ em "-E-AMBIG, /", ThisQual, " is an ambiguous qualifier"
$ goto abort
$
$BOO_NonNegQual:
$ em "-E-NONNEG, /NO", ThisQual, " is not a negatable qualifier"
$ goto abort
$
$BOO_QualNoValue:
$ em "-E-NOTREQ, ", ThisQual, " does not take a value specification"
$ goto abort
$
BOO_Qual_Val_Required:
$ em "-E-VALREQ, ", Thisqual, " requires a value specification"
$ goto abort
$!
$! ---------------------------------------------------------------------------
$! CHECK PARAMETERS
$!
$!   Check that the user supplied parameters have reasonable values and
$!   give errors if not. In some cases, set up defaults.
$!
$! ---------------------------------------------------------------------------
$Check_Parameters:
$! CONVERT VARIOUS FLAGS TO SUITABLE INTEGER VALUES
$! FALSE becomes numeric 0, TRUE becomes numeric 1; if integer, use that value
$ Compression= f$integer(Compression)
$ Debugging  = f$integer(Debugging)
$ Logging    = f$integer(Logging)
$ if DirKeep  .eqs. "" then $ DirKeep  = "T"	! Default is to keep them
$ Dirkeep    = f$integer(DirKeep)
$ if Share    .eqs. "" then $ Share    = "T"	! Default is to write them
$ Share      = f$integer(Share)
$ if SpaceEnc .eqs. "" then $ SpaceEnc = "T"	! Default is to encode spaces
$ SpaceEnc   = f$integer(SpaceEnc)
$ if Verskeep .eqs. "" then $ Verskeep = "T"	! Default is to keep them
$ VersKeep   = f$integer(VersKeep)
$
$
$! CHECK THE PART SIZE SPECIFICATION
$ if Part_Size .eqs. "" then $ Part_Size = Def_Part_Size
$ if f$type(Part_Size) .nes. "INTEGER"
$   then
$     em "-E-NONNUMSIZE, Part size is not numeric"
$     goto abort
$ endif
$ if Part_Size .le. 0
$   then
$     em "-E-NONPOSSIZE, Part size is not a positive value"
$     goto abort
$ endif
$
$
$! CHECK THE SPLIT VALUE, SET A DEFAULT TO A FUNCTION OF WSEXTENT AND PART SIZE
$ if Split .eqs. ""
$   then
$    ! We deduct two lots of parts as we generally keep 2 in memory
$     Split = f$getjpi("","WSEXTENT") - (2*Part_Size)
$     if Split .lt. 1000 then $ Split = 1000	! Use respectable minimum
$ endif
$ Split      = f$integer(Split)
$
$
$! CHECK THE LONG LINES OPTION; ONLY ALLOWED IF THIS VMS SUPPORTS IT
$ if LongLines
$   then
$     if .not. LongLinesAvailable
$       then
$         em "-E-NOLONG, Long lines not supported prior to VMS ", Min_VMS_For_LongLines
$         goto abort
$     endif
$     Max_TPU_Line_Length = Max_Line_NewTPU
$     Min_VMS_To_Unpack   = Min_VMS_For_LongLines
$ endif
$
$
$! MASSAGE THE SHARE FILE NAME
$ ShareFile = ShareFile - f$parse(ShareFile,,,"VERSION")
$ if f$parse(ShareFile,,,"TYPE") .nes. "."
$   then ! non-null extension
$     ShareFile = ShareFile + "_"
$   else ! null extension OR a single dot extension; force to single dot
$     if f$extract(f$length(ShareFile)-1,1,ShareFile) .nes. "."
$       then
$         ShareFile = ShareFile + "."
$     endif
$ endif
$ 
$
$! CHECK THE TEMPORARY FILE/DIRECTORY SPECIFICATION
$ if f$parse(TempFile) .eqs. ""
$   then
$     em "-E-INVTEMP, Invalid temporary file spec - '", TempFile,"'"
$     goto abort
$ endif
$
$ TempFile = f$parse(TempFile,"SYS$SCRATCH:SHARE_TEMP."+f$getjpi("","PID"))
$ TempFile = TempFile - f$parse(TempFile,,,"VERSION")
$
$
$! WORKFILE CHECKING
$ if WorkFile .nes. ""
$   then
$
$  ! DO WE ALLOW A WORKFILE SPEC ???
$     if .not. WorkFileAvailable
$       then
$         em "-E-WORKNOTSUP, Workfile specification not supported on this VMS"
$         goto abort
$     endif
$
$  ! CHECK THE WORK FILE/DIRECTORY SPECIFICATION
$     if f$parse(WorkFile) .eqs. ""
$       then
$         em "-E-INVWORK, Invalid work file spec - '", WorkFile, "'"
$         goto abort
$     endif
$     WorkFile = "/WORK=" + WorkFile
$ endif
$
$
$! CHECK THE PACKAGE INDEX SUFFIX SPECIFICATION
$ if f$extract(0,1,PackageIdx) .nes. "."
$   then
$     if PackageIdx
$       then PackageIdx = ".$PACKAGE"	! Set default suffix for "TRUE" value
$       else PackageIdx = ""		! Set no suffix for "FALSE" value
$     endif
$   else
$     if f$parse(PackageIdx,,,"TYPE") .nes. PackageIdx
$       then
$         em "-E-INVSUFFIX, Invalid package index file suffix specification ", PackageIdx
$         goto abort
$     endif
$ endif
$
$
$! SET UP THE DEFAULT ARCHIVE NAME (FROM /NAME=XXX)
$ if Name .eqs. ""  then $ Name = f$parse(ShareFile,,,"NAME")
$ if Name .eqs. "=" then $ Name = ""
$
$
$! WE ALLOW DEBUGGING ONLY FOR SYSTEMS PEOPLE, WITH EITHER SYSPRV OR SETPRV
$! AUTHORIZED (DOESN'T HAVE TO BE ENABLED, JUST AUTHORIZED)
$ x = f$getjpi("","AUTHPRIV")
$ l = f$length(x)
$ if f$locate("SYSPRV",x) .eq. l .and. f$locate("SETPRV",x) .eq. l then $ Debugging = 0
$
$
$! CHECK COMPRESSION VALUE IS WITHIN LIMITS CURRENTLY SUPPORTED
$ if Compression .lt. 0  .or. Compression .gt. 2
$   then
$     em "-E-UNSUPCOM, Unsupported compression type"
$     goto abort
$ endif
$
$
$! CHECK THE SPLIT SIZE SPECIFICATION
$ if f$type(Split) .nes. "INTEGER"
$   then
$     em "-E-NONNUMSIZE, Split size is not numeric"
$     goto abort
$ endif
$ if Split .le. 0
$   then
$     em "-E-NONPOSSIZE, Split size is not a positive value"
$     goto abort
$ endif
$
$
$! CHECK THE BEFORE/SINCE DATE SPECIFICATIONS
$ if Before .nes. ""
$   then
$     Before = f$cvtime(Before,"COMPARISON")
$     if .not. $status
$       then
$         em "-E-INVTIME, Bad /BEFORE time specification - ", Before
$         goto abort
$     endif
$ endif
$ if Since .nes. ""
$   then
$     Since  = f$cvtime(Since,"COMPARISON")
$     if .not. $status
$       then
$         em "-E-INVTIME, Bad /SINCE time specification - ", Since
$         goto abort
$     endif
$ endif
$
$
$! CONFIRMATION OF EACH FILE SELECTED IS ONLY AVAILABLE IF INTERACTIVE AS WE
$! CAN'T PROMPT ANYONE OTHERWISE.
$ if Confirm .and. f$mode() .nes. "INTERACTIVE"
$   then
$      em "-w-CONFNOTINT, /CONFIRM ignored - not interactive"
$      Confirm = "FALSE"
$ endif
$ return
$
$!
$! ---------------------------------------------------------------------------
$! GET EMAIL ADDRESS
$!
$!    Determine the network name of the system by translating the logical names
$!    known to exist under the various network packages. The first one we find
$!    will be appended to the username to form the local e-mail address.
$!
$!    If the logical name SHARE_EMAIL_ADDRESS exist, then this is used as the
$!    e-mail address and no attempt is made to determine automatically the
$!    e-mail address.
$!
$! ---------------------------------------------------------------------------
$Get_Email_Address:
$ username  = f$edit(f$getjpi("","USERNAME"),"TRIM,LOWERCASE")
$ emailaddr = f$trnlnm("SHARE_EMAIL_ADDRESS")
$ if emailaddr .nes. "" then $ return
$
$! Try each of the known network packages in turn until we find one
$ sitename = f$trnlnm("MX_NODE_NAME","LNM$SYSTEM")		! MX mailer
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("PONY$LOCAL_HOST_NAME","LNM$SYSTEM")	! Pony express
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("INTERNET_HOST_NAME","LNM$SYSTEM")	! CMU TCP/IP
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("MULTINET_HOST_NAME","LNM$SYSTEM")	! Multinet TCP/IP
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("ARPANET_HOST_NAME","LNM$SYSTEM")		! Wollongong
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     sitename = f$trnlnm("INET_DOMAIN_NAME","LNM$SYSTEM")
$     if sitename .nes. "" then emailaddr = emailaddr + "." + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("TCPWARE_SMTP_FROM_DOMAIN","LNM$SYSTEM")	! TCPware
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("TCPWARE_DOMAINNAME","LNM$SYSTEM")	! TCPware
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("UUCP_DOMAIN_NAME","LNM$SYSTEM")		! DECUS UUCP
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$ sitename = f$trnlnm("FTP$CONTEXT","LNM$SYSTEM")		! CBS
$ if sitename .nes. ""
$   then
$     emailaddr = username + "@" + sitename
$     return
$ endif
$
$! Can't work out the e-mail address !!!
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! BUILD IDENT LINE
$!
$!    This routine builds the originator ident line that is placed at the
$!    head of the generated archive file. There are several formats for this
$!    line depending on available information.
$!
$!    If the logical SHARE_IDENT exist, its translation is used as the ident
$!    line.
$!
$!    If the email address is available, then the ident line is generated in
$!    one of these two formats, dependent on whether the SHARE_REAL_NAME
$!    logical translates to the user's real name:
$!
$!	1.		Real Name <username@sitename>
$!	2.		username@sitename
$!
$!    If the email address is not available, then the ident line is generated
$!    as one of these two formats, dependent on whether the SHARE_REAL_NAME
$!    logical translates to the user's real name:
$!
$!	3.		username (Real Name)
$!	4.		username
$!
$! ---------------------------------------------------------------------------
$Build_Ident_Line:
$ IdentLine = f$trnlnm("SHARE_IDENT")
$ if IdentLine .nes. "" then $ return
$
$ RealName = f$trnlnm("SHARE_REAL_NAME")
$ if Realname .nes. ""
$   then
$     if emailaddr .nes. ""
$       then
$         IdentLine = RealName + " <" + EmailAddr + ">"
$       else
$         IdentLine = UserName + " ( " + RealName + " )"
$     endif
$   else
$     if emailaddr .nes. ""
$       then
$         IdentLine = EmailAddr
$       else
$         IdentLine = Username
$     endif
$ endif
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! CREATE PARAMETER FILE
$!
$!   Write a file containing all the parameters, options and filenames which the
$!   Packing routine needs to process.
$!
$! ---------------------------------------------------------------------------
$Create_Parameter_File:
$ create &TempFile
$ if .not. $status then $ goto CPF_error1
$ open/append/error=CPF_error2 SHARE_PARAMS &TempFile
$ write/error=CPF_error2 SHARE_PARAMS Facility
$ IfDebugShowParams "-I-DBGPARAM, Facility           = ", Facility
$ write/error=CPF_error2 SHARE_PARAMS Version
$ IfDebugShowParams "-I-DBGPARAM, Version            = ", Version
$ write/error=CPF_error2 SHARE_PARAMS Min_VMS_to_Unpack
$ IfDebugShowParams "-I-DBGPARAM, Min_VMS_To_Unpack  = ", Min_VMS_To_Unpack
$ write/error=CPF_error2 SHARE_PARAMS Max_TPU_Line_Length
$ IfDebugShowParams "-I-DBGPARAM, Max_TPU_Line_Length= ", Max_TPU_Line_Length
$ write/error=CPF_error2 SHARE_PARAMS Max_Share_Length
$ IfDebugShowParams "-I-DBGPARAM, Max_Share_Length   = ", Max_Share_Length
$ write/error=CPF_error2 SHARE_PARAMS Debugging
$ IfDebugShowParams "-I-DBGPARAM, Debugging          = ", Debugging
$ write/error=CPF_error2 SHARE_PARAMS Logging
$ IfDebugShowParams "-I-DBGPARAM, Logging            = ", Logging
$ write/error=CPF_error2 SHARE_PARAMS Part_Size
$ IfDebugShowParams "-I-DBGPARAM, Part_Size          = ", Part_Size
$ write/error=CPF_error2 SHARE_PARAMS Compression
$ IfDebugShowParams "-I-DBGPARAM, Compression        = ", Compression
$ write/error=CPF_error2 SHARE_PARAMS DirKeep
$ IfDebugShowParams "-I-DBGPARAM, DirKeep            = ", DirKeep
$ write/error=CPF_error2 SHARE_PARAMS VersKeep
$ IfDebugShowParams "-I-DBGPARAM, VersKeep           = ", VersKeep
$ write/error=CPF_error2 SHARE_PARAMS IdentLine
$ IfDebugShowParams "-I-DBGPARAM, IdentLine          = ", IdentLine
$ write/error=CPF_error2 SHARE_PARAMS ShareFile
$ IfDebugShowParams "-I-DBGPARAM, ShareFile          = ", ShareFile
$ write/error=CPF_error2 SHARE_PARAMS Share
$ IfDebugShowParams "-I-DBGPARAM, Share              = ", Share
$ write/error=CPF_error2 SHARE_PARAMS SpaceEnc
$ IfDebugShowParams "-I-DBGPARAM, Space Encoding     = ", SpaceEnc
$ write/error=CPF_error2 SHARE_PARAMS Split
$ IfDebugShowParams "-I-DBGPARAM, Split              = ", Split
$ write/error=CPF_error2 SHARE_PARAMS Name
$ IfDebugShowParams "-I-DBGPARAM, Name               = ", Name
$
$ FileCount = 0
$ gosub Scan_For_Files			! Scan directory for matching files
$ close SHARE_PARAMS
$ return
$
$
$CPF_error1:
$ em "-E-CPFCRE, Unable to create parameter file"
$ goto abort
$CPF_error2:
$ em "-E-CPFAPP, Unable to append parameters to parameter file"
$ goto abort
$
$
$!
$! ---------------------------------------------------------------------------
$! SCAN FOR FILES
$!
$!   Scan the file system for the files specified for the user; then add them
$!   if required into the parameter list of files to be packed.
$!
$! ---------------------------------------------------------------------------
$Scan_For_Files:
$Next_File_Spec:
$ if filespec .nes. ""
$   then
$     pattern     = f$element(0,",",filespec)
$     filespec    = filespec - pattern - ","
$     File_Device = f$parse(pattern,,,"DEVICE")
$
$   ! Make sure the syntax of the pattern is a legal filespec
$     if File_Device .eqs. ""
$       then
$         em "-E-BADFILE, Bad file specification '", pattern, "'"
$         goto abort
$     endif
$
$   ! Action the filespec...
$     if DirKeep .and. File_Device .nes. Cur_Device
$       then
$         em "-E-NOTCURDEV, Skipped ", pattern, " : does not refer to current device"
$       else
$         fileversion = f$parse(pattern,,,"VERSION")
$         searchstring= f$parse(pattern-fileversion,"[]")-";" + fileversion
$         Previous_File = ""
$         gosub DoPattern
$     endif
$     goto Next_File_Spec
$ endif
$ return
$
$
$DoPattern:
$ File = f$search(searchstring)
$ if File .eqs. ""
$   then
$     em "-W-NOMATCH, No files matching ", pattern
$     return
$ endif
$
$ProcessNextFile:
$ gosub Check_Out_File
$ Previous_File = File
$ File = f$search(searchstring)
$ if File .nes. "" .and File .nes. Previous_File then $ goto ProcessNextFile
$ return
$!
$! ---------------------------------------------------------------------------
$! CHECK OUT FILE
$!
$!   Given the latest filename that matches the user's filespec, examine it
$!   to see if it's a candidate for selection. If it is, record its name,
$!   checksum and record attributes in the parameter file.
$!
$!   Files are rejected if they
$!      - Are directories
$!	- Are not sequential files
$!	- Fall outside the range of dates specified by /BEFORE and /SINCE
$!	- Match one of the exclusion clauses (/EXCLUDE etc)
$!	- Are not in the directory tree starting at the current directory
$!	  (unless the /NODIRECTORY option is given)
$!	- Resemble a sharefile part
$!	- Have an unsupported record type or size
$!	- If they are not confirmed by the user (/CONFIRM)
$!
$! ---------------------------------------------------------------------------
$Check_Out_File:
$! Get the elements of the filename that we need
$ File_Dir      = f$parse(File,,,"DIRECTORY")
$ File_Name     = f$parse(File,,,"NAME")
$ File_Type     = f$parse(File,,,"TYPE") 
$ File_Version  = f$parse(File,,,"VERSION")
$ File_Filename = File_Name + File_Type
$
$ SubFile       = File
$
$
$! ------------------------------------------------------------------------
$! Ignore directories always
$! ------------------------------------------------------------------------
$ if File_Type .eqs. ".DIR"
$   then
$     IfDebugRejected "-I-REJDIR, Skipped ", File, " : is a directory"
$     return
$ endif
$
$
$! ------------------------------------------------------------------------
$! We can only deal with files that are SEQUENTIAL
$! ------------------------------------------------------------------------
$ if f$file_attributes(File,"ORG") .nes. "SEQ"
$   then
$     em "-W-NOTSEQ, Skipped ", File, " : not a SEQUENTIAL file"
$     return
$ endif
$
$
$! ------------------------------------------------------------------------
$! Scan the exclusions list element by element to find any that match
$! this one. An `*' Wildcard may be used to represent a complete field of
$! the file, such as the directory, filename, type, or version. It may not
$! be used to represent a partial field of any of these.
$! ------------------------------------------------------------------------
$ if Exclude .nes. ""
$   then
$     N=0
$   Exclude_Loop:
$     Epattern = f$element(N,",",Exclude)
$     if Epattern .nes. ","
$       then
$
$         p = f$parse(Epattern,"[*]",,"DIRECTORY")
$         xdir  = p .eqs. "[*]"  .or. p .eqs. File_Dir
$         p = f$parse(Epattern,"*",,"NAME")
$         xname = p .eqs. "*"    .or. p .eqs. File_Name
$         p = f$parse(Epattern,".*",,"TYPE")
$         xtype = p .eqs. ".*"   .or. p .eqs. File_Type
$         p = f$parse(Epattern,";*",,"VERSION")
$         xvers = p .eqs. ";*"   .or. p .eqs. File_Version
$         if xdir .and. xname .and. xtype .and. xvers
$           then
$             ifDebugRejected "-I-REJEXC, Skipped ", File, " : matches /EXCLUDE=", Epattern
$             return
$         endif
$
$         N=N+1
$         goto Exclude_Loop
$     endif
$ endif
$
$
$! ------------------------------------------------------------------------
$! Date based file selection; Before/Since dates already in COMPARISON format
$! ------------------------------------------------------------------------
$ Date = f$cvtime(f$file(File,Criteria),"COMPARISON")
$ if Before .nes. "" .and. Date .ges. Before
$   then
$     IfDebugRejected "-I-REJBEF, Skipped ", File, " : ", Criteria, " date not /BEFORE=", Before
$     return
$ endif
$ if Since  .nes. "" .and. Date .lts. Since 
$  then
$     IfDebugRejected "-I-REJAFT, Skipped ", File, " : ", Criteria, " date not /AFTER=", After
$    return
$ endif
$
$
$! ------------------------------------------------------------------------
$! If the /DIRECTORY option has been specified, Ensure that the file exists
$! at or below the current directory tree. This prevents random top level
$! directories being created when the share file is unpacked. The
$! subdirectory structure is preserved.
$!
$! If current directory is the MFD ([000000]) then we should allow any
$! underlying directory on the same device, otherwise we just allow a sub
$! directory of the current one. In either case, convert it to a subdirectory
$! format
$! ------------------------------------------------------------------------
$ if DirKeep
$   then
$
$     Subfile  = SubFile - File_Device	! Strip device name
$
$    ! Massage directory name into a suitable subdirectory format
$     if Cur_Directory .eqs. "[000000]"
$       then
$       ! Convert [000000.xxx] and [xxx] into [.xxx]
$         if f$locate("[000000]",SubFile) .eq. 0 then $ SubFile = SubFile - "[000000]"
$         if f$locate("[000000.",SubFile) .eq. 0 then $ SubFile = SubFile - "000000" 
$         if f$extract(0,1,SubFile) .eqs. "[" .and. f$extract(0,2,SubFile) .nes. "[."
$           then
$             SubFile = "[." + (SubFile - "[")
$         endif
$       else
$       ! Strip off the leading current directory to leave a subdirectory
$         if f$locate(Cur_Directory-"]",SubFile) .ne. 0
$           then
$             em "-W-NOTINTREE, Skipped ", SubFile, " : not a valid subdirectory"
$             return
$         endif
$         SubFile = "[" + (SubFile - (Cur_Directory-"]"))
$     endif
$     SubFile = SubFile - "[]"	! Don't need a null directory spec
$ endif
$
$
$! ------------------------------------------------------------------------
$! Get file's record attributes
$! ------------------------------------------------------------------------
$ Gosub Get_File_Record_Attributes
$ if (Attributes .eqs. "UNKNOWN")
$   then
$      em "-W-RFMNOTSUP, Skipped ", Subfile, " : Unsupported record format (RFM=''rfmmode',RAT=''ratmode')"
$      return
$ endif
$
$ if (Recsiz .ge. Max_TPU_Line_Length)
$   then
$      em "-W-RECTOOBIG, Skipped ", Subfile, " : Max Record size of ''Recsiz' exceeds system limit of ''Max_TPU_Line_Length'"
$      return
$ endif
$
$
$
$! ------------------------------------------------------------------------
$! Get file checksum; note that the CHECKSUM command is currently undocumented
$! ------------------------------------------------------------------------
$ define/user sys$output nl:
$ define/user sys$error  nl:
$ checksum &File
$ if .not. $status
$   then
$     em "-W-RECFMTERR, ", Subfile, " : Record format cannot be processed"
$     return
$ endif
$
$
$! ------------------------------------------------------------------------
$! If the user has requested it, confirm this selection interactively
$! ------------------------------------------------------------------------
$ if Confirm
$   then
$     read /end=NotConfirmed /prompt="Select ''Subfile' ? (Y/N) [N] " sys$command reply
$     if .not. reply then $ return
$ endif
$
$
$! ------------------------------------------------------------------------
$! Log this file as selected
$! ------------------------------------------------------------------------
$ IfLogSelected     "-I-SELECTED, ", f$fao("!8%T, !AS", 0, SubFile)
$ IfDebugShowParams "-I-DBGPARAM, ", SubFile, ", Attributes='", Attributes,"'"
$
$
$! ------------------------------------------------------------------------
$! All checks passed; add new entry to the parameter file
$! ------------------------------------------------------------------------
$ write SHARE_PARAMS SubFile, " ", CheckSum$CheckSum, " ", """''Attributes'"""
$ FileCount = FileCount + 1
$NotConfirmed:
$ return
$
$
$!
$! ---------------------------------------------------------------------------
$! GET FILE RECORD ATTRIBUTES
$!
$!   The file record attributes are returned, in a form that can be passed
$!   directly to CONVERT/FDL, so that the record structure of the selected
$!   file can be re-instated later.
$!
$!   TPU, used to encode files, always writes files out in a variable length
$!   format with carriage return controls.  If this is the format of the
$!   original file no conversion is needed so we return null attributes
$!
$!   A return type of "UNKNOWN" indicates a record type that we do not support.
$!
$!   The record size of a file is also returned so that it can be tested
$!   against the maximum record size supported by this TPU. We can therefore
$!   reject files with records that are too long early on in the packing
$!   process.
$!
$! ---------------------------------------------------------------------------
$Get_File_Record_Attributes:
$ rfmmode = f$file_attributes(File,"RFM")	! Get file information
$ recsiz  = f$file_attributes(File,"MRS")
$ ratmode = f$file_attributes(File,"RAT")
$ fsz     = f$file_attributes(File,"FSZ")
$
$! Convert the record format to an FDL spec
$ Format  = ""
$ if rfmmode .eqs. "STMLF" then $ Format = "FORMAT STREAM_LF"
$ if rfmmode .eqs. "STMCR" then $ Format = "FORMAT STREAM_CR"
$ if rfmmode .eqs. "STM"   then $ Format = "FORMAT STREAM"
$ if rfmmode .eqs. "FIX"   then $ Format = "FORMAT FIX;SIZE ''recsiz'"
$ if rfmmode .eqs. "VAR"   then $ Format = "FORMAT VARIABLE"
$ if rfmmode .eqs. "VFC"   then $ Format = "FORMAT VFC;CONTROL_FIELD ''fsz'"
$
$! Convert the record attributes to an FDL spec
$ Attributes = ""
$ if ratmode .eqs. ""    then $ Attributes = "CARRIAGE_CONTROL NONE"
$ if ratmode .eqs. "CR"  then $ Attributes = "CARRIAGE_CONTROL CARRIAGE_RETURN"
$ if ratmode .eqs. "PRN" then $ Attributes = "CARRIAGE_CONTROL PRINT"
$ if ratmode .eqs. "FTN" then $ Attributes = "CARRIAGE_CONTROL FORTRAN"
$
$! Check for unsupported formats; return an unknown FDL if found
$ if Format .eqs. "" .or. Attributes .eqs. ""
$   then
$     Attributes = "UNKNOWN"
$     return
$ endif
$
$! Return the complete FDL spec; special case the standard format
$ if rfmmode .eqs. "VAR" .and. ratmode .eqs. "CR"
$   then $ Attributes = ""
$   else $ Attributes = Format + ";" + Attributes
$ endif
$ return
$
$    
$!
$! ---------------------------------------------------------------------------
$! RENAME PARTS
$!
$!   This routine renames the parts generated by the packing routine so that
$!   the names have the format   xxx.nnn-OF-mmm. We need to know in advance
$!   how many parts are generated.
$!
$! ---------------------------------------------------------------------------
$Rename_Parts:
$ if .not. Share then $ return
$! Get the number of parts written by the packing code
$ open/read/error=RP_Error1 SHARE_PARAMS &TempFile
$ read/end=RP_Error2 SHARE_PARAMS NumParts
$ close SHARE_PARAMS
$
$! OK, make sure that we read a sensible number of parts (in case tpu failed)
$ if f$type(NumParts) .nes. "INTEGER"
$   then
$     em "-F-NUMPTSNOTINT, TPU has returned a non integer part count!"
$     goto abort
$ endif
$ if NumParts .le. 0
$   then
$     em "-F-NUMPTSNOTPOS, TPU has returned a negative/zero part count!"
$     goto abort
$ endif
$
$ NumParts = f$integer(NumParts)
$ L=f$length(f$string(NumParts))
$
$! Rename the parts
$ IfLogRenameParts f$fao("-I-RENAMING, !8%T, Renaming parts to "+format1,0,ShareFile,L,L,NumParts)
$
$ N = 0
$RP_Loop:
$ N = N+1
$ if N .le. NumParts
$   then
$     NewPart = f$fao(format2,ShareFile,L,N,L,NumParts)
$     if f$search(ShareFile+"''N'") .eqs. ""
$       then $ em "-E-MISSPART, Unable to find ", ShareFile, N
$       else $ rename 'ShareFile''N' 'NewPart'
$     endif
$     goto RP_Loop
$ endif
$ return
$
$RP_Error1:
$ em "-F-RPOPEN, Unable to open return parameter file"
$ goto abort
$RP_Error2:
$ em "-F-RPREAD, Unable to read part count from return parameter file"
$ goto abort
$
$!
$! ---------------------------------------------------------------------------
$! CREATE PACKAGE INDEX
$!
$!   If selected, we create the package index file suitable for using
$!   with a file server that recognizes package files. The package file
$!   takes its name from the sharefile, with a suffix selected by the
$!   user (default = ".$PACKAGE")
$!
$! ---------------------------------------------------------------------------
$Create_Package_Index:
$ if (PackageIdx .eqs. "") .or. (.not. Share) then $ return
$
$! Create the package index - remove type field, add package suffix
$ l = f$length(f$parse(ShareFile,,,"TYPE"))
$ PackIndex = f$extract(0,f$length(ShareFile)-l,ShareFile) + PackageIdx
$ IfLogPackageIndex f$fao("-I-CREINDEX, !8%T, Creating package index !AS", 0, PackIndex)
$
$ l = f$length(f$string(NumParts))
$ open/write/error=CPI_error1 SHARE_PACKIDX &PackIndex
$ N=0
$RP_PKIND_LOOP:
$ N=N+1
$ if N .le. NumParts
$   then
$     NewPart = f$fao(format2,ShareFile,L,N,L,NumParts)
$     write/error=CPI_error2 SHARE_PACKIDX f$parse(NewPart,,,"NAME"),f$parse(NewPart,,,"TYPE")
$     goto RP_PKIND_LOOP
$ endif
$ close SHARE_PACKIDX
$ return
$
$
$! ERRORS
$CPI_Error1:
$ em "-F-RPPACKCRE, Unable to create package index file"
$ goto abort
$CPI_Error2:
$ em "-F-RPPACKWRITE, Unable to write part name to package index file"
$ goto abort
$
$
$!
$! ---------------------------------------------------------------------------
$! PACK FILES
$!
$!   This routine processes the parameter list in file "TempFile", packs the
$!   files specified and produces the multiple parts of the share file.
$!
$! ---------------------------------------------------------------------------
$PackFiles:
$ edit/tpu/nodisplay/nosection/com=sys$input/nojournal'WorkFile'  &TempFile
! ---------------------------------------------------------------------------
! +                                                                         +
! +                 O V E R V I E W   O F   V M S _ S H A R E               +
! +                                                                         +
! +                                                                         +
! +         To package a series of files into a format that can be          +
! +         successfully mailed through most networks without damage.       +
! +                                                                         +
! +         Characters prone to translation by mailers/networks are         +
! +         encoded into a form that will (hopefully) not be altered        +
! +         but we cannot recover if they ARE altered despite the           +
! +         encoding. A checksum is included so we can at least detect      +
! +         corruption.                                                     +
! +                                                                         +
! +         Files are packaged into a single self-unpacking share file      +
! +         which is split into multiple parts, each small enough to be     +
! +         mailed out without truncation (the part size is configurable)   +
! +                                                                         +
! +                                                                         +
! +   Version 8.4                Andy Harper                      Jun 1993  +
! +                                                                         +
! +                                                                         +
! +                                                                         +
! +                        R E S T R I C T I O N S                          +
! +                                                                         +
! +         1. This version does not support the UDF (undefined record)     +
! +            format. All other record formats are supported although only +
! +            the carriage control record attribute is preserved.          +
! +                                                                         +
! +         2. Text files are fully supported. Binary files should be       +
! +            supported because all non-printing characters are encoded    +
! +            to a printable escape sequence. However, it is advisable to  +
! +            stick to plain text files where possible.                    +
! +                                                                         +
! +         3. Because the files are intended to be mailed, the encoding    +
! +            relies on the integrity of the underlying mailer and network.+
! +            If files are corrupted, we are unable to recover.            +
! +                                                                         +
! +                                                                         +
! +                            C R E D I T S                                +
! +                                                                         +
! +         All due credit must go to the following people:                 +
! +                                                                         +
! +         1.   James Gray for the first version of VMS_SHARE              +
! +         2.   Michael Bednarek for the original idea and prototype       +
! +                                                                         +
! +         Credits are also due to the various people who have submitted   +
! +         ideas and code fragments for some of the features of VMS_SHARE  +
! +         including the following for major features:                     +
! +                                                                         +
! +              Mark Pizzolato, for run-length encoding and attributes     +
! +              Dick Munroe, for better part name conventions              +
! +              Joe Meadows, for various useful updates                    +
! +              Wolfgang Moeller, for directory/version stripping          +
! +                                                                         +
! +-------------------------------------------------------------------------+

! ----------------------------------------------------------------------
! INITIALIZE
!
!     Initialisation routine to set up global constant values
!
!   Globals:
!	MANY!				See code
!
!   Inputs:
!	NONE
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Initialize;
LOCAL Ascii_Code;

 FAC_unpack          := "UNPACK";	! Facility name of unpack code

! Various Logging levels - activate if 'Logging' is at least this high
 LOG_ShowProgress    := 4;	! Log progress of compression - every 3%
 LOG_ShowNewpart     := 3;	! Log creation of each new part
 LOG_ShowStats       := 2;	! Log statistics of buffer compression
 LOG_ShowNumparts    := 1;	! Log number of parts written at end
 LOG_ShowSplit       := 1;	! Log records if preliminary splitting is active
 LOG_ShowFile        := 1;	! Log filename being processed

! Various DEBUG levels - activate if `Debugging' is at least this high
 DEBUG_PackTPU       := 4;	! Shows TPU unpacking code being generated
 DEBUG_ShowTrailing  := 4;	! SHows data on trailing blank handling
 DEBUG_FindBreak     := 3;	! Shows the search for a part breakpoint
 DEBUG_PartSplit     := 3;	! Shows sizes of buffers prior to splitting
 DEBUG_AddToPrologue := 2;	! Shows files added to share file prologue
 DEBUG_Resources     := 1;	! Shows marks/ranges deleted when buffer reused
 DEBUG_SIMess        := 1;	! Show both Success and Informational messages

! Various COMPRESS techniques
 COMPRESS_LZW_Complex:= 3;	! Lempel-Ziv-Welch Compression; full
 COMPRESS_LZW_Simple := 2;	! Lempel-Ziv-Welch Compression; within same line
 COMPRESS_RunLength  := 1;	! Run-length encoding
 COMPRESS_None       := 0;	! No special compression

! Define separator flags and associated variables, used to separate parts
 Part_Begin         := "-+-+-+-+-+-+-+-+";
 Part_End           := "+-+-+-+-+-+-+-+-";

! Define separator lines for user data
 FAO_Start_Part     := Part_Begin + " START OF PART !UL " + Part_Begin;
 FAO_End_Part       := Part_End   + "  END  OF PART !UL " + Part_End;

! Define separator lines for DCL portions of the code
 FAO_start_label    := "$PART!UL:";
 FAO_end_goto       := "$ GOTO PART!UL";	! Format strings for DCL part separators

! Constants
 MaxHexEnc          := 255;	! Max number encodeable in 2 hex digits
 EOL                := 3;	! Record length overhead in file, per record
 Max_Parts          := 999;
 FakeEOL_Flag       := "~";	! Used during lzw searches to cross lines
 Initial_Flag       := "X";
 Continuation_Flag  := "V";
 TPU_separators     := "[](),=;>*";

! Flags to mark encoded characters
 RL_Flag            := "&";	! Flag for run length encoding "&nnhh"
 QU_Flag            := "`";	! Flag for quoted chars        "`hh"
 LZ_Flag            := "\";	! Flag for LZW coded substrings"\bbll"

! Set up the ascii code string and the quotable characters string
 Quoteable_Chars    := "";
 Ascii_Code         := 0;
 LOOP
   EXITIF Ascii_Code > 255;
     IF (Ascii_Code < 32) OR (Ascii_Code >= 127) THEN
       Quoteable_Chars := Quoteable_Chars + ASCII(Ascii_Code)	! Ones to be escaped
     ENDIF;
     Ascii_Code := Ascii_Code + 1;
 ENDLOOP;


! Extra printable characters that get munged; treat them as non-printing
 Quoteable_Chars := Quoteable_Chars + "[]^{|}~" + QU_Flag + RL_Flag + LZ_Flag;

! Abbreviations to save time later
 QuotedBlank        := "`20";
 QuotedBlankLen     := LENGTH(QuotedBlank);
ENDPROCEDURE;

! +--------------------------------------------------------------------+
! +                                                                    +
! +          G E N E R A L   U T I L I T Y   R O U T I N E S           +
! +                                                                    +
! +   This section provides a number of miscellaneous simple routines  +
! +   to assist in the general packing code.                           +
! +                                                                    +
! +   Version 8.4           Andy Harper                  Jun 1993      +
! +                                                                    +
! +                                                                    +
! +                                                                    +
! +                  R O U T I N E   S U M M A R Y                     +
! +                                                                    +
! +   Inform      General messages and information a la VMS format     +
! +                                                                    +
! +   SizeBuffer  Routine to return the size, in bytes, of a buffer    +
! +               allowing for the end of line mark; to approximate    +
! +               how much disk space it will occupy when written.     +
! +                                                                    +
! +   MoveInfo    Routine to move data from one buffer to another      +
! +                                                                    +
! +   MvCount     Determine MOVE_HORIZONTAL value between two points   +
! +                                                                    +
! +   CopyLine    Like COPY_TEXT but adds a new line after it          +
! +                                                                    +
! +   ReuseBuffer Empty a specified buffer and position to it          +
! +                                                                    +
! +   StripFile   Parse the file name and remove unwanted bits         +
! +                                                                    +
! +--------------------------------------------------------------------+

! ----------------------------------------------------------------------
! INFORM
!
!   Issue a message in standard VMS syntax. The current time is added to the
!   text part of the message.
!
!   Globals:
!	Facility			The facility name of this utility
!
!   Inputs:
!	Xsev				The severity code for the message
!	Xident				The message identification code
!	Xtext				The text of the message
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	Message written to the user
!
! ----------------------------------------------------------------------

PROCEDURE Inform(Xsev,Xident,Xtext)
  MESSAGE( FAO("%!AS-!AS-!AS, !8%T, !AS", Facility, Xsev, Xident, 0, Xtext) )
ENDPROCEDURE;



! ----------------------------------------------------------------------
! SIZE BUFFER
!
!   Return the size, in bytes, of a buffer full of characters taking account
!   of the space occupied by the record header.  This gives an approximation
!   to the amount of disk space the buffer will occupy when it is written out
!   to disk
!
!   Globals:
!	EOL				No of Bytes to allow for end of record
!
!   Inputs:
!	Xbuffer				The buffer to be `sized'
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	The disk space, in blocks, occupied by the written buffer
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE SizeBuffer(Xbuffer)
LOCAL b,e,r,Size_Chars, Size_Lines;
   b := BEGINNING_OF(Xbuffer);
   e := END_OF(Xbuffer);
   r := CREATE_RANGE(b,e);
   Size_Chars := LENGTH(r);
   Size_Lines := GET_INFO(Xbuffer,"RECORD_COUNT");
   DELETE(r);
   DELETE(b);
   DELETE(e);
   RETURN( Size_Chars + EOL * Size_Lines );
ENDPROCEDURE;

! ----------------------------------------------------------------------
! MV COUNT
!
!   Given a range (start,end), this routine determines the number of positions
!   to advance to get from A to B using a MOVE_HORIZONTAL built-in. This 
!   is necessary because the LENGTH built-in ignores end of lines which
!   MOVE_HORIZONTAL does not.
!
!   Globals:
!	NONE
!
!   Inputs:
!	A					Start of range
!	B					End of range
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	Distance between the points
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE MVCOUNT(A,B)
LOCAL L,M,RecNum1,RecNum2;

 ! Remember where we are
   M:= MARK(NONE);

 ! Get key details of specified end points
   POSITION(A);
   RecNum1 := GET_INFO(CURRENT_BUFFER,"RECORD_NUMBER");
   POSITION(B);
   RecNum2 := GET_INFO(CURRENT_BUFFER,"RECORD_NUMBER");

 ! Work out distance between them; add 1 byte for each new line marker
   L := LENGTH( CREATE_RANGE(A,B) ) + (RecNum2-RecNum1);
   IF B <> END_OF(CURRENT_BUFFER) THEN L := L-1; ENDIF;

 ! Restore original position and return the length
   POSITION(M);
   DELETE(m);
   RETURN L;

ENDPROCEDURE




! ----------------------------------------------------------------------
! MOVE INFO
!
!   Move a buffer of information to the current point in another buffer
!
!   Globals:
!	NONE
!
!   Inputs:
!	FromBuf			The buffer or range to be moved
!	ToBuf			The position to which `fromBuf' is moved
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The buffer or range so moved is deleted from the original position
!
! ----------------------------------------------------------------------

PROCEDURE MoveInfo(FromBuf, ToBuf)
   POSITION(ToBuf);
   MOVE_TEXT(FromBuf);
ENDPROCEDURE;



! ----------------------------------------------------------------------
! COPY LINE
!
!   This routine works like COPY_TEXT but adds a new line at the end. Care
!   is taken when inserting the text not to exceed the maximum record
!   length, by creating a blank line first and then copying the text
!   into it.
!
!   Globals:
!	NONE
!
!   Inputs:
!	NewText				The new string to be added
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The current buffer is modified and the current position moved
!
! ----------------------------------------------------------------------

PROCEDURE CopyLine(NewText)
   SPLIT_LINE;
   MOVE_HORIZONTAL(-1);
   COPY_TEXT(NewText);
   MOVE_HORIZONTAL(1);
ENDPROCEDURE;

! ----------------------------------------------------------------------
! REUSE BUFFER
!
!   This routine re-initializes a particular buffer for use by emptying
!   it and then positioning to it.
!
!   As a safety precaution, to prevent excessive memory usage, all known markers
!   and ranges within the buffer are removed. Hopefully, they will have been
!   properly removed when used so this is a bit of defensive programming!
!
!   Globals:
!	NONE
!
!   Inputs:
!	Xbuffer				The buffer to be reset
!
!   Outputs:
!	Xbuffer                         The reset buffer (done in place)
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE ReuseBuffer(Xbuffer)
LOCAL m,mcount,r,rcount;

   ERASE(Xbuffer);
   POSITION(Xbuffer);

  ! Delete all markers in the buffer, to free the memory resources consumed
   mcount := 0;
   m := GET_INFO(Xbuffer,"FIRST_MARKER");  
   LOOP
     EXITIF m=0;
     DELETE(m);
     mcount :=mcount+1;
     m := GET_INFO(Xbuffer,"NEXT_MARKER");
   ENDLOOP;

  ! Delete all ranges in the buffer, to free the memory resources consumed
   rcount := 0;
   r := GET_INFO(Xbuffer,"FIRST_RANGE");  
   LOOP
     EXITIF r=0;
     DELETE(r);
     rcount :=rcount+1;
     r := GET_INFO(Xbuffer,"NEXT_RANGE");
   ENDLOOP;

  ! If debugging, then show what was deleted
   IF Debugging >= DEBUG_Resources
     THEN
       Inform("I","DBGDLRSC",FAO("ReUseBuffer: deleted resources- !SL mark!%S, !SL range!%S",mcount,rcount));
   ENDIF;
ENDPROCEDURE;



! ----------------------------------------------------------------------
! STRIP FILE
!
!   This routine implements the optional stripping of directory name and
!   version number from the name stored in the archive file. NOTE that
!   specifying the keyword DIRECTORY to FILE_PARSE has to be avoided as this
!   returns the FULL directory spec, not the relative spec that we want.
!
!   Globals:
!	DirKeep				Flag to specify [No] keep of directory
!	VersKeep			Flag to specify [No] keep of version
!
!   Inputs:
!	File				Filename to be converted
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	Converted file name
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE StripFile(File);

  CASE 2*DirKeep+Verskeep FROM 0 TO 3
    [0]: File := FILE_PARSE(File,"","",NAME,TYPE);		! /NODIR/NOVER
    [1]: File := FILE_PARSE(File,"","",NAME,TYPE,VERSION);	! /NODIR/VER
    [2]: File := File - FILE_PARSE(File,"","",VERSION);		! /DIR/NOVER
    [3]:;							! /DIR/VER
    [OUTRANGE]: Inform("F","BADDIRVER", FAO("StripFile: Bad strip value- DIR=!SL,VERS=!SL",DirKeep,VersKeep)); ABORT;
  ENDCASE;

  RETURN(File);
ENDPROCEDURE;

! +--------------------------------------------------------------------+
! +                                                                    +
! +           P A R T   S P L I T T I N G   R O U T I N E S            +
! +                                                                    +
! +   The share file is constrained to be of a maximum fixed size so   +
! +   that it can pass unscathed through mailers that truncate long    +
! +   messages.  These routines deal with splitting the share file     +
! +   into smaller manageable parts, each of which may be mailed       +
! +   without corruption.                                              +
! +                                                                    +
! +                                                                    +
! +   8.4                  Andy Harper                       Jun 1993  +
! +                                                                    +
! +                                                                    +
! +                                                                    +
! +                                                                    +
! +                   R O U T I N E   S U M M A R Y                    +
! +                                                                    +
! +                                                                    +
! +   WritePart   Routine to write a buffer to disk (to specified part)+
! +                                                                    +
! +   FlushPart   If the part buffer contains data, flush it to disk   +
! +               and update the Part Number.                          +
! +                                                                    +
! +   FindSplitPoint                                                   +
! +               Locate the line in the buffer which straddles a given+
! +               size in bytes.                                       +
! +                                                                    +
! +   Terminate_Part                                                   +
! +               Determine the best part separator flags to use; add  +
! +               them to the part buffer and remaining data, then     +
! +               empty the part buffer                                +
! +                                                                    +
! +   Add_To_Part Given a buffer full of data, add it to the current   +
! +               Part buffer, splitting where necessary               +
! +                                                                    +
! +   Add_To_Part_Nosplit                                              +
! +               Adds a buffer to the the current part but takes care +
! +               not to split it                                      +
! +                                                                    +
! +--------------------------------------------------------------------+
                                               
! ----------------------------------------------------------------------
! WRITE PART
!
!   Write the named buffer to disk as the given sharefile part
!
!   Globals:
!	Share				Flag to indicate [Dont] write sharefile
!	Share_File			Leading part of share file name
!	Logging				Current logging value
!	LOG_ShowNewPart			Min log level to display part name
!
!   Inputs:
!	Xbuffer				The buffer to be written out
!	Part				The part number of the part
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The specified buffer is cleared
!
! ----------------------------------------------------------------------

PROCEDURE WritePart(Xbuffer,Part)
LOCAL f,r,b;

! Trap any errors writing to a file (disk quota etc.)
   ON_ERROR
     Inform("E","FILWRERR", "Error writing part to file " + f);
     ABORT;
   ENDON_ERROR;

   f := Share_File+STR(Part);

! Log this part, if requested
   IF Logging >= Log_ShowNewpart
     THEN
       r := GET_INFO(Xbuffer, "RECORD_COUNT");
       b := SizeBuffer(Xbuffer);
       IF Share
         THEN
           Inform("I","WRITPART",FAO("Writing !AS, records !SL, bytes !SL",f,r,b));
         ELSE
           Inform("I","NOWRIPRT",FAO("!AS Not written, records !SL, bytes !SL",f,r,b));
       ENDIF;
   ENDIF;
       
! Write out the buffer
   IF Share
     THEN
       WRITE_FILE(Xbuffer,f);
   ENDIF;
   ReUseBuffer(Xbuffer);
ENDPROCEDURE;





! ----------------------------------------------------------------------
! FLUSH PART
!
!   If the part buffer is not empty, update the part number and flush its
!   contents to disk. Treat part 1 especially as we need to do some
!   final massaging of its contents, so save it in a separate buffer.
!
!   Globals:
!	PartBuffer			The part buffer to be flushed
!	Part1Buffer			Buffer holding the first part
!	PartNo				Number of the current part
!
!   Inputs:
!	NONE
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE FlushPart

  IF GET_INFO(PartBuffer,"RECORD_COUNT") > 0
     THEN
        PartNo := PartNo + 1;
        IF PartNo = 1
           THEN MoveInfo(PartBuffer, Part1Buffer);
           ELSE WritePart(PartBuffer, PartNo);
        ENDIF;
  ENDIF;

ENDPROCEDURE;

! ----------------------------------------------------------------------
! FIND SPLIT POINT
!
!    This routine locates the line in the buffer that ends just before
!    a particular buffer size is reached.
!
!    A simple linear positioning is used, as empirical evidence suggests
!    this is actually faster than a binary search due to the way things are
!    implemented in TPU.
!
!    Inputs:
!	Xbuffer		The name of the buffer to be examined
!	MaxSize		The maximum size in bytes to be returned
!
!    Outputs:
!	NONE
!
!    Function Return Value:
!	The range of lines that fit into the specified size, or 0 if there
!       are no lines that fit
!
!    Side Effects:
!       If no range is returned, the position is left at the start of the
!       buffer, otherwise, the position is on the last character of the
!       ramge (the null character at the end of the last record)
!
! ----------------------------------------------------------------------

PROCEDURE FindSplitPoint(Xbuffer,MaxSize)
LOCAL StartB,EndB,LineNo,TotalLines,BufLen;

   TotalLines := GET_INFO(Xbuffer,"RECORD_COUNT");
   StartB     := BEGINNING_OF(Xbuffer);
   EndB       := END_OF(Xbuffer);

 ! Position at approximately the right place in the buffer; make sure we don't
 ! go past the end of the buffer, to avoid error messages
   BufLen     := MVCOUNT(StartB,EndB);
   IF BufLen > MaxSize
     THEN
        POSITION(StartB);
        MOVE_HORIZONTAL(MaxSize);
        MOVE_HORIZONTAL(-CURRENT_OFFSET);	! Return to start of line
     ELSE
        POSITION(EndB);
   ENDIF;

 ! Say where we are ...
   LineNo := GET_INFO(Xbuffer,"RECORD_NUMBER")-1;
   IF Debugging >= DEBUG_FindBreak
     THEN
       Inform("I","DBGFDBRK",FAO("FindSplitPoint: At line !SL of !SL",LineNo,TotalLines));
   ENDIF;

 ! If there are no lines fitting the requirements, return 0 otherwise the range
   IF LineNo = 0 THEN RETURN (0); ENDIF;

   MOVE_HORIZONTAL(-1);
   RETURN ( CREATE_RANGE(StartB,MARK(NONE) ) );

ENDPROCEDURE;

! ----------------------------------------------------------------------
! TERMINATE PART
!
!    Inserts a suitable part separator into the part buffer and into the
!    current work buffer.
!
!    The type of part separator is determined by looking at the data in
!    the buffer immediately following the break point. If it starts with
!    '$' then we are splitting the buffer at the DCL code which is part
!    of the unpacking control thus we can use $GOTO and a label.  If not,
!    then we are in the midst of user data and should use the special flag
!    lines.
!
!    Global Variables:
!	PartBuffer			The current part buffer
!	FAO_end_goto			Format of a part terminator for DCL
!	FAO_start_label			Format of a part begin for DCl
!	FAO_end_part			Format of a part terminator for data
!	DAO_start_part			Format of a part begin for data
!
!    Inputs:
!	Part	The number of the part being terminated
!	Xbuffer	The buffer containing remaining the remaining data to be
!		processed.
!
!    Outputs:
!	NONE
!
!    Side Effects:
!	Both the part buffer and the Xbuffer are modified.
!
! ----------------------------------------------------------------------

PROCEDURE Terminate_Part(Part,Xbuffer)
LOCAL c,end_text;

   POSITION( BEGINNING_OF(Xbuffer) );
   IF (MARK(NONE) = END_OF(CURRENT_BUFFER))
      THEN
         c := "";			! No first character; use a dummy
      ELSE
         c := CURRENT_CHARACTER;	! First character
   ENDIF;

   IF (c = "$")
      THEN			! Executable DCL code insert GOTO/label pair
         end_text  := FAO(FAO_end_goto, Part+1);
         CopyLine( FAO(FAO_start_label, Part+1) );
      ELSE			! User data, insert recognizable separators
         end_text  := FAO(FAO_end_part, Part);
         CopyLine( FAO(FAO_start_part, Part+1) );
   ENDIF;        

   POSITION(END_OF(PartBuffer));
   COPY_TEXT(end_text);

   FlushPart;

ENDPROCEDURE;

! ----------------------------------------------------------------------
! ADD TO PART
!
!    Add a given work buffer to the current part of the sharefile, creating
!    one or more new parts if necessary as the maximum part size is exceeded.
!    Each time the part buffer fills up, it is flushed out.
!
!   Globals:
!	Max_Bytes				Max size of part buffer
!	PartNo					Current part no
!	Debugging				Current debug flag
!	DEBUG_PartSplit				Min debug value to show part split
!
!   Inputs:
!	Xbuffer					Buffer to be added
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	Size of the buffer (in disk blocks)
!
!   Side Effects:
!	Work Buffer and current Part Buffer are modified; data written to
!	disk.
!
! ----------------------------------------------------------------------

PROCEDURE Add_To_Part(Xbuffer)
LOCAL m, r, Size_Xbuffer, Size_PartBuffer;

  LOOP
    Size_Xbuffer    := SizeBuffer(Xbuffer);
    Size_PartBuffer := SizeBuffer(PartBuffer); 

    IF Debugging >= DEBUG_PartSplit
      THEN
       m:="Add_To_Part: Current: !SL, Adding: !SL, Maximum: !SL";
       Inform("I","DBGADPRT",FAO(m,Size_PartBuffer,Size_Xbuffer,Max_Bytes));
    ENDIF;

    EXITIF Size_PartBuffer + Size_Xbuffer  <=  Max_Bytes ;

    r := FindSplitPoint(Xbuffer,Max_Bytes - Size_PartBuffer);
    IF r <> 0
      THEN
        POSITION( END_OF(PartBuffer) );
        MoveInfo(r,PartBuffer);
        DELETE(r);
    ENDIF;

    Terminate_Part(PartNo+1, Xbuffer);
  ENDLOOP;

  MoveInfo(Xbuffer, PartBuffer);

ENDPROCEDURE;

! ----------------------------------------------------------------------
! ADD TO PART NOSPLIT
!
!    Adds an unsplittable buffer to the current part. Buffers are unsplittable
!    if:
!      1. They contain the TPU unpack code
!      2. They contain DCL code that has, or might have, continuation lines.
!
!   Globals:
!	Max_Bytes			Maximum size (in disk block bytes)
!	PartNo				Current Part number
!
!   Inputs:
!	Xbuffer				Buffer to be split
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The supplied buffer is modified and the part buffer is modified.
!
! ----------------------------------------------------------------------

PROCEDURE Add_To_Part_Nosplit(Xbuffer)
LOCAL s;

  s := SizeBuffer(Xbuffer);

! We can't deal with large unsplittable buffers bigger than the part size
  IF s > Max_Bytes
    THEN
      Inform("F","CANTSPLT","Unsplittable buffer bigger than part size");
      Inform("I","MINPTSIZ", FAO("Need minimum part size of !UL block!%S",(s+511)/512));
      ABORT;
  ENDIF;

! If this buffer would be split then flush the current part and start a new
! one
  IF SizeBuffer(PartBuffer)+s > Max_Bytes
    THEN
      Terminate_Part(PartNo+1, Xbuffer);
  ENDIF;

  Add_To_Part(Xbuffer);
ENDPROCEDURE;

! +--------------------------------------------------------------------+
! +                                                                    +
! +            T P U   C O D E   C O M P R E S S I O N                 +
! +                                                                    +
! +                                                                    +
! +    Part of the created share file contains some TPU code which     +
! +    is executed to perform the unpacking.  In order to save space   +
! +    we arrange to pack lines of TPU code into the smallest possible +
! +    space by compressing spaces and new lines wherever feasible but +
! +    keeping the total line length below the maximum for this share  +
! +    file.                                                           +
! +                                                                    +
! +    8.0                    Andy Harper               September 1992 +
! +                                                                    +
! +                                                                    +
! +                R O U T I N E   S U M M A R Y                       +
! +                                                                    +
! +    PackTPU     Takes a line of TPU code and adds it to the current +
! +                buffer. The current buffer is assumed to be the TPU +
! +                code that will form the unpacking part of the share +
! +                file. We remove as much redundant info (spaces etc.)+
! +                as possible to save space.                          +
! +                                                                    +
! +    AddDCL      Add additional DCL to an existing line, forcing a   +
! +                continuation line if necessary.                     +
! +                                                                    +
! +                                                                    +
! +--------------------------------------------------------------------+

! ---------------------------------------------------------------------
! PACK TPU
!
!   The generated share file contains a small program in TPU to perform
!   the unpacking. To save space, this program is created without
!   unnecessary spaces and newlines.
!
!   No line of the program is longer than the maximum share file length
!   and splits are only made after characters in the set `TPU_Separators'
!   (which clearly should NOT contain any characters likely to occur within
!   quotes!
!
!   Globals:
!	NONE
!
!   Inputs:
!	NewTPUCode			A line of TPU to be added
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The current buffer is modified
!
! ---------------------------------------------------------------------

PROCEDURE PackTPU(NewTPUCode)
LOCAL s,c;

! Append the new TPU code to the end of the current line, trimming blanks first
  s := NewTPUCode;
  EDIT(s,TRIM);
  COPY_TEXT(s);

! If it doesn't end in an acceptable separator, then add a space to delimit
! it from the next one added.
  c := SUBSTR(s,LENGTH(s),1);
  IF INDEX(TPU_separators,c) = 0 THEN COPY_TEXT(" "); ENDIF;


! If the current line is now too long, find a point to split it
  IF LENGTH(CURRENT_LINE) > Max_Share_Length
    THEN

    ! Find a TPU separator character on which to split
      LOOP
        MOVE_HORIZONTAL(-1);
        POSITION( SEARCH( ANY(TPU_separators),REVERSE) );
        EXITIF CURRENT_OFFSET < Max_Share_Length;
      ENDLOOP;
                        
    ! Split just past the TPU separator and reposition at the end of the line
      MOVE_HORIZONTAL(1);
      SPLIT_LINE;
      MOVE_HORIZONTAL( LENGTH(CURRENT_LINE) );
  ENDIF;


  IF Debugging >= DEBUG_PackTPU
    THEN
      Inform("I","DBGPKTPU","Adding """+s+""", To give: """ + CURRENT_LINE+"""")
  ENDIF;

ENDPROCEDURE;


! ----------------------------------------------------------------------
! ADD DCL
!
!   Add a parameter to an incomplete line of DCL, forcing a DCL continuation
!   if the resulting DCL line exceeds the maximum share file length.
!
!   This is used only for generating DCL lines that contain variable
!   such as filenames and attributes. Without this, some fussy mailers might
!   truncate these important lines and lose key data.
!
!   Globals:
!	Max_Share_Length			Max length of share file line
!
!   Inputs:
!	DCLParameter				DCL param to add to line
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	Current buffer is modified
!
! ----------------------------------------------------------------------

PROCEDURE AddDCL(DCLParameter)
  IF CURRENT_OFFSET + LENGTH(DCLParameter) >= Max_Share_Length
    THEN
      COPY_TEXT("-"); SPLIT_LINE; COPY_TEXT(" ");
  ENDIF;
  COPY_TEXT(DCLParameter);
ENDPROCEDURE;



! -----------------------------------------------------------------------------
! +                 B U F F E R    E N C O D I N G                            +
! +                                                                           +
! +   This module is responsible for encoding the file buffer in the most     +
! +   effective way commensurate with the user's stated options.              +
! +                                                                           +
! +   8.4                   Andy Harper                        July 1993      +
! +                                                                           +
! +                                                                           +
! +   ShowProgress            Display progress through file as a percentage   +
! +                                                                           +
! +   WriteBlanks             Write out trailing blanks                       +
! +                                                                           +
! +   WrapLong                Check when to wrap lines                        +
! +                                                                           +
! +   RightMost               Return rightmost part of a string               +
! +                                                                           +
! +   Quoteable               Check if a character is quoteable               +
! +                                                                           +
! +   PutN                    Put out N occurences of a single character      +
! +                                                                           +
! +   PutQuoted               Put out the quoted form of a character          +
! +                                                                           +
! +   PutCompressedRun        Put out the run-length encoded form of a run    +
! +                                                                           +
! +   PutCompressedString     Put out the LZ compressed form of a substring   +
! +                                                                           +
! +   PutRun                  Encode a run in the most efficient way          +
! +                                                                           +
! +   PutLine                 Encode/put a string, just quoting and wrapping  +
! +                                                                           +
! +   PutString               Encode/put a string, incl. run-length coding    +
! +                                                                           +
! +   PutLZ                   Encode/put a string, incl. LZ compression       +
! +                                                                           +
! +   PutLZSimple             Scan a line for potential LZ compression        +
! +                                                                           +
! +   EncodeBuffer            Encode buffer for transmission                  +
! +                                                                           +
! -----------------------------------------------------------------------------



! ----------------------------------------------------------------------
! SHOW PROGRESS
!
!   Issue an informational message giving the percentage of the current
!   buffer that has been processed so far. The logging level must be
!   sufficient for the display to be output.
!
!   Globals:
!	Logging				Current logging value
!	LOG_ShowProgress		Min log value to display progress stats
!	LastP				Last percentage at which message issued
!
!   Inputs:
!	NONE
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	Message written to user
!
! ----------------------------------------------------------------------

PROCEDURE ShowProgress
LOCAL m,b,e,x,y,p;

  IF Logging >= LOG_ShowProgress
    THEN
      m := MARK(NONE);
      b := BEGINNING_OF(CURRENT_BUFFER);
      e := END_OF(CURRENT_BUFFER);

    ! Get stats on the buffer length
      x := MVCOUNT(b,m);
      y := MVCOUNT(b,e);

    ! Get rid of the memory resources
      DELETE(m);
      DELETE(b);
      DELETE(e);

    ! Compute the statistics
      p := (100*x)/y;
      IF p >= Lastp + 3
        THEN
          Inform("I","ENCDPROG", FAO("Encoded: !3SL% (!SL of !SL)",p,x,y));
          LastP := p;
      ENDIF;
  ENDIF;

ENDPROCEDURE;




! -------------------------------------------------------------------------
! WRITE BLANKS
!
!   Output a string of blanks, with the last blank written in its quoted
!   form.
!
!   Globals:
!	QuotedBlank			The quoted form of a space character
!
!   Inputs:
!	N				The number of blanks to be written
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE WriteBlanks(n)

   IF N <= 0 THEN RETURN; ENDIF;

   IF N > 1
     THEN
       COPY_TEXT( " " * (n-1) );
   ENDIF;

   COPY_TEXT( QuotedBlank );

ENDPROCEDURE




! -------------------------------------------------------------------------
! WRAP LONG
!
!   Checks to see whether a string of a specific length will fit into the
!   current line.  If not, then the line is split and a continuation line
!   flag added.
!
!   Globals:
!	PendingBlanks		Number of blanks to insert before next char
!	CURRENT_OFFSET		The current position within the output line
!	Max_Share_Length	The maximum length of the output line
!	Initial_Flag		The marker for the start of a record
!	Continuation_Flag	The marker for a continuation record
!
!   Inputs:
!	N			The length of the string to be output
!
!   Outputs:
!	None
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The current line may be modified and the current position altered
!
! -------------------------------------------------------------------------

PROCEDURE WrapLong(n)
LOCAL B,Avail;

 ! Are we flushing lines out? If so, last byte must be quoted
  IF n = 0 THEN B := 2; ELSE B := n; ENDIF;

 ! Fill up as many complete lines as possible with blanks, always ensuring that
 ! the very last blank on a line is quoted
  LOOP
     Avail := Max_Share_Length - CURRENT_OFFSET;
     EXITIF Avail >= PendingBlanks + B;

    ! We have enough blanks to fill the current line and leave at least one over
     IF Debugging > DEBUG_ShowTrailing
       THEN
         Inform("I","DBGENDBLK", FAO("Avail: !UL, Pend: !UL, N: !UL",Avail,PendingBlanks,N));
         Inform("I","DBGENDBLK", "'"+CURRENT_LINE+"'");
     ENDIF;

    ! Work out the maximum number of blanks we can write to this line
     IF Avail > PendingBlanks		! Have we got more blanks than will fit
       THEN Avail := PendingBlanks;	! No, output all of them
       ELSE Avail := Avail-2;		! YES, output enough to fill the line
     ENDIF;

     IF Avail > 0			! Don't output ZERO blanks
       THEN
         WriteBlanks( Avail );
         PendingBlanks := PendingBlanks - Avail;
     ENDIF;
         
     SPLIT_LINE;
     COPY_TEXT( Continuation_Flag );

  ENDLOOP;

 ! The remaining blanks can be output as is, leaving enough space for the `n'
 ! bytes that follow.
  IF (n > 0)
    THEN COPY_TEXT( " "*PendingBlanks );! When something follows the blanks
    ELSE WriteBlanks( PendingBlanks );	! When nothing follows the blanks
  ENDIF;

  PendingBlanks := 0;

ENDPROCEDURE




! -------------------------------------------------------------------------
! RIGHTMOST
!
!   Returns the rightmost part of a string
!
!   Globals:
!	NONE
!
!   Inputs:
!	s					The string to be truncated
!	n					The last `n' chars required
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	The truncated string
!
!   Side Effects:
!	NONE
!
! -------------------------------------------------------------------------

PROCEDURE RightMost(s,n);

   IF LENGTH(s) > n
      THEN
         RETURN SUBSTR(s,LENGTH(s)-n+1,n);
      ELSE
         RETURN s;
   ENDIF;

ENDPROCEDURE


! -------------------------------------------------------------------------
! QUOTEABLE
!
!    Checks whether a character is quoteable
!
!    Globals:
!	Quoteable_Chars		Table of characters that must be quoted
!
!    Inputs:
!	c			The character to be checked
!
!    Outputs:
!	NONE
!
!    Function Return Value
!	TRUE if the character is a quoteable one, else FALSE
!
!    Side Effects:
!	None
!
! -------------------------------------------------------------------------

PROCEDURE Quoteable(c)
   IF INDEX(Quoteable_Chars,c) > 0
      THEN RETURN (1);
      ELSE RETURN (0);
   ENDIF;
ENDPROCEDURE




! -------------------------------------------------------------------------
! PUT N
!
!   Given a run of a particular character, output that run to the current
!   line. However, a sequence of blanks is held back until the next sequence
!   is output, to allow special checks to be made for trailing blanks.
!
!    Globals:
!	PendingBlanks		Number of blanks held back
!
!    Inputs:
!	c			The character making up the sequence
!	n			The length of the sequence
!
!    Outputs:
!	NONE
!
!    Function Return Value:
!	NONE
!
!    Side Effects:
!	The current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutN(c,n)
   IF c = " "
     THEN PendingBlanks := PendingBlanks + n;
     ELSE WrapLong(n); COPY_TEXT(c*n);
   ENDIF;
ENDPROCEDURE


! -------------------------------------------------------------------------
! PUT QUOTED
!
!    Given a particular byte, add the quoted form of that byte to the
!    current output line.
!
!    Globals:
!	QU_Flag				The byte used as the quote escape
!
!    Inputs:
!	c				The byte to be output
!
!    Outputs:
!	NONE
!
!    Function Return Value:
!	NONE
!
!    Side Effects:
!	The current output line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutQuoted(c)
LOCAL QuotedString;
   QuotedString := FAO("!AS!2XL", QU_Flag, ASCII(c));
   WrapLong( LENGTH(QuotedString) );
   COPY_TEXT( QuotedString );
ENDPROCEDURE



! -------------------------------------------------------------------------
! PUT COMPRESSED RUN
!
!    Given a run of a specific character, add the run-length compressed
!    equivalent to the current output line.
!
!    Note that, because the encoding scheme allows for a maximum run length
!    of 255, we break up the encoding into several sections, each of 255
!    bytes or less.
!
!    Globals:
!	RL_Flag			The run-length compression escape flag
!
!    Inputs:
!	c			The character making up the sequence
!	n			Number of occurrences (length)
!
!    Outputs:
!	NONE
!
!    Function Return Value:
!	NONE
!
!    Side Effects:
!	Output line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutCompressedRun(c,n)
LOCAL Size, CompressedString;

   LOOP

     IF n > 255 THEN Size := 255; ELSE Size := n; ENDIF;

     CompressedString := FAO("!AS!2XL!2XL", RL_Flag, Size, ASCII(c) );
     WrapLong( LENGTH(CompressedString) );
     COPY_TEXT( CompressedString );

     n : = n-size;
     EXITIF n<=0;

   ENDLOOP;

ENDPROCEDURE



! -------------------------------------------------------------------------
! PUT COMPRESSED STRING
!
!   Given a substring and a backwards offset, output the string in a compressed
!   format.
!
!   Globals:
!	LZ_Flag				The escape flag for the compressed string
!
!   Inputs:
!	s				The string to be written
!	b				backwards pointer to previous instance
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutCompressedString(s,b);
LOCAL LZString;

   LZString := FAO("!AS!2XL!2XL", LZ_Flag, b, LENGTH(s));
   WrapLong( LENGTH(LZString) );
   COPY_TEXT( LZString );

ENDPROCEDURE




! -------------------------------------------------------------------------
! PUT RUN
!
!    Given a sequence of the same character, work out the optimum way to
!    encode the sequence under the currently selected options and then
!    add that encoding to the current output line.
!
!    Encodings considered by this routine are:
!	- Encoding of quoteable characters
!	- Compression of long runs of characters
!
!    Globals:
!	NONE
!
!    Inputs:
!	c			The character forming the sequence
!	N			The length of the sequence
!
!    Outputs:
!	NONE
!
!    Function Return Value:
!	NONE
!
!    Side Effects:
!	The current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutRun(c,n)

   IF n = 0 THEN RETURN; ENDIF;

   IF Quoteable(c)
      THEN
        IF n <= 1	! Cheaper to quote than to compress run??
          THEN PutQuoted(c);
          ELSE PutCompressedRun(c,n);
        ENDIF;
      ELSE
        IF n <= 5	! Cheaper to output `as is' than to compress run??
          THEN PutN(c,n);
          ELSE PutCompressedRun(c,n);
        ENDIF;
   ENDIF;

ENDPROCEDURE



! -------------------------------------------------------------------------
! PUT LINE
!
!    Given a line of text, split it into single characters for simple
!    encoding into the buffer
!
!    Globals:
!	NONE
!
!    Inputs:
!	s				String to be encoded
!
!    Outputs:
!	NONE
!
!    Function Return Value:
!	NONE
!
!    Side Effects:
!	Current Line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutLine(s)
LOCAL L,OffSet,c;

  ! Initialize to scan the supplied string
     L        := LENGTH(s);	! Record its length
     OffSet   := 0;		! Prepare to step thru string L -> R

     LOOP

        OffSet := OffSet + 1;
        EXITIF OffSet > L;
        c := SUBSTR(s,OffSet,1);

        IF Quoteable(c)
          THEN PutQuoted(c);
          ELSE PutN(c,1);
        ENDIF;
 
     ENDLOOP;

ENDPROCEDURE

! -------------------------------------------------------------------------
! PUT STRING
!
!   Given a string, output it to the current line, parsing it for runs
!
!   Globals:
!	NONE
!
!   Inputs:
!	s				The string to be output
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	The current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutString(s)
LOCAL L, OffSet, Count, PrevChar, C;

  ! Initialize to scan the supplied string
     L        := LENGTH(s);	! Record its length
     OffSet   := 0;		! Prepare to step thru string L -> R
     Count    := 0;		! Length of current run
     PrevChar := "";		! Previous character

     LOOP

      ! Step to next character; exit if at end of string
        OffSet := OffSet + 1;
        EXITIF OffSet > L;
        c := SUBSTR(s,OffSet,1);

      ! Check character against previous one to locate a run
        IF c <> PrevChar
           THEN
              PutRun(PrevChar,Count);
              PrevChar := c ;
              Count    := 1 ;
           ELSE
              Count := Count + 1;
        ENDIF;

     ENDLOOP;

     PutRun(PrevChar,Count);	! Flush last sequence

ENDPROCEDURE



! -------------------------------------------------------------------------
! PUTLZ
!
!   Try to output a string in the Compressed Substring format; if not
!   possible, output it as a normal string (deal with trailing blanks
!   separately in this case).
!
!   Globals:
!	NONE
!
!   Inputs:
!	SubString			The string to be output
!	Prevtext			Previous text - might contain SubString
!	MatchPosition			The offset into PrevText of SubString
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	Current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutLZ(SubString, PrevText, MatchPosition)

   IF (MatchPosition = 0)  OR  (LENGTH(SubString) <= 5)
      THEN
         PutString(SubString);
      ELSE
         PutCompressedString(SubString,LENGTH(PrevText)-MatchPosition+1);
   ENDIF;

ENDPROCEDURE



! -------------------------------------------------------------------------
! PUT LZ SIMPLE
!
!   Scan the input string, looking for common substrings. Output unmatched
!   strings normally; output matched strings as a coded pointer back to the
!   previous occurence.
!
!   This version only looks for substrings occurring in the same string!!
!
!   Globals:
!	NONE
!
!   Inputs:
!	s				String to be encoded
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	Current line is modified
!
! -------------------------------------------------------------------------

PROCEDURE PutLZSimple(s)
LOCAL L, OffSet, PrevChar, c,  PrevText, SubString, MatchPosition, PrevMatchPosition;

   ! Initialize for common substring search
     PrevText := "";
     SubString:= "";
     PrevMatchPosition := 0;

   ! Initialize to process current record
     L        := LENGTH(S);	! ... and record its length
     OffSet   := 0;		! Prepare to step thru line L -> R
     PrevChar := "";		! Previous character

     LOOP

      ! Step to next character; exit if at end of line
        OffSet := OffSet + 1;
        EXITIF OffSet > L;
        c := SUBSTR(S,OffSet,1);

      ! Examine the current substring to see if it has previously occurred; if
      ! not, then output it normally otherwise remember it and try to find a 
      ! longer substring
        MatchPosition := INDEX(PrevText,SubString+c);
        IF MatchPosition > 0
          THEN
             SubString := SubString + c;
             PrevMatchPosition := MatchPosition;
          ELSE
             PutLZ(SubString,PrevText,PrevMatchPosition);
             PrevText := RightMost(PrevText+SubString,255);
             SubString := c;
             PrevMatchPosition := 0;
        ENDIF;

     ENDLOOP;

     PutLZ(SubString,PrevText,PrevMatchPosition);

ENDPROCEDURE

! -----------------------------------------------------------------------------
! ENCODE BUFFER
!
!   This routine is the interface to the buffer encoding methods.
!   It selects the compression technique to be used based on the
!   user specified `compression' value. If selected, it will also
!   issue statistics after compression to indicate the effectiveness
!   of any selected compression method.
!
!   In this version, the current recrd is removed to a string buffer first.
!   This helps to overcome a problem in TPU that fails to reclaim memory when
!   a line is split until the line is erase. This leads to very large virtual
!   memory requirements. By removing the line first, we circumvent the problem
!   and keep the memory usage as low as possible.
!
!   Globals:
!	Logging				Current log level
!	LOG_ShowStats			Min log level to display buffer statistics
!	Compression			Current compression type
!	COMPRESS_None			No compression requested
!	COMPRESS_RunLength		Run length encoding requested
!	COMPRESS_LZW_Simple		Common substring encoding requested
!
!   Inputs:
!	Xbuffer				Buffer to be compressed
!
!   Outputs:
!	Xbuffer				Compressed buffer (done in place)
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	Messages written to the user
!
! -----------------------------------------------------------------------------

PROCEDURE Encode_Buffer(XBuffer)
LOCAL s,f,l1,l2,b1,b2,c;

! Unexpected errors, such as insufficient memory, can occur during the
! processing of a buffer. Here we trap them and abort
  ON_ERROR
    Inform("F","FATALERR","Unexpected error encoding the data - aborted");
    ABORT;
  ENDON_ERROR;

! Collect initial sizing statistics
  l1:=GET_INFO(Xbuffer,"RECORD_COUNT");
  b1:=MVCOUNT(BEGINNING_OF(Xbuffer),END_OF(Xbuffer));

! Show size if suitable log level enabled
  IF Logging >= LOG_ShowStats
    THEN
      f:="Records: !UL, Bytes: !UL";
      Inform("I","ORIGSIZE",FAO(f,l1,b1));
  ENDIF;

! Encode buffer, using the user's requested compression
! Always quote non-printable chars and wrap lines regardless
  POSITION( BEGINNING_OF(XBuffer) );
  LastP :=0;
  LOOP
     EXITIF MARK(NONE) = END_OF(CURRENT_BUFFER);

     s := ERASE_LINE;		! Remove current record to string buffer
     PendingBlanks :=0;		! Blank characters waiting to be output

     SPLIT_LINE;
     MOVE_HORIZONTAL(-1);
     COPY_TEXT(Initial_Flag);

     IF Compression = COMPRESS_None       THEN PutLine(s);     ENDIF;
     IF Compression = COMPRESS_Runlength  THEN PutString(s);   ENDIF;
     IF Compression = COMPRESS_LZW_Simple THEN PutLZSimple(s); ENDIF;

     WrapLong(0);		! Force out any stored trailing blanks

   ! Move on to the next record
     MOVE_HORIZONTAL(1);
     ShowProgress;

  ENDLOOP;


! Compute final size stats and compression ratio
  l2:=GET_INFO(Xbuffer,"RECORD_COUNT");
  b2:=MVCOUNT(BEGINNING_OF(Xbuffer),END_OF(Xbuffer));
  IF b1 = 0
    THEN c := 0
    ELSE c := (1000*(b2-b1)+5)/(10*b1);	! Compute % change in size
  ENDIF;


! Concise summary of encoding/compression if logging requested
  IF Logging >= LOG_ShowStats
    THEN
      f:="Records: !UL, Bytes: !UL, Change !SL%";
      Inform("I","ENCDSIZE",FAO(f,l2,b2,c));
  ENDIF;

ENDPROCEDURE;

! +--------------------------------------------------------------------+
! +                                                                    +
! +     S H A R E   F I L E   H E A D E R S   A N D   T R A I L E R S  +
! +                                                                    +
! +                                                                    +
! +   The share file which is created consists of a mixture of DCL, TPU+
! +   and user data.  The DCL and TPU code surrounding each file of    +
! +   user data create a sequence which, when executed, unpacks the    +
! +   encoded user data to its original format.                        +
! +                                                                    +
! +                                                                    +
! +   8.4              Andy Harper                         Jun 1993    +
! +                                                                    +
! +                                                                    +
! +                                                                    +
! +               R O U T I N E   S U M M A R Y                        +
! +                                                                    +
! +                                                                    +
! +   Create_Prologue_Head       Create the share file header plus the +
! +                              start of the UNPACK subroutine        +
! +                                                                    +
! +   Create_Prologue_Unpacker   Create the TPU unpack code            +
! +                                                                    +
! +   Create_Prologue_Trail      Create the remainder of the unpack    +
! +                              subroutine                            +
! +                                                                    +
! +   Create_Epilogue            Create the share file completion code +
! +                                                                    +
! +--------------------------------------------------------------------+

! ----------------------------------------------------------------------
! CREATE PROLOGUE HEAD
!
!   The share file contains a prolog consisting of a chunk of DCL code
!   and some TPU code which, together, form the unpacking and decoding
!   routine for a given file.
!
!   This routine writes in the share file identification, version checks
!   and various initialization tasks, followed by the DCL code forming
!   the start of unpacking routine.
!
!   Globals:
!	ArchiveName			Name of archive (null = no name)
!	Debugging			Current debugging value
!	DEBUG_AddToPrologue		Min debug value to display prologue detail
!	VersKeep			Flag to [Not] keep version numbers
!
!   Inputs:
!	Xbuffer				The buffer in which to write prologue
!	Creator				Identification of the share file creator
!	Blocks				Max part size, in blocks
!	FileNameBuffer			Buffer containing names of files to be packaged
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_Prologue_Head(Xbuffer, Creator, Blocks, FilenameBuffer)
LOCAL m,file;

  ReuseBuffer(Xbuffer);

  CopyLine( "$! ------------------ CUT HERE -----------------------");
  CopyLine( "$ v='f$verify(f$trnlnm(""SHARE_UNPACK_VERIFY""))'");
  CopyLine( "$!" );
  CopyLine( "$! This archive created:");
  IF ArchiveName <> "" THEN
  CopyLine( FAO("$!!  Name : !AS", ArchiveName));
  ENDIF;
  CopyLine( FAO("$!!  By   : !AS", Creator));
  CopyLine( FAO("$!!  Date : !%D", 0) );
  CopyLine( FAO("$!!  Using: !AS !AS, (C) 1993 Andy Harper, Kings College London UK", Facility, Version_Number));
  CopyLine( "$!" );
  CopyLine( "$! Credit is due to these people for their original ideas:");
  CopyLine( "$!    James Gray, Michael Bednarek ");
  CopyLine( "$!");
  CopyLine( "$!+ THIS PACKAGE DISTRIBUTED IN 999 PARTS, TO KEEP EACH PART");
  CopyLine( "$!  BELOW " + STR(Blocks) + " BLOCKS");
  CopyLine( "$!");
  CopyLine( "$! TO UNPACK THIS SHARE FILE, CONCATENATE ALL PARTS IN ORDER");
  CopyLine( "$! AND EXECUTE AS A COMMAND PROCEDURE  (  @name  )");
  CopyLine( "$!");
  CopyLine( "$! THE FOLLOWING FILE(S) WILL BE CREATED AFTER UNPACKING:");

  FileCount := 0;
  LOOP
     POSITION(FilenameBuffer);
     m := MARK(NONE);
     EXITIF m = END_OF(CURRENT_BUFFER);
     DELETE(m);

   ! extract next filename from list
     file  := StripFile( SUBSTR(CURRENT_LINE,1,INDEX(CURRENT_LINE," ")-1) );

     FileCount := FileCount + 1;
     MOVE_VERTICAL(1);

   ! Copy filename into the share file prologue
     POSITION(Xbuffer);
     CopyLine( FAO("$!!!8UL. !AS", FileCount, file));

     IF Debugging >= DEBUG_AddToPrologue
        THEN
          Inform("I","DBGADFIL","Create_Prologue_Head: Adding file """ + file +"""");
     ENDIF;

  ENDLOOP;

  POSITION(Xbuffer);
  CopyLine( "$!");
  CopyLine( "$ set=""set""");
  CopyLine( "$ set symbol/scope=(nolocal,noglobal)");
  CopyLine( "$ f=f$parse(""SHARE_UNPACK_TEMP"",""SYS$SCRATCH:.""+f$getjpi("""",""PID""))");
  CopyLine( "$ e=""write sys$error  """"%"+FAC_unpack+""""", """);
  CopyLine( "$ w=""write sys$output """"%"+FAC_unpack+""""", """);
  CopyLine( "$ if .not. f$trnlnm(""SHARE_UNPACK_LOG"") then $ w = ""!""");
  CopyLine( "$ if f$getsyi(""CPU"") .gt. 127 then $ goto start");
  CopyLine( "$ ve=f$getsyi(""version"")");
  CopyLine( "$ if ve-f$extract(0,1,ve) .ges. """+Min_VMS+""" then $ goto start");
  CopyLine( "$ e ""-E-OLDVER, Must run at least VMS "+Min_VMS+"""");
  CopyLine( "$ v=f$verify(v)");
  CopyLine( "$ exit 44");

! Insert the DCL procedure which unpacks a specific file, restores the
! record attributes, and validates the checksum
  CopyLine( "$unpack: subroutine ! P1=filename, P2=checksum, P3=attributes,P4=size");

! Insert code to check directory exists and create if not
  CopyLine( "$ if f$parse(P1) .nes. """" then $ goto dirok");
  CopyLine( "$ dn=f$parse(P1,,,""DIRECTORY"")");
  CopyLine( "$ w ""-I-CREDIR, Creating directory ''dn'""");
  CopyLine( "$ create/dir 'dn'");
  CopyLine( "$ if $status then $ goto dirok");
  CopyLine( "$ e ""-E-CREDIRFAIL, Unable to create ''dn' File skipped""");
  CopyLine( "$ delete 'f'*");
  CopyLine( "$ exit");

! Directory now in place
  CopyLine( "$dirok:");

! Check file existence
  CopyLine( "$ x=f$search(P1)");
  CopyLine( "$ if x .eqs. """" then $ goto file_absent");

! If we're not preserving versions, then we can't bomb out if an old version
! already exists when we unpack; so alter the check dependent on the setting
! of the `preserve versions' flag.
  IF VersKeep
    THEN
      CopyLine( "$ e ""-W-EXISTS, File ''P1' exists. Skipped""");
      CopyLine( "$ delete 'f'*");
      CopyLine( "$ exit");
    ELSE
      CopyLine("$ e ""-W-HIGHVERS, Creating higher version of "", P1");
  ENDIF;

  CopyLine( "$file_absent:");
  CopyLine( "$ w ""-I-UNPACK, Unpacking "", P5, "" of "", P6, "" - "", P1, "" - "", P4, "" Blocks""");
  CopyLine( "$ n=P1");
  CopyLine( "$ if P3 .nes. """" then $ n=f");
  COPY_TEXT( "$ if .not. f$verify() then $ define/user sys$output nl:");

ENDPROCEDURE;

! ----------------------------------------------------------------------
! CREATE PROLOGUE UNPACKER
!
!   This routine creates the packed TPU code which makes up the heart of
!   the decoding subroutine in the share file.  To save space, the TPU
!   code is tightly packed in the share file - see packTPU for details
!
!   Globals:
!	Compression			Current compression value
!	COMPRESS_RunLength		Min compression to run compress
!	COMPRESS_LZW_Simple		Min compression to LZW encode
!	SpaceEnc			Flag to indicate [No] space encoding
!
!   Inputs:
!	NONE
!
!   Outputs:
!	Xbuffer				Buffer in which to write unpack code
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_Prologue_Unpacker(Xbuffer)

  ReuseBuffer(Xbuffer);

  CopyLine("$ EDIT/TPU/NOSEC/NODIS/COM=SYS$INPUT/NOJOURNAL 'f'/OUT='n'");

! Insert procedure to extract the next two digits as a hex character
  PackTPU( "PROCEDURE GetHex(s,p)");
  PackTPU( "LOCAL x1,x2;");
  PackTPU( "  x1:=INDEX(t,SUBSTR(s,p,1))-1;");
  PackTPU( "  x2:=INDEX(t,SUBSTR(s,p+1,1))-1;");
  PackTPU( "  RETURN 16*x1+x2;");
  PackTPU( "ENDPROCEDURE;");

! Insert procedure to skip part separators
  PackTPU( "PROCEDURE SkipPartsep");
  PackTPU( "LOCAL m;" );
  PackTPU( "  LOOP");
  PackTPU( "    m:=MARK(NONE);" );
  PackTPU( "    EXITIF m=END_OF(b);" );
  PackTPU( "    DELETE(m);" );
  PackTPU( "    EXITIF INDEX(ERASE_LINE,"""+Part_Begin+""")=1;");
  PackTPU( "  ENDLOOP;");
  PackTPU( "ENDPROCEDURE;");

! Insert procedure to expand complex LZW encoded substrings (that cross lines)
!  IF Compression = COMPRESS_LZW_Complex THEN
!  PackTPU( "PROCEDURE COPY_PREVIOUS(b,n)");
!  PackTPU( "LOCAL m,s,e,r;");
!  PackTPU( "  m:=MARK(NONE);MOVE_HORIZONTAL(-b);");
!  PackTPU( "  s:=MARK(NONE);MOVE_HORIZONTAL(n-1);");
!  PackTPU( "  e:=MARK(NONE);POSITION(m);r:=CREATE_RANGE(s,e);COPY_TEXT(r);");
!  PackTPU( "  DELETE(m);DELETE(s);DELETE(e);DELETE(r);" );
!  PackTPU( "ENDPROCEDURE;");
!  ENDIF;

! Insert procedure to decode the current line back to useful data
  PackTPU( "PROCEDURE ProcessLine");
  PackTPU( "LOCAL c,s,l,b,n,p;");
  PackTPU( "  c := ERASE_CHARACTER(1);");
  PackTPU( "  s := ERASE_LINE;");
  IF (NOT SpaceEnc) THEN
  PackTPU( "  EDIT(s,""TRIM"");");
  ENDIF;
  PackTPU( "  IF c = """+Initial_Flag+""" THEN SPLIT_LINE; ENDIF;");
  PackTPU( "  MOVE_HORIZONTAL(-1);");
  PackTPU( "  l := LENGTH(s);");
  PackTPU( "  p := 1;");
  PackTPU( "  LOOP");
  PackTPU( "    EXITIF p > l;");
  PackTPU( "    c := SUBSTR(s,p,1);");
  PackTPU( "    p := p+1;");
  PackTPU( "    CASE c FROM ' ' TO '`'");

  IF Compression = COMPRESS_LZW_Simple THEN
  PackTPU( "      ['"+LZ_Flag+"']: b:=GetHex(s,p); n:=GetHex(s,p+2); p:=p+4; COPY_TEXT( SUBSTR(CURRENT_LINE,CURRENT_OFFSET-b+1,n));");
  ENDIF;

  IF Compression >= COMPRESS_RunLength THEN
  PackTPU( "      ['"+RL_Flag+"']: b:=GetHex(s,p); n:=GetHex(s,p+2); p:=p+4; COPY_TEXT(ASCII(n)*b);");
  ENDIF;

  PackTPU( "      ['"+QU_Flag+"']: COPY_TEXT(ASCII(GetHex(s,p))); p:=p+2;");

  IF SpaceEnc THEN
  PackTPU( "      [' ']: p:=p+1;");
  ENDIF;

  PackTPU( "      [INRANGE,OUTRANGE]: COPY_TEXT(c);");
  PackTPU( "    ENDCASE;");
  PackTPU( "  ENDLOOP;");
  PackTPU( "ENDPROCEDURE;");

! Insert procedure to scan and decode the whole buffer
  PackTPU( "PROCEDURE Decode" );
  PackTPU( "LOCAL m;" );
  PackTPU( "  POSITION(BEGINNING_OF(b));" );
  PackTPU( "  LOOP" );
  PackTPU( "    m:=MARK(NONE);" );
  PackTPU( "    EXITIF m=END_OF(b);" );
  PackTPU( "    DELETE(m);" );
  PackTPU( "    IF INDEX(CURRENT_LINE,"""+Part_End+""")=1 THEN");
  PackTPU( "      SkipPartSep;");
  PackTPU( "    ELSE");
  PackTPU( "      ProcessLine;");
  PackTPU( "      MOVE_HORIZONTAL(1);");
  PackTPU( "    ENDIF;");
  PackTPU( "  ENDLOOP;" );
  PackTPU( "ENDPROCEDURE;" );

! Start of main program code here
  PackTPU( "SET(FACILITY_NAME,"""+FAC_unpack+""");" );
  PackTPU( "SET(SUCCESS,OFF);" );
  PackTPU( "SET(INFORMATIONAL,OFF);" );
  PackTPU( "t:=""0123456789ABCDEF"";" );	! For decoding hex (see gethex)
  PackTPU( "f:=GET_INFO(COMMAND_LINE,""file_name"");" );
  PackTPU( "b:=CREATE_BUFFER(f,f);" );
  
! Call the decoding routine
  PackTPU( "Decode;");				! Procedure call
  PackTPU( "WRITE_FILE(b,GET_INFO(COMMAND_LINE,""output_file""));" );
  PackTPU( "QUIT;" );

ENDPROCEDURE;

! ----------------------------------------------------------------------
! CREATE PROLOGUE TRAIL
!
!   Write the share file prologue code containing the subroutines to
!   restore the file format and analyze the file checksum.
!
!   Globals:
!	NONE
!
!   Inputs:
!	NONE
!
!   Outputs:
!	Xbuffer					Buffer in which to write code
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_Prologue_Trail(Xbuffer)

  ReuseBuffer(Xbuffer);

! Insert code to Restore the file's record attributes
  CopyLine( "$ if p3 .eqs. """" then $ goto dl");
  CopyLine( "$ open/write fdl &f");
  CopyLine( "$ write fdl ""RECORD""");
  CopyLine( "$ write fdl P3");
  CopyLine( "$ close fdl");
  CopyLine( "$ w ""-I-CONVRFM, Converting record format to "", P3");
  CopyLine( "$ convert/fdl=&f &f-1 &P1");
  CopyLine( "$dl: delete 'f'*");

! Insert code to do a checksum and validate the created file; then tidy up
  CopyLine( "$ checksum 'P1'");
  CopyLine( "$ if checksum$checksum .nes. P2 then $ -");
  CopyLine( "  e ""-E-CHKSMFAIL, Checksum of ''P1' failed.""");
  CopyLine( "$ exit");
  CopyLine( "$ endsubroutine");
  COPY_TEXT( "$start:");

ENDPROCEDURE;




! ----------------------------------------------------------------------
! CREATE EPILOGUE
!
!   Writes out the final part of the share file.
!
!   Globals:
!	NONE
!
!   Inputs:
!	NONE
!
!   Outputs:
!	Xbuffer					Buffer in which to write stuff
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_Epilogue(Xbuffer);

  ReuseBuffer(Xbuffer);
  CopyLine("$ v=f$verify(v)");
  COPY_TEXT("$ exit");

ENDPROCEDURE;


! +--------------------------------------------------------------------+
! +                                                                    +
! +            F I L E   P A C K A G I N G   R O U T I N E S           +
! +                                                                    +
! +   Each file taken from the parameter list is read into a buffer and+
! +   encoded. The encoded buffer, together with a file header and     +
! +   trailer, is then added to the existing part buffer.              +
! +                                                                    +
! +                                                                    +
! +   8.4                Andy Harper                         Jun 1993  +
! +                                                                    +
! +                                                                    +
! +                                                                    +
! +                 R O U T I N E   S U M M A R Y                      +
! +                                                                    +
! +                                                                    +
! +   Create_File_Header     Generate the DCL prolog for the file      +
! +                          which copies the data to a scratch file   +
! +                                                                    +
! +   Create_File_Trailer    Generates the DCL epilog for the file     +
! +                          which generates the call to the unpacker  +
! +                                                                    +
! +                                                                    +
! +   Create_File            Gets the file into a buffer and encodes   +
! +                          the data.                                 +
! +                                                                    +
! +   PackageFile            Master routine called to deal with a      +
! +                          single file                               +
! +                                                                    +
! +--------------------------------------------------------------------+

! ----------------------------------------------------------------------
! CREATE FILE HEADER
!
!   Create the initial DCL code that copies the encoded data from the share
!   file into a temporary file.
!
!   Globals:
!	NONE
!
!   Inputs:
!
!   Outputs:
!	Xbuffer					Buffer in which to write stuff
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_File_Header(Xbuffer)

  ReuseBuffer(Xbuffer);
  CopyLine("$!");
  COPY_TEXT("$ create 'f'");

ENDPROCEDURE;


! ----------------------------------------------------------------------
! CREATE FILE
!
!   Get the named file into the named buffer; give up on any failure to
!   get the file.
!
!   Globals:
!	NONE
!
!   Inputs:
!	Xbuffer				Buffer in which to place file contents
!	Filename			Name of file to be read
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	Size of the file (in an approximation to disk blocks)
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_File(Xbuffer, Filename)

  ON_ERROR
    Inform("E","FILRDERR", FAO("Error reading !AS", FileName));
    ABORT;
  ENDON_ERROR;

  ReuseBuffer(Xbuffer);
  READ_FILE(FileName);

  RETURN (MVCount(BEGINNING_OF(XBUFFER),END_OF(XBUFFER))+511)/512;

ENDPROCEDURE;

! ----------------------------------------------------------------------
! CREATE FILE TRAILER
!
!   Add the DCL trailer line to the file. If the resultant parameters would
!   make the line longer than the maximum share file line length, then use
!   normal DCL continuation flags to indicate this and continue onto several
!   lines
!
!   Globals:
!	NONE
!
!   Inputs:
!	Xbuffer					Buffer in which to write trailer
!	File					Name of file in buffer
!	FileAttr				Attributes of file
!	FileSize				Size of file (in blocks)
!	FileNum					Current file number
!	FileCnt					Total number of files
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE Create_File_Trailer(Xbuffer, File, FileAttr, FileSize, FileNum, FileCnt)

 ! Strip parts of the file name if this has been requested
  File := StripFile(File);

  ReuseBuffer(Xbuffer);
  COPY_TEXT("$ call unpack ");
  AddDCL( File+" ");
  AddDCL( FileAttr+" ");
  AddDCL( FAO("!UL ", FileSize) );
  AddDCL( FAO("!UL ", FileNum ) );
  AddDCL( FAO("!UL ", FileCnt ) );

ENDPROCEDURE;

! ----------------------------------------------------------------------
! PACKAGE FILE
!
!   Input the contents of the specified file, encode the buffer and add
!   it to the share file surrounded by the appropriate header and trailer
!   unpacking code.
!
!   If the /SPLIT[=nnn] parameter has been given, then the named file
!   is read multiple times, each time processing only as much as will
!   fit into roughly the amount of space specified by nnn (blocks).
!   This helps to keep the amount of virtual memory usage small and reduces
!   paging dramatically, thus speeding up the packaging process for large
!   files. Of course, that large file is read multiple times but this is
!   much faster.
!
!   Globals:
!	WorkBuffer				Temp buffer used for packaging
!	Split					Max size of part being processed
!	Logging					Current log level
!	LOG_ShowFile				Min log level to display file log
!
!   Inputs:
!	FileDetails				Details of file to be packaged
!
!   Outputs:
!	NONE
!
!   Function Return Value:
!	NONE
!
!   Side Effects:
!	NONE
!
! ----------------------------------------------------------------------

PROCEDURE PackageFile(FileDetails)
LOCAL Separator,FileAttr,FileName,MaxVirtM,FilePart,StartLine,FileSize,
StartBuff,EndBuff,MaxLine,LineCount,SplitRange;

   Separator := INDEX(FileDetails,' ');
   FileName  := SUBSTR(FileDetails,1,separator-1);
   FileAttr  := SUBSTR(FileDetails,separator+1,LENGTH(FileDetails)-separator);

   Create_File_Header(WorkBuffer);
   Add_To_Part(WorkBuffer);

   IF Logging >= LOG_ShowFile
     THEN
       Inform("I","PACKFILE",FAO("Packaging file !AS",FileName));
   ENDIF;

 ! If preliminary file splitting has been requested, then let's adopt a
 ! different algorithm.
   IF Split > 0
     THEN
       FilePart  := 0;
       StartLine := 0;
       MaxVirtM  := Split * 512;
       LOOP

       ! Read the file back in; quit if we've now done all the lines
         FileSize := Create_File(WorkBuffer,FileName);
         MaxLine := GET_INFO(WorkBuffer,"RECORD_COUNT")-1;
         EXITIF StartLine > MaxLine;
         FilePart := FilePart + 1;
         StartBuff := BEGINNING_OF(WorkBuffer);
         EndBuff   := END_OF(WorkBuffer);

       ! Move to the start of the first unprocessed line and delete everything
       ! up to but not including this line...
         POSITION(StartBuff);
         MOVE_VERTICAL(StartLine);         
         IF StartLine > 0
           THEN
             MOVE_HORIZONTAL(-1);
             ERASE( CREATE_RANGE(StartBuff,MARK(NONE)) );
         ENDIF;

       ! Find, and position at, the end of the last line in the range that fits
         SplitRange := FindSplitPoint(WorkBuffer,MaxVirtM);

       ! We cannot be at EOH here, so a failure to find a split point can only
       ! mean that the first record is larger than available free space.
       ! However, we need to process at least 1 record so advance if necessary.
         IF SplitRange = 0
           THEN				! No lines fit, so let's advance 1 line at least
             MOVE_VERTICAL(1);
           ELSE
             MOVE_HORIZONTAL(1);	! Move to start of next line
         ENDIF;

         LineCount := GET_INFO(CURRENT_BUFFER,"RECORD_NUMBER")-1;

       ! Delete from here to end as we can't process remaining lines effectively
          ERASE( CREATE_RANGE(MARK(NONE),EndBuff) );

       ! Tell user which lines we are doing and then go for it ....
       ! Say nothing if this is the only part!
         IF Logging >= LOG_ShowSplit
           THEN
	     IF (StartLine > 0) OR (LineCount <= MaxLine)
               THEN
                 Inform("I","SPLITFIL", FAO("Processing Part !SL, records !SL-!SL of !SL",FilePart,StartLine+1,StartLine+LineCount,MaxLine+1));
             ENDIF;
         ENDIF;

         Encode_Buffer(WorkBuffer);
         Add_To_Part(WorkBuffer);

       ! Remember where we start the next time around;
         StartLine := StartLine + LineCount;
         EXITIF StartLine > MaxLine;
       ENDLOOP;         

     ELSE
       FileSize := Create_File(WorkBuffer,FileName);
       Encode_Buffer(WorkBuffer);
       Add_To_Part(WorkBuffer);
   ENDIF;

  
   Create_File_Trailer(WorkBuffer,FileName,FileAttr,FileSize,FileNumber,FileCount);
   Add_To_Part_Nosplit(WorkBuffer);

ENDPROCEDURE;

! +--------------------------------------------------------------------+
! +   MAIN PROGRAM                                                     +
! +                                                                    +
! +   This is the main control loop of the program, responsible for    +
! +   picking up the parameters, creating the prologue and epilogue of +
! +   the share file, setting up global constants and variables etc.   +
! +                                                                    +
! +   It also contains the main loop which goes around each of the     +
! +   specified files to be packed into the share file.                +
! +                                                                    +
! +--------------------------------------------------------------------+

Initialize;

SET(SUCCESS,OFF);		! Suppress non-error messages
SET(INFORMATIONAL,OFF);

ParamFile   := GET_INFO(COMMAND_LINE,"FILE_NAME");
ParamBuff   := CREATE_BUFFER("{info}", ParamFile);
PartBuffer  := CREATE_BUFFER("{part}");
Part1Buffer := CREATE_BUFFER("{part_1}");
WorkBuffer  := CREATE_BUFFER("{work}");


! Pick up parameters from the outside world
POSITION(ParamBuff);
Facility            := ERASE_LINE;
Version_Number      := ERASE_LINE;
Min_VMS             := ERASE_LINE;
Max_TPU_Line_Length := INT(ERASE_LINE);
Max_Share_Length    := INT(ERASE_LINE);
Debugging           := INT(ERASE_LINE);
Logging             := INT(ERASE_LINE);
Part_Size           := INT(ERASE_LINE);
Compression         := INT(ERASE_LINE);
DirKeep             := INT(ERASE_LINE);
VersKeep            := INT(ERASE_LINE);
IdentLine           := ERASE_LINE;
Share_File          := ERASE_LINE;
Share               := INT(ERASE_LINE);
SpaceEnc            := INT(ERASE_LINE);
Split               := INT(ERASE_LINE);
ArchiveName         := ERASE_LINE;

SET(FACILITY_NAME, Facility);	! identify ourself in errors

IF Debugging >= DEBUG_SIMess THEN SET(SUCCESS,ON); SET(INFORMATIONAL,ON); ENDIF;
IF SpaceEnc THEN Quoteable_Chars := Quoteable_Chars + " "; ENDIF;

! Initialize stuff to go through files
PartNo := 0;
Max_Bytes := (512 * Part_Size) - LENGTH(FAO(FAO_start_part,Max_Parts)) - EOL;

! Create the initial share file header
Create_Prologue_Head(WorkBuffer, IdentLine, Part_Size, ParamBuff);
Add_To_Part(WorkBuffer);
Create_Prologue_Unpacker(WorkBuffer);
Add_To_Part_Nosplit(WorkBuffer);
Create_Prologue_Trail(WorkBuffer);
Add_To_Part_Nosplit(WorkBuffer);


! Loop around, filling the part buffer with data from files
FileNumber := 0;
LOOP
   EXITIF GET_INFO(ParamBuff,"RECORD_COUNT") = 0;
   POSITION(BEGINNING_OF(ParamBuff));
   FileNumber := FileNumber + 1;
   PackageFile(ERASE_LINE);
ENDLOOP;


! Add the share file epilogue to exit cleanly
Create_Epilogue(WorkBuffer);
Add_To_Part(WorkBuffer);


! Flush out the last part if its contents are not empty
FlushPart;


! Massage the contents of the initial part to accurately reflect the number of
! Parts generated and to give the appropriate instructions if more than 1 part
POSITION( BEGINNING_OF(Part1Buffer) );
POSITION( SEARCH( "$!+", FORWARD) );
IF PartNo > 1
  THEN	! Modify the inital message to state exact number of parts
     POSITION( SEARCH( STR(Max_Parts), FORWARD) );
     ERASE_CHARACTER(3);
     COPY_TEXT( STR(PartNo) );
  ELSE	! Erase initial attention message as there's only 1 part!
     ERASE_LINE; ERASE_LINE; ERASE_LINE;
ENDIF;


! Finally flush out the first part
WritePart(Part1Buffer,1);


! In this version, we write a value back to the parameter file to indicate how
! many parts were generated.
ReuseBuffer(WorkBuffer);
COPY_TEXT(STR(PartNo));
WRITE_FILE(WorkBuffer, ParamFile);

! Log number of parts
IF Logging >= Log_ShowNumparts
  THEN
    IF Share
      THEN
        Inform("I","NUMPARTS",FAO("Share file written in !SL part!%S",PartNo));
      ELSE
        Inform("I","NUMPARTS",FAO("Share file not written in !SL part!%S",PartNo));
    ENDIF;
ENDIF;

QUIT;
$ if .not. $status then $ goto abort
$ return
