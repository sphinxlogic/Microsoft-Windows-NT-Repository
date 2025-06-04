{************************************************************************
*									*
*  SD  =  Set Default.	A program to set the default disk and directory *
*  of a process.  SD also optionally may update the session prompt of	*
*  an interactive session to reflect the current default directory.	*
*									*
*  SD is stack-based, storing as many past directories as one requires	*
*  in one of two styles for recall.  Flexible options allow easy	*
*  configuration of the program.					*
*									*
*  See the DCL help file for details of command syntax. 		*
*									*
*  The program must be installed with SYSNAM privilege to allow 	*
*  supervisor mode logical name definitions.				*
*									*
*.......................................................................*
*  Author:  Jonathan Ridler.						*
*	    Information Technology Services,				*
*	    The University of Melbourne.				*
*	    E-mail:  jonathan@unimelb.edu.au				*
*.......................................................................*
*  History:								*
*	29-Nov-1994	JER	Original version.			*
*	05-Dec-1994	JER	Allow Help subtopic specification.	*
*	13-Feb-1995	JER	Cope with parser errors.		*
*	23-Feb-1995	JER	Correct problem with Swap_With_Current. *
*	01-Mar-1995	JER	Fix handling underscore in diskname.	*
*	09-Aug-1995	JER	Fix minor bug in Swap_With_Current.	*
*	27-Sep-1995	JER	Recognise and flag as unsupported the	*
*				use of a node access string.		*
*	27-Nov-1995	JER	Minor fix to character sets.		*
*	18-Jan-1996	JER	Handle empty directory as first entry	*
*				logical name search list.		*
*	20-Feb-1996	JER	Implement Quiet option. 		*
*	11-Mar-1996	JER	Add shortcut operator ("`") for going	*
*				up directory tree.			*
*	13-Mar-1996	JER	Fix unnecessary msgs from first parse.	*
*	08-Jul-1996	JER	Handle empty wildcarded directory specs *
*				more neatly.				*
*	28-Aug-1996	JER	Eliminate version numbers from dirspec. *
*	16-Jan-1997	JER	Ensure dynamic stack re-size fails;	*
*				Implement 'G' (Get) option.		*
* v3.0	30-Jul-1998	JER	Prevent multiple sequential pushes of	*
*				the same directory onto the stack.	*
* v3.1	29-Jan-1999	JER	Fix directory variations on "[" & "]".	*
* v3.2	17-Aug-1999	JER	Add 'D' (Delete slot) option.		*
************************************************************************}

[ INHERIT ('SYS$LIBRARY:STARLET',
	   'SYS$LIBRARY:PASCAL$LIB_ROUTINES',
	   'SYS$LIBRARY:PASCAL$LBR_ROUTINES',
	   'SYS$LIBRARY:PASCAL$MTH_ROUTINES') ]

PROGRAM Set_Default (OUTPUT,Data) ;

TYPE

{ Define stack style types: Absolute == Fixed; Relative == Sliding }

Stack_Type   = (Absolute,Fixed,Relative,Sliding) ;

Prompt_Style = (Centre,Left) ;

CONST

Null = ''(0) ;

Default_Stack_Size = 20 ;
Max_Stack_Size = 200 ;			{ Set Max_Digs accordingly }
Max_Digs = 3 ;				{ Digits in Max_Stack_Size }
Initial_Tos = -9999 ;

Digits = ['0'..'9'] ;

Max_Prompt_Size     = 32 ;		{ Limit in DCL (OpenVMS v6.2) }
Default_Prompt_Size = Max_Prompt_Size ;
Default_Head   = Null ;
Default_Tail   = ' ' ;

Empty_Stack_Msg = '%SD-I-EMPTY, Directory stack is empty.' ;

TYPE

$UBYTE = [BYTE] 0..255 ;

$UWORD = [WORD] 0..65535 ;

Status_Block_Type = [UNSAFE] PACKED ARRAY [1..4] OF $UWORD ;

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

VAR

Quiet , 				{ Display new default directory ? }
Undo ,					{ Undo expansion errors ? }
Expanding ,				{ Expanding a spec ? }
Phantom_Dir ,				{ Directory does not exist ? }
Phantom_File ,				{ File does not exist ? }
Update_Prompt : BOOLEAN := FALSE ;	{ Make prompt reflect current dir ? }

Stack_Fixed : BOOLEAN := TRUE ; 	{ Stack style is FIXED ? }

Poppy , 				{ Stack slot number for Pop }
Poppy2 ,				{ Stack slot number for Pop too! }
Table , 				{ Symbol definition table }
Tos ,					{ Top-Of-Stack }
Bos ,					{ Bottom-Of-Stack }
Tos_Vs ,				{ Top-Of-Stack	  Virtual slot }
Bos_Vs ,				{ Bottom-Of-Stack Virtual slot }
Depth , 				{ Stack depth }
Max_Prompt_Size_User : INTEGER := 0 ;	{ Maximum prompt size set by user }

Stack_Size : INTEGER := Default_Stack_Size ;	{ Number of stack slots }

Stack_Style : Stack_Type := Absolute ;		{ Stack slot numbering scheme }

Tos_Str ,				  { Top-Of-Stack    number as a string }
Bos_Str ,				  { Bottom-Of-Stack number as a string }
Depth_Str ,				  { Depth-Of-Stack  number as a string }
Str_Poppy ,				  { Slot number as a string }
Str_Poppy2 : VARYING [4] OF CHAR := '' ;  { Slot number as a string too! }

Prompt_Format : Prompt_Style ;		{ Where to squeeze prompt overflow }

Prompt_Head ,					{ Start of prompt }
Prompt_Tail : VARYING [12] OF CHAR := '' ;	{ End	of prompt }

Symbol ,				{ Scratch symbol register }
Symbol2 ,				{ Scratch symbol register too! }
Search_Result , 			{ Filename returned from $SEARCH }
Target_Dir ,				{ NEW default directory }
Default_Dir ,				{ Default directory }
Current_Dir ,				{ Current directory }
Cmd ,					{ Collapsed  input line }
Command : VARYING [255] OF CHAR := '' ; { Compressed input line }

Data : TEXT ;				{ File of directory specifications }


[ASYNCHRONOUS] FUNCTION SYS$SETDDIR (
	New_Dir_Addr :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Length_Addr : [VOLATILE] $UWORD := %IMMED 0 ;
	%STDESCR Cur_Dir_Addr :
		PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;


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


PROCEDURE Set_Logical_Name (
		Lognam : [CLASS_S] PACKED ARRAY [L1..U1:INTEGER] OF CHAR ;
		Xlate  : [CLASS_S] PACKED ARRAY [L2..U2:INTEGER] OF CHAR ;
		Table  : [CLASS_S] PACKED ARRAY [L3..U3:INTEGER] OF CHAR :=
				'LNM$FILE_DEV' ;
		Mode   : $UBYTE := PSL$C_SUPER
		) ;

{ Define the logical name specified.  BEWARE!  SYSNAM privilege is REQUIRED
  for supervisor mode or greater. }

  VAR Rst : UNSIGNED := 0 ;
      Item_List : Item_List_Template (2) := ZERO ;

  BEGIN       { Set_Logical_Name }
  Item_List[1].Buffer_Length := U2 ;
  Item_List[1].Item_Code     := LNM$_STRING ;
  Item_List[1].Buffer_Addr   := IADDRESS (Xlate) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Terminator    := 0 ;

  Rst := $CRELNM (Tabnam := Table,
		  Lognam := Lognam,
		  Acmode := Mode,
		  Itmlst := Item_List) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ;       { of Set_Logical_Name }


PROCEDURE Get_Command ;

{ Get the foreign command input line. }

  VAR	Rst : UNSIGNED := 0 ;

  BEGIN 	{ Get_Command }
  Rst := LIB$GET_FOREIGN (%DESCR Command) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { Compress it. }

  Rst := Str_Compress (Command,Command,FALSE) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { One is both the same. }

  IF (Command.LENGTH > 0) AND_THEN (Command[1] = '/')  THEN
     Command[1] := '\' ;

  { ... and collapse it. }

  Rst := Str_Compress (Command,Cmd,TRUE) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of Get_Command }


PROCEDURE Exit (Msg : VARYING [Len] OF CHAR) ;

{ Just a dinky shorthand routine. }

  BEGIN 	{ Exit }
  WRITELN (Msg) ;
  $EXIT ;
  END ; 	{ of Exit }


PROCEDURE Change_Prompt (BODY	 : VARYING [Len] OF CHAR ;
			 Literal : BOOLEAN := FALSE) ;

{ Modify the session prompt.  Using LIB$DO_COMMAND to do this means that this
  routine terminates the program.  It must therefore be the last thing called
  before the desired end of program logic. }

  VAR	Rst : UNSIGNED := 0 ;
	Max_Avail , Max_Div2 : INTEGER := 0 ;
	Prompt : VARYING [Max_Prompt_Size] OF CHAR := '' ;

  BEGIN 	{ Change_Prompt }
  IF Prompt_Head = Null  THEN
     Prompt_Head := '' ;
  Max_Avail := Max_Prompt_Size_User - Prompt_Head.LENGTH - Prompt_Tail.LENGTH ;
  Max_Div2  := (Max_Avail - 1) DIV 2 ;
  IF Literal  THEN
     IF BODY.LENGTH > Max_Prompt_Size  THEN
	Exit ('%SD-F-INTERROR, Bad prompt literal - notify Systems Programmer.')
      ELSE
	Prompt := BODY
   ELSE
  IF BODY.LENGTH <= Max_Avail  THEN
     Prompt := Prompt_Head + BODY + Prompt_Tail
   ELSE
  IF Prompt_Format = Centre  THEN
     Prompt := Prompt_Head + SUBSTR (BODY,1,Max_Div2) + '*' +
	       SUBSTR (BODY,BODY.LENGTH-Max_Div2+1,Max_Div2) + Prompt_Tail
   ELSE
  IF Prompt_Format = Left  THEN
     BEGIN
     Max_Avail := Max_Div2 * 2 ;
     Prompt := Prompt_Head + '*' +
	       SUBSTR (BODY,BODY.LENGTH-Max_Avail+1,Max_Avail) + Prompt_Tail ;
     END ;

  Rst := LIB$DO_COMMAND ('$ SET PROMPT = "' + Prompt + '"') ;	{ Black Hole!! }
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of Change_Prompt }


PROCEDURE Set_New_Default (Dir	  : VARYING [Len] OF CHAR ;
			   Notify : BOOLEAN := TRUE) ;

{ Set new default directory and new default disk. }

  VAR	Rst : UNSIGNED := 0 ;
	Colon : INTEGER := 0 ;
	Def_Dev : VARYING  [32] OF CHAR := '' ;
	Def_Dir : VARYING [255] OF CHAR := '' ;

  BEGIN 	{ Set_New_Default }
  Colon := INDEX (Dir,':') ;
  IF Colon = 0	THEN
     Exit ('%SD-F-BADDIR, Directory specification is corrupt.')
   ELSE
     BEGIN
     IF Dir [Colon+1] = ":"  THEN	{ Double colon -> node name }
	Exit ('%SD-F-NONODE, Node name specification is not supported.') ;
     Def_Dev := SUBSTR (Dir,1,Colon) ;
     Def_Dir := SUBSTR (Dir,Colon+1,Dir.LENGTH-Colon) ;
     END ;

  Rst := SYS$SETDDIR (SUBSTR (Def_Dir.BODY,1,Def_Dir.LENGTH)) ;
  IF NOT ODD (Rst)  THEN
     BEGIN
     IF Undo AND (Current_Dir <> '')  THEN
	Set_New_Default (Current_Dir,FALSE) ;	{ Recurse! }
     LIB$SIGNAL (Rst) ;
     END ;

  Set_Logical_Name ('SYS$DISK',Def_Dev) ;

  IF Notify  THEN
     BEGIN
     IF NOT Quiet  THEN  WRITELN (Def_Dev,Def_Dir) ;
     IF Phantom_Dir  THEN
	WRITELN ('%SD-W-NODIR, Directory does not exist.') ;
     END ;
  END ; 	{ of Set_New_Default }


PROCEDURE Parse_Filename (File_Spec    : VARYING [Len1] OF CHAR ;
			  Default_Spec : VARYING [Len2] OF CHAR := '[]*.*' ;
			  First_Parse  : BOOLEAN := FALSE) ;

{ Perform basic RMS file specification parsing. }

  VAR	Rst : UNSIGNED := 0 ;
	Saved_Length ,			{ Length of useful saved $PARSE result }
	Zap : INTEGER  := 0 ;		{ Offset for underscore in diskname }
	Fab : FAB$TYPE := ZERO ;	{ File Access Block }
	Nam : NAM$TYPE := ZERO ;	{ File Name Block }
	Wild_Dir ,			{ Wildcard in directory spec ? }
	Use_Saved : BOOLEAN := FALSE ;	{ Ignore $SEARCH result ? }
	Parse_Result ,
	Saved_Result : VARYING [255] OF CHAR := '' ;

  BEGIN 	{ Parse_Filename }
  Fab.FAB$B_BID := FAB$C_BID ;				{ FAB Block ID }
  Fab.FAB$B_BLN := FAB$C_BLN ;				{ FAB Block Length }
  Fab.FAB$L_FNA := IADDRESS (File_Spec.BODY) ;		{ Primary File Spec }
  Fab.FAB$B_FNS := Len1 ;				{ 1ry File Spec Size }
  Fab.FAB$L_DNA := IADDRESS (Default_Spec.BODY) ;	{ Default File Spec }
  Fab.FAB$B_DNS := Len2 ;				{ Def File Spec Size }
  Fab.FAB$L_NAM := IADDRESS (Nam) ;			{ NAM Block Address }

  Nam.NAM$B_BID := NAM$C_BID ;				{ NAM Block ID }
  Nam.NAM$B_BLN := NAM$C_BLN ;				{ NAM Block Length }
  Nam.NAM$L_ESA := IADDRESS (Parse_Result.BODY) ;	{ Expanded String Area }
  Nam.NAM$B_ESS := SIZE (Parse_Result.BODY) ;		{ Max Expand Strng Syz }
  Nam.NAM$L_RSA := IADDRESS (Search_Result.BODY) ;	{ Resultant Strng Area }
  Nam.NAM$B_RSS := SIZE (Search_Result.BODY) ;		{ Max Result Strng Syz }

  Rst := $PARSE (Fab) ;
  IF ODD (Rst) OR (Rst = RMS$_DNF)  THEN
     BEGIN
     Parse_Result.LENGTH := Nam.NAM$B_ESL ;
     Saved_Result := Parse_Result ;
     Saved_Length := Nam.NAM$B_DEV + Nam.NAM$B_DIR ;
     Wild_Dir := Nam.NAM$V_WILD_DIR ;
     IF Rst = RMS$_DNF	THEN
	Search_Result := Parse_Result
      ELSE
	BEGIN
	Rst := $SEARCH (Fab) ;
	IF ODD (Rst) OR (Rst = RMS$_FNF) OR (Rst = RMS$_DNF) OR
	   (Rst = RMS$_PRV)  THEN
	   BEGIN

	   { With logical name search-lists which have an empty target directory
	     in the first directory in the list, the parse succeeds, but the
	     search fails with RMS$_DNF.  In this case, use the original result
	     from the $PARSE because the $SEARCH will change the Parse_Result
	     again. }

	   IF Rst = RMS$_DNF  THEN
	      BEGIN
	      Use_Saved := TRUE ;
	      Search_Result := Saved_Result ;
	      IF Wild_Dir AND Undo AND (Current_Dir <> '')  THEN
		 BEGIN
		 Set_New_Default (Current_Dir,FALSE) ;
		 LIB$SIGNAL (Rst) ;
		 END
	       ELSE
		 Rst := RMS$_NORMAL ;	{ Prevent flagging no directory }
	      END
	    ELSE
	   IF (Rst = RMS$_FNF) AND Wild_Dir  THEN
	      Exit ('%SD-F-EMPTYWILDDIR, Cannot determine name ' +
		    'of empty wildcard directory.')
	    ELSE
	      Search_Result.LENGTH := Nam.NAM$B_RSL ;
	   IF Search_Result[1] = '_'  THEN
	      BEGIN
	      Search_Result := SUBSTR (Search_Result,2,Search_Result.LENGTH-1) ;
	      Zap := 1 ;
	      END ;
	   END
	 ELSE
	   BEGIN
	   IF Undo AND (Current_Dir <> '') THEN
	      Set_New_Default (Current_Dir,FALSE) ;
	   LIB$SIGNAL (Rst) ;
	   END ;
	END ;
     Target_Dir := Search_Result ;
     IF Use_Saved  THEN
	Target_Dir.LENGTH := Saved_Length - Zap
      ELSE
	Target_Dir.LENGTH := Nam.NAM$B_DEV + Nam.NAM$B_DIR - Zap ;
     Phantom_Dir  := (Rst = RMS$_DNF) AND NOT First_Parse ;
     Phantom_File := (Rst = RMS$_FNF) OR Phantom_Dir ;
     IF (Rst = RMS$_PRV) AND NOT First_Parse  THEN
	WRITELN ('%SD-E-NOPRIV, Insufficient privilege or file protection ',
		 'violation on directory.') ;
     END
   ELSE
     BEGIN
     Phantom_Dir := FALSE ;
     IF Undo AND (Current_Dir <> '') THEN  Set_New_Default (Current_Dir,FALSE) ;
     IF Rst = RMS$_DNR	THEN
	$EXIT (Rst)
      ELSE
	LIB$SIGNAL (Rst) ;
     END ;
  END ; 	{ of Parse_Filename }


PROCEDURE Zip (Rst : UNSIGNED) ;

{ Another very dinky shorthand routine. }

  BEGIN 	{ Zip }
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of Zip }


PROCEDURE Get_Symbol (Cymbal : VARYING [Len] OF CHAR) ;

{ Get the symbol definition if it exists. }

  VAR	Rst : UNSIGNED := 0 ;

  BEGIN 	{ Get_Symbol }
  Rst := LIB$GET_SYMBOL (Cymbal,%DESCR Symbol,,Table) ;
  IF NOT ODD (Rst)	THEN
     IF Rst = LIB$_NOSUCHSYM  THEN
	Exit ('%SD-F-BADSTACK, Directory stack is corrupted.')
      ELSE
	LIB$SIGNAL (Rst) ;
  END ; 	{ of Get_Symbol }


PROCEDURE Set_Symbol (Cymbal_Nam : VARYING [Len1] OF CHAR ;
		      Cymbal_Val : VARYING [Len2] OF CHAR) ;

{ Define the symbol globally. }

  VAR	Rst : UNSIGNED := 0 ;

  BEGIN 	{ Set_Symbol }
  Rst := LIB$SET_SYMBOL (Cymbal_Nam,Cymbal_Val,LIB$K_CLI_GLOBAL_SYM) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of Set_Symbol }


PROCEDURE Get_Number (Source  : VARYING [Len1] OF CHAR ;
		      VAR Num : INTEGER ;
		      Msg     : VARYING [Len2] OF CHAR) ;

{ From a string variable, read a number. }

  BEGIN 	{ Get_Number }
  READV (Source,Num,Error:=CONTINUE) ;
  IF STATUSV <> 0  THEN
     Exit (Msg) ;
  END ; 	{ of Get_Number }


FUNCTION Rs (Vsn : INTEGER) : INTEGER ;

{ The left hemisphere of the brain of the stack - return the real slot number of
  a given virtual slot number.	Take stack style into account. }

  BEGIN 	{ Rs }
  IF Stack_Fixed  THEN
     Rs := (Vsn - 1) REM Stack_Size + 1
   ELSE
     Rs := Vsn + Bos - 1 - (Stack_Size *
		   ((Vsn > (Stack_Size - Bos + 1)) AND (Depth > 0))::INTEGER)
  END ; 	{ of Rs }


FUNCTION Vs (Rsn : INTEGER) : INTEGER ;

{ The right hemisphere of the brain of the stack - return the virtual slot
  number of a given real slot number.  Take stack style into account. }

  BEGIN 	{ Vs }
  IF Stack_Fixed  THEN
     Vs := Rsn + (Depth - ((Rsn >= Bos) AND (Tos < Bos))::INTEGER) * Stack_Size
   ELSE
     Vs := Rsn - Bos + 1 + (Stack_Size * ((Rsn < Bos) AND (Depth > 0))::INTEGER)
  END ; 	{ of Vs }


PROCEDURE Zero_Stack (Notify : BOOLEAN := FALSE) ;

{ Reset the stack completely. }

  VAR	Rst : UNSIGNED := 0 ;
	I : INTEGER := 0 ;
	Str_I : VARYING [4] OF CHAR := '' ;

  BEGIN 	{ Zero_Stack }
  FOR I := 1 TO Stack_Size  DO
      BEGIN
      Str_I := DEC (I,Max_Digs,Max_Digs) ;
      Rst := LIB$DELETE_SYMBOL ('SD__'+Str_I,LIB$K_CLI_GLOBAL_SYM) ;
      IF NOT ODD (Rst)	THEN
	 IF Rst <> LIB$_NOSUCHSYM  THEN  LIB$SIGNAL (Rst) ;
      END ;

  Zip (LIB$DELETE_SYMBOL ('SD__TS',LIB$K_CLI_GLOBAL_SYM)) ;
  Zip (LIB$DELETE_SYMBOL ('SD__BS',LIB$K_CLI_GLOBAL_SYM)) ;
  Zip (LIB$DELETE_SYMBOL ('SD__SD',LIB$K_CLI_GLOBAL_SYM)) ;
  Zip (LIB$DELETE_SYMBOL ('SD__SZ',LIB$K_CLI_GLOBAL_SYM)) ;

  IF Notify  THEN  Exit ('%SD-S-ZEROED, Directory stack zeroed.') ;
  END ; 	{ of Zero_Stack }


PROCEDURE Get_Prompt_Specs ;

{ Get the user's specifications for the prompt changes.  If we are not
  interactive, ignore update request. }

  VAR	Rst , Mode : INTEGER := 0 ;
	Iosb : Status_Block_Type := ZERO ;
	Item_List : Item_List_Template (2) := ZERO ;

  BEGIN 	{ Get_Prompt_Specs }
  Item_List[1].Buffer_Length := 4 ;
  Item_List[1].Item_Code	:= JPI$_MODE ;
  Item_List[1].Buffer_Addr	:= IADDRESS (Mode) ;
  Item_List[1].Return_Addr	:= 0 ;

  Item_List[2].Terminator	:= 0 ;	{ Terminate the item list }

  Rst := $GETJPIW (Itmlst := Item_List,
		   Iosb   := Iosb) ;

  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1])
   ELSE
  IF Mode = JPI$K_INTERACTIVE	THEN
     BEGIN

     { Header to the prompt? }

     Get_Logical_Name (Lognam  := 'SD_PROMPT_HEAD',
		       Default := Default_Head,
		       Actual  := Prompt_Head) ;

     IF Prompt_Head.LENGTH > 8	THEN
	Exit ('%SD-F-BADHEAD, Prompt head definition is too long.') ;

     { Trailer to the prompt? }

     Get_Logical_Name (Lognam  := 'SD_PROMPT_TAIL',
		       Default := Default_Tail,
		       Actual  := Prompt_Tail) ;

     IF Prompt_Tail.LENGTH > 6	THEN
	Exit ('%SD-F-BADTRAIL, Prompt tail definition is too long.') ;

     { The prompt can be squeezed in the centre or at the left - which one? }

     Get_Logical_Name (Lognam  := 'SD_PROMPT_FORMAT',
		       Default := 'LEFT',
		       Actual  := Symbol) ;

     READV (Symbol,Prompt_Format,Error:=CONTINUE) ;
     IF STATUSV <> 0  THEN
	Exit ('%SD-F-BADFORM, Prompt format definition is invalid.') ;

     IF (Prompt_Format <> Centre) AND
	(Prompt_Format <> Left)   THEN
	Exit ('%SD-F-INVFORM, Prompt format definition is invalid.') ;

     { Determine the maximum prompt size. }

     Get_Logical_Name (Lognam  := 'SD_MAX_PROMPT_SIZE',
		       Default := DEC (Default_Prompt_Size),
		       Actual  := Symbol) ;

     Get_Number (Symbol,Max_Prompt_Size_User,
		 '%SD-F-BADPSIZE, Maximum prompt size is invalid.') ;
     IF (Max_Prompt_Size_User < 2) OR
	(Max_Prompt_Size_User > Max_Prompt_Size)  THEN
	Exit ('%SD-F-INVPSIZE, Maximum prompt size is invalid.') ;
     END
   ELSE			{ Don't do it }
     BEGIN
     WRITELN ('%SD-W-NOTINTER, Not interactive - update prompt ignored.') ;
     Update_Prompt := FALSE ;
     END ;
  END ; 	{ of Get_Prompt_Specs }


PROCEDURE Initialize_Stack ;

{ Find and validate all logical name definitions and set appropriate variables;
  check the stack state, possibly initializing it. }

  VAR	Rst , Stack_Size_Sym : INTEGER := 0 ;

  BEGIN 	{ Initialize_Stack }

  { Size of stack, i.e. number of real slots. }

  Get_Logical_Name (Lognam  := 'SD_STACK_SIZE',
		    Default := DEC (Default_Stack_Size),
		    Actual  := Symbol) ;

  Get_Number (Symbol,Stack_Size,
	      '%SD-F-BADSIZE, Directory stack size is invalid.') ;
  IF (Stack_Size < 2) OR (Stack_Size > Max_Stack_Size)	THEN
     Exit ('%SD-F-INVSIZE, Directory stack size is invalid.') ;

  { Ensure consistency of stack size across invocations. }

  Rst := LIB$GET_SYMBOL ('SD__SZ',%DESCR Symbol,,Table) ;     { Size last time }
  IF NOT ODD (Rst)  THEN
     IF Rst = LIB$_NOSUCHSYM  THEN			      { Zeroed stack }
	Set_Symbol ('SD__SZ',DEC (Stack_Size,Max_Digs,Max_Digs))
      ELSE
	LIB$SIGNAL (Rst)
   ELSE
     BEGIN
     Get_Number (Symbol,Stack_Size_Sym,
		 '%SD-F-BADSYMSIZ, Directory stack pointer is corrupted.') ;
     IF Stack_Size_Sym <> Stack_Size  THEN
	BEGIN

	{ Warning required - ignore invalid value. }

	WRITELN ('%SD-E-NODYNSIZE, Dynamic re-sizing of stack not supported.') ;
	WRITELN ('%SD-I-STACKSIZE, Current stack size is ',Stack_Size_Sym:1,
		 ' slots.') ;
	Stack_Size := Stack_Size_Sym ;		{ Use valid value only }
	END ;
     END ;

  { Determine stack style. }

  Get_Logical_Name (Lognam  := 'SD_STACK_STYLE',
		    Default := 'ABSOLUTE',
		    Actual  := Symbol) ;

  READV (Symbol,Stack_Style,Error:=CONTINUE) ;
  IF STATUSV <> 0  THEN
     Exit ('%SD-F-BADSTYLE, Directory stack style is invalid.') ;
  Stack_Fixed := (Stack_Style = Absolute) OR (Stack_Style = Fixed) ;

  { Determine if we must undo changes to default disk and directory if an error
    occurs during expansion of the specification. }

  Get_Logical_Name (Lognam  := 'SD_UNDO_IF_ERROR',
		    Default := 'TRUE',
		    Actual  := Symbol) ;

  READV (Symbol,Undo,Error:=CONTINUE) ;
  IF STATUSV <> 0  THEN
     Exit ('%SD-F-BADLNM, Logical name is invalid: SD_UNDO_IF_ERROR') ;

  { Change and keep the prompt up-to-date? }

  Get_Logical_Name (Lognam  := 'SD_UPDATE_PROMPT',
		    Default := 'FALSE',
		    Actual  := Symbol) ;

  READV (Symbol,Update_Prompt,Error:=CONTINUE) ;
  IF STATUSV <> 0  THEN
     Exit ('%SD-F-BADLNM, Logical name is invalid: SD_UPDATE_PROMPT') ;

  IF Update_Prompt  THEN
     Get_Prompt_Specs ;

  { Determine if new directory names are to be displayed - the Quiet option? }

  Get_Logical_Name (Lognam  := 'SD_QUIET',
		    Default := 'FALSE',
		    Actual  := Symbol) ;

  READV (Symbol,Quiet,Error:=CONTINUE) ;
  IF STATUSV <> 0  THEN
     Exit ('%SD-F-BADLNM, Logical name is invalid: SD_QUIET') ;

  { Check the state of the stack - start with Top-Of-Stack.  If TOS is not
    defined, assume the stack needs to be initialized. }

  Rst := LIB$GET_SYMBOL ('SD__TS',%DESCR Symbol,,Table) ;
  IF NOT ODD (Rst)  THEN
     IF Rst = LIB$_NOSUCHSYM  THEN
	Symbol := DEC (Initial_Tos)
      ELSE
	LIB$SIGNAL (Rst)
   ELSE
  IF Table <> LIB$K_CLI_GLOBAL_SYM  THEN
     Exit ('%SD-F-BADTABLE, Directory stack symbol is not global.') ;

  Get_Number (Symbol,Tos,
	      '%SD-F-BADTOS, Directory stack pointer is corrupted.') ;

  IF Tos = Initial_Tos	THEN
     BEGIN
     Tos := 0 ;
     Tos_Str := '0' ;
     END
   ELSE
  IF (Tos < 1) OR (Tos > Stack_Size)  THEN
     Exit ('%SD-F-INVTOS, Directory stack pointer is invalid.')
   ELSE
     BEGIN
     Tos_Str := DEC (Tos,Max_Digs,Max_Digs) ;

     Get_Symbol ('SD__BS') ;		{ Bottom }
     Get_Number (Symbol,Bos,
		 '%SD-F-BADBOS, Directory stack pointer is corrupted.') ;
     Bos_Str := DEC (Bos,Max_Digs,Max_Digs) ;

     Get_Symbol ('SD__SD') ;		{ Depth }
     Get_Number (Symbol,Depth,
		 '%SD-F-BADDEPTH, Directory stack pointer is corrupted.') ;
     Depth_Str := DEC (Depth,Max_Digs,Max_Digs) ;
     END ;
  END ; 	{ of Initialize_Stack }


PROCEDURE Push ;

{ Push the stack *pointers* to reflect a directory push onto the stack. }

  BEGIN 	{ Push }
  IF Tos = 0  THEN
     BEGIN
     Tos := 1 ;
     Bos := 1 ;
     END
   ELSE
     BEGIN
     Tos := Tos + 1 ;
     IF Tos > Stack_Size  THEN
	BEGIN
	Tos := 1 ;
	Depth := Depth + 1 ;
	END ;
     IF Tos = Bos  THEN
	BEGIN
	Bos := Bos + 1 ;
	IF Bos > Stack_Size  THEN  Bos := 1 ;
	END ;
     END ;
  Tos_Str := DEC (Tos,Max_Digs,Max_Digs) ;
  Bos_Str := DEC (Bos,Max_Digs,Max_Digs) ;
  Depth_Str := DEC (Depth,Max_Digs,Max_Digs) ;
  END ; 	{ of Push }


PROCEDURE Pop ;

{ Pop the stack *pointers* to reflect a directory pop from the stack. }

  BEGIN 	{ Pop }
  IF Tos = Bos	THEN	{ Last in stack - re-initialize }
     Zero_Stack
   ELSE
     BEGIN
     Tos := Tos - 1 ;
     IF (Tos = 0) AND (Depth > 0)  THEN
	BEGIN
	Tos := Stack_Size ;
	Depth := Depth - 1 ;
	END ;
     Tos_Str := DEC (Tos,Max_Digs,Max_Digs) ;
     Bos_Str := DEC (Bos,Max_Digs,Max_Digs) ;
     Depth_Str := DEC (Depth,Max_Digs,Max_Digs) ;
     END ;
  END ; 	{ of Pop }


PROCEDURE Push_Stack (Dir : VARYING [Len] OF CHAR) ;

{ Push the stack *symbols* to reflect a directory push onto the stack. }

  BEGIN 	{ Push_Stack }
  Push ;
  Set_Symbol ('SD__'+Tos_Str,Dir) ;
  Set_Symbol ('SD__TS',Tos_Str) ;
  Set_Symbol ('SD__BS',Bos_Str) ;
  Set_Symbol ('SD__SD',Depth_Str) ;
  END ; 	{ of Push_Stack }


FUNCTION Getuai (User : VARYING [Len] OF CHAR) : BOOLEAN ;

{ Get the required information for the particular user from the UAF. }

  VAR	Rst : INTEGER := 0 ;
	Def_Dev , Def_Dir : VARYING [255] OF CHAR := '' ;
	Item_List : Item_List_Template (3) := ZERO ;

  BEGIN 	{ Getuai }
  Item_List[1].Buffer_Length := SIZE (Def_Dir) ;
  Item_List[1].Item_Code     := UAI$_DEFDIR ;
  Item_List[1].Buffer_Addr   := IADDRESS (Def_Dir.BODY) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Buffer_Length := SIZE (Def_Dev) ;
  Item_List[2].Item_Code     := UAI$_DEFDEV ;
  Item_List[2].Buffer_Addr   := IADDRESS (Def_Dev.BODY) ;
  Item_List[2].Return_Addr   := 0 ;

  Item_List[3].Terminator    := 0 ;	{ Terminate the item list }

  Rst := $GETUAI (Usrnam := %STDESCR SUBSTR (User,1,User.LENGTH),
		  Itmlst := Item_List) ;

  Getuai := ODD (Rst) ;

  IF NOT ODD (Rst)  THEN
     BEGIN
     IF Rst = RMS$_RNF	THEN
	Exit ('%SD-F-NOSUCHUSER, Username does not exist.')
      ELSE
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     BEGIN
     Def_Dir.LENGTH := INT (Def_Dir.BODY[1]) ;
     Def_Dev.LENGTH := INT (Def_Dev.BODY[1]) ;
     Def_Dir.BODY := SUBSTR (Def_Dir.BODY,2,Def_Dir.LENGTH) ;
     Def_Dev.BODY := SUBSTR (Def_Dev.BODY,2,Def_Dev.LENGTH) ;
     Target_Dir := Def_Dev + Def_Dir ;
     END ;
  END ; 	{ of Getuai }


PROCEDURE Get_User_Dir ;

{ Determine the default disk and directory of the target user from the UAF. }

  CONST Alpha_Plus = ['a'..'z','A'..'Z','_','-','$'] ;

  VAR	Stop : INTEGER := 0 ;
	User ,
	Stub : VARYING [255] OF CHAR := '' ;	{ Save any extras }

  BEGIN 	{ Get_User_Dir }
  Stub := SUBSTR (Cmd,2,Cmd.LENGTH-1) ;
  Stop := FIND_NONMEMBER (Stub,Alpha_Plus+Digits) ;
  IF Stop = 0  THEN
     User := Stub
   ELSE
     BEGIN
     User := SUBSTR (Stub,1,Stop-1) ;
     Stub := SUBSTR (Stub,Stop,Stub.LENGTH-Stop+1) ;
     END ;
  Getuai (User) ;
  Command := Target_Dir + Stub ;
  Cmd := Command ;
  END ; 	{ of Get_User_Dir }


PROCEDURE Parse_Spec_And_Set_Default ;

{ Parse the input specification and set default as we go.
  Handle all shortcut characters except tilde ("~") here. }

  VAR

  Braced , Brace_It , Done : BOOLEAN := FALSE ;
  Target : INTEGER := 0 ;
  Spec , Stem , Root , Result : VARYING [255] OF CHAR := '' ;

  PROCEDURE Replace_Char (Old_Char ,
			  New_Char : CHAR) ;

  { Replace all old_char characters with new_char characters. }

    BEGIN	{ Replace_Char }
    REPEAT
      Target := INDEX (Root,Old_Char) ;
      IF Target <> 0  THEN  Root[Target] := New_Char ;
    UNTIL Target = 0 ;
    END ;	{ of Replace_Char }

  BEGIN 	{ Parse_Spec_And_Set_Default }
  Root := Cmd ;
  Result := Current_Dir ;
  Expanding := TRUE ;

  { Replace all "`" characters with "-" to allow us to go back up
    levels in the subdirectory tree without the need for braces. }

  Replace_Char ('`','-') ;

  { Replace all curly braces with square brackets to cater for a common typo. }

  Replace_Char ('{','[') ;
  Replace_Char ('}',']') ;

  { Replace all "<" and ">" characters with "[" and "]" to cater for
    the alternative VMS syntax for directories. }

  Replace_Char ('<','[') ;
  Replace_Char ('>',']') ;

  WHILE NOT Done  DO
    BEGIN
    Target := INDEX (Root,',') ;
    IF Target = 0  THEN
       BEGIN
       Stem := Root ;
       Done := TRUE ;
       END
     ELSE
       BEGIN
       Stem := SUBSTR (Root,1,Target-1) ;
       Root := SUBSTR (Root,Target+1,Root.LENGTH-Target) ;
       END ;

    IF Target = 1  THEN 				{ Comma ONLY }
       Set_New_Default (Default_Dir,Done)
     ELSE
       BEGIN
       Target := INDEX (Stem,':') ;			{ Device ? }
       IF Target <> 0  THEN
	  BEGIN
	  Spec := SUBSTR (Stem,1,Target) ;
	  Stem := SUBSTR (Stem,Target+1,Stem.LENGTH-Target) ;
	  END ;

       Target := INDEX (Stem,';') ;			{ Version number ? }
       IF Target <> 0  THEN
	  Stem := SUBSTR (Stem,1,Target-1) ;		{ Get rid of it }

       IF Stem <> ''  THEN
	  BEGIN
	  Braced := INDEX (Stem,'[') <> 0 ;		{ [] ? }
	  Brace_It := (NOT Braced) AND
		      ((Stem[1] = '.') OR
		       (Stem[1] = '-') OR
		       (Stem[1] = '^')) ;		{ MFD? }
	  IF Brace_It  THEN  Spec := Spec + '[' ;
	  IF (Stem.LENGTH > 0) AND_THEN (Stem[1] = '^')  THEN
	     Spec := Spec + '000000' + SUBSTR (Stem,2,Stem.LENGTH-1)
	   ELSE
	     Spec := Spec + Stem ;
	  IF Brace_It  THEN
	     Spec := Spec + ']*.*'
	   ELSE
	  IF Braced  THEN
	     Spec := Spec + '*.*' ;
	  END
	ELSE
	  Spec := Spec + '*.*' ;
       Parse_Filename (Spec,Result) ;
       Result := Target_Dir ;
       Spec := '' ;
       Set_New_Default (Result,Done) ;
       END ;
    END ;
  Expanding := FALSE ;
  END ; 	{ of Parse_Spec_And_Set_Default }


PROCEDURE Show_Stack_Attributes ;

{ Display directory stack attributes. }

  VAR	I : INTEGER := 0 ;

  BEGIN 	{ Show_Stack_Attributes }
  WRITELN ('SD Directory stack attributes:') ;
  WRITELN ;
  WRITELN ('Stack size:  ',Stack_Size:1) ;
  WRITELN ('Stack limit: ',Max_Stack_Size:1) ;
  WRITEV (Symbol,Stack_Style) ;
  Str_Compress (Symbol,Symbol,TRUE) ;
  WRITELN ('Stack style: ',Symbol) ;
  WRITE ('Stack usage: ') ;
  IF Tos = 0  THEN		{ Empty }
     WRITELN ('None.')
   ELSE
     BEGIN
     I := Vs (Tos) - Vs (Bos) + 1 ;
     WRITE (I:1,' slot') ;
     IF I = 1 THEN
	BEGIN
	WRITELN ('.') ;
	WRITELN ('Slot in use: ',Vs(Tos):1) ;
	END
      ELSE
	BEGIN
	WRITELN ('s.') ;
	WRITELN ('Slot range:  ',Vs(Bos):1,' to ',Vs(Tos):1) ;
	END ;
     END ;

  WRITELN ;
  WRITEV (Symbol,Undo) ;
  Str_Compress (Symbol,Symbol,TRUE) ;
  WRITELN ('Undo errors:   ',Symbol) ;
  WRITEV (Symbol,Quiet) ;
  Str_Compress (Symbol,Symbol,TRUE) ;
  WRITELN ('Quiet:         ',Symbol) ;
  WRITEV (Symbol,Update_Prompt) ;
  Str_Compress (Symbol,Symbol,TRUE) ;
  WRITELN ('Update Prompt: ',Symbol) ;
  IF Update_Prompt  THEN
     BEGIN
     WRITEV (Symbol,Prompt_Format) ;
     Str_Compress (Symbol,Symbol,TRUE) ;
     WRITELN ('Prompt format: ',Symbol) ;
     WRITE ('Prompt head:   ') ;
     IF Prompt_Head = Default_Head  THEN
	WRITELN ('Default.')
      ELSE
	WRITELN ('User specified.') ;
     WRITE ('Prompt tail:   ') ;
     IF Prompt_Tail = Default_Tail  THEN
	WRITELN ('Default.')
      ELSE
	WRITELN ('User specified.') ;
     WRITELN ('Max Prompt size: ',Max_Prompt_Size_User:1) ;
     END ;
  $EXIT ;
  END ; 	{ of Show_Stack_Attributes }


PROCEDURE Stack_Typer ;

{ Allow directory stack style to be verified or changed. }

  BEGIN 	{ Stack_Typer }
  IF Cmd.LENGTH = 2  THEN
     BEGIN
     WRITEV (Symbol,Stack_Style) ;
     Str_Compress (Symbol,Symbol,TRUE) ;
     Exit ('%SD-I-STYLE, Directory stack style is ' + Symbol) ;
     END
   ELSE
     BEGIN
     READV (Cmd[3],Stack_Style,Error:=CONTINUE) ;
     IF STATUSV <> 0  THEN
	Exit ('%SD-F-BADSTYLE, Directory stack style is invalid.') ;
     WRITEV (Symbol,Stack_Style) ;
     Str_Compress (Symbol,Symbol,TRUE) ;
     Set_Logical_Name ('SD_STACK_STYLE',Symbol) ;
     Exit ('%SD-S-STYLE, Directory stack style is ' + Symbol) ;
     END ;
  END ; 	{ of Stack_Typer }


PROCEDURE Stack_Undoer ;

{ Allow undo error option to be verified or changed. }

  BEGIN 	{ Stack_Undoer }
  IF Cmd.LENGTH > 2  THEN
     BEGIN
     READV (Cmd[3],Undo,Error:=CONTINUE) ;
     IF STATUSV <> 0  THEN
	Exit ('%SD-F-BADUNDO, Undo value must be "True" or "False".') ;
     WRITEV (Symbol,Undo) ;
     Str_Compress (Symbol,Symbol,TRUE) ;
     Set_Logical_Name ('SD_UNDO_IF_ERROR',Symbol) ;
     END ;

  IF Undo  THEN
     Exit ('%SD-I-UNDO, Errors during expansion will be undone.' )
   ELSE
     Exit ('%SD-I-NOUNDO, Errors during expansion will not be undone.' ) ;
  END ; 	{ of Stack_Undoer }


PROCEDURE List_Stack ;

{ Display the contents of the stack (if any) and the current default disk and
  directory. }

  VAR	I , Sig_Digs : INTEGER := 0 ;
	Str_I : VARYING [4] OF CHAR := '' ;

  BEGIN 	{ List_Stack }
  IF Tos = 0  THEN			{ Empty }
     BEGIN
     WRITELN (Current_Dir) ;
     Exit (Empty_Stack_Msg) ;
     END
   ELSE
     BEGIN
     Sig_Digs := TRUNC (MTH$ALOG10(Vs(Tos))) + 1 ;
     WRITELN (PAD('','-',Sig_Digs),'> ',Current_Dir) ;
     END ;

  FOR I := Vs (Tos) DOWNTO Vs (Bos)  DO
      BEGIN
      Str_I := DEC (Rs(I),Max_Digs,Max_Digs) ;
      Get_Symbol ('SD__'+Str_I) ;
      WRITELN (I:Sig_Digs,': ',Symbol) ;
      END ;
  $EXIT ;
  END ; 	{ of List_Stack }


PROCEDURE Read_File ;

{ Load the stack with the contents of a disk file - one default spec per line. }

  VAR	Count : INTEGER := 0 ;

  BEGIN 	{ Read_File }
  IF Cmd.LENGTH = 2  THEN
     Symbol := 'SD_INPUT_FILE'
   ELSE
     Symbol := SUBSTR (Cmd,3,Cmd.LENGTH-2) ;
  Parse_Filename (Symbol) ;
  IF Phantom_File  THEN
     Exit ('%SD-F-FILNOTFOU, Input file not found.') ;

  OPEN (Data,FILE_NAME:=Search_Result,HISTORY:=Old,SHARING:=READONLY) ;
  RESET (Data) ;
  WRITELN ('%SD-I-READING, Reading stack data from ',Search_Result) ;

  WHILE NOT EOF (Data)	DO
    BEGIN
    READLN (Data,Symbol) ;
    Push_Stack (Symbol) ;
    Count := Count + 1 ;
    END ;

  CLOSE (Data) ;
  IF Count = 1	THEN
     Symbol := 'entry'
   ELSE
     Symbol := 'entries' ;
  WRITELN ('%SD-I-LOADED,  Loaded ',Count:1,' ',Symbol,'.') ;
  END ; 	{ of Read_File }


PROCEDURE Write_File ;

{ Dump the contents of the stack to a disk file - one default spec per line. }

  VAR	I , Count : INTEGER := 0 ;
	Str_I : VARYING [4] OF CHAR := '' ;

  BEGIN 	{ Write_File }
  IF Cmd.LENGTH = 2  THEN
     Symbol := 'SD_OUTPUT_FILE'
   ELSE
     Symbol := SUBSTR (Cmd,3,Cmd.LENGTH-2) ;
  Parse_Filename (Symbol) ;
  IF Phantom_Dir  THEN
     Exit ('%SD-F-DIRNOTFOU, Output directory not found.')
   ELSE 					{ Remove version }
     Search_Result := SUBSTR (Search_Result,1,INDEX(Search_Result,';')) ;

  OPEN (Data,FILE_NAME:=Search_Result,HISTORY:=NEW) ;
  REWRITE (Data) ;
  WRITELN ('%SD-I-WRITING, Writing stack data to ',Search_Result) ;

  FOR I := Vs (Bos) TO Vs (Tos)  DO
      BEGIN
      Str_I := DEC (Rs(I),Max_Digs,Max_Digs) ;
      Get_Symbol ('SD__'+Str_I) ;
      WRITELN (Data,Symbol) ;
      Count := Count + 1 ;
      END ;

  CLOSE (Data) ;
  IF Count = 1	THEN
     Symbol := 'entry'
   ELSE
     Symbol := 'entries' ;
  WRITELN ('%SD-I-STORED,  Stored ',Count:1,' ',Symbol,'.') ;
  END ; 	{ of Write_File }


PROCEDURE Swap_Slots ;

{ Swap the contents of two slots with each other.  This is an on-stack
  manipulation ONLY - the current default is not affected. }

  VAR	Tmp : INTEGER := 3 ;			{ !!! Note initial value !!! }
	Str : VARYING [80] OF CHAR := '' ;

  BEGIN 	{ Swap_Slots }
  Bos_Vs := Vs (Bos) ;
  Tos_Vs := Vs (Tos) ;

  IF (Command.LENGTH < 5) OR			{ "/S" + digit + " " + digit }
     (INDEX (Command,' ') = 0)	THEN
       Exit ('%SD-F-INVSWAP, Directory number is invalid.') ;

  Tmp := Tmp + (Command[3] = ' ')::INTEGER ;
  Str := SUBSTR (Command,Tmp,Command.LENGTH-Tmp+1) ;
  Get_Number (Str,Poppy,'%SD-F-BADSWAP, Directory number must be integer.') ;
  IF (Poppy < Bos_Vs) OR (Poppy > Tos_Vs)  THEN
     Exit ('%SD-F-INVSWAP, Directory number is invalid.') ;

  Tmp := INDEX (Str,' ') ;
  IF Tmp <> 0  THEN
     BEGIN
     Str := SUBSTR (Str,Tmp+1,Str.LENGTH-Tmp) ;
     Get_Number (Str,Poppy2,'%SD-F-BADSWAP, Directory number must be integer.');
     IF (Poppy2 < Bos_Vs) OR (Poppy2 > Tos_Vs)	THEN
	Exit ('%SD-F-INVSWAP, Directory number is invalid.') ;
     END ;

  Str_Poppy  := DEC (Rs(Poppy) ,Max_Digs,Max_Digs) ;
  Str_Poppy2 := DEC (Rs(Poppy2),Max_Digs,Max_Digs) ;
  Get_Symbol ('SD__'+Str_Poppy2) ;
  Symbol2 := Symbol ;
  Get_Symbol ('SD__'+Str_Poppy) ;
  Set_Symbol ('SD__'+Str_Poppy,Symbol2) ;
  Set_Symbol ('SD__'+Str_Poppy2,Symbol) ;
  END ; 	{ of Swap_Slots }


PROCEDURE Slot_To_Current (Swap : BOOLEAN) ;

{ Make the contents of a given slot the current default, and optionally swap
  the default being replaced into the slot being used. }

  VAR	Position , Bos_Vs , Tos_Vs : INTEGER := 0 ;

  BEGIN 	{ Slot_To_Current }
  Bos_Vs := Vs (Bos) ;
  Tos_Vs := Vs (Tos) ;

  IF Swap  THEN  Position := 2	ELSE  Position := 3 ;
  Get_Number (SUBSTR (Cmd,Position,Cmd.LENGTH-Position+1),Poppy,
	      '%SD-F-BADSLOT, Directory number must be integer.') ;

  IF Poppy < 0	THEN
     IF ABS (Poppy) > (Tos_Vs - Bos_Vs + 1)  THEN
	Exit ('%SD-F-INVRELSLOT, Relative directory number is invalid.')
      ELSE
	Poppy := Tos_Vs + Poppy + 1	{ Make it a virtual slot number }
   ELSE
  IF (Poppy = 0) OR (Poppy < Bos_Vs) OR (Poppy > Tos_Vs)  THEN
     Exit ('%SD-F-INVSLOT, Directory number is invalid.') ;

  Str_Poppy := DEC (Rs(Poppy),Max_Digs,Max_Digs) ;
  Get_Symbol ('SD__'+Str_Poppy) ;
  Parse_Filename (Symbol) ;
  Set_New_Default (Target_Dir) ;
  IF Swap  THEN  Set_Symbol ('SD__'+Str_Poppy,Current_Dir) ;
  IF Update_Prompt  THEN  Change_Prompt (Target_Dir) ;
  END ; 	{ of Slot_To_Current }


PROCEDURE Overwrite_Slot ;

{ Overwrite the contents of a given slot with the current default.  This is an
  on-stack manipulation ONLY - the current default is not affected. }

  BEGIN 	{ Overwrite_Slot }
  Get_Number (SUBSTR (Cmd,3,Cmd.LENGTH-2),Poppy,
	      '%SD-F-BADSLOT, Directory number must be integer.') ;
  IF (Poppy < Vs (Bos)) OR (Poppy > Vs (Tos))  THEN
     Exit ('%SD-F-INVSLOT, Directory number is invalid.') ;

  Str_Poppy := DEC (Rs(Poppy),Max_Digs,Max_Digs) ;
  Set_Symbol ('SD__'+Str_Poppy,Current_Dir) ;
  END ; 	{ of Overwrite_Slot }


PROCEDURE Pop_Stack ;

{ Pop the stack. }

  VAR	I : INTEGER := 0 ;
	Str_I : VARYING [4] OF CHAR := '' ;

  BEGIN 	{ Pop_Stack }
  Bos_Vs := Vs (Bos) ;
  Tos_Vs := Vs (Tos) ;

  IF Cmd.LENGTH = 2  THEN		{ Pop the Top }
     Poppy := Tos_Vs
   ELSE
     BEGIN
     Get_Number (SUBSTR (Cmd,3,Cmd.LENGTH-2),Poppy,
			 '%SD-F-BADPOP, Directory number must be integer.') ;
     IF Cmd[2] = 'B'  THEN
	Poppy := - ABS (Poppy) ;	{ In case they put "/B -n" ! }
     END ;

  { Validate slot to which to pop. }

  IF Poppy < 0	THEN
     IF ABS (Poppy) > (Tos_Vs - Bos_Vs + 1)  THEN
	Exit ('%SD-F-INVRELPOP, Relative directory number is invalid.')
      ELSE
	Poppy := Tos_Vs + Poppy + 1	{ Make it a virtual slot number }
   ELSE
  IF (Poppy = 0) OR (Poppy < Bos_Vs) OR (Poppy > Tos_Vs)  THEN
     Exit ('%SD-F-INVPOP, Directory number is invalid.') ;

  { Set default to valid slot contents. }

  Str_Poppy := DEC (Rs(Poppy),Max_Digs,Max_Digs) ;
  Get_Symbol ('SD__'+Str_Poppy) ;
  Parse_Filename (Symbol) ;
  Set_New_Default (Target_Dir) ;

  { Stack emptied?  Reset! }

  IF Poppy = Bos_Vs  THEN
     Zero_Stack
   ELSE
     BEGIN

     { Pop the *symbols* to reflect a directory pop from the stack. }

     FOR I := Tos_Vs DOWNTO Poppy  DO
	 BEGIN
	 Str_I := DEC (Rs(I),Max_Digs,Max_Digs) ;
	 Zip (LIB$DELETE_SYMBOL ('SD__'+Str_I,LIB$K_CLI_GLOBAL_SYM)) ;
	 Pop ;
	 END ;

     Set_Symbol ('SD__TS',Tos_Str) ;
     Set_Symbol ('SD__BS',Bos_Str) ;
     Set_Symbol ('SD__SD',Depth_Str) ;
     END ;

  IF Update_Prompt  THEN  Change_Prompt (Target_Dir) ;
  END ; 	{ of Pop_Stack }


PROCEDURE Give_Help ;

{ M'aidez!  Allow a subtopic specification. }

  VAR	Spot  : INTEGER := 2 ;			{ Note initial value }
	Topic : VARYING [80] OF CHAR := '' ;

  BEGIN 	{ Give_Help }
  IF Cmd[1] = '\'  THEN
     Spot := 3 ;

  IF (Spot <= Cmd.LENGTH) AND_THEN (Cmd[Spot] <> '')  THEN
     Topic := SUBSTR (Command,Spot,Command.LENGTH-Spot+1) ;

  Zip (LBR$OUTPUT_HELP (Output_Routine := %IMMED LIB$PUT_OUTPUT,
			Line_Desc      := 'SD ' + Topic,
			Library_Name   := 'SYS_HELP:PUBHELP.HLB',
			Input_Routine  := %IMMED LIB$GET_INPUT)) ;
  END ; 	{ of Give_Help }


PROCEDURE Prompt_Handler ;

{ Allow prompt updating to be verified or changed. }

  VAR	B4 , Modified : BOOLEAN := FALSE ;

  BEGIN 	{ Prompt_Handler }
  IF Cmd.LENGTH > 2  THEN
     BEGIN
     B4 := Update_Prompt ;
     READV (Cmd[3],Update_Prompt,Error:=CONTINUE) ;
     IF STATUSV <> 0  THEN
	Exit ('%SD-F-BADPVAL, Change prompt value must be "True" or "False".') ;

     WRITEV (Symbol,Update_Prompt) ;
     Str_Compress (Symbol,Symbol,TRUE) ;
     Set_Logical_Name ('SD_UPDATE_PROMPT',Symbol) ;
     Modified := Update_Prompt <> B4 ;
     END ;

  IF Update_Prompt  THEN
     BEGIN
     WRITELN ('%SD-I-UPDATE, Prompt will be updated.') ;
     Get_Prompt_Specs ;
     Change_Prompt (Current_Dir) ;	{ Update prompt now - just in case }
     END
   ELSE
     BEGIN
     WRITELN ('%SD-I-NOUPDATE, Prompt will be not updated.') ;
     IF Modified  THEN	Change_Prompt ('$ ',TRUE) ;
     END ;
  END ; 	{ of Prompt_Handler }


PROCEDURE Hush_Hush ;

{ Allow quiet flag to be verified or changed. }

  BEGIN 	{ Hush_Hush }
  IF Cmd.LENGTH > 2  THEN
     BEGIN
     READV (Cmd[3],Quiet,Error:=CONTINUE) ;
     IF STATUSV <> 0  THEN
	Exit ('%SD-F-BADPVAL, Quiet value must be "True" or "False".') ;

     WRITEV (Symbol,Quiet) ;
     Str_Compress (Symbol,Symbol,TRUE) ;
     Set_Logical_Name ('SD_QUIET',Symbol) ;
     END ;

  IF Quiet  THEN
     WRITELN ('%SD-I-QUIET, New default directory name will not be displayed.')
   ELSE
     WRITELN ('%SD-I-NOISY, New default directory name will be displayed.') ;
  END ; 	{ of Hush_Hush }


PROCEDURE Display_Current_Dir ;

{ Show current directory }

  BEGIN 	{ Display_Current_Dir }
  WRITELN (Current_Dir) ;
  IF Phantom_Dir  THEN
     WRITELN ('%SD-W-NODIR, Directory does not exist.') ;
  $EXIT ;
  END ; 	{ of Display_Current_Dir }


PROCEDURE Delete_Slot ;

{ Delete the specified slot from the stack. }

  VAR	Tmp  : INTEGER := 3 ;			{ !!! Note initial value !!! }
	Slot : INTEGER := 0 ;
	Str  : VARYING [80] OF CHAR := '' ;

  BEGIN 	{ Delete_Slot }
  Quiet := TRUE ;

  Bos_Vs := Vs (Bos) ;
  Tos_Vs := Vs (Tos) ;

  IF Command.LENGTH < 3  THEN			{ "/D" + digit }
       Exit ('%SD-F-INVSLOT, Directory number is invalid.') ;

  Tmp := Tmp + (Command[3] = ' ')::INTEGER ;
  Str := SUBSTR (Command,Tmp,Command.LENGTH-Tmp+1) ;
  Get_Number (Str,Slot,'%SD-F-BADSLOT, Directory number must be integer.') ;
  IF (Slot < Bos_Vs) OR (Slot > Tos_Vs)  THEN
     Exit ('%SD-F-INVSLOT, Directory number is invalid.') ;

  WHILE Slot < Tos_Vs  DO
    BEGIN
    WRITEV (Command,'/S ',Slot:1,' ',(Slot+1):1) ;
    Swap_Slots ;
    Slot := Slot + 1 ;
    END ;

  Pop ; 					{ Adjust the pointers }
  Set_Symbol ('SD__TS',Tos_Str) ;
  Set_Symbol ('SD__BS',Bos_Str) ;
  Set_Symbol ('SD__SD',Depth_Str) ;
  Str := DEC (Tos+1,Max_Digs,Max_Digs) ;
  Zip (LIB$DELETE_SYMBOL ('SD__'+Str,LIB$K_CLI_GLOBAL_SYM)) ;
  END ; 	{ of Delete_Slot }


{ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * *	M A I N   P R O G R A M   * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * }


BEGIN	{ Set_Default }

Get_Command ;
Initialize_Stack ;

Get_Logical_Name ('SYS$LOGIN',' ',Default_Dir) ;

Parse_Filename ('[]*.*',,TRUE) ;	{ Get current directory. }
Current_Dir := Target_Dir ;

IF Cmd = ''  THEN		{ No parameter = SYS$LOGIN: }
   BEGIN
   IF (Default_Dir = Current_Dir) AND NOT Quiet  THEN
      Display_Current_Dir
    ELSE
      BEGIN
      Set_New_Default (Default_Dir) ;
      Push_Stack (Current_Dir) ;
      IF Update_Prompt	THEN  Change_Prompt (Default_Dir) ;
      END ;
   END
 ELSE
IF Cmd[1] = '?'  THEN		{ HELP! }
   Give_Help
 ELSE
IF Cmd[1] <> '\'  THEN		{ Directory Spec or shortcut character }
   BEGIN
   IF Cmd[1] = '~'  THEN	{ Particular user's default }
      Get_User_Dir ;
   Parse_Spec_And_Set_Default ;
   IF Target_Dir <> Current_Dir  THEN
      BEGIN
      Push_Stack (Current_Dir) ;
      IF Update_Prompt	THEN  Change_Prompt (Target_Dir) ;
      END ;
   END
 ELSE
IF Cmd = '\'  THEN		{ "\" ONLY }
   IF Tos = 0  THEN		{ Empty }
      Exit (Empty_Stack_Msg)
    ELSE
      BEGIN
      WRITEV (Cmd,'\',Vs (Tos):1) ;
      Slot_To_Current (TRUE) ;
      END
 ELSE					{ A backslash command ... }
   CASE Cmd[2] OF
     'A': Show_Stack_Attributes ;	{ List the stack attributes }
     'C': Display_Current_Dir ; 	{ Show current directory }
     'E': Stack_Undoer ;		{ Change undo action }
     'H',				{ Help! }
     '?': Give_Help ;			{ Help! }
     'L': List_Stack ;			{ List the stack }
     'Q': Hush_Hush ;			{ Change Quiet action }
     'R': Read_File ;			{ Read from file }
     'T': Stack_Typer ; 		{ Change stack type }
     'U': Prompt_Handler ;		{ Change prompt updating action }
    OTHERWISE
     IF Tos = 0  THEN			{ Empty }
	Exit (Empty_Stack_Msg)
      ELSE
	CASE Cmd[2] OF
	  'P',				{ "Pop" the stack  - absolute number }
	  'B': Pop_Stack ;		{ ... or go "Back" - relative number }
	  'D': Delete_Slot ;		{ Delete the slot from the stack }
	  'G': Slot_To_Current (FALSE); { Set slot into current, no swap }
	  '-': Slot_To_Current (TRUE) ; { Negative number - swap back nth slot }
	  'O': Overwrite_Slot ; 	{ Overwrite slot with current }
	  'S': Swap_Slots ;		{ Swap two slots on the stack }
	  'W': Write_File ;		{ Write to file }
	  'Z': Zero_Stack (TRUE) ;	{ Zero the stack }
	 OTHERWISE
	  IF Cmd[2] IN Digits  THEN	{ Swap current with slot on stack }
	     Slot_To_Current (TRUE)
	   ELSE
	     Exit ('%SD-F-BADPARAM, Bad parameter - ' +
		   'check HELP for correct syntax (or use "$ SD ?").') ;
	END ;	{ Case }
   END ;	{ Case }

END.	{ of it all }
