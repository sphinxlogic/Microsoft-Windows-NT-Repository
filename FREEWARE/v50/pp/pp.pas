{************************************************************************
*									*
*  PP:	Pascal Pre-processor (or "Pretty Pascal").			*
*	====================						*
*									*
*  This program takes a DEC Pascal source file and performs a number	*
*  of "text standardization" tasks on it.				*
*									*
*  Four types of symbols are identified:				*
*	- tokens that are identified as "Pascal reserved words";	*
*	  (these are specified in a vocabulary file)			*
*	- tokens that contain a dollar sign ($);			*
*	- tokens that are considered as user-declared identifiers;	*
*	- all characters not included above.				*
*  The first three types of symbols are known as "tokens".		*
*									*
*  Three contexts are identified:					*
*	- in a comment; 						*
*	- in a quoted string;						*
*	- not in either a comment nor a quoted string.			*
*									*
*  Tokens are only identified when they do not occur in a comment nor	*
*  a quoted string.							*
*									*
*  The tasks which the Pascal Pre-processor is capable of performing	*
*  are as follows:							*
*	- each of the token types (reserved symbol, identifier and	*
*	   dollar symbol) can be converted to one of the following	*
*	  lexical styles:						*
*		. all letters uppercased;				*
*		. all letters lowercased;				*
*		. no change from current style; 			*
*		. the first letter and each first letter following an	*
*		  underscore or dollar sign are uppercased, all other	*
*		  letters are lowercased.				*
*	- lines which have white noise beyond the last significant	*
*	  character of a line are unequivocally truncated to the	*
*	  last significant character;					*
*	- lines may be re-formatted as necessary to maximize the use	*
*	  of TAB characters to eliminate SPACES where possible; 	*
*	- lines which are longer than the working screen-width (either	*
*	   80 or 132 characters) are identified;			*
*									*
*  Progress of the processing can be watched with Control-W.		*
*									*
*-----------------------------------------------------------------------*
*									*
*  Caveat:  It is desirable that the Pascal source program should have	*
*	    previously been compiled without error.			*
*									*
*.......................................................................*
*									*
*  Author:  Jonathan Ridler,						*
*	    Information Technology Services,				*
*	    The University of Melbourne.				*
*									*
*  Email:   jonathan@unimelb.edu.au					*
*.......................................................................*
*									*
*  History:								*
* v1.0	03-Jan-1986	JER	Original version.			*
* v1.1	04-Jun-1986	JER	Add a few warnings & fix binary search. *
* v1.2	29-Jan-1987	JER	Add a few more reserved words.		*
* v1.3	31-Jul-1987	JER	Re-organize syntaxing of filename etc.	*
* v1.4	13-Oct-1987	JER	Add the /ALL & /TAB options etc.	*
* v1.5	21-Oct-1987	JER	Write Formatter & add /OUTPUT.		*
* v1.6	03-Nov-1987	JER	Add more reserved words & /STANDARD.	*
* v1.7	10-Dec-1987	JER	Remove /DUPLICATE; add DISPOSITION:=	*
* v1.8	04-Mar-1988	JER	Add /LOG qualifier for Informationals.	*
* v2.0	20-Jul-1988	JER	Completely revised lexical styling.	*
* v2.1	04-Aug-1988	JER	Changed formatter to avoid quoted tabs. *
* v2.2	22-Sep-1988	JER	Command interface via foreign command.	*
* v2.3	08-Nov-1988	JER	Minor changes to summary lines. 	*
* v2.4	16-Nov-1995	JER	Migrated to Alpha.			*
* v3.0	13-Aug-1998	JER	Make available as freeware.		*
************************************************************************}

[INHERIT ('SYS$LIBRARY:STARLET',
	  'SYS$LIBRARY:PASCAL$CLI_ROUTINES',
	  'SYS$LIBRARY:PASCAL$LIB_ROUTINES',
	  'SYS$LIBRARY:PASCAL$SMG_ROUTINES',
	  'SYS$LIBRARY:PASCAL$STR_ROUTINES')]

PROGRAM  Pp (INPUT,OUTPUT,Inprog,Outprog) ;

CONST			{ ***  C O N S T  *** }

Version = '3.0 (13-Aug-1998)' ; { Program verson }

Min_Tabw =  2 ; 		{ Minimum Tab width }
Max_Tabw = 20 ; 		{ Maximum Tab width }

Maxtargets = 500 ;		{ Maximum number of targets catered for }
Targetsize =  31 ;		{ Size of Targets }

Maxfsize = 255 ;		{ Max Characters in a file/directory spec }

Progress_Char = 'W' ;		{ Control char to get progress report }

Bell = ''(7) ;
Tab  = ''(9) ;

Alphalo  = ['a'..'z'] ;
Alphaup  = ['A'..'Z'] ;
Digits	 = ['0'..'9'] ;
Alphanum = Alphalo + Alphaup + Digits ;

{ "TokenChars" contains the valid characters for forming reserved symbols }

Tokenchars = Alphanum + ['_','$'] ;


TYPE			{ ***  T Y P E	 *** }

$UBYTE = [BYTE] 0..255 ;
$UWORD = [WORD] 0..65535 ;

Token_Type    = (Unknown, Identifier, Reserved, Dollar) ;
Lexical_Style = (Nochange, First_Chars_Up, Lowercase, Uppercase) ;

Char80 = VARYING [80] OF CHAR ;

Item_List_Cell = RECORD
		   CASE INTEGER OF
		     1: (			{ Normal Cell }
			 Buffer_Length : $UWORD ;
			 Item_Code     : $UWORD ;
			 Buffer_Addr   : UNSIGNED ;
			 Return_Addr   : UNSIGNED
			) ;
		     2: (			{ Terminator }
			 Terminator    : UNSIGNED
			) ;
		 END ;

Item_List_Template (Count:INTEGER) = ARRAY [1..Count] OF Item_List_Cell ;

VAR			{ ***  V A R  *** }

Rst ,				{ General return status }
Reform_Count ,			{ # of lines reformatted }
Linecnt ,			{ # of lines in file }
Lasttarget,			{ Index of LAST  used element in Targets }
Inreclen ,			{ Input record length in bytes }
Intab_Width ,			{ Width of inserted horizontal tabs in infile }
Outtab_Width ,			{ Width of inserted horizontal tabs in outfile }
Screen_Width ,			{ Max. length of line }
Chars_Purged_Count ,		{ Number of blanks removed by various means }
Original_Length ,		{ Length of input record as read }
Tokenstart ,			{ Where the current reserved symbol starts }
Tokenlen : INTEGER := 0 ;	{ Length of current reserved symbol }

Incomment ,			{ Scanning a comment }
Inquote ,			{ Scanning a quoted string }
Mentor ,			{ User has requested HELP ? }
Logging ,			{ Log information to the user ? }
Warnings ,			{ Print warnings ? }
Outqual ,			{ An output filename supplied ? }
Widescreen ,			{ Allow lines with length > 80 & <= 132. }
Reformatting ,			{ Modify the format of the text in some way }
Underway : BOOLEAN := FALSE ;	{ Actually reading and translating file }

Current_Token_Type : Token_Type := Unknown ;

Fver , Ftype : VARYING [40] OF CHAR := '' ;

PAS$K_SUCCESS ,
PAS$K_FILNOTFOU : [EXTERNAL,VALUE] UNSIGNED ;

Indata ,
Fname ,
Pas_Source ,
Pas_Dest ,
Token ,
Originaltoken : VARYING [Maxfsize] OF CHAR := '' ;

{ The reserved words array contains tokens which are valid DEC Pascal
  identifiers or reserved words or other selected targets.  These MUST be
  in alphabetical/length order.  (N.B. the character underscore (i.e. "_")
  is cardinally GREATER than uppercase letters). }

Targets : ARRAY [1..Maxtargets] OF VARYING [Targetsize] OF CHAR := ZERO ;

Count : ARRAY [Identifier..Dollar] OF INTEGER := ZERO ;
Mods  : ARRAY [Identifier..Dollar] OF INTEGER := ZERO ;
Token_Style : ARRAY [Identifier..Dollar] OF Lexical_Style := ZERO ;

Inprog , Outprog : TEXT ;	{ Input source file and output file }


	{ ***  F U N C T I O N S   and	 P R O C E D U R E S  *** }


FUNCTION  Str_Compress (Source : [CLASS_S] PACKED ARRAY [L..U:INTEGER] OF CHAR ;
			VAR  Dest : VARYING [D] OF CHAR ;
			Collapse : BOOLEAN := FALSE) : UNSIGNED ;

{ Compress a string by removing leading and trailing white space (blanks and
  tabs), and replacing multiple consecutive white space with a single blank.
  If collapse is set, remove ALL white space. }

  CONST  Blanks = [' ',''(9),''(0)] ;
	 Maxsize = 1024 ;
	 Warn_Inpstrtru = UAND (LIB$_INPSTRTRU,%Xfffffff8) ;	{ Warning only }

  VAR  S , J , K : INTEGER := 0 ;
       Done : BOOLEAN := FALSE ;
       Spacer : VARYING [1] OF CHAR := ' ' ;
       Dstr, Sstr : VARYING [Maxsize] OF CHAR := '' ;

  BEGIN 	{ Str_Compress }
  Str_Compress := SS$_NORMAL ;		{ Presume so }
  S := LENGTH (Source) ;

  IF S = 0  THEN			{ Nothing passed }
     Dest := ''
   ELSE
     BEGIN
     IF S > Maxsize  THEN
	BEGIN
	Sstr := SUBSTR (Source,1,Maxsize) ;
	Str_Compress := INT(Warn_Inpstrtru) ;
	END
      ELSE
	Sstr := Source ;

     IF Collapse  THEN	Spacer := '' ;

     IF FIND_MEMBER (Source,Blanks) = 0  THEN		{ Nothing to change }
	Dstr := Sstr
      ELSE
	WHILE NOT Done	DO
	  BEGIN
	  J := FIND_NONMEMBER (Sstr,Blanks) ;
	  IF J = 0  THEN
	     Done := TRUE
	   ELSE
	     BEGIN
	     Sstr := SUBSTR (Sstr,J,Sstr.LENGTH-J+1) ;
	     K := FIND_MEMBER (Sstr,Blanks) ;
	     IF K = 0  THEN
		BEGIN
		Dstr := Dstr + Sstr ;
		Done := TRUE ;
		END
	      ELSE
		BEGIN
		Dstr := Dstr + SUBSTR (Sstr,1,K-1) + Spacer ;
		Sstr := SUBSTR (Sstr,K,Sstr.LENGTH-K+1) ;
		END ;
	     END ;
	  END ; 	{ of While }

     IF Dstr <> ''  THEN
	IF Dstr[Dstr.LENGTH] = ' '  THEN
	   Dstr:= SUBSTR (Dstr,1,Dstr.LENGTH-1) ;

     IF Dstr.LENGTH <= D  THEN
	Dest := Dstr
      ELSE
	BEGIN
	Dest := SUBSTR (Dstr,1,D) ;
	Str_Compress := LIB$_OUTSTRTRU ;
	END ;
     END ;
  END ; 	{ of Str_Compress }


FUNCTION Get_Logical_Name (
		Lognam	: [CLASS_S] PACKED ARRAY [L1..U1:INTEGER] OF CHAR ;
		Default : VARYING [Sz1] OF CHAR ;
		VAR Actual : VARYING [Sz2] OF CHAR ;
		Table	: [CLASS_S] PACKED ARRAY [L2..U2:INTEGER] OF CHAR :=
				'LNM$FILE_DEV' ;
		Mode	: $UBYTE := PSL$C_USER ;
		Lnm_Index  : UNSIGNED := 0
		) : UNSIGNED ;

{ Get the translation of the logical name specified.  If it does not exist,
  use the default value if the index is zero, or flag a non-existant index. }

  VAR Rst : UNSIGNED := 0 ;
      Attributes : LNM$TYPE := ZERO ;
      Item_List  : Item_List_Template (4) := ZERO ;

  BEGIN       { Get_Logical_Name }
  Item_List[1].Buffer_Length := 4 ;
  Item_List[1].Item_Code     := LNM$_INDEX ;
  Item_List[1].Buffer_Addr   := IADDRESS (Lnm_Index) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Buffer_Length := SIZE (Attributes) ;
  Item_List[2].Item_Code     := LNM$_ATTRIBUTES ;
  Item_List[2].Buffer_Addr   := IADDRESS (Attributes) ;
  Item_List[2].Return_Addr   := 0 ;

  Item_List[3].Buffer_Length := SIZE (Actual.BODY) ;
  Item_List[3].Item_Code     := LNM$_STRING ;
  Item_List[3].Buffer_Addr   := IADDRESS (Actual.BODY) ;
  Item_List[3].Return_Addr   := IADDRESS (Actual.LENGTH) ;

  Item_List[4].Terminator    := 0 ;   { Terminate the item list }

  Rst := $TRNLNM (Attr	 := %REF LNM$M_CASE_BLIND,
		  Tabnam := Table,
		  Lognam := Lognam,
		  Acmode := %REF Mode,
		  Itmlst := Item_List) ;
  Get_Logical_Name := Rst ;

  IF Rst = SS$_NOLOGNAM  THEN
     Actual := Default
   ELSE
  IF (Rst = SS$_NORMAL) AND_THEN (NOT Attributes.LNM$V_EXISTS)	THEN
     Get_Logical_Name := SS$_VALNOTVALID       { Index not found - tell caller }
   ELSE
  IF Rst = SS$_BUFFEROVF  THEN		     { Do nothing - caller must handle }
   ELSE
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ;       { of Get_Logical_Name }


FUNCTION  Check_Syntax (Spec : VARYING [L1] OF CHAR) : BOOLEAN ;

{ This function performs basic RMS file specification parsing, prints any
  error messages, and returns TRUE if the syntax is as correct as RMS can
  tell and the Device/Directory exists (Dev/Dir Not checked if DECNet). }

  CONST Validchars =
	Alphanum + ['$', '-', '_', '[', ']', '<', '>', ':', '"', '.', ';', ' '];

  VAR  Rst : INTEGER := 0 ;
       Result : VARYING [Maxfsize] OF CHAR := '' ;
       Fab : FAB$TYPE := ZERO ;
       Nam : NAM$TYPE := ZERO ;


  PROCEDURE  Set_File_Info ;

    { Plonk all the info into the appropriate fields }

    VAR X : INTEGER := 0 ;

    BEGIN
    Result.LENGTH := Nam.NAM$B_ESL ;
    X := Nam.NAM$B_NODE + Nam.NAM$B_DEV + Nam.NAM$B_DIR + Nam.NAM$B_NAME ;
    Fname := SUBSTR (Result,1,X) ;
    Ftype := SUBSTR (Result,X+1,Nam.NAM$B_TYPE) ;
    Fver  := SUBSTR (Result,X+1+Nam.NAM$B_TYPE,Nam.NAM$B_VER) ;
    END ;	{ of Set_File_Info }


  BEGIN 	{ Check_Syntax }
  Check_Syntax := FALSE ;		{ Presume so }
  Result := '' ;

  IF (Spec.LENGTH > 0) AND
     (FIND_NONMEMBER (Spec,Validchars) <> 0)  THEN
     WRITELN ('%PP-F-INVCHARS, File Specification contains Invalid ',
	      'Character(s).',Bell)
   ELSE
     BEGIN
     Fab := ZERO ;
     Fab.FAB$B_BID := FAB$C_BID ;			{ FAB Block ID }
     Fab.FAB$B_BLN := FAB$C_BLN ;			{ FAB Block Length }
     Fab.FAB$L_FNA := IADDRESS (Spec.BODY) ;		{ Primary File Spec }
     Fab.FAB$B_FNS := Spec.LENGTH ;			{ 1ry File Spec Size }
     Fab.FAB$L_NAM := IADDRESS(Nam) ;			{ NAM Block Address }

     Nam := ZERO ;
     Nam.NAM$B_BID := NAM$C_BID ;			{ NAM Block ID }
     Nam.NAM$B_BLN := NAM$C_BLN ;			{ NAM Block Length }
     Nam.NAM$L_ESA := IADDRESS (Result.BODY) ;		{ Expanded String Area }
     Nam.NAM$B_ESS := Maxfsize ;			{ Max Expand Strng Syz }

     Rst := $PARSE (Fab) ;

     IF ODD (Rst)  THEN
	BEGIN
	Check_Syntax := TRUE ;
	Set_File_Info ;
	END
      ELSE
	BEGIN
	WRITE (Bell) ;
	CASE Rst  OF
	  RMS$_ACS: WRITELN ('%Error in Node Access Control Specification.') ;
	  RMS$_CHN: BEGIN
		    WRITELN ('%Error: Could Not Assign an I/O Channel.') ;
		    WRITELN ('%Status = ',HEX(Fab.FAB$L_STV)) ;
		    END ;
	  RMS$_DEV: WRITELN ('%Error in Device Specification.') ;
	  RMS$_DIR: WRITELN ('%Error in Directory Specification.') ;
	  RMS$_DNF: BEGIN
		    Set_File_Info ;
		    WRITELN ('%Error: Non-Existant Directory: ',Result) ;
		    END ;
	  RMS$_DNR: WRITELN ('%Error: Device is NOT READY.') ;
	  RMS$_ESS: BEGIN
		    WRITELN ('** Program Error: RMS$_ESS in Check_Syntax **') ;
		    $EXIT ;
		    END ;
	  RMS$_FNM: WRITELN ('%Error in File Name Specification.') ;
	  RMS$_LNE: WRITELN ('%Error in Logical Name.') ;
	  RMS$_NOD: WRITELN ('%Error in Node Specification.') ;
	  RMS$_QUO: WRITELN ('%Error in Quoted String.') ;
	  RMS$_SYN: WRITELN ('%Error in File Specification.') ;
	  RMS$_TYP: WRITELN ('%Error in File Type') ;
	  RMS$_VER: WRITELN ('%Error in Version Number.') ;
	    OTHERWISE  $EXIT (Rst) ;
	  END ; { of Case }
	END ;
     END ;
  END ; 	{ of Check_Syntax }


FUNCTION Put_Count (Count : INTEGER ;
		    Caption : VARYING [L] OF CHAR) : Char80 ;

  VAR  Temp : Char80 ;

  BEGIN 	{ Put_Count }
  CASE Count OF
   0: Put_Count := 'No ' + Caption + 's' ;
   1: Put_Count := '1 ' + Caption ;
   OTHERWISE
	BEGIN
	WRITEV (Temp,Count:1,' ',Caption,'s') ;
	Put_Count := Temp ;
	END ;
   END ;
  END ; 	{ of Put_Count }


[UNBOUND] PROCEDURE Report_Progress ;

  VAR  T , C : INTEGER := 0 ;
       Temp : VARYING [80] OF CHAR := '' ;

  BEGIN
  T := Count[Reserved] + Count[Identifier] + Count[Dollar] ;
  C := Mods[Reserved]  + Mods[Identifier]  + Mods[Dollar] ;
  IF Underway  THEN
     IF Reformatting  THEN
	WRITEV (Temp,'%PP-I-STATUS, @ Line ',Linecnt:4,' (',
		Put_Count (Reform_Count,'Reform'),'): ',
		Put_Count (T,'Token'),'; ',
		Put_Count (C,'Change'),'.')
      ELSE
	WRITEV (Temp,'%PP-I-STATUS, @ Line ',Linecnt:4,': ',
		Put_Count (T,'Token'),'; ',
		Put_Count (C,'Change'),'.')
   ELSE
     WRITEV (Temp,'%PP-I-STATUS, Initializing ...') ;
  WRITELN (Temp) ;
  END ;


FUNCTION  Open_Files : BOOLEAN ;

{ This procedure opens the input and output files.  The output file has the
  same name as the input file, but is a new version. }

  VAR	Rst : INTEGER := 0 ;

  BEGIN 	{ Open_Files }

  Open_Files := FALSE ; 	{ Presume so }

  IF Check_Syntax (Pas_Source)	THEN
     BEGIN
     IF (Ftype <> '.PAS') AND (Ftype <> '.')  THEN
	BEGIN
	WRITELN ('%PP-F-BADFILTYP, File_Type must be .PAS or nothing.',Bell) ;
	$EXIT ;
	END
      ELSE
	Pas_Source := Fname + '.PAS' + Fver ;

     OPEN (Inprog,FILE_NAME:=Pas_Source,HISTORY:=READONLY,Error:=CONTINUE) ;
     Rst := STATUS (Inprog) ;
     IF Rst = PAS$K_FILNOTFOU  THEN
	WRITELN ('%PP-F-FILNOTFOU, File not found.',Bell)
      ELSE
     IF Rst <> PAS$K_SUCCESS  THEN	{ Problem with file }
	WRITELN ('%PP-F-OPENINPFAIL, Could not open INPUT file; ',
		 'Pascal Status = ',Rst:1,'.',Bell)
      ELSE
	BEGIN
	RESET (Inprog) ;
	IF NOT Outqual	THEN
	   Pas_Dest := Pas_Source ;				{ by default }

	IF Check_Syntax (Pas_Dest)  THEN
	   IF (Ftype <> '.PAS') AND (Ftype <> '.')  THEN
	      BEGIN
	      WRITELN ('%PP-F-BADFILTYP, File_Type must be .PAS or nothing.',
		       Bell) ;
	      $EXIT ;
	      END
	    ELSE
	      Pas_Dest := Fname + '.PAS' + Fver ;

	{ Open the output file with disposition = DELETE.  This will prevent
	  an incomplete file being left if the image terminates prematurely.
	  The file is then closed with disposition = SAVE. }

	OPEN (Outprog,FILE_NAME:=Pas_Dest,HISTORY:=NEW,DISPOSITION:=DELETE,
	      Error:=CONTINUE) ;
	Rst := STATUS (Outprog) ;
	IF Rst <> PAS$K_SUCCESS  THEN	{ Problem with file }
	   WRITELN ('%PP-F-OPENOUTFAIL, Could not open OUTPUT file; ',
		    'Pascal Status = ',Rst:1,'.',Bell)
	 ELSE
	   BEGIN
	   REWRITE (Outprog) ;
	   Open_Files := TRUE ; 	{ Success all round }
	   END ;
	END ;
     END ;

  END ; 	{ of Open_Files }


FUNCTION  Gettoken : BOOLEAN ;

{ This function scans the input buffer and attempts to pick off a valid token,
  returning TRUE if one is found else FALSE at EOLN.  A valid token is defined
  to be any consecutive set of valid token characters. }

  VAR	Intoken : [STATIC] BOOLEAN := FALSE ;
	Scanpos : INTEGER := 0 ;
	Done : BOOLEAN := FALSE ;

  BEGIN 	{ GetToken }

  Gettoken := FALSE ;		{ Presume so }

  IF Inreclen > 0  THEN 	{ Ignore null record (i.e. empty record) }
     BEGIN
     Tokenstart := Tokenstart + Tokenlen ;
     Tokenlen := 0 ;
     Token := '' ;
     Current_Token_Type := Unknown ;
     Done := FALSE ;
     Scanpos := Tokenstart ;

     WHILE NOT Done  DO
	BEGIN
	IF Scanpos > Inreclen  THEN	{ Passed EOLN }
	   BEGIN
	   Done := TRUE ;
	   IF Intoken  THEN		{ We were in a token at the time }
	      BEGIN
	      Intoken := FALSE ;
	      Gettoken := TRUE ;
	      Token := SUBSTR (Indata,Tokenstart,Tokenlen) ;
	      Originaltoken := Token ;
	      END ;
	   END
	 ELSE
	IF Incomment  THEN		{ In a comment; looking for end of it }
	   BEGIN
	   IF Indata[Scanpos] = '}'  THEN
	      Incomment := FALSE
	    ELSE
	   IF Indata[Scanpos] = '*'  THEN	{ Alternate comment form ? *)
	      BEGIN
	      IF Scanpos + 1 <= Inreclen  THEN
		 IF Indata[Scanpos+1] = ')'  THEN
		    BEGIN
		    Scanpos := Scanpos + 1 ;
		    Incomment := FALSE ;
		    END ;
	      END ;
	   END
	 ELSE
	IF Inquote  THEN		{ In a string; looking for end of it }
	   BEGIN
	   IF Indata[Scanpos] = ''''  THEN
	      Inquote := FALSE ;
	   END
	 ELSE
	IF Intoken  THEN				{ In a token sequence }
	   IF Indata[Scanpos] IN Tokenchars  THEN	{ Still valid }
	      BEGIN
	      Tokenlen := Tokenlen + 1 ;
	      IF Indata [Scanpos] = '$'  THEN
		 Current_Token_Type := Dollar ;
	      END
	    ELSE
	      BEGIN			{ Found invalid char: end of token }
	      Intoken := FALSE ;
	      Gettoken := TRUE ;
	      Token := SUBSTR (Indata,Tokenstart,Tokenlen) ;
	      Originaltoken := Token ;
	      Done := TRUE ;
	      END
	 ELSE
	IF Indata[Scanpos] IN Tokenchars  THEN		{ Start of token }
	   BEGIN
	   Tokenstart := Scanpos ;
	   Tokenlen := 1 ;
	   Intoken := TRUE ;
	   IF Indata [Scanpos] = '$'  THEN
	      Current_Token_Type := Dollar ;
	   END
	 ELSE
	IF Indata[Scanpos] = '{'  THEN			{ Start of comment }
	   Incomment := TRUE
	 ELSE
	IF Indata[Scanpos] = '}'  THEN		{ Unexpected end of comment }
	   WRITELN ('%PP-E-UNMATCOM, Unmatched End of Comment @ line ',
		    Linecnt:1,' **',Bell)
	 ELSE
	IF Indata[Scanpos] = '('  THEN		(* Alternate comment form ? *)
	   BEGIN
	   IF Scanpos + 1 <= Inreclen  THEN
	      IF Indata[Scanpos+1] = '*'  THEN
		 BEGIN
		 Scanpos := Scanpos + 1 ;
		 Incomment := TRUE ;
		 END ;
	   END
	 ELSE
	IF Indata[Scanpos] = '*'  THEN		{ Unmatched end of comment ? *)
	   BEGIN
	   IF Scanpos + 1 <= Inreclen  THEN
	      IF Indata[Scanpos+1] = ')'  THEN
		 BEGIN
		 Scanpos := Scanpos + 1 ;
		 WRITELN ('%PP-E-UNMATCOM, Unmatched End of Comment @ line ',
			  Linecnt:1,'.',Bell) ;
		 END ;
	   END
	 ELSE
	IF Indata[Scanpos] = ''''  THEN
	   Inquote := TRUE ;
	Scanpos := Scanpos + 1 ;
	END ;	{ of While }
     END ;	{ of If }
  END ; 	{ of GetToken }


FUNCTION  Lookup_Token : BOOLEAN ;

{ This function will do a binary search of the reserved words array for the
  current token, returning TRUE if found else FALSE. }

  VAR	Search_Spot , Search_Lo , Search_Hi : INTEGER := 0 ;
	Found : BOOLEAN := FALSE ;

  BEGIN 	{ Lookup_Token }
  Search_Lo := 1 ;
  Search_Hi := Lasttarget  ;

  WHILE NOT Found  DO
    BEGIN
    Search_Spot := Search_Lo + (Search_Hi - Search_Lo + 1) DIV 2 ;
    IF (Search_Spot = Search_Lo) OR (Search_Spot = Search_Hi)  THEN
       BEGIN
       Found := TRUE ;
       Lookup_Token := (STR$COMPARE_MULTI (Token,Targets[Search_Hi],1,1) = 0) OR
		       (STR$COMPARE_MULTI (Token,Targets[Search_Lo],1,1) = 0) ;
       END
     ELSE
       CASE  STR$COMPARE_MULTI (Token,Targets[Search_Spot],1,1) OF
	 -1: Search_Hi := Search_Spot ; 	{ Token < Reserved word }
	  0: BEGIN
	     Found := TRUE ;			{ Token = Reserved word }
	     Lookup_Token := TRUE ;
	     END ;
	  1: Search_Lo := Search_Spot ; 	{ Token > Reserved word }
	 END ;	{ of Case }
    END ;	{ of While }
  END ; 	{ of Lookup_Token }


PROCEDURE Translate_Token ;

  PROCEDURE  Standardize_Token ;

  { This procedure will standardize the current token.	Standard format is all
    lowercase letters except for the first letter of the token and first
    letters immediately following embedded underscores or dollar signs - these
    are uppercased. (e.g. my_bOOk_sHoP => My_Book_Shop) }

    VAR I : INTEGER := 0 ;
	Capitalize : BOOLEAN := TRUE ;	{ !!! }

    BEGIN	{ Standardize_Token }
    FOR I := 1 TO Token.LENGTH	DO
	BEGIN
	IF Capitalize  THEN
	   BEGIN
	   IF Token[I] IN Alphalo  THEN
	      Token[I] := CHR (ORD (Token[I])-32) ;
	   END
	 ELSE
	IF Token[I] IN Alphaup	THEN
	   Token[I] := CHR (ORD (Token[I])+32) ;
	Capitalize := (Token[I] = '_') OR (Token[I] = '$') ;
	END ;
    END ;	{ of Standardize_Token }


  PROCEDURE Str_Lower ;

      VAR  I : INTEGER := 0 ;

    BEGIN
    FOR I := 1 TO Token.LENGTH	DO
	IF Token[I] IN Alphaup	THEN
	   Token[I] := CHR (ORD (Token[I])+32) ;
    END ;


  BEGIN 	{ Translate_Token }
  CASE	Token_Style[Current_Token_Type]  OF
    First_Chars_Up:	Standardize_Token ;
    Lowercase:		Str_Lower ;
    Nochange:		;			{ No action }
    Uppercase:		STR$UPCASE (%DESCR Token,%DESCR Token) ;
    END ; { of Case }

  IF Originaltoken <> Token  THEN	{ May not need to be changed ? }
     BEGIN
     STR$REPLACE (%DESCR Indata,Indata,Tokenstart,Tokenstart+Tokenlen-1,Token) ;
     Mods[Current_Token_Type] := Mods[Current_Token_Type] + 1 ;
     END ;
  END ; 	{ Translate_Token }


PROCEDURE Formatter ;

{ Clean up the use of tabs such that redundant spaces are removed and replaced
  by tabs.  Tab spacing is assumed to be equal for all tabs.  An input tab
  spacing and an output tab spacing may be specified, such that a file may be
  "converted" from one tab spacing to another.	If only one is specified, it
  is assumed to be the same for both input and output.	This reformatting
  routine does not reformat lines that contain a quoted string which itself
  contains a TAB }

  LABEL Dropout ;		{ Just to avoid a complicated structure }

  VAR	Spot , Expandedlen , Spaces , Tab_Spot , Tab_Count : INTEGER := 0 ;
	Instring : BOOLEAN := FALSE ;
	Str , Str2 : VARYING [512] OF CHAR := '' ;

  BEGIN 	{ Formatter }

  Spot := 1 ;
  Str := '' ;
  Instring := FALSE ;

  WHILE Spot <= Inreclen  DO
    BEGIN
    IF Indata[Spot] = ''''  THEN
       BEGIN
       Instring := NOT Instring ;
       Str := Str + '''' ;
       END
     ELSE
    IF Instring  THEN
       IF Indata[Spot] = Tab  THEN
	  BEGIN
	  IF Warnings  THEN
	     WRITELN ('%PP-W-QUOTEDTAB, Line ',Linecnt:1,
			' contains quoted tab(s) - not reformatted.') ;
	  GOTO	Dropout ;
	  END
	ELSE
	  Str := Str + Indata[Spot]
     ELSE
    IF Indata[Spot] = Tab  THEN
       Str := PAD (Str,' ',((Str.LENGTH DIV Intab_Width) + 1) * Intab_Width)
     ELSE
       Str := Str + Indata[Spot] ;
    Spot := Spot + 1 ;
    END ;

  IF Instring  THEN  GOTO  Dropout ;	{ There is a problem in the source! }

  Expandedlen := Str.LENGTH ;
  IF (Expandedlen > Screen_Width) AND Warnings	THEN
     WRITELN ('%PP-W-LONGLINE, Line ',Linecnt:1,' exceeds standard length (',
	      Screen_Width:1,') by ',
	      Put_Count (Expandedlen-Screen_Width,'character'),'.') ;

  Str2 := '' ;
  Spot := 1 ;
  Spaces := 0 ;
  Tab_Count := 1 ;
  Tab_Spot := 0 ;

  WHILE Tab_Spot <= Expandedlen  DO
    BEGIN
    Tab_Spot := Tab_Count * Outtab_Width + 1 ;
    IF Tab_Spot <= Expandedlen	THEN
       BEGIN
       WHILE Spot < Tab_Spot  DO
	 BEGIN
	 IF Str[Spot] = ''''  THEN
	    BEGIN
	    Instring := NOT Instring ;
	    IF Spaces > 0  THEN
	       BEGIN
	       Str2 := PAD (Str2,' ',Str2.LENGTH+Spaces) ;
	       Spaces := 0 ;
	       END ;
	    Str2 := Str2 + '''' ;
	    END
	  ELSE
	 IF Instring  THEN
	    Str2 := Str2 + Str[Spot]
	  ELSE
	 IF Str[Spot] = ' '  THEN
	    Spaces := Spaces + 1
	  ELSE
	    BEGIN
	    IF Spaces > 0  THEN
	       BEGIN
	       Str2 := PAD (Str2,' ',Str2.LENGTH+Spaces) ;
	       Spaces := 0 ;
	       END ;
	    Str2 := Str2 + Str[Spot] ;
	    END ;
	 Spot := Spot + 1 ;
	 END ;
       IF Spaces > 0  THEN
	  BEGIN
	  IF Spaces = 1  THEN
	     Str2 := Str2 + ' '
	   ELSE
	     Str2 := Str2 + Tab ;
	  Spaces := 0 ;
	  END ;
       END
     ELSE
       Str2 := Str2 + SUBSTR (Str,Spot,Str.LENGTH-Spot+1) ;
    Tab_Count := Tab_Count + 1 ;
    END ;

  IF Indata <> Str2  THEN
     BEGIN
     Indata := Str2 ;		{ Update }
     Reform_Count := Reform_Count + 1 ;
     END ;

  Dropout:	{ Just do nothing if we jump to here }
  END ; 	{ of Formatter }


PROCEDURE Teach ;

  BEGIN
  WRITELN ;
  WRITELN ('Pascal Pre-processor (Pretty Pascal) Version ',Version,
	   ' - Command Format:') ;
  WRITELN ;
  WRITELN ('$ PP /HELP') ;
  WRITELN ;
  WRITELN ('   or') ;
  WRITELN ;
  WRITELN ('$ PP  Pascal_source_filename  [qualifiers]') ;
  WRITELN ;
  WRITELN ('Qualifiers (optional):') ;
  WRITELN (' /ALL[=lexical_style]              ! convert all tokens to style.');
  WRITELN (' /DOLLARS[=lexical_style]          ! convert dollar symbols to ',
		'style.') ;
  WRITELN (' /IDENTIFIERS[=lexical_style]      ! convert identifiers to ',
		'style.') ;
  WRITELN (' /LOG                              ! output messages (default).') ;
  WRITELN (' /OUTPUT=filename                  ! output filename.') ;
  WRITELN (' /RESERVED_SYMBOLS[=lexical_style] ! convert reserved ',
		'symbols to style.') ;
  WRITELN (' /TAB                              ! replace spaces with tabs ',
		'where possible.') ;
  WRITELN (' /VERSION                          ! show version.') ;
  WRITELN (' /VOCABULARY=vocab_filename        ! vocabulary filename.') ;
  WRITELN (' /WARNINGS                         ! output warnings (default).') ;
  WRITELN (' /WIDE_SCREEN                      ! presume terminal width = 132');
  WRITELN ;
  WRITELN ('Lexical_Style = First_Chars_Up | Lowercase | Nochange | Uppercase');
  WRITELN ;
  END ; 	{ of Teach }


PROCEDURE Initialize ;

  VAR  Rst , Pasteboard , Mask : UNSIGNED := 0 ;
       Current_Token_Style : Lexical_Style ;
       Temp : VARYING [255] OF CHAR := '' ;

  [ASYNCHRONOUS] PROCEDURE  Pp_Cld ; EXTERNAL ;

  FUNCTION Check_Qualifier (
	Qualifier : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR) : BOOLEAN ;

    VAR  Rst : UNSIGNED := 0 ;

    BEGIN	{ Check_Qualifier }
    Rst := CLI$PRESENT(Qualifier) ;
    Check_Qualifier := (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;
    END ;	{ Check_Qualifier }


  FUNCTION Read_Vocabulary : BOOLEAN ;

    VAR Rst : INTEGER := 0 ;
	Symbol : VARYING [80] OF CHAR := '' ;
	Vocab_File : VARYING [Maxfsize] OF CHAR := '' ;
	Vocab : TEXT ;

    BEGIN	{ Read_Vocabulary }
    Read_Vocabulary := FALSE ;
    Get_Logical_Name (Lognam  := 'PP_VOCABULARY',
		      Default := 'SYS_SYSTEM:PP_VOCAB.TXT',   { SITE-specific }
		      Actual  := Vocab_File) ;

    IF Check_Qualifier ('VOCABULARY') THEN		   { Overrides logical }
       CLI$GET_VALUE ('VOCABULARY',%DESCR Vocab_File) ;

    OPEN (Vocab,FILE_NAME:=Vocab_File,HISTORY:=Old,Error:=CONTINUE) ;
    Rst := STATUS (Vocab) ;
    IF Rst <> PAS$K_SUCCESS  THEN
       IF Rst = PAS$K_FILNOTFOU  THEN
	  WRITELN ('%PP-F-VOCABNOTFOU, Vocabulary file not found.')
	ELSE
	  WRITELN ('%PP-F-OPENERROR, Vocabulary file open error: ',
		   'Pascal status = ',Rst:1)
     ELSE
       BEGIN
       RESET (Vocab) ;
       Lasttarget := 0 ;
       WHILE (Lasttarget < Maxtargets) AND NOT EOF (Vocab)  DO
	 BEGIN
	 READLN (Vocab,Symbol) ;
	 Str_Compress (Symbol,Symbol) ;
	 IF Symbol.LENGTH > 0  THEN
	    IF Symbol[1] = '!'	THEN		{ Comment }
	     ELSE
	       BEGIN
	       STR$UPCASE (%DESCR Symbol,%DESCR Symbol) ;{ case_blind collating}
	       IF Lasttarget > 0  THEN
		  IF Symbol <= Targets[Lasttarget]  THEN
		     BEGIN
		     WRITELN ('%PP-F-BADVOCAB, Vocabulary file is not ',
				'alphabetically sequential @ "',Symbol,'"') ;
		     $EXIT ;
		     END ;
	       Lasttarget := Lasttarget + 1 ;
	       Targets[Lasttarget] := Symbol ;
	       END ;
	 END ;
       IF Lasttarget = 0	THEN
	  WRITELN ('%PP-F-EMPTYVOCAB, Vocabulary file contains no symbols.')
	ELSE
       IF Lasttarget = Maxtargets  THEN
	  WRITELN ('%PP-F-VOCABOVERFLOW, Vocabulary array not large enough.')
	ELSE
	  Read_Vocabulary := TRUE ;
       CLOSE (Vocab) ;
       END ;
    END ;	{ of Read_Vocabulary }


  FUNCTION Get_Qualifier (
	Qualifier : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR) : BOOLEAN ;

    BEGIN	{ Get_Qualifier }
    IF Check_Qualifier (Qualifier)  THEN
       BEGIN
       Get_Qualifier := TRUE ;
       CLI$GET_VALUE (Qualifier,%DESCR Temp) ;
       IF Temp[1] = 'F'  THEN
	  Current_Token_Style := First_Chars_Up
	ELSE
       IF Temp[1] = 'L'  THEN
	  Current_Token_Style := Lowercase
	ELSE
       IF Temp[1] = 'N'  THEN
	  Current_Token_Style := Nochange
	ELSE
       IF Temp[1] = 'U'  THEN
	  Current_Token_Style := Uppercase ;
       END
     ELSE
       Get_Qualifier := FALSE ;
    END ;	{ of Get_Qualifier }

  BEGIN 	{ Initialize }
  Temp := ZERO ;
  Rst := LIB$GET_FOREIGN (%DESCR Temp) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  Temp := 'PP ' + Temp ;
  Rst := CLI$DCL_PARSE (Temp,%REF Pp_Cld,%IMMED LIB$GET_INPUT,
			%IMMED LIB$GET_INPUT,'PP> ') ;
  IF (Rst = RMS$_EOF) OR (Rst = CLI$_NOCOMD) OR NOT ODD (Rst)  THEN  $EXIT ;

  IF Check_Qualifier ('VERSION')  THEN
     WRITELN ('%PP-I-VERSION, Pascal Pre-processor Version ',Version) ;

  IF NOT Read_Vocabulary  THEN	$EXIT ;

  { Enable out-of-band AST for Control-W }

  Rst := SMG$CREATE_PASTEBOARD (Pasteboard,
				Flags := SMG$M_KEEP_CONTENTS) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  Mask := 2**23 ;	{ = 'W' }
  Rst := SMG$SET_OUT_OF_BAND_ASTS (Pasteboard,Mask,%IMMED Report_Progress) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { Get the command line parameter which is required and therefore must be
    present: it is the Pascal input filename. }

  Mentor := Check_Qualifier ('HELP') ;
  IF NOT Mentor  THEN
     CLI$GET_VALUE ('PASCALSOURCE',%DESCR Pas_Source) ;

  Outqual := Check_Qualifier ('OUTPUT') ;
  IF Outqual  THEN
     CLI$GET_VALUE ('OUTPUT',%DESCR Pas_Dest) ;

  Reformatting := Check_Qualifier ('TAB') ;
  IF Reformatting  THEN
     FOR Rst := 1 TO 2	DO	{ It doesn't matter if there's only one }
	 BEGIN
	 IF CLI$GET_VALUE ('TAB',%DESCR Temp) <> CLI$_ABSENT  THEN
	    READV (Temp,Outtab_Width) ;
	 IF (Outtab_Width < Min_Tabw) OR (Outtab_Width > Max_Tabw)  THEN
	    BEGIN
	    WRITELN ('%PP-F-BADTABWID, Bad Tab width: must be between ',
		     Min_Tabw:1,' and ',Max_Tabw:1,'.') ;
	    $EXIT ;
	    END ;
	 IF Intab_Width = 0  THEN
	    Intab_Width := Outtab_Width ;
	 END ;

  Current_Token_Style := Nochange ;		{ Choose any initial default }

  IF Get_Qualifier ('ALL')  THEN
     BEGIN
     Token_Style[Identifier] := Current_Token_Style ;
     Token_Style[Reserved]   := Current_Token_Style ;
     Token_Style[Dollar]     := Current_Token_Style ;
     END
   ELSE
     BEGIN
     IF Get_Qualifier ('IDENTIFIERS')  THEN
	Token_Style[Identifier] := Current_Token_Style ;
     IF Get_Qualifier ('RESERVED_SYMBOLS')  THEN
	Token_Style[Reserved] := Current_Token_Style ;
     IF Get_Qualifier ('DOLLARS')  THEN
	Token_Style[Dollar] := Current_Token_Style ;
     END ;

  Logging  := Check_Qualifier ('LOG') ;
  Warnings := Check_Qualifier ('WARNINGS') ;

  Widescreen := Check_Qualifier ('WIDE_SCREEN') ;
  IF Widescreen  THEN
     Screen_Width := 132
   ELSE
     Screen_Width := 80 ;
  END ; 	{ of Initialize }


PROCEDURE Clean_Up_And_Report ;

{ All done.  Give some info to user }

  BEGIN 	{ Clean_Up_And_Report }
  IF Linecnt = 0  THEN	{ Empty source file }
     WRITELN ('%PP-I-EMPTYFILE, Empty Pascal source file - no action taken.')
   ELSE
  IF Logging  THEN
     BEGIN
     WRITE ('%PP-I-LINES, ',Put_Count (Linecnt,'line')) ;
     IF Reformatting  THEN
	IF Linecnt = 1	THEN
	   IF Reform_Count = 1	THEN
	      WRITE (' (Reformatted)')
	    ELSE
	      WRITE (' (Not Reformatted)')
	 ELSE
	   BEGIN
	   IF Reform_Count = 0	THEN
	      WRITE (' (None')
	    ELSE
	      WRITE (' (',Reform_Count:1) ;
	   WRITE (' Reformatted)') ;
	   END
      ELSE
	WRITE (' (Reformatting not requested)') ;

     IF Chars_Purged_Count > 0	THEN
	WRITE (' [',Put_Count (Chars_Purged_Count,'blank'),' purged]') ;

     WRITELN ;
     WRITELN ('%PP-I-FOUND, Found  ',
		Put_Count (Count[Identifier],'identifier'),'  ',
		Put_Count (Count[Reserved],'reserved symbol'),'  ',
		Put_Count (Count[Dollar],'dollar symbol')) ;
     IF (Mods[Identifier] + Mods[Reserved] + Mods[Dollar]) = 0	THEN
	WRITELN ('%PP-I-NOCHANGES, No changes made.')
      ELSE
	BEGIN
	WRITE ('%PP-I-CHANGED, Changed  ') ;
	IF Mods[Identifier] > 0  THEN
	   WRITE (Put_Count (Mods[Identifier],'identifier'),'  ') ;
	IF Mods[Reserved] > 0  THEN
	   WRITE (Put_Count (Mods[Reserved],'reserved symbol'),'  ') ;
	IF Mods[Dollar] > 0  THEN
	   WRITE (Put_Count (Mods[Dollar],'dollar symbol')) ;
	WRITELN ;
	END ;
     END ;

  IF Incomment	THEN
     WRITELN ('%PP-E-UNFINCOM, Program Ended in an Unfinished Comment.',Bell)
   ELSE
  IF Inquote  THEN
     WRITELN ('%PP-E-UNFINSTR, Program Ended in an Unfinished String.',Bell) ;

  IF (Linecnt > 0) AND
     ((Reform_Count > 0) OR
      (Mods[Reserved] > 0) OR
      (Mods[Identifier] > 0) OR
      (Mods[Dollar] > 0) OR
      (Chars_Purged_Count > 0))  THEN
     BEGIN
     CLOSE (Outprog,DISPOSITION:=Save,Error:=CONTINUE) ;
     Rst := STATUS (Outprog) ;
     IF Rst = PAS$K_SUCCESS  THEN
	BEGIN
	IF Logging  THEN
	   WRITELN ('%PP-I-NEWFILE, New Pascal source file created.') ;
	END
      ELSE
	WRITELN ('%PP-E-ERRDURCLO, Error during close of OUTPUT file; ',
		 'Pascal status = ',Rst:1) ;
     END
   ELSE
     BEGIN
     CLOSE (Outprog,DISPOSITION:=DELETE,Error:=CONTINUE) ;
     Rst := STATUS (Outprog) ;
     IF Rst <> PAS$K_SUCCESS  THEN
	WRITELN ('%PP-E-ERRDURCLO, Error during close of OUTPUT file; ',
		 'Pascal status = ',Rst:1) ;
     END ;
  END ; 	{ of Clean_Up_And_Report }


{************************************************************************
 * * * * * * * * * * * * * * *	 M  A  I  N   * * * * * * * * * * * * * *
 ************************************************************************}


BEGIN	{ Pascal Pre-processor }

Initialize ;

IF Mentor  THEN
   Teach
 ELSE
IF Open_Files  THEN
   BEGIN
   IF Logging  THEN
      WRITELN ('%PP-I-FILENAME, ',Pas_Source) ; { Identify ourselves }

   Underway := TRUE ;
   Chars_Purged_Count := 0 ;

   WHILE NOT EOF(Inprog)  DO			{ Keep going till all done }
     BEGIN
     READLN (Inprog,Indata) ;
     Original_Length := Indata.LENGTH ;
     STR$TRIM (%DESCR Indata,%DESCR Indata) ;	{ Trim trailing blanks & tabs }
     Linecnt := Linecnt + 1 ;
     Tokenstart := 1 ;				{ New record }
     Tokenlen := 0 ;
     Inreclen := LENGTH (Indata) ;
     IF (Inreclen > Screen_Width) AND Warnings	THEN
	WRITELN ('%PP-W-LONGLINE, Line ',Linecnt:1,' exceeds standard length (',
		 Screen_Width:1,') by ',
		 Put_Count (Inreclen-Screen_Width,'character'),'.') ;
     IF Inquote  THEN
	BEGIN
	WRITELN ('%PP-E-STRSPAN, Quoted string spans record end @ line ',
		 (Linecnt-1):1,'.',Bell) ;
	WRITELN ('%PP-W-STRTERM, Quoted string presumed to be terminated.') ;
	Inquote := FALSE ;
	END ;

     WHILE Gettoken  DO
       BEGIN
       IF Current_Token_Type = Unknown	THEN
	  IF Lookup_Token  THEN
	     Current_Token_Type := Reserved
	   ELSE
	     Current_Token_Type := Identifier ;

       { Ignore any tokens that start with a digit: they are either numbers or
	 invalid identifiers. }

       IF NOT ((Current_Token_Type = Identifier) AND (Token[1] IN Digits)) THEN
	  BEGIN
	  Count[Current_Token_Type] := Count[Current_Token_Type] + 1 ;
	  Translate_Token ;
	  END ;
       END ;

     IF Reformatting  THEN  Formatter ;

     Inreclen := Indata.LENGTH ;
     IF Inreclen <> Original_Length  THEN
	Chars_Purged_Count := Chars_Purged_Count + (Original_Length - Inreclen);

     WRITELN (Outprog,Indata) ;
     END ;	{ of While }

   Clean_Up_And_Report ;
   END ;	{ of IF Open_Files }

END.	{ of Pascal Pre-processor }
