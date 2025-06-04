$ !	DCL procedure to dispose of a CARE report.
$ !
$ !	Paramters:
$ !
$ !	P1 = Report Filename
$ !	P2 = Dispose Command - a Formatted ASCII Outpurt(FAO) String
$ !
$ !	The dispose command string is searched for the "MS Exchange flag"
$ !	string.  If the flag string is found, it means that the recipient 
$ !	of the report wants it sent to their MS Exchange e-mail account.  
$ !	If so, the report will be converted to Rich Text Format (RTF), 
$ !	UUencoded and te resultant file disposed of using the dispose 
$ !	command string.  If the flag string is not found, the report 
$ !	is disposed of withot any convertion or encoding.
$ !
$ !	Define a couple of commands for Text To RTF and UUencode utilities...
$ !
$ t2r == "$care$command:text2rtf.exe"
$ uue == "$ucx$uuencode.exe"
$ !
$ !	Check input parameters...
$ !
$ if p1 .eqs. ""
$ then
$	write sys$output "FileName Must Be Spcified!  Aborting..."
$	goto Exit
$ endif
$ !
$ FileName = p1
$ !
$ if p2 .eqs. ""
$ then
$	write sys$output "Dispose Command Must Be Spcified!  Aborting..."
$	goto Exit
$ endif
$ !
$ DisposeCommand = p2
$ ! 
$ MSExchangeFlag = "@mail.dec.com"
$ CAREReportFlag = ".CARE_DIST_"
$ !
$ RTFFileName = ""
$ UUEncodedFileName = ""
$ !
$ !	Make a copy of the dispose command and make it upper case so
$ !	that string searches are easier.  Unfortuantley, UPCASE will 
$ !	NOT upcase quoted characters, so some searches will have to be 
$ !	in upper and lower case and may fail if the string of interest 
$ !	is mixed case...
$ !
$ temp = f$edit( DisposeCommand, "UPCASE" )
$ !
$ if ( ( f$locate( MSExchangeFlag, Temp ) .eq. f$length( Temp ) ) .eq. 0 ) .or. -
     ( ( f$locate( f$edit( MSExchangeFlag, "UPCASE" ), Temp ) .eq. f$length( Temp ) ) .eq. 0 )
$ then
$ !	The flag string is in the dispose command, so
$ !	we have to convert the report file before executing the command...
$ !
$ !	write sys$output "Recipient is a MS Exchange user"
$ !
$ !	Important part of CARE report filename is after the string in 
$ !	CAREReportFlag (".CARE_DIST_" when this was written), so
$ !	we'll look for this string in the filename and then extract the 
$ !	stuff to the right...
$ !
$	CAREReportNameLoc = f$locate( CAREReportFlag, FileName ) + f$length( CAREReportFlag )
$ !	show symbol CAREReportNameLoc
$ !
$	CAREReport = f$extract( CAREReportNameLoc,					-
				( f$length( FileName ) - CAREReportNameLoc ) - 1,	-
				FileName )
$ !	show symbol CAREReport
$ !
$	RTFFileName = CAREReport + ".RTF"
$ !	show symbol RTFFileName
$ !
$	UUEncodedFileName = CAREReport + ".UUE"
$ !	show symbol UUEncodedFileName
$ !
$ !	Convert CARE Report (in text format) to Rich Text Format (RTF)...
$ !
$	t2r 'FileName 'RTFFileName
$ !
$ !	UUencode the RTF file...
$ !
$	uue 'RTFFileName 'UUEncodedFileName
$ !
$ !	Set FileName to UUEncodedFileName so it is sent rather than 
$ !	the CARE report...
$ !
$	FileName = UUEncodedFileName
$ else
$ !	The flag string is not in the dispose command, so
$ !	we can just execute the command with no conversions...
$ !
$ !	write sys$output "Recipient is NOT a MS Exchange user"
$ endif
$ !
$ FormatDisposeCommand:
$ !
$ FormattedDisposeCommand = f$fao( DisposeCommand, FileName )
$ !
$ !show symbol FileName
$ !show symbol DisposeCommand
$ !show symbol FormattedDisposeCommand
$ !
$ 'FormattedDisposeCommand
$ !
$ if RTFFileName .nes. ""
$ then
$	RTFFileName = RTFFileName + ";*"
$	Delete 'RTFFileName
$ endif
$ !
$ if UUEncodedFileName .nes. ""
$ then
$ 	UUEncodedFileName = UUEncodedFileName + ";*"
$	delete 'UUEncodedFileName
$ endif
$ !
$ Exit:
$ !
$ exit
