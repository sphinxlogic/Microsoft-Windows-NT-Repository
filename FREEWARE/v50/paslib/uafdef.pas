{************************************************************************
*									*
*  This file contains declarations for use in programs that reference	*
*  Version 6 User Authorization Files.					*
*  The following documents should be refered to:			*
*									*
*	Authorize Utility Manual;		)			*
*	$UAFDEF module of SYS$LIBRARY:LIB.MLB;	 )  Version 6		*
*	$PRVDEF module of STARLET.MLB;		)			*
*									*
*.......................................................................*
*									*
*  Author: Jonathan Ridler,						*
*	   Information Technology Services,				*
*	   The University of Melbourne. 				*
*									*
*.......................................................................*
*									*
*  History:								*
*	02-Feb-1986	JER	Version 4 version.			*
*	16-Nov-1992	JER	Version 5 version.			*
*	20-Dec-1995	JER	Version 6 version.			*
************************************************************************}

CONST			{ ***  C O N S T  *** }

{ "Maxavail" declared here is the length in bytes of the maximum size variable
  length record in the User Authorization File, MINUS 2 .  This adjustment
  accounts for the way in which Pascal interprets the varying array declaration
  in the variant record declaration for the file, treating the .LENGTH field
  of the varying array as the first two bytes of data in the record and
  consequently allowing two less bytes in the maximum size of the .BODY }

Maxavail = 1410 ;	{ Max. size of variable length UAF records (1412 - 2) }

TYPE			{ ***  T Y P E	*** }

Flag = (Disctly,	{  0: Disable future CTRL/Y interrupts }
	Defcli, 	{  1: Restricts user to default command interpreter }
	Lockpwd,	{  2: Lock user's password (i.e. it can't be changed) }
	Restricted,	{  3: Several functions including those of flags above }
	Disuser,	{  4: Disable the user's account entirely }
	Diswelcome,	{  5: Suppresses login msg : "Welcome to ..." }
	Disnewmail,	{  6: Suppresses announcement of new mail at login }
	Dismail,	{  7: Disable mail delivery to the user }
	Genpwd, 	{  8: Requires user to use generated passwords }
	Pwd_Expired,	{  9: Marks password as expired }
	Pwd2_Expired,	{ 10: Marks second password as expired }
	Audit_Flag,	{ 11: Enables auditing of security relevant actions }
	Disreport,	{ 12: Disables reports of login information }
	Disreconnect,	{ 13: Disables automated terminal reconnection }
	Autologin,	{ 14: Account available only by autologin mechanism }
	Disforce_Pwd_Change,	{ 15: Disable forced password change }
	Captive,	{ 16: Make account captive }
	Disimage,	{ 17: Cannot run images }
	Dispwddic,	{ 18: Disable password dictionary check }
	Dispwdhis,	{ 19: Disable password history check }
	Fbit20, Fbit21, Fbit22, Fbit23, Fbit24, Fbit25, Fbit26, Fbit27, Fbit28,
	Fbit29, Fbit30, Fbit31) ; { 20-31: Dummies allow packed set of 4 bytes }

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

Privset = PACKED SET OF Privilege ;	{ To allow easy bit union, etc. }
Flagset = PACKED SET OF Flag ;

Uafrec = PACKED  RECORD 	{ A UAF record }
	 CASE BOOLEAN  OF
	   TRUE:  (A : [ALIGNED(0)] VARYING [Maxavail] OF CHAR) ; { Complete }
	   FALSE: (B : PACKED RECORD
			   Rtype : [ALIGNED(0)] $UBYTE ;
			   Version : $UBYTE ;
			   Usrdatoff : $UWORD ;
			   Username : [Key(0),ALIGNED(0)]
					PACKED ARRAY [1..32] OF CHAR ;
			   Uic : PACKED RECORD
				   CASE BOOLEAN  OF
				    TRUE: (All : [Key(1),ALIGNED(0)] UNSIGNED);
				    FALSE: (Mem : [ALIGNED(0)] $UWORD ;
					     Grp : $UWORD) ;
				   END ;  { of Record Uic }
			   Subid : UNSIGNED ;
			   Parentid : PACKED ARRAY [1..8] OF CHAR ;
			   Account ,
			   Owner ,
			   Defdev : PACKED ARRAY [1..32] OF CHAR ; { 31+len }
			   Defdir ,
			   Lgicmd : PACKED ARRAY [1..64] OF CHAR ; { 63+len }
			   Defcli_Name ,
			   Clitables : PACKED ARRAY [1..32] OF CHAR ;
			   Pwd ,
			   Pwd2 : PACKED ARRAY [1..8] OF CHAR ;
			   Logfails ,
			   Salt : $UWORD ;
			   Encrypt ,
			   Encrypt2 ,		{ Always assume = 1 (Purdy) }
			   Pwdlength ,
			   Dummybyte363 : $UBYTE ;
			   Expiration ,
			   Pwdlifetime ,
			   Pwddate ,
			   Pwd2date ,
			   Lastlogin_I ,
			   Lastlogin_N : PACKED ARRAY [0..1] OF UNSIGNED ;
			   Privauth ,		{ Authorized privileges }
			   Privdef : Privset ;	{ Default privileges }
			   Minclass ,
			   Maxclass : PACKED ARRAY [1..20] OF CHAR ;
			   Flags : Flagset ;
			   Networkaccess_P,	{ Hours of primary access }
			   Networkaccess_S,	{ Hours of secondary access }
			   Batchaccess_P,
			   Batchaccess_S,
			   Localaccess_P,
			   Localaccess_S,
			   Dialupaccess_P,
			   Dialupaccess_S,
			   Remoteaccess_P,
			   Remoteaccess_S : PACKED ARRAY [0..23] OF BOOLEAN ;
			   Dummy12bytes502 : PACKED ARRAY [1..12] OF CHAR ;
			   Primedays : PACKED ARRAY [0..7] OF BOOLEAN ;
			   Dummybyte515,
			   Pri, 	{ Default Base Priority }
			   Quepri : $UBYTE ;
			   Maxjobs,	{ Total active processes limit }
			   Maxacctjobs, { Max. processes for one account }
			   Maxdetach,	{ Max. detached processes limit }
			   Prccnt,	{ Sub-process creation limit }
			   Biolm,	{ Buffered I/O count limit }
			   Diolm,	{ Direct I/O count limit }
			   Tqcnt,	{ Timer queue entry limit }
			   Astlm,	{ Asynchronous System Trap queue lim }
			   Enqlm,	{ Locks queued limit }
			   Fillm,	{ Open file limit }
			   Shrfillm : $UWORD ;	{ Max. open shared files }
			   Wsquota,	{ Working Set quota }
			   Dfwscnt,
			   Wsextent ,	{ Working Set extent }
			   Pgflquota ,	{ Page file quota }
			   Cputime ,	{ Maximum CPU time }
			   Bytlm,	{ Buffered I/O byte limit }
			   Pbytlm ,
			   Jtquota : INTEGER ;	{ Job-wide LNT initial bytes }
			   Proxylim,
			   Proxies,
			   Accountlim,
			   Accounts : $UWORD ;
			 END) ; { of Uafrec.Part }
	END ;	{ of Uafrec Record }


VAR			{ ***  V A R  *** }

Priv0	 : Privilege := Cmkrnl ;	{ Bit  0 of Privileges }
Privhigh : Privilege := Security ;	{ Bit 38 of Privileges }
Allprivs : Privset   := [Cmkrnl..Pbit63] ;	{ All privileges }

Flag0	 : Flag := Disctly ;			{ Bit  0 of Flags word }
Flaghigh : Flag := Dispwdhis ;			{ Bit 19 of Flags word }
Allflags : Flagset := [Disctly..Fbit31] ;	{ All flags }

{ Digital standard names (distinguished by leading "_") equated to local
  synonyms }

_Disacnt   : Flag := Disuser ;			{ Bit 4 of Flags word }
_Diswelcom : Flag := Diswelcome ;		{ Bit 5 of Flags word }
_Dismail   : Flag := Disnewmail ;		{ Bit 6 of Flags word }
_Nomail    : Flag := Dismail ;			{ Bit 7 of Flags word }

