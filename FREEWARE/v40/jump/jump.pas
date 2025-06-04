{************************************************************************
*									*
*  J U M P								*
*									*
*  JUMP is a program which allows selected users (Operators, Systems	*
*  Programmers and other specifically authorised users) to either	*
*  change elements of their process to those of another user, or to	*
*  actually become a given user in a separate process attached to a	*
*  pseudo-terminal.							*
*									*
*  When not becoming exactly the target user, the items which are	*
*  changed to those of the target user are:				*
*									*
*    Username (Systems Programmers only, using the SETUSER or		*
*	       ALL qualifier)						*
*									*
*    UIC, Default Directory, Default Disk, LNM$GROUP.			*
*									*
*    Privileges and process rights are NOT changed.			*
*									*
*  The syntax of the JUMP command generally is				*
*									*
*      $ JUMP [username] [qualifiers]					*
*									*
*  If no username is supplied, JUMP returns the UIC, Default Directory, *
*  Default Disk and LNM$GROUP to those of the current username. 	*
*.......................................................................*
*  ****  CAUTION: KERNEL-mode code fiddles things !!! ****		*
*									*
*  INSTALL with CMEXEC, CMKRNL, DETACH, SYSNAM, SYSPRV privileges.	*
*									*
*  NOTE: The definition of privilege sets has been adopted for ease of	*
*  coding.  The source for the information is SYS$LIBRARY:STARLET.PAS.	*
*  The definition will need reviewing with each release of OpenVMS!	*
*.......................................................................*
*  Note: Some ideas in this code have been drawn from Eric Wentz's	*
*	 program BECOME.  The code for pseudo-terminal creation is	*
*	 derived from Anthony McCracken's GLOGIN program.  Thanks to	*
*	 Jeremy Begg for a minor code fix and Bob Beckerhof for testing.*
*.......................................................................*
*  Author:  Jonathan Ridler,						*
*	    Information Technology Services,				*
*	    The University of Melbourne,				*
*	    Parkville, Victoria,					*
*	    AUSTRALIA, 3052.						*
*									*
*	    Internet: jonathan@unimelb.edu.au				*
*.......................................................................*
*  History:								*
* v1.0	02-Mar-1993	JER	Original version.			*
*	09-Mar-1993	JER	Audit security violations.		*
*	04-Mar-1994	JER	Add change username capability. 	*
*	14-Jul-1994	JER	Allow change of UIC, etc. AND username; *
*				Allow username NOT in UAF.		*
*	21-Nov-1995	JER	Allow Sysprogs to avoid audit.		*
* v1.4	11-Jan-1996	JER	Increase size of audit message buffer.	*
* v2.0	11-Jul-1996	JER	Add EXACT (pseudo-terminal) form.	*
* v2.1	26-Jul-1996	JER	Add general user restricted access.	*
* v2.2	07-Aug-1996	JER	Only Sysprogs and authorised users can	*
*				jump to privileged users. Also, modify	*
*				method of restoring terminal chars.	*
* v2.3	12-Sep-1996	JER	Audit physical terminal device.  Also,	*
*				validate Operators against access file	*
*				to see if access is specifically denied.*
*				Also, allow minor privs to be specified *
*				by logical name.			*
* v2.4	16-Sep-1996	JER	Audit pseudo-terminal device name too.	*
* v2.5	17-Sep-1996	JER	Add JUMP_SELF logical.			*
* v2.6	01-Nov-1996	JER	Fix username glitch on EXACT return.	*
*				Caused by VAX (only) compiler behaviour.*
* v2.7	15-Nov-1996	JER	Validate current privs at startup.	*
* v2.8	09-Jan-1997	JER	Fix short string in audit routine.	*
* v2.9	18-Sep-1997	JER	Improve kernel and exec mode code -	*
*				simplify and generalize. Also, put port *
*				name into pseudo-terminal ACCPORNAM -	*
*				this introduces architecture-specific	*
*				code modules.				*
* v3.0 26-Jun-1998	JER	Fix audit to log correct return UIC.	*
************************************************************************}

[INHERIT ('SYS$LIBRARY:STARLET',
	  'SYS$LIBRARY:PASCAL$LIB_ROUTINES',
	  'SYS$LIBRARY:PASCAL$STR_ROUTINES')]

PROGRAM Jump (OUTPUT) ;

CONST

Lf = CHR (10) ; 		{ Linefeed }

Io_Buflen = 1 * 512 ;		{ *WORDS* = 1 pair of 512 byte R/W blocks }
Rsts = 1 ;			{ Index to read status in pseudo-terminal buf }
Rcnt = 2 ;			{ Count of chars read in pseudo-terminal buf }
Rbuf = 3 ;			{ Actual read data area begins }
Wsts = Io_Buflen DIV 2 + 1 ;	{ Start of write buffer area and write status }
Wcnt = Wsts + 1 ;		{ Count of chars writ in pseudo-terminal buf }
Wbuf = Wsts + 2 ;		{ Actual read data area begins }
Ft_Buflen = Io_Buflen - 4 ;	{ *BYTES* Size of buffer for FTA device }
Py_Buflen = Io_Buflen ; 	{ *BYTES* Size of buffer for PYA device }

Rightsize = 50 ;		{ Number of rightslist entries to retrieve }

Bad_Access_Data =
   '%JUMP-F-BADDATA, Format of Access List data record seems to be invalid.' ;

TYPE

$BOOL  = [BIT(1),UNSAFE] BOOLEAN ;
$UBYTE = [BYTE] 0..255 ;
$UWORD = [WORD] 0..65535 ;
$UQUAD = [QUAD,UNSAFE] RECORD  L0 , L1 : UNSIGNED ;  END ;

Word_Ptr     = ^$UWORD ;
Unsigned_Ptr = ^UNSIGNED ;

Privilege = (Cmkrnl,	{  0: May change Mode to Kernel }
	     Cmexec,	{  1: May change Mode to Exec; MUST follow Cmkrnl }
	     Sysnam,	{  2: May insert in system logical name table }
	     Grpnam,	{  3: May insert in group l.n.tab; MUST follow Sysnam }
	     Allspool,	{  4: May allocate spooled device }
	     Detach,	{  5: May create detached processes }
	     Diagnose,	{  6: May diagnose devices }
	     Log_Io,	{  7: May do logical I/O }
	     Group,	{  8: May affect other processes in same UIC group }
	     Acnt,	{  9: May suppress accounting }
	     Prmceb,	{ 10: May create permanent common event clusters }
	     Prmmbx,	{ 11: May create permanent mail box }
	     Pswapm,	{ 12: May change process mode }
	     Altpri,	{ 13: May set any priority value }
	     Setprv,	{ 14: May set any privilege bits }
	     Tmpmbx,	{ 15: May create temporary mailbox }
	     World,	{ 16: May affect other processes in the world }
	     Mount,	{ 17: May execute mount ACP functions }
	     Oper,	{ 18: Operator privilege }
	     Exquota,	{ 19: May exceed disk quotas }
	     Netmbx,	{ 20: May create network device }
	     Volpro,	{ 21: May override volume protection }
	     Phy_Io,	{ 22: May do physical I/O }
	     Bugchk,	{ 23: May make bug check error log entries }
	     Prmgbl,	{ 24: May create permanent global sections }
	     Sysgbl,	{ 25: May create system-wide global sections }
	     Pfnmap,	{ 26: May map to section by PFN }
	     Shmem,	{ 27: May allocate structures in shared memory }
	     Sysprv,	{ 28: Eligible for system protection field }
	     Bypass,	{ 29: May bypass UIC-based protection }
	     Syslck,	{ 30: May create system-wide locks }
	     Share,	{ 31: May assign channel to non-shared device }
	     Upgrade,	{ 32: May upgrade object integrity }
	     Downgrade, { 33: May downgrade object secrecy }
	     Grpprv,	{ 34: Group access via system protection field }
	     Readall,	{ 35: May read any object }
	     Import,	{ 36: May set classification for unlabeled object }
	     Audit,	{ 37: May direct audit to system security audit log }
	     Security,	{ 38: May perform security functions }
	     Pbit39, Pbit40, Pbit41, Pbit42, Pbit43, Pbit44, Pbit45,
	     Pbit46, Pbit47, Pbit48, Pbit49, Pbit50, Pbit51, Pbit52,
	     Pbit53, Pbit54, Pbit55, Pbit56, Pbit57, Pbit58, Pbit59,
	     Pbit60, Pbit61, Pbit62, Pbit63) ;	{ 39-63: dummy bits: 2 LONGs }

Privset = PACKED SET OF Privilege ;		{ Allow easy bit union, etc. }

Access_Status = (Granted,Denied,Unspecified) ;	{ Status from access file }

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

Username_Type = PACKED ARRAY [1..12] OF CHAR ;

Io_Buffer = [ALIGNED(9),STATIC,UNSAFE]			{ Pagelet aligned }
		ARRAY [1..Io_Buflen] OF $UWORD	VALUE ZERO ;

Terminal_Chars = PACKED RECORD
		   Tt_Class    : [POS(0)]  $UBYTE ;
		   Tt_Type     : [POS(8)]  $UBYTE ;
		   Tt_Width    : [POS(16)] $UWORD ;
		   Tt_Devchar  : [POS(32)] TT$TYPE ;
		   Tt_Devchar2 : [POS(64)] TT2$TYPE ;
		 END ;

Prtctl_Type = PACKED RECORD
		TTY$V_PC_NOTIME       : [POS(0)]  $BOOL ;
		TTY$V_PC_DMAENA       : [POS(1)]  $BOOL ;
		TTY$V_PC_DMAAVL       : [POS(2)]  $BOOL ;
		TTY$V_PC_PRMMAP       : [POS(3)]  $BOOL ;
		TTY$V_PC_MAPAVL       : [POS(4)]  $BOOL ;
		TTY$V_PC_XOFAVL       : [POS(5)]  $BOOL ;
		TTY$V_PC_XOFENA       : [POS(6)]  $BOOL ;
		TTY$V_PC_NOCRLF       : [POS(7)]  $BOOL ;
		TTY$V_PC_BREAK	      : [POS(8)]  $BOOL ;
		TTY$V_PC_PORTFDT      : [POS(9)]  $BOOL ;
		TTY$V_PC_NOMODEM      : [POS(10)] $BOOL ;
		TTY$V_PC_NODISCONNECT : [POS(11)] $BOOL ;
		TTY$V_PC_SMART_READ   : [POS(12)] $BOOL ;
		TTY$V_PC_ACCPORNAM    : [POS(13)] $BOOL ;
		TTY$V_PC_MULTISESSION : [POS(15)] $BOOL ;
	      END ;

CONST

Required_Privs = [Cmexec,Cmkrnl,Detach,Sysnam,Sysprv] ; { Needed to run JUMP }

VAR

Log ,				{ Log success messages? }
Alter_Ego ,			{ Change Username? }
Transmute ,			{ Change UIC, etc.? }
Auditing ,			{ Audit successful jumps? }
Real_Mccoy ,			{ Use a pseudo-terminal and *really* do it!? }
Double_Check ,			{ Double check general user's access? }
Narcissus ,			{ Allow users to jump to themselves? }
Figment : BOOLEAN := FALSE ;	{ Allow username NOT in UAF? }

Max_Sys_Group : INTEGER := 0 ;	{ Maximum UIC group with system privileges }

Orig_User : VARYING [12] OF CHAR := PAD ('',' ',12) ;	{ Caller's username }

New_User : [VOLATILE] VARYING [12] OF CHAR := PAD ('',' ',12) ; { Target user }

Sanity_Ctl_User ,					{ For CMKRNL checks }
Sanity_Jib_User : [VOLATILE] Username_Type := '' ;	{ For CMKRNL checks }

Command : VARYING [80] OF CHAR := '' ;			{ Input command line }

Uic ,							{ UIC of caller }
New_Uic : [VOLATILE] UIC$TYPE := ZERO ; 		{ UIC of target user }

Terminal ,						{ Audit this }
Port ,							{ Audit this }
Physical_Device ,					{ Audit this }
Def_Dev : [VOLATILE] VARYING [64] OF CHAR := '' ;	{ Default device }

Access_List ,						{ Filespec }
Audit_Trail : VARYING [255] OF CHAR := '' ;		{ Filespec }

Def_Dir : [VOLATILE] VARYING [255] OF CHAR := '' ;	{ Default directory }

Uic_Str : VARYING [15] OF CHAR := ZERO ;		{ String form }

Eq_Id_Str ,						{ UIC string handling }
Id_Str	: VARYING [32] OF CHAR := ZERO ;		{ UIC string handling }

Minor_Privs ,						{ Not major privs! :) }
Def_Priv ,						{ Target's def  privs }
Auth_Priv : [UNSAFE,VOLATILE]  Privset := ZERO ;	{ Target's auth privs }

Flags : [VOLATILE]  FLAGS$TYPE := ZERO ;		{ Target's UAF flags }

Jib_User_Ptr : [VOLATILE] ^[VOLATILE] Username_Type := NIL ;

Jib_Ptr ,
Uic_Ptr : [VOLATILE] Unsigned_Ptr := NIL ;

Pseudo_Ft : [VOLATILE] BOOLEAN := FALSE ;	{ Is pseudo-terminal FTA0:? }

Pchan , 					{ Pseudo-terminal channel }
Rchan , 					{ Real terminal channel }
Mchan : [VOLATILE] $UWORD := 0 ;		{ Mailbox channel }

Pdev : VARYING [12] OF CHAR := '' ;		{ Pseudo-terminal device name }

Pbuf_Range : ARRAY [1..2] OF UNSIGNED := ZERO ; { Quasi descriptor of I/O buf }

Mbbuf : ARRAY [1..ACC$K_TERMLEN] OF $UBYTE := ZERO ;	{ Termination MBX buf }

Piosb : [VOLATILE] Status_Block_Type := ZERO ;	{ IOSB for pseudo-terminal IOs }

Buffer : [VOLATILE] Io_Buffer := ZERO ; 	{ Pseudo-terminal I/O buffer }

Rchars : Terminal_Chars := ZERO ;		{ Device chars of real term }

Exit_Rst ,					{ Exit Handler status }
Pid ,						{ Current process PID }
Master_Pid ,					{ Current process master PID }
Proc_Cnt : [VOLATILE] UNSIGNED := 0 ;		{ Current proc subproc count }

Proc_Cur_Priv ,					      { Caller's current privs }
Proc_Def_Priv ,					      { Caller's default privs }
Proc_Auth_Priv : [UNSAFE,VOLATILE]  Privset := ZERO ; { Caller's auth'd  privs }

Rights : ARRAY [1..Rightsize] OF $UQUAD := ZERO ;	{ Caller's proc rights }

PCB$L_JIB ,
PCB$L_UIC ,
UCB$W_TT_PRTCTL ,
UCB$L_TT_ACCPORNAM ,
JIB$T_USERNAME ,
PAS$K_SUCCESS ,
PAS$K_FILNOTFOU : [EXTERNAL,VALUE] UNSIGNED ;

CTL$GL_PCB ,
CTL$GA_CCB_TABLE : [EXTERNAL] UNSIGNED ;

CTL$T_USERNAME : [EXTERNAL,VOLATILE] Username_Type ;


{ The following formal function declarations are present to allow compatability
  with OpenVMS v5.5-2 and lower where these declarations do not appear in
  environment files.  For OpenVMS v6.0 (I think) and onwards, $SETDDIR appears
  in STARLET, and the CLI routines appear in PASCAL$CLI_ROUTINES. }

[ASYNCHRONOUS] FUNCTION SYS$SETDDIR (
	New_Dir_Addr :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Length_Addr : [VOLATILE] $UWORD := %IMMED 0 ;
	%STDESCR Cur_Dir_Addr :
		PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION CLI$DCL_PARSE (
	Command_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] PROCEDURE Table ;
	%IMMED [ASYNCHRONOUS,UNBOUND]
		FUNCTION Param_Routine	: INTEGER := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND]
		FUNCTION Prompt_Routine : INTEGER := %IMMED 0 ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION CLI$GET_VALUE (
	Entity_Desc : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Retdesc : VARYING [$L2] OF CHAR ;
	VAR Retlength : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION CLI$PRESENT (
	Entity_Desc : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;


PROCEDURE Exit (Msg : VARYING [Len] OF CHAR) ;

{ Just a dinky shorthand routine. }

  BEGIN 	{ Exit }
  WRITELN (Msg) ;
  $EXIT ;
  END ; 	{ of Exit }


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


PROCEDURE Get_Caller_Info ;

{ Get relevant information about the invoker of the program. }

  VAR	Rst : INTEGER := 0 ;
	Iosb : Status_Block_Type := ZERO ;
	Item_List : Item_List_Template (13) := ZERO ;

  BEGIN 	{ Get_Caller_Info }
  Item_List[1].Buffer_Length := 4 ;
  Item_List[1].Item_Code     := SYI$_MAXSYSGROUP ;
  Item_List[1].Buffer_Addr   := IADDRESS (Max_Sys_Group) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Terminator    := 0 ;   { Terminate the item list }

  Rst := $GETSYIW (Itmlst := Item_List,
		   Iosb   := Iosb) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1]) ;

  Item_List[1].Buffer_Length := 8 ;
  Item_List[1].Item_Code     := JPI$_PROCPRIV ;
  Item_List[1].Buffer_Addr   := IADDRESS (Proc_Def_Priv) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Buffer_Length := 8 ;
  Item_List[2].Item_Code     := JPI$_AUTHPRIV ;
  Item_List[2].Buffer_Addr   := IADDRESS (Proc_Auth_Priv) ;
  Item_List[2].Return_Addr   := 0 ;

  Item_List[3].Buffer_Length := 4 ;
  Item_List[3].Item_Code     := JPI$_UIC ;
  Item_List[3].Buffer_Addr   := IADDRESS (Uic.UIC$L_UIC) ;
  Item_List[3].Return_Addr   := 0 ;

  Item_List[4].Buffer_Length := SIZE (Orig_User) ;
  Item_List[4].Item_Code     := JPI$_USERNAME ;
  Item_List[4].Buffer_Addr   := IADDRESS (Orig_User.BODY) ;
  Item_List[4].Return_Addr   := IADDRESS (Orig_User.LENGTH) ;

  Item_List[5].Buffer_Length := 4 ;
  Item_List[5].Item_Code     := JPI$_PID ;
  Item_List[5].Buffer_Addr   := IADDRESS (Pid) ;
  Item_List[5].Return_Addr   := 0 ;

  Item_List[6].Buffer_Length := 4 ;
  Item_List[6].Item_Code     := JPI$_MASTER_PID ;
  Item_List[6].Buffer_Addr   := IADDRESS (Master_Pid) ;
  Item_List[6].Return_Addr   := 0 ;

  Item_List[7].Buffer_Length := 4 ;
  Item_List[7].Item_Code     := JPI$_PRCCNT ;
  Item_List[7].Buffer_Addr   := IADDRESS (Proc_Cnt) ;
  Item_List[7].Return_Addr   := 0 ;

  Item_List[8].Buffer_Length := SIZE (Port.BODY) ;
  Item_List[8].Item_Code     := JPI$_TT_ACCPORNAM ;
  Item_List[8].Buffer_Addr   := IADDRESS (Port.BODY) ;
  Item_List[8].Return_Addr   := IADDRESS (Port.LENGTH) ;

  Item_List[9].Buffer_Length := SIZE (Terminal.BODY) ;
  Item_List[9].Item_Code     := JPI$_TERMINAL ;
  Item_List[9].Buffer_Addr   := IADDRESS (Terminal.BODY) ;
  Item_List[9].Return_Addr   := IADDRESS (Terminal.LENGTH) ;

  Item_List[10].Buffer_Length := SIZE (Physical_Device.BODY) ;
  Item_List[10].Item_Code     := JPI$_TT_PHYDEVNAM ;
  Item_List[10].Buffer_Addr   := IADDRESS (Physical_Device.BODY) ;
  Item_List[10].Return_Addr   := IADDRESS (Physical_Device.LENGTH) ;

  Item_List[11].Buffer_Length := SIZE (Rights) ;
  Item_List[11].Item_Code     := JPI$_PROCESS_RIGHTS ;
  Item_List[11].Buffer_Addr   := IADDRESS (Rights) ;
  Item_List[11].Return_Addr   := 0 ;

  Item_List[12].Buffer_Length := 8 ;
  Item_List[12].Item_Code     := JPI$_CURPRIV ;
  Item_List[12].Buffer_Addr   := IADDRESS (Proc_Cur_Priv) ;
  Item_List[12].Return_Addr   := 0 ;

  Item_List[13].Terminator   := 0 ;	{ Terminate the item list }

  Rst := $GETJPIW (Itmlst := Item_List,
		   Iosb   := Iosb) ;

  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1]) ;

  Rst := STR$TRIM (%DESCR Orig_User,%DESCR Orig_User) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  IF (Physical_Device <> '') AND_THEN
     (Physical_Device[1] = '_')  THEN
      Physical_Device := SUBSTR (Physical_Device,2,Physical_Device.LENGTH-1) ;

  IF NOT (Required_Privs <= Proc_Cur_Priv)  THEN	{ Got needed privs? }
     $EXIT (SS$_NOPRIV) ;
  END ; 	{ of Get_Caller_Info }


FUNCTION Get_And_Parse_Command : BOOLEAN ;

{ Get and parse the DCL command line.  Do some basic username checks. }

  VAR	Rst , Spot : UNSIGNED := 0 ;
	Prv : Privilege := Tmpmbx ;
	Str : VARYING [254] OF CHAR := '' ;

  [ASYNCHRONOUS] PROCEDURE Jump_Cld ; EXTERNAL ;	{ CLD module }

  BEGIN 	{ Get_And_Parse_Command }
  Get_And_Parse_Command := TRUE ;

  Rst := LIB$GET_FOREIGN (%DESCR Command) ;		{ Get the command line }
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  Command := 'JUMP ' + Command ;
  Rst := CLI$DCL_PARSE (Command,Jump_Cld,%IMMED LIB$GET_INPUT,
			%IMMED LIB$GET_INPUT,'JUMP> ') ;
  IF (Rst = RMS$_EOF) OR (Rst = CLI$_NOCOMD) OR (NOT ODD (Rst)) THEN  $EXIT ;

  Rst := CLI$PRESENT ('EXACT') ;
  Real_Mccoy := (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;

  Rst := CLI$PRESENT ('SETUSER') ;
  Alter_Ego := (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;
  Transmute := NOT (Alter_Ego OR Real_Mccoy) ;

  Rst := CLI$PRESENT ('ALL') ;
  Alter_Ego := Alter_Ego OR (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;

  Rst := CLI$PRESENT ('OVERRIDE_UAF') ;
  Figment := (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;

  Rst := CLI$PRESENT ('LOG') ;
  Log := (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;

  Rst := CLI$PRESENT ('AUDIT') ;
  Auditing := (Rst = CLI$_PRESENT) OR (Rst = CLI$_DEFAULTED) ;
  Get_Logical_Name (Lognam  := 'JUMP_AUDIT_TRAIL',
		    Default := 'SYS_MANAGER:JUMP_AUDIT.DAT',   { SITE-specific }
		    Actual  := Audit_Trail,
		    Table   := 'LNM$SYSTEM',
		    Mode    := PSL$C_EXEC) ;

  Get_Logical_Name (Lognam  := 'JUMP_ACCESS_LIST',
		    Default := 'SYS_MANAGER:JUMP_ACCESS.DAT',  { SITE-specific }
		    Actual  := Access_List,
		    Table   := 'LNM$SYSTEM',
		    Mode    := PSL$C_EXEC) ;

  Get_Logical_Name (Lognam  := 'JUMP_DOUBLE_CHECK',
		    Default := 'TRUE',			       { SITE-specific }
		    Actual  := Str,
		    Table   := 'LNM$SYSTEM',
		    Mode    := PSL$C_EXEC) ;
  Str_Compress (Str,Str) ;
  Rst := STR$UPCASE (%DESCR Str,Str) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  Double_Check := Str = 'TRUE' ;

  Get_Logical_Name (Lognam  := 'JUMP_SELF',
		    Default := 'TRUE',			       { SITE-specific }
		    Actual  := Str,
		    Table   := 'LNM$SYSTEM',
		    Mode    := PSL$C_EXEC) ;
  Str_Compress (Str,Str) ;
  Rst := STR$UPCASE (%DESCR Str,Str) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  Narcissus := Str = 'TRUE' ;

  Get_Logical_Name (Lognam  := 'JUMP_MINOR_PRIVS',
		    Default := 'NETMBX TMPMBX', 	       { SITE-specific }
		    Actual  := Str,
		    Table   := 'LNM$SYSTEM',
		    Mode    := PSL$C_EXEC) ;
  Str_Compress (Str,Str) ;
  REPEAT
    READV (Str,Prv,Error:=CONTINUE) ;
    IF STATUSV <> 0  THEN
       Exit('%JUMP-F-BADPRIVSET, Minor privilege set specification is invalid.')
     ELSE
       BEGIN
       Minor_Privs := Minor_Privs + [Prv] ;
       Spot := INDEX (Str,' ') ;
       IF Spot = 0  THEN
	  Str := ''
	ELSE
	  Str := SUBSTR (Str,Spot+1,Str.LENGTH-Spot) ;
       END ;
  UNTIL Str = '' ;

  Rst := CLI$GET_VALUE ('USERNAME',New_User) ;	{ Assume will be there }
  IF Rst = CLI$_ABSENT	THEN
     New_User := Orig_User			{ Default to current username }
   ELSE
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
     BEGIN
     Rst := STR$TRIM (%DESCR New_User,%DESCR New_User) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END ;

  IF FIND_NONMEMBER (New_User,['A'..'Z','0'..'9','_','$']) <> 0 THEN
     BEGIN
     WRITELN ('%JUMP-F-BADUSER, Username contains invalid characters.') ;
     Get_And_Parse_Command := FALSE ;
     END ;
  END ; { of Get_And_Parse_Command }


FUNCTION Get_Target_Info : BOOLEAN ;

{ Get the required information for the particular new user from the UAF. }

  VAR	Rst : INTEGER := 0 ;
	Item_List : Item_List_Template (7) := ZERO ;

  BEGIN 	{ Get_Target_Info }
  Item_List[1].Buffer_Length := 8 ;
  Item_List[1].Item_Code     := UAI$_DEF_PRIV ;
  Item_List[1].Buffer_Addr   := IADDRESS (Def_Priv) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Buffer_Length := 8 ;
  Item_List[2].Item_Code     := UAI$_PRIV ;
  Item_List[2].Buffer_Addr   := IADDRESS (Auth_Priv) ;
  Item_List[2].Return_Addr   := 0 ;

  Item_List[3].Buffer_Length := 4 ;
  Item_List[3].Item_Code     := UAI$_UIC ;
  Item_List[3].Buffer_Addr   := IADDRESS (New_Uic.UIC$L_UIC) ;
  Item_List[3].Return_Addr   := 0 ;

  Item_List[4].Buffer_Length := 4 ;
  Item_List[4].Item_Code     := UAI$_FLAGS ;
  Item_List[4].Buffer_Addr   := IADDRESS (Flags) ;
  Item_List[4].Return_Addr   := 0 ;

  Item_List[5].Buffer_Length := SIZE (Def_Dir) ;
  Item_List[5].Item_Code     := UAI$_DEFDIR ;
  Item_List[5].Buffer_Addr   := IADDRESS (Def_Dir.BODY) ;
  Item_List[5].Return_Addr   := 0 ;

  Item_List[6].Buffer_Length := SIZE (Def_Dev) ;
  Item_List[6].Item_Code     := UAI$_DEFDEV ;
  Item_List[6].Buffer_Addr   := IADDRESS (Def_Dev.BODY) ;
  Item_List[6].Return_Addr   := 0 ;

  Item_List[7].Terminator    := 0 ;	{ Terminate the item list }

  Rst := $GETUAI (Usrnam := %STDESCR SUBSTR (New_User,1,New_User.LENGTH),
		  Itmlst := Item_List) ;

  Get_Target_Info := ODD (Rst) ;

  IF NOT ODD (Rst)  THEN
     BEGIN
     IF Rst <> RMS$_RNF  THEN
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     BEGIN
     Def_Dir.LENGTH := INT (Def_Dir.BODY[1]) ;
     Def_Dev.LENGTH := INT (Def_Dev.BODY[1]) ;
     Def_Dir.BODY := SUBSTR (Def_Dir.BODY,2,Def_Dir.LENGTH) ;
     Def_Dev.BODY := SUBSTR (Def_Dev.BODY,2,Def_Dev.LENGTH) ;
     END ;
  END ; 	{ of Get_Target_Info }


PROCEDURE Format_User (Uic : UIC$TYPE) ;

{ Create a string with the UIC in numeric and rights identifier formats }

  VAR  Rst : INTEGER := 0 ;

  BEGIN 	{ Format_User }
  Rst := $FAO ('!%U',Uic_Str.LENGTH,%STDESCR Uic_Str.BODY,%IMMED Uic) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  Rst := $FAO ('!%I',Id_Str.LENGTH,%STDESCR Id_Str.BODY,%IMMED Uic) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  IF Uic_Str = Id_Str  THEN
     Eq_Id_Str := ''
   ELSE
     Eq_Id_Str := ' = ' + Id_Str ;
  END ; 	{ of Format_User }


PROCEDURE Audit_Jump (Ok : BOOLEAN) ;

{ Record who, when, where, how, etc. for auditing purposes }

  VAR	Rst : UNSIGNED := 0 ;
	Imprint : VARYING [240] OF CHAR := '' ; 	{ 3 lines!! }
	Audit : TEXT ;
	Stamp : PACKED ARRAY [1..23] OF CHAR := '' ;

  BEGIN 	{ Audit_Jump }
  OPEN	 (Audit,FILE_NAME:=Audit_Trail,HISTORY:=Unknown,SHARING:=READWRITE) ;
  EXTEND (Audit) ;

  Rst := $ASCTIM (Timbuf := %STDESCR Stamp) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  IF Stamp[1] = ' '  THEN  Stamp[1] := '0' ;
  Imprint := SUBSTR (Stamp,1,20) + ' ' + Orig_User ;
  IF Ok  THEN
     BEGIN
     IF New_User = Orig_User  THEN
	BEGIN
	Imprint := Imprint + ' from ' ;
	Format_User (Uic) ;
	END
      ELSE
	BEGIN
	Imprint := Imprint + '  to  ' ;
	Format_User (New_Uic) ;
	END ;
     Imprint := Imprint + Uic_Str + Eq_Id_Str ;
     IF New_User = Orig_User  THEN
	Format_User (New_Uic) ; 	{ Provide format for display }
     END
   ELSE
     Imprint := Imprint + ' PRIV violation: ' + Command ;

  IF Real_Mccoy  THEN
     BEGIN
     Imprint := Imprint + ' *EXACT*' ;
     IF Pdev <> ''  THEN  Imprint := Imprint + ' ' + Pdev ;
     END ;
  IF Port <> ''  THEN  Imprint := Imprint + ' ' + Port ;
  IF Terminal <> ''  THEN  Imprint := Imprint + ' ' + Terminal ;
  IF Physical_Device <> ''  THEN  Imprint := Imprint + ' ' + Physical_Device ;

  WRITELN (Audit,Imprint) ;
  CLOSE (Audit) ;
  END ; 	{ of Audit_Jump }


FUNCTION In_List (Hopeful : VARYING [L1] OF CHAR ;
		  Targets : VARYING [L2] OF CHAR) : UNSIGNED ;

{ Determine if a given string appears in a list of strings which may contain
  wildcarded values and "negated" values.  Use some CLI return status values
  for convenience. }

  VAR	Spot : INTEGER := 0 ;
	Rst : UNSIGNED := 0 ;
	Negated : BOOLEAN := FALSE ;
	Debutante : VARYING [120] OF CHAR := '' ;

  BEGIN 	{ In-List }
  In_List := CLI$_ABSENT ;
  REPEAT
    Spot := INDEX (Targets,',') ;
    IF Spot = 0  THEN
       Debutante := Targets
     ELSE
       BEGIN
       Debutante := SUBSTR (Targets,1,Spot-1) ;
       Targets	 := SUBSTR (Targets,Spot+1,Targets.LENGTH-Spot) ;
       END ;
    IF Debutante.LENGTH = 0  THEN  Exit (Bad_Access_Data) ;
    IF Debutante[1] = '!'  THEN
       BEGIN
       Debutante := SUBSTR (Debutante,2,Debutante.LENGTH-1) ;
       Negated := TRUE ;
       END
     ELSE
       Negated := FALSE ;
    Rst := STR$MATCH_WILD (Hopeful,Debutante) ;
    IF Rst = STR$_MATCH  THEN
       IF Negated  THEN
	  In_List := CLI$_NEGATED
	ELSE
	  In_List := CLI$_PRESENT
     ELSE
    IF NOT ODD (Rst) AND_THEN (Rst <> STR$_NOMATCH)  THEN
       LIB$SIGNAL (Rst) ;
  UNTIL (Spot = 0)
  END ; 	{ of In-List }


PROCEDURE Validate_Access ;

{ For the type of user running the program, validate the user's access
  to the target user's UAF record.

  Non-SysProgs can only jump (/NOEXACT) to users who do not have any
  privileges other than those that are "OK".  (SITE-specific privilege list) }

  VAR	Rst , I : INTEGER := 0 ;
	Chekov ,			{ User has JUMP's rights ID? }
	Sysprog ,			{ SETPRV or group <= MAXSYSGROUP ? }
	Operator ,			{ OPER ? }
	Priv_Target ,			{ Target is "privileged" ? }
	Id_Check_Ok : BOOLEAN := FALSE ;{ JUMP_ACCESS rights ID check ok ? }
	Access : Access_Status := Unspecified ; { Result of access list checks }
	Jump_Id : $UQUAD := ZERO ;

  FUNCTION Check_Access_List : Access_Status ;

  { Determine if the caller is specifically authorised to access the target
    user in the access list data file. }

    VAR Spot : INTEGER := 0 ;
	Rst , Target_Status : UNSIGNED := 0 ;
	Valid : BOOLEAN := FALSE ;
	Src , Dst , Buf : VARYING [120] OF CHAR := '' ;
	Access : TEXT ;

    BEGIN	{ Check_Access_List }
    Check_Access_List := Unspecified ;			{ Default }

    OPEN (Access,FILE_NAME:=Access_List,HISTORY:=Old,SHARING:=READONLY,
		 Error:=CONTINUE) ;
    Rst := STATUS (Access) ;
    IF Rst = PAS$K_SUCCESS  THEN
       BEGIN
       RESET (Access) ;
       WHILE NOT (EOF (Access) OR Valid)  DO
	 BEGIN
	 READLN (Access,Buf) ;
	 Str_Compress (Buf,Buf,TRUE) ;			{ Squeeeeeeze! }
	 Rst := STR$UPCASE (%DESCR Buf,Buf) ;
	 IF NOT ODD (Rst)  THEN
	    LIB$SIGNAL (Rst) ;
	 IF (Buf.LENGTH > 0) AND_THEN (Buf[1] <> '#')  THEN    { Not a comment }
	    BEGIN
	    Spot := INDEX (Buf,':') ;
	    IF Spot = 0  THEN  Exit (Bad_Access_Data) ;
	    Src  := SUBSTR (Buf,1,Spot-1) ;
	    Dst  := SUBSTR (Buf,Spot+1,Buf.LENGTH-Spot) ;
	    Spot := INDEX (Dst,'#') ;			{ Trailing comment? }
	    IF Spot > 0  THEN
	       Dst := SUBSTR (Dst,1,Spot-1) ;
	    IF (Src = '') OR (Dst = '')  THEN  Exit (Bad_Access_Data) ;
	    Target_Status := In_List (New_User,Dst) ;
	    IF In_List (Orig_User,Src) = CLI$_PRESENT  THEN   { User in source }
	       IF Target_Status = CLI$_PRESENT	THEN	{ New user in target...}
		  Check_Access_List := Granted		{ ... NOT negated }
		ELSE
	       IF Target_Status = CLI$_NEGATED	THEN	{ ... negated! }
		  Check_Access_List := Denied ;
	    END ;
	 END ;
       CLOSE (Access) ;
       END
     ELSE
    IF Rst <> PAS$K_FILNOTFOU  THEN
       BEGIN
       WRITELN ('%JUMP-F-BADACCFIL, Failed to open access list file; error ',
		Rst:1) ;
       $EXIT ;
       END ;
    END ;	{ of Check_Access_List }

  BEGIN 	{ Validate_Access }

  { Check that the invoker has the required access to run this program.
    This is independant of any installed privileges.  Identify SysProgs.
    If need be, check to see if process has JUMP_ACCESS rights ID }

  IF Double_Check  THEN
     BEGIN
     Rst := $ASCTOID (Name   := 'JUMP_ACCESS',
		      Id     := Jump_Id.L0,
		      Attrib := Jump_Id.L1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     I := 1 ;
     REPEAT
       Chekov := Jump_Id.L0 = Rights[I].L0 ;
       I := I + 1 ;
     UNTIL Chekov OR (I > Rightsize) ;
     END ;

  IF Proc_Cnt > 0  THEN
     BEGIN
     WRITELN ('%JUMP-F-NOSUB, Cannot JUMP while process has sub-processes.') ;
     $EXIT ;
     END
   ELSE
  IF Pid <> Master_Pid	THEN
     BEGIN
     WRITELN ('%JUMP-F-NOINSUB, Cannot JUMP from a sub-process.') ;
     $EXIT ;
     END
   ELSE
     BEGIN
     Sysprog  := (Uic.UIC$V_GROUP <= Max_Sys_Group) OR
		 (Setprv IN Proc_Auth_Priv) OR
		 (Setprv IN Proc_Def_Priv) ;

     Operator := (Oper IN Proc_Auth_Priv) OR
		 (Oper IN Proc_Def_Priv) ;
     END ;

  Id_Check_Ok := (Double_Check AND Chekov) OR (NOT Double_Check) ;
  Access := Check_Access_List ;

  IF NOT Get_Target_Info  THEN
     IF Figment  THEN
	WRITELN ('%JUMP-W-INVUSER, Invalid username - user does not exist.')
      ELSE
	BEGIN
	WRITELN ('%JUMP-F-INVUSER, Invalid username - user does not exist.') ;
	$EXIT (RMS$_RNF) ;
	END ;

  Priv_Target := ((Auth_Priv - Minor_Privs) <> []) OR
		 ((Def_Priv  - Minor_Privs) <> []) OR
		 (New_Uic.UIC$V_GROUP <= Max_Sys_Group) ;

  IF (NOT (Sysprog OR
	   ((Access = Granted) AND Id_Check_Ok) OR
	   (Operator AND NOT Priv_Target AND (Access <> Denied)) OR
	   ((New_User = Orig_User) AND Narcissus))) OR
     (Alter_Ego AND NOT Sysprog)  THEN
     BEGIN
     Audit_Jump (FALSE) ;
     $EXIT (SS$_NOPRIV) ;
     END ;

  IF NOT Auditing AND NOT Sysprog  THEN
     BEGIN
     WRITELN ('%JUMP-F-MUSTAUDIT, ',
	      'Only Systems Programmers may disable auditing.') ;
     $EXIT (SS$_NOPRIV) ;
     END ;

  IF Alter_Ego AND (New_User = Orig_User)  THEN
     BEGIN
     WRITELN ('%JUMP-I-SAMEUSER, Same username as current (',Orig_User,
	      ') - no action taken.') ;
     $EXIT ;
     END ;

  IF Real_Mccoy AND Flags.UAI$V_DISACNT  THEN
     Exit ('%JUMP-F-DISABLED, Username is disabled.') ;

  IF Transmute	THEN
     IF New_Uic.UIC$L_UIC = Uic.UIC$L_UIC  THEN
	BEGIN
	Format_User (Uic) ;
	WRITELN ('%JUMP-I-SAMEUIC, Same UIC as current (',Uic_Str,
		 ') - no action taken.') ;
	$EXIT ;
	END
      ELSE
     IF (Flags.UAI$V_RESTRICTED OR Flags.UAI$V_CAPTIVE)  THEN
	IF Sysprog  THEN
	   WRITELN ('%JUMP-W-RESTRICT, Username is Restricted or Captive.')
	 ELSE
	   BEGIN
	   WRITELN ('%JUMP-F-RESTRICT, Username is Restricted or Captive.') ;
	   $EXIT (SS$_NOPRIV) ;
	   END
      ELSE
     IF Flags.UAI$V_DISACNT  THEN
	IF Sysprog  THEN
	   WRITELN ('%JUMP-W-DISABLED, Username is disabled.')
	 ELSE
	   BEGIN
	   WRITELN ('%JUMP-F-DISABLED, Username is disabled.') ;
	   $EXIT (SS$_NOPRIV) ;
	   END ;
  END ; 	{ of Validate_Access }


[ASYNCHRONOUS,Check(None)] PROCEDURE Getuser ;

{ In EXEC MODE, peek at the Username in the Control Region and in the PCB. }

  BEGIN
  Sanity_Ctl_User := CTL$T_USERNAME ;
  Sanity_Jib_User := Jib_User_Ptr^ ;
  END ;


[ASYNCHRONOUS,Check(None)] PROCEDURE Setuser ;

{ In KERNEL MODE, poke a new Username into the Control Region and into the PCB.}

  BEGIN
  CTL$T_USERNAME := New_User.BODY ;
  Jib_User_Ptr^  := New_User.BODY ;
  END ;


[ASYNCHRONOUS,Check(None)] PROCEDURE Read_Longword
				(VAR Location ,
				 Pointer  : [VOLATILE,UNSAFE] Unsigned_Ptr) ;

{ In EXEC MODE, peek at a location. }

  BEGIN
  Location::UNSIGNED := Pointer^ ;
  END ;


[ASYNCHRONOUS,Check(None)] PROCEDURE Write_Longword
				(VAR Location ,
				 Pointer  : [VOLATILE,UNSAFE] Unsigned_Ptr) ;

{ In KERNEL MODE, poke a value into a location. }

  BEGIN
  Pointer^ := Location::UNSIGNED ;
  END ;


[GLOBAL] PROCEDURE Getmem (VAR Location ,
			   Pointer  : [VOLATILE,UNSAFE] Unsigned_Ptr) ;

{ Jacket routine to peek at a location in EXEC MODE. }

  VAR Arglst : [UNSAFE] ARRAY [1..3] OF UNSIGNED := (2,0,0) ;  { Argument list }

  BEGIN
  Arglst[2] := IADDRESS (Location) ;
  Arglst[3] := IADDRESS (Pointer) ;
  $CMEXEC (%IMMED Read_Longword,%REF Arglst) ;
  END ;


PROCEDURE Putmem (VAR Location ,
		  Pointer  : [VOLATILE,UNSAFE] Unsigned_Ptr) ;

{ Jacket routine to poke a value into a location in KERNEL MODE. }

  VAR Arglst : [UNSAFE] ARRAY [1..3] OF UNSIGNED := (2,0,0) ;  { Argument list }

  BEGIN
  Arglst[2] := IADDRESS (Location) ;
  Arglst[3] := IADDRESS (Pointer) ;
  $CMKRNL (%IMMED Write_Longword,%REF Arglst) ;
  END ;


PROCEDURE Poteroo (Faking : BOOLEAN := TRUE) ;

{ Change the UIC.  Do sanity checks except when reverting from
  pseudo-terminal. }

  VAR Sanity_Uic : [VOLATILE] UIC$TYPE := ZERO ;

  BEGIN 	{ Poteroo }

  { Check that the UIC as returned by GETJPI and as peeked at in EXEC MODE
    agree -- do this as a sanity check. }

  Uic_Ptr::UNSIGNED := CTL$GL_PCB + PCB$L_UIC ;
  Getmem (Sanity_Uic.UIC$L_UIC,Uic_Ptr) ;

  IF Faking AND (Sanity_Uic.UIC$L_UIC <> Uic.UIC$L_UIC)  THEN
     BEGIN
     WRITELN ('%JUMP-F-INSANEUIC, Sanity Check FAILED for UIC!') ;
     WRITELN ('%JUMP-F-UICVALUES, GetUAI =',HEX (Uic.UIC$L_UIC),' Kernel =',
	      HEX (Sanity_Uic.UIC$L_UIC)) ;
     $EXIT (SS$_ABORT) ;
     END
   ELSE
     Putmem (New_Uic.UIC$L_UIC,Uic_Ptr) ;	{ Change UIC to be target UIC. }
  END ; 	{ of Poteroo }


PROCEDURE Wallaby (Faking : BOOLEAN := TRUE) ;

{ Change the username.	Do sanity checks except when reverting from
  pseudo-terminal }

  BEGIN 	{ Wallaby }

  { Check that the username as returned by GETJPI and as peeked at in EXEC MODE
    agree -- do this as a sanity check. }

  Jib_Ptr::UNSIGNED := CTL$GL_PCB + PCB$L_JIB ;
  Getmem (Jib_Ptr,Jib_Ptr) ;

  Jib_User_Ptr::UNSIGNED := Jib_Ptr::UNSIGNED + JIB$T_USERNAME ;
  $CMEXEC (Getuser,%IMMED 0) ;

  IF Faking AND ((Sanity_Ctl_User <> Orig_User.BODY)  OR
     (Sanity_Jib_User <> Orig_User.BODY))  THEN
     BEGIN
     WRITELN ('%JUMP-F-INSANEUSER, Sanity Check FAILED for Username!') ;
     WRITELN ('%JUMP-F-USERVALUES, GetJPI = ',Orig_User,' Control = ',
	      Sanity_Ctl_User,' JIB = ',Sanity_Jib_User) ;
     $EXIT (SS$_ABORT) ;
     END
   ELSE
     $CMKRNL (Setuser,%IMMED 0) ;
  END ; 	{ of Wallaby }


PROCEDURE Kangaroo ;

{ Do all that is required to "jump" to the new user, or to return to the
  original user. }

  VAR	Rst , Attributes : UNSIGNED := 0 ;
	Aclstr : VARYING [64] OF CHAR := '' ;
	Aclent : PACKED ARRAY [1..32] OF CHAR := '' ;
	Grptbl : PACKED ARRAY [1..16] OF CHAR := '' ;
	Item_List : Item_List_Template (3) := ZERO ;

  BEGIN 	{ Kangaroo }

  { Set new UIC. }

  Poteroo ;

  { Set new default directory. }

  Rst := SYS$SETDDIR (SUBSTR (Def_Dir.BODY,1,Def_Dir.LENGTH)) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { Set new default disk. }

  Item_List[1].Buffer_Length := Def_Dev.LENGTH ;
  Item_List[1].Item_Code     := LNM$_STRING ;
  Item_List[1].Buffer_Addr   := IADDRESS (Def_Dev.BODY) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Terminator    := 0 ;

  Rst := $CRELNM (Tabnam := %STDESCR 'LNM$PROCESS',
		  Lognam := %STDESCR 'SYS$DISK',
		  Acmode := PSL$C_SUPER,
		  Itmlst := Item_List) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { Point LNM$GROUP logical to group table for new UIC. }

  Attributes := LNM$M_TERMINAL ;
  Grptbl := 'LNM$GROUP_' + OCT (New_Uic.UIC$V_GROUP,6,6) ;

  Item_List[1].Buffer_Length := 4 ;
  Item_List[1].Item_Code     := LNM$_ATTRIBUTES ;
  Item_List[1].Buffer_Addr   := IADDRESS (Attributes) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Buffer_Length := SIZE (Grptbl) ;
  Item_List[2].Item_Code     := LNM$_STRING ;
  Item_List[2].Buffer_Addr   := IADDRESS (Grptbl) ;
  Item_List[2].Return_Addr   := 0 ;

  Item_List[3].Terminator    := 0 ;

  Rst := $CRELNM (Tabnam := %STDESCR 'LNM$PROCESS_DIRECTORY',
		  Lognam := %STDESCR 'LNM$GROUP',
		  Acmode := PSL$C_KERNEL,
		  Itmlst := Item_List) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { If going to a different UIC, allow the current LNM$JOB logical name table
    to be accessed by the new UIC.  If returning to original user, remove the
    ACL access previously applied. }

  Item_List[2].Terminator := 0 ;

  IF New_User = Orig_User  THEN 	{ Return to original user. }
     BEGIN
     Item_List[1].Buffer_Length := 0 ;
     Item_List[1].Item_Code	:= ACL$C_DELETEACL ;
     Item_List[1].Buffer_Addr	:= 0 ;
     Item_List[1].Return_Addr	:= 0 ;
     END
   ELSE
     BEGIN
     Aclstr := '(IDENTIFIER=' + New_User + ',ACCESS=READ+WRITE)' ;
     Rst := $PARSE_ACL (Aclstr := SUBSTR (Aclstr.BODY,1,Aclstr.LENGTH),
			Aclent := %STDESCR Aclent) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Item_List[1].Buffer_Length := INT (Aclent[1]) ;
     Item_List[1].Item_Code	:= ACL$C_ADDACLENT ;
     Item_List[1].Buffer_Addr	:= IADDRESS (Aclent) ;
     Item_List[1].Return_Addr	:= 0 ;
     END ;

  Rst := $CHANGE_ACL (Objtyp := ACL$C_LOGICAL_NAME_TABLE,
		      Objnam := 'LNM$JOB',
		      Itmlst := Item_List) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of Kangaroo }


PROCEDURE Display_Jump ;

{ Display data about the requested jump. }

  BEGIN 	{ Display_Jump }
  IF Transmute	THEN
     BEGIN
     WRITELN ('%JUMP-S-JUMPED, ',Orig_User,' jumped to ',
	      New_User,' (',Uic_Str,Eq_Id_Str,')') ;
     WRITELN ('%JUMP-I-DEFAULT, Default is ',Def_Dev,Def_Dir) ;
     END ;

  IF Alter_Ego	THEN
     WRITELN ('%JUMP-S-SETUSER, Changed username from ',Orig_User,' to ',
	      New_User) ;
  END ; 	{ of Display_Jump }


PROCEDURE Get_Channel (Device : VARYING [L1] OF CHAR ;
		       VAR Channel : [VOLATILE] $UWORD) ;

{ Assign a channel to a device. }

  VAR Rst : UNSIGNED := 0 ;

  BEGIN 	{ Get_Channel }
  Rst := $ASSIGN (Devnam := Device,
		  Chan	 := Channel) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of Get_Channel }


PROCEDURE Exit_Handler (Condition : UNSIGNED) ;

{ Clean up on exit from pseudo-terminal. }

  VAR Rst  : UNSIGNED := 0 ;
      Iosb : Status_Block_Type := ZERO ;

  BEGIN 	{ Exit_Handler }

  $SETAST (0) ; 			{ Disable ASTs }

  IF Pseudo_Ft	THEN
     BEGIN
     Rst := PTD$CANCEL (Pchan) ;	{ Cancel I/O to pseudo-terminal }
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := PTD$DELETE (Pchan) ;	{ Delete pseudo-terminal }
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     $DASSGN (Pchan) ;			{ Deassign pseudo-terminal channel }

  $CANCEL (Rchan) ;			{ Cancel I/Os on real terminal }

  Rst := $QIOW (Chan := Rchan,		{ Restore original characteristics }
		Func := IO$_SETMODE,
		Iosb := Iosb,
		P1   := Rchars,
		P2   := 12) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1]) ;

  $DASSGN (Rchan) ;			{ Shut up shop. }

  END ; 	{ of Exit_Handler }


[ASYNCHRONOUS] PROCEDURE Rchan_Ast ;

{ Called when a keystroke occurs on the real keyboard - the keystrokes are
  passed to the pseudo-terminal and another read queued. }

  VAR  Rst : UNSIGNED := 0 ;

  BEGIN 	{ Rchan_Ast}
  IF Pseudo_Ft	THEN
     BEGIN
     Rst := PTD$WRITE (Chan	  := Pchan,
		       Wrtbuf	  := Buffer[Wsts],
		       Wrtbuf_Len := 1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := $QIO (Chan	 := Rchan,
		  Func	 := IO$_READVBLK,
		  Astadr := Rchan_Ast,
		  P1	 := Buffer[Wbuf],
		  P2	 := 1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     BEGIN
     Rst := $QIO (Chan := Pchan,
		  Func := IO$_WRITEVBLK,
		  P1   := Buffer[Wsts],
		  P2   := 1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := $QIO (Chan	 := Rchan,
		  Func	 := IO$_READVBLK,
		  Astadr := Rchan_Ast,
		  P1	 := Buffer[Wsts],
		  P2	 := 1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END ;
  END ; 	{ of Rchan_Ast }


[ASYNCHRONOUS] PROCEDURE Pchan_Ast ;

{ Called when characters are received from the pseudo-terminal - the data are
  passed to the real screen and another read queued. }

  VAR  Rst : UNSIGNED := 0 ;

  BEGIN 	{ Pchan_Ast }
  IF Pseudo_Ft	THEN
     BEGIN
     Rst := $QIOW (Chan := Rchan,
		   Func := IO$_WRITEVBLK,
		   P1	:= Buffer[Rbuf],
		   P2	:= Buffer[Rcnt]) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := PTD$READ (Chan	  := Pchan,
		      Astadr	  := Pchan_Ast,
		      Readbuf	  := Buffer[Rsts],
		      Readbuf_Len := Ft_Buflen) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     BEGIN
     Rst := $QIOW (Chan := Rchan,
		   Func := IO$_WRITEVBLK,
		   P1	:= Buffer[Rsts],
		   P2	:= Piosb[2]) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := $QIO (Chan	 := Pchan,
		  Func	 := IO$_READVBLK,
		  Iosb	 := Piosb,
		  Astadr := Pchan_Ast,
		  P1	 := Buffer[Rsts],
		  P2	 := Py_Buflen) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END ;
  END ; 	{ of Pchan_Ast }


[ASYNCHRONOUS] PROCEDURE Mbast ;

{ Invoked by the detached process termination mailbox AST - WAKE UP! }

  VAR  Rst : UNSIGNED := 0 ;

  BEGIN 	{ MBast }
  Rst := $WAKE ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;
  END ; 	{ of MBast }


PROCEDURE Transmography ;

{ Create a pseudo-terminal connected to a detached process and actually *be*
  the user! }

  TYPE

  Desc_Blk = PACKED RECORD			{ Exit handler descriptor }
	       Fwd_Link : UNSIGNED ;
	       Exit_Handler_Addr : UNSIGNED ;
	       Argcnt : $UBYTE ;
	       Fill_Zero : [UNSAFE] ARRAY [1..3] OF $UBYTE ;
	       Condition ,
	       P2 , P3 , P4 , P5 , P6 , P7 : UNSIGNED ;
	     END VALUE ZERO ;

  VAR

  Rst , Mbunit : UNSIGNED := 0 ;
  Pctl : [VOLATILE,LONG] Prtctl_Type := ZERO ;
  Pctl_Ptr : [VOLATILE] Word_Ptr := NIL ;
  Rucb , Pucb ,
  Rapn, Papn: [VOLATILE] Unsigned_Ptr := NIL ;
  Newchars : Terminal_Chars := ZERO ;
  Exit_Desc : [STATIC] Desc_Blk := ZERO ;
  Iosb : Status_Block_Type := ZERO ;
  Item_List : Item_List_Template (2) := ZERO ;

  FUNCTION Find_Device (Device : VARYING [L1] OF CHAR ;
			Chan   : $UWORD := 0) : BOOLEAN ;

  { Determine if a device exists and return its name. }

    VAR Rst : UNSIGNED := 0 ;
	Iosb : Status_Block_Type := ZERO ;
	Item_List : Item_List_Template (2) := ZERO ;

    BEGIN	{ Find_Device }
    Find_Device := TRUE ;

    Item_List[1].Buffer_Length := SIZE (Pdev.BODY) ;
    Item_List[1].Item_Code     := DVI$_DEVNAM ;
    Item_List[1].Buffer_Addr   := IADDRESS (Pdev.BODY) ;
    Item_List[1].Return_Addr   := IADDRESS (Pdev.LENGTH) ;

    Item_List[2].Terminator    := 0 ;		{ Terminate the item list }

    IF (Chan = 0) AND (Device <> '')  THEN	{ Device name supplied }
       Rst := $GETDVIW (Itmlst := Item_List,
			Devnam := Device,
			Iosb   := Iosb)
     ELSE					{ Channel number supplied }
       Rst := $GETDVIW (Itmlst := Item_List,
			Chan   := Chan,
			Iosb   := Iosb) ;

    IF Rst = SS$_NOSUCHDEV  THEN
       Find_Device := FALSE
     ELSE
    IF NOT ODD (Rst)  THEN
       LIB$SIGNAL (Rst)
     ELSE
    IF NOT ODD (Iosb[1])  THEN
       LIB$SIGNAL (Iosb[1]) ;
    END ;	{ of Find_Device }

  PROCEDURE Get_Ucb (Chan_Num : UNSIGNED ;
		     VAR Ucb : [VOLATILE] Unsigned_Ptr) ; EXTERNAL ;

  BEGIN 	{ Transmography }

  { Determine which pseudo-terminal type (if any) exists on the system. }

  Pseudo_Ft := Find_Device ('FTA0:') ;
  IF NOT Pseudo_Ft  THEN
     IF NOT Find_Device ('PYA0:')  THEN
	Exit ('%JUMP-F-NOPSEUDO, No pseudo-terminal driver on system.') ;

  Get_Channel ('TT:',Rchan) ;

  { Get current (real) terminal process-specific device characteristics }

  Rst := $QIOW (Chan := Rchan,
		Func := IO$_SENSEMODE,
		Iosb := Iosb,
		P1   := Rchars,
		P2   := 12) ;

  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1]) ;

  { Set up and declare the exit handler }

  Exit_Desc.Exit_Handler_Addr := IADDRESS (Exit_Handler) ;
  Exit_Desc.Argcnt := 1 ;
  Exit_Desc.Condition := IADDRESS (Exit_Rst) ;

  Rst := $DCLEXH (Desblk := Exit_Desc) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { Set new terminal characteristics }

  Newchars := Rchars ;
  Newchars.Tt_Devchar.TT$V_NOECHO    := TRUE ;
  Newchars.Tt_Devchar.TT$V_WRAP      := FALSE ;
  Newchars.Tt_Devchar2.TT2$V_PASTHRU := TRUE ;

  Rst := $QIOW (Chan := Rchan,
		Func := IO$_SETMODE,
		Iosb := Iosb,
		P1   := Newchars,
		P2   := 12) ;

  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1]) ;

  Wallaby ;	{ Change to new username }
  Poteroo ;	{ Change to new UIC }

  { Create the pseudo-terminal }

  Pbuf_Range[1] := IADDRESS (Buffer) ;
  Pbuf_Range[2] := Pbuf_Range[1] + Io_Buflen * 2 - 1 ;

  IF Pseudo_Ft	THEN
     BEGIN
     Rst := PTD$CREATE (Chan  := Pchan,
			Inadr := Pbuf_Range) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     BEGIN
     Get_Channel ('PYA0:',Pchan) ;
     END ;

  Find_Device ('',Pchan) ;		{ Sets Pdev to device name }

  { If the "real" process has a valid value for ACCPORNAM, set the "pseudo"
    process to point to it. }

  Get_Ucb (Pchan,Pucb) ;
  Pctl_Ptr::UNSIGNED := Pucb::UNSIGNED + UCB$W_TT_PRTCTL ;
  Getmem (Pctl,Pctl_Ptr) ;
  Papn::UNSIGNED := Pucb::UNSIGNED + UCB$L_TT_ACCPORNAM ;

  IF Port = ''	THEN
     BEGIN

     { Welcome to a futureware section of code!  When ACCPORNAM is not valid,
       the port name will be empty.  In this case, use the terminal name.
       Allocate an appropriate buffer, copy the terminal name into it,
       put the address of the buffer into ACCPORNAM of the pseudo-terminal
       and set the validity bit in the port control mask.  When the pseudo-
       terminal is terminated, deallocate the buffer.  This exercise remains to
       be attempted ... or supplied by an eager code jockey somewhere else! :) }

     END
   ELSE
     BEGIN
     Get_Ucb (Rchan,Rucb) ;
     Rapn::UNSIGNED := Rucb::UNSIGNED + UCB$L_TT_ACCPORNAM ;
     Getmem (Rapn,Rapn) ;
     Putmem (Rapn,Papn) ;
     Pctl::Prtctl_Type.TTY$V_PC_ACCPORNAM := TRUE ;
     Putmem (Pctl,Pctl_Ptr) ;
     END ;

  { Create a termination mailbox for the soon-to-be detached process,
    and get its unit number }

  Rst := $CREMBX (Chan	 := Mchan,
		  Maxmsg := ACC$K_TERMLEN) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  Item_List[1].Buffer_Length := 4 ;
  Item_List[1].Item_Code     := DVI$_UNIT ;
  Item_List[1].Buffer_Addr   := IADDRESS (Mbunit) ;
  Item_List[1].Return_Addr   := 0 ;

  Item_List[2].Terminator    := 0 ;	{ Terminate the item list }

  Rst := $GETDVIW (Itmlst := Item_List,
		   Chan   := Mchan,
		   Iosb   := Iosb) ;
  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst)
   ELSE
  IF NOT ODD (Iosb[1])	THEN
     LIB$SIGNAL (Iosb[1]) ;

  { Queue an asynchronous read to the mailbox }

  Rst := $QIO (Chan   := Mchan,
	       Func   := IO$_READVBLK,
	       Astadr := Mbast,
	       P1     := Mbbuf,
	       P2     := ACC$K_TERMLEN) ;

  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  IF Auditing  THEN
     Audit_Jump (TRUE) ;

  WRITELN ('%JUMP-S-TRANSFER, Control transferred to user ',New_User,Lf) ;

  { Create the detached process }

  Rst := $CREPRC (Image  := 'SYS$SYSTEM:LOGINOUT.EXE',
		  INPUT  := Pdev,
		  OUTPUT := Pdev,
		  Baspri := 4,
		  Mbxunt := Mbunit,
		  Stsflg := PRC$M_DETACH + PRC$M_INTER + PRC$M_NOPASSWORD) ;

  IF NOT ODD (Rst)  THEN
     LIB$SIGNAL (Rst) ;

  { Restore username and UIC }

  New_User.BODY := PAD ('',' ',12) ;	{ Completely blat any previous name }
  New_User := Orig_User ;
  New_Uic  := Uic ;

  Wallaby (FALSE) ;	{ Change to original username }
  Poteroo (FALSE) ;	{ Change to original UIC }

  { Queue the appropriate reads to both the real terminal and pseudo-terminal }

  IF Pseudo_Ft	THEN
     BEGIN
     Rst := $QIO (Chan	 := Rchan,
		  Func	 := IO$_READVBLK,
		  Astadr := Rchan_Ast,
		  P1	 := Buffer[Wbuf],
		  P2	 := 1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := PTD$READ (Chan	  := Pchan,
		      Astadr	  := Pchan_Ast,
		      Readbuf	  := Buffer[Rsts],
		      Readbuf_Len := Ft_Buflen) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END
   ELSE
     BEGIN
     Rst := $QIO (Chan	 := Rchan,
		  Func	 := IO$_READVBLK,
		  Astadr := Rchan_Ast,
		  P1	 := Buffer[Wsts],
		  P2	 := 1) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;

     Rst := $QIO (Chan	 := Pchan,
		  Func	 := IO$_READVBLK,
		  Iosb	 := Piosb,
		  Astadr := Pchan_Ast,
		  P1	 := Buffer[Rsts],
		  P2	 := Py_Buflen) ;
     IF NOT ODD (Rst)  THEN
	LIB$SIGNAL (Rst) ;
     END ;

  { Hibernate until termination mailbox message wakes us up. }

  $HIBER ;

  WRITELN ('%JUMP-S-RETURN, Control returned to user ',Orig_User,Lf,Lf) ;
  END ; 	{ of Transmography }


{ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * *	M A I N   P R O G R A M   * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * }


BEGIN	{ Jump }

Get_Caller_Info ;

IF Get_And_Parse_Command  THEN
   BEGIN
   Validate_Access ;			{ Stop intruders }

   IF Real_Mccoy  THEN
      Transmography			{ Clone! }
    ELSE
      BEGIN
      IF Alter_Ego  THEN		{ Change username }
	 Wallaby ;
      IF Transmute  THEN		{ Long jump! }
	 BEGIN
	 Kangaroo ;			{ Boing! Change miscellany of items }
	 IF Auditing  THEN
	    Audit_Jump (TRUE)
	  ELSE
	    Format_User (New_Uic) ;
	 END ;
      IF Log  THEN  Display_Jump ;
      END ;
   END ;

END.	{ of it all }
