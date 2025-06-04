 
CONST	SMG$_FACILITY = 18;
	SMG$_NORMAL = 1212417;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	NORMAL SUCCESSFUL COMPLETION.                               *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_PREDEFREP = 1212433;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A PREVIOUS KEY DEFINITION WAS REPLACED BY A NEW             *)
(*		DEFINITION.                                                 *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
(*+                                                                         *)
(* THE NEXT THREE MESSAGES ARE USED BY THE "batching"                       *)
(* PROCEDURES, SMG$BEGIN_DISPLAY_UPDATE AND SMG$END_DISPLAY_UPDATE.         *)
(* THE TEXT OF THE MESSAGES SHOULD CHANGE IF WE DECIDE NOT                  *)
(* TO PUBLICLY USE THE TERM "batching".                                     *)
(*-                                                                         *)
 
CONST	SMG$_BATSTIPRO = 1212441;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A USER TRIED TO END BATCHING (UPDATING) WITH                *)
(*		A CALL TO SMG$END_DISPLAY_UPDATE OR                         *)
(*		SMG$END_PASTEBOARD_UPDATE.  THIS DID NOT                    *)
(*		TERMINATE BATCHING BECAUSE THERE WERE MORE CALLS            *)
(*		MADE TO BEGIN BATCHING THAN TO END BATCHING.                *)
(* USER ACTION:	NONE.  SOME OUTER LEVEL ROUTINE PROBABLY STILL              *)
(*		WANTS BATCHING TO BE IN EFFECT.                             *)
(*--                                                                        *)
 
CONST	SMG$_BATWASOFF = 1212449;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A PROGRAM TRIED TO TURN OFF BATCHING WHEN                   *)
(*		BATCHING WAS ALREADY OFF.                                   *)
(* USER ACTION:	PROBABLY NONE.  CHECK THAT THERE WAS ONE CALL               *)
(*		TO TURN ON BATCHING FOR EACH CALL TO TURN IT OFF.           *)
(*--                                                                        *)
 
CONST	SMG$_BATWAS_ON = 1212457;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A PROGRAM TRIED TO TURN ON BATCHING WHEN                    *)
(*		BATCHING WAS ALREADY ON.  THIS IS NORMAL IN                 *)
(*		A MODULAR ENVIRONMENT.                                      *)
(* USER ACTION:	NONE.  CHECK THAT THERE WAS ONE CALL                        *)
(*		TO TURN ON BATCHING FOR EACH CALL TO TURN IT OFF.           *)
(*--                                                                        *)
 
CONST	SMG$_PASALREXI = 1212465;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A USER TRIED TO CREATE A PASTEBOARD FOR A DEVICE,           *)
(*		WHEN ONE WAS ALREADY CREATED.  THIS IS NORMAL IN            *)
(*		A MODULAR ENVIRONMENT.  THE EXISTING PASTEBOARD ID          *)
(*		IS RETURNED.                                                *)
(* USER ACTION:	NONE. MAKE SURE YOU DON'T DELETE A PASTEBOARD               *)
(*		THAT YOU ACTUALLY DIDN'T CREATE.                            *)
(*--                                                                        *)
 
CONST	SMG$_NOT_A_TRM = 1212473;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		THE PASTEBOARD HAS BEEN CREATED, BUT THE OUTPUT             *)
(*		DEVICE IS NOT A TERMINAL.                                   *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	SMG$_NO_MORMSG = 1212481;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A CALL TO SMG$GET_BROADCAST_MESSAGE FOUND THAT              *)
(*		THERE WERE NO MORE MESSAGES TO BE OBTAINED.                 *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	SMG$_WILUSERMS = 1212489;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	NORMALLY THE SCREEN IS UPDATED AT THE END OF                *)
(*		EVERY OUTPUT OPERATION.  IF, HOWEVER, THE 'DEVICE'          *)
(*		IS ACTUALLY A FILE OR AN UNKNOWN TERMINAL, THEN             *)
(*		THIS MESSAGE IS GIVEN AT THE TIME OUTPUT WOULD              *)
(*		HAVE BEEN SENT TO THE SCREEN.                               *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_NOTRMSOUT = 1212497;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	SMG$SNAPSHOT IS USED WITH FILES AND UNKNOWN DEVICES         *)
(*		TO CAPTURE THE CONTENTS OF THE PASTEBOARD AT VARIOUS        *)
(*		TIMES.  IF SMG$SNAPSHOT IS CALLED FOR A PASTEBOARD          *)
(*		ASSOCIATED WITH A VIDEO TERMINAL, THEN IT TAKES NO          *)
(*		ACTION AND RETURNS THIS INFORMATIONAL MESSAGE.              *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_NO_CHADIS = 1212505;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED NUMBER OF COLUMNS TO CHANGE WAS ZERO,         *)
(*		SO NOTHING IN THE VIRTUAL DISPLAY WAS CHANGED.              *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_PRISECMAP = 1212513;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED TERMINAL DEFINITION WAS FOUND IN              *)
(*		THE CALLER'S PRIVATE COPY OF TERMTABLE.                     *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_GBLSECMAP = 1212521;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED TERMINAL DEFINITION WAS FOUND IN              *)
(*		THE SYSTEM TERMTABLE.  (REQUESTED TERMINAL DEFINITION       *)
(*		DID NOT EXIST IN THE CALLER'S PRIVATE TERMTABLE.)           *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_WINTRUNCFIT = 1212529;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE VIEWPORT SPECIFICATION WILL NOT FIT WHOLLY WITHIN       *)
(*		THE VIRTUAL DISPLAY. THE VIEWPORT HAS BEEN TRUNCATED        *)
(*		TO FIT WITHIN THE VIRTUAL DISPLAY.                          *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_SUBALREXI = 1212537;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A USER TRIED TO CREATE A SUBPROCESS FOR A VIRTUAL           *)
(*		DISPLAY, WHEN ONE WAS ALREADY CREATED.  THIS IS NORMAL      *)
(*		IN A MODULAR ENVIRONMENT.                                   *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	SMG$_FATERRLIB = 1212932;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INTERNAL ERROR HAS OCCURED WITHIN SMG.                   *)
(* USER ACTION:	SUBMIT AN SPR DESCRIBING THE PROBLEM.                       *)
(*--                                                                        *)
 
CONST	SMG$_INVARG = 1212940;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE VALUE OF SOME ARGUMENT IS NOT IN THE CORRECT RANGE.     *)
(* USER ACTION:	EXAMINE EACH ARGUMENT. DETERMINE WHICH ONE IS               *)
(*		OUT OF RANGE AND CORRECT YOUR PROGRAM.                      *)
(*--                                                                        *)
 
CONST	SMG$_INVCOL = 1212948;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID COLUMN NUMBER WAS SPECIFIED.  IT WAS EITHER      *)
(*		LESS THAN 0 OR LARGER THAN THE NUMBER OF COLUMNS            *)
(*		IN THE VIRTUAL DISPLAY.                                     *)
(* USER ACTION:	SPECIFY A VALID COLUMN NUMBER.  COLUMNS ARE NUMBERED        *)
(*		BEGINNING WITH 1.                                           *)
(*--                                                                        *)
 
CONST	SMG$_INVDIS_ID = 1212956;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID DISPLAY-ID WAS SPECIFIED.                        *)
(* USER ACTION:	CHECK THAT YOU ARE PASSING THE DISPLAY ID BY REFERENCE      *)
(*		NOT BY VALUE. ENSURE THAT THE DISPLAY ID WAS                *)
(*		PREVIOUSLY SET UP BY A CALL TO SMG$CREATE_VIRTUAL_DISPLAY.  *)
(*--                                                                        *)
 
CONST	SMG$_INVPAS_ID = 1212964;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID PASTEBOARD ID WAS SPECIFIED.                     *)
(* USER ACTION:	CHECK THAT YOU ARE PASSING THE PASTEBOARD ID BY REFERENCE   *)
(*		NOT BY VALUE. (A REFERENCE TO A 0 IS VALID, SINCE 0         *)
(*		IS A LEGAL PASTEBOARD ID.) ENSURE THAT THE PASTEBOARD       *)
(*		WAS PREVIOUSLY SET UP BY A CALL TO                          *)
(*		SMG$CREATE_PASTEBOARD AND THAT THE PASTEBOARD HAS NOT       *)
(*		BEEN DELETED.                                               *)
(*--                                                                        *)
 
CONST	SMG$_INVROW = 1212972;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID ROW NUMBER WAS SPECIFIED.  IT WAS EITHER         *)
(*		LESS THAN 0 OR LARGER THAN THE NUMBER OF ROWS               *)
(*		IN THE VIRTUAL DISPLAY.                                     *)
(* USER ACTION:	SPECIFY A VALID ROW NUMBER.  ROWS ARE NUMBERED              *)
(*		BEGINNING WITH 1.                                           *)
(*--                                                                        *)
 
CONST	SMG$_TOOMANDIS = 1212980;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INTERNAL LIMIT ON THE NUMBER OF VIRTUAL DISPLAYS         *)
(*		THAT YOUR PROGRAM MAY HAVE HAS BEEN EXCEEDED.               *)
(*		THE LIMIT IS CURRENTLY 255.  THIS INCLUDES ANY              *)
(*		VIRTUAL DISPLAYS THAT SOME OUTER ROUTINE MAY BE USING.      *)
(*		THIS INCLUDES DISPLAYS ON ALL PASTEBOARDS.                  *)
(* USER ACTION:	USE FEWER VIRTUAL DISPLAYS.                                 *)
(*		MAKE SURE THAT YOUR PROGRAM IS NOT IN A LOOP                *)
(*		CREATING VIRTUAL DISPLAYS.  BE SURE THAT YOU DELETE         *)
(*		A VIRTUAL DISPLAY WHEN YOU ARE ALL DONE WITH IT.            *)
(*--                                                                        *)
 
CONST	SMG$_TOOMANPAS = 1212988;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INTERNAL LIMIT ON THE NUMBER OF PASTEBOARDS              *)
(*		THAT YOUR PROGRAM MAY HAVE HAS BEEN EXCEEDED.               *)
(*		THE LIMIT IS CURRENTLY 16.                                  *)
(* USER ACTION:	USE FEWER PASTEBOARDS.                                      *)
(*--                                                                        *)
 
CONST	SMG$_WRONUMARG = 1212996;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN SMG ROUTINE WAS CALLED WITH TOO FEW ARGUMENTS            *)
(*		(LESS THAN THE NUMBER OF REQUIRED ARGUMENTS)                *)
(*		OR TOO MANY ARGUMENTS.                                      *)
(* USER ACTION:	CORRECT THE CODE TO CALL THE ROUTINE WITH                   *)
(*		THE PROPER NUMBER OF ARGUMENTS.                             *)
(*--                                                                        *)
 
CONST	SMG$_INVKBD_ID = 1213004;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID KEYBOARD ID WAS PASSED.                          *)
(* USER ACTION:	CHECK THAT YOU ARE PASSING THE KEYBOARD ID BY REFERENCE     *)
(*		NOT BY VALUE. ENSURE THAT THE KEYBOARD ID WAS               *)
(*		PREVIOUSLY SET UP BY A CALL TO                              *)
(*		SMG$CREATE_VIRTUAL_KEYBOARD.                                *)
(*--                                                                        *)
 
CONST	SMG$_INVKTB_ID = 1213012;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID KEY TABLE ID WAS PASSED.                         *)
(* USER ACTION:	CHECK THAT YOU ARE PASSING THE KEY TABLE ID BY REFERENCE    *)
(*		NOT BY VALUE. ENSURE THAT THE KEY TABLE ID WAS              *)
(*		PREVIOUSLY SET UP BY A CALL TO                              *)
(*		SMG$CREATE_KEY_TABLE.                                       *)
(*--                                                                        *)
 
CONST	SMG$_INVMAXLEN = 1213020;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE MAXIMUM LENGTH SPECIFIED FOR AN INPUT STRING            *)
(*		WAS TOO LONG.                                               *)
(* USER ACTION:	CORRECT YOUR PROGRAM SO THAT IT REQUESTS AN INPUT           *)
(*		STRING OF 512 CHARACTERS OR LESS.                           *)
(*--                                                                        *)
 
CONST	SMG$_FILTOOLON = 1213028;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE LENGTH OF THE SPECIFIED STRING                          *)
(*		WAS TOO LONG.                                               *)
(* USER ACTION:	CORRECT YOUR PROGRAM SO THAT IT REQUESTS A FILE             *)
(*		NAME OF 255 CHARACTERS OR LESS.                             *)
(*--                                                                        *)
 
CONST	SMG$_PROTOOLON = 1213036;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE LENGTH OF A PROMPT STRING EXCEEDED 512 CHARACTERS.      *)
(* USER ACTION:	SHORTEN THE PROMPT TO 512 CHARACTERS OR LESS.               *)
(*--                                                                        *)
 
CONST	SMG$_NOTPASTED = 1213044;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN ATTEMPT WAS MADE TO UNPASTE OR MOVE A DISPLAY            *)
(*		THAT WAS NOT PASTED TO THE SPECIFIED PASTEBOARD.            *)
(* USER ACTION:	CHECK THAT YOU ARE REFERENCING THE CORRECT VIRTUAL          *)
(*		DISPLAY.  VERIFY THAT THE DISPLAY HAS NOT BEEN              *)
(*		UNPASTED OR THAT YOU FORGOT TO PASTE IT.                    *)
(*--                                                                        *)
 
CONST	SMG$_PBDIN_USE = 1213052;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE OPERATION YOU HAVE TRIED TO PERFORM IS NOT              *)
(*		LEGAL WHILE OUTPUT TO THE PASTEBOARD IS BEING BATCHED.      *)
(*		FOR EXAMPLE, YOU CAN'T CHANGE THE PASTEBOARD                *)
(*		CHARACTERISTICS WHILE THE PASTEBOARD IS BATCHED.            *)
(* USER ACTION:	TURN OFF PASTEBOARD BATCHING BEFORE ATTEMPTING THE          *)
(*		OPERATION.  YOU MAY TURN BATCHING ON AGAIN AFTER THE        *)
(*		OPERATION.                                                  *)
(*--                                                                        *)
 
CONST	SMG$_INVWIDARG = 1213060;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN ATTEMPT WAS MADE TO CHANGE THE CHARACTERISTICS           *)
(*		OF THE TERMINAL, SETTING IT'S WIDTH TO 0.                   *)
(* USER ACTION:	CORRECT THE CODE. A WIDTH OF 0 IS ILLEGAL.                  *)
(*--                                                                        *)
 
CONST	SMG$_INVPAGARG = 1213068;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN ATTEMPT WAS MADE TO CHANGE THE CHARACTERISTICS           *)
(*		OF THE TERMINAL, SETTING IT'S HEIGHT (PAGE SIZE) TO 0.      *)
(* USER ACTION:	CORRECT THE CODE. A HEIGHT OF 0 IS ILLEGAL.                 *)
(*--                                                                        *)
 
CONST	SMG$_INVCOLARG = 1213076;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN ATTEMPT WAS MADE TO CHANGE THE CHARACTERISTICS           *)
(*		OF THE TERMINAL, CHANGING ITS BACKGROUND COLOR              *)
(*		TO A VALUE THAT WAS NOT LEGAL.                              *)
(* USER ACTION:	CHECK THE SYMBOLIC NAME FOR THE BACKGROUND COLOR            *)
(*		DESIRED.  SPECIFY IT CORRECTLY.                             *)
(*--                                                                        *)
 
CONST	SMG$_DIALINNOT = 1213084;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE VERTICES SPECIFIED WILL RESULT IN A DIAGONAL LINE,      *)
(*		RATHER THAN A VERTICAL OR HORIZONTAL LINE.                  *)
(* USER ACTION:	RE-SPECIFY THE VERTICES SO THAT THE LINE DRAWN WILL BE      *)
(*		VERTICAL OR HORIZONTAL.                                     *)
(*--                                                                        *)
 
CONST	SMG$_TABID_MIS = 1213092;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE TERMTABLE INTERNAL CONSISTENCY CHECK FAILED.  THE       *)
(*		ROUTINES WHICH FETCH INFORMATION FROM TERMTABLE HAVE        *)
(*		A DIFFERENT VERSION NUMBER THAN THE TERMTABLE DATA.         *)
(* USER ACTION:	PROBABLY THE ROUTINES WHICH FETCH THE DATA ARE NEWER        *)
(*		THAN YOUR TERMTABLE.EXE.  RE-COMPILE TERMTABLE.TXT TO       *)
(*		UPDATE YOUR TERMTABLE.EXE.                                  *)
(*--                                                                        *)
 
CONST	SMG$_UNDTERNAM = 1213100;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED TERMINAL DEFINITION WAS NOT FOUND IN          *)
(*		THE CALLER'S PRIVATE COPY OF TERMTABLE OR THE SYSTEM        *)
(*		COPY OF TERMTABLE.                                          *)
(* USER ACTION:	IF YOU INTENDED TO USE A PRIVATE TERMINAL DEFINITION,       *)
(*		MAKE SURE THAT TERMTABLE.EXE EXISTS IN YOUR PRIVATE         *)
(*		DIRECTORY AND THAT THE LOGICAL NAME TERM$TABLOC POINTS      *)
(*		TO IT.  IF YOU EXPECTED YOUR TERMINAL TO BE DEFINED IN      *)
(*		THE SYSTEM TERMTABLE, CHECK WITH YOUR SYSTEM MANAGER.       *)
(*--                                                                        *)
 
CONST	SMG$_INVTERTAB = 1213108;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN INVALID TERMINAL TABLE ADDRESS WAS SPECIFIED.            *)
(* USER ACTION:	CHECK THAT YOU ARE PASSING THE TERMINAL TABLE ARGUMENT      *)
(*		BY REFERENCE AND NOT BY VALUE.  MAKE SURE THE TERMINAL      *)
(*		TABLE ADDRESS WAS ACQUIRED BY CALLING                       *)
(*		SMG$INIT_TERM_TABLE.                                        *)
(*--                                                                        *)
 
CONST	SMG$_INVREQCOD = 1213116;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	DATA IS REQUESTED FROM TERMTABLE VIA REQUEST CODES          *)
(*		WHICH ARE AVAILABLE AS SYMBOLIC NAMES.	THE CODE YOU        *)
(*		HAVE SPECIFIED IS INVALID.                                  *)
(* USER ACTION:	CHECK THE SPELLING OF THE SYMBOLIC NAME YOU ARE USING.      *)
(*--                                                                        *)
 
CONST	SMG$_NOTBOOCAP = 1213124;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER EXPECTED A BOOLEAN CAPABILITY        *)
(*		NAME BUT ENCOUNTERED SOMETHING ELSE.                        *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITION.  PERHAPS YOU HAVE           *)
(*		MISSPELLED THE BOOLEAN CAPABILITY NAME OR OMITTED           *)
(*		ANOTHER STATEMENT SUCH AS STRING OR NUMERIC WHICH           *)
(*		WOULD HAVE TERMINATED BOOLEAN CAPABILITIES.                 *)
(*--                                                                        *)
 
CONST	SMG$_SYNERR = 1213132;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER ENCOUNTERED AN INVALID               *)
(*		SYNTAX.  FOR INSTANCE, THERE MAY BE A MISPLACED OR          *)
(*		MISSING EQUALS SIGN.                                        *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITION.                             *)
(*--                                                                        *)
 
CONST	SMG$_UNDTERTYP = 1213140;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE DEFINITION FOR THE REQUESTED TERMINAL TYPE WAS          *)
(*		NOT FOUND IN THE CALLER'S PRIVATE COPY OF TERMTABLE         *)
(*		OR THE SYSTEM TERMTABLE.                                    *)
(* USER ACTION:	IF YOU INTENDED TO USE A PRIVATE DEFINITION, MAKE           *)
(*		SURE THAT TERMTABLE.EXE EXISTS IN YOUR PRIVATE DIRECTORY    *)
(*		AND THAT THE LOGICAL NAME TERM$TABLOC POINTS TO IT.         *)
(*		IF YOU EXPECTED YOUR TERMINAL DEFINITION TO EXIST IN        *)
(*		THE SYSTEM TERMTABLE, CONSULT YOUR SYSTEM MANAGER.          *)
(*--                                                                        *)
 
CONST	SMG$_MISTERNAM = 1213148;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	CAPABILITIES WERE NOT PRECEEDED BY THE NAME OF A            *)
(*		TERMINAL.                                                   *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITIONS.  MAKE SURE THAT            *)
(*		EACH DEFINITION STARTS WITH NAME = <terminal_name>.         *)
(*--                                                                        *)
 
CONST	SMG$_EXPTOOCOM = 1213156;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE SPECIFIED EXPRESSION WAS                                *)
(*		TOO COMPLEX FOR THE TERMTABLE COMPILER TO PARSE.            *)
(* USER ACTION:	CORRECT YOUR CAPABILITY SO THAT IT IS LESS COMPLEX.         *)
(*--                                                                        *)
 
CONST	SMG$_ILLVAL = 1213164;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	A BOOLEAN CAPABILITY WAS GIVEN A VALUE OTHER THAN           *)
(*		0 OR 1.                                                     *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITIONS.  BOOLEAN CAPABILITIES      *)
(*		MUST BE 0 OR 1 (OFF OR ON, RESPECTIVELY).                   *)
(*--                                                                        *)
 
CONST	SMG$_INVEXP = 1213172;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER ENCOUNTERED AN INVALID               *)
(*		ARITHMETIC EXPRESSION.                                      *)
(* USER ACTION:	CHECK YOUR DEFINITION.  MAKE SURE THAT YOU HAVE             *)
(*		USED ONLY VALID OPERATORS AND OPERANDS IN YOUR              *)
(*		EXPRESSIONS.                                                *)
(*--                                                                        *)
 
CONST	SMG$_ERRAT_LIN = 1213180;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	GENERAL INFORMATION MESSAGE INCLUDED WITH MANY              *)
(*		TERMTABLE COMPILER MESSAGES.  ATTEMPTS TO IDENTIFY          *)
(*		SPECIFICALLY WHERE AN ERROR OCCURRED.                       *)
(* USER ACTION:	CHECK THE SPECIFIED LINE IN YOUR DEFINITION.                *)
(*--                                                                        *)
 
CONST	SMG$_ERRLIN = 1213188;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	GENERAL INFORMATION MESSAGE INCLUDED WITH MANY              *)
(*		TERMTABLE COMPILER MESSAGES.  ATTEMPTS TO IDENTIFY          *)
(*		SPECIFICALLY WHICH LINE CONTAINED AN ERROR.                 *)
(* USER ACTION:	CHECK THE SPECIFIED LINE IN YOUR DEFINITION.                *)
(*--                                                                        *)
 
CONST	SMG$_UNDTERNOP = 1213196;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED TERMINAL DEFINITION WAS NOT FOUND IN THE      *)
(*		CALLER'S PRIVATE COPY OF TERMTABLE OR THE SYSTEM            *)
(*		TERMTABLE.  THE CALLER'S PRIVATE TERMTABLE COULDN'T BE      *)
(*		BE ACCESSED, EITHER BECAUSE IT DIDN'T EXIST OR BECAUSE      *)
(*		RMS COULD NOT OPEN THE FILE.                                *)
(* USER ACTION:	IF YOU INTENDED TO USE A DEFINITION FROM YOUR PRIVATE       *)
(*		TERMTABLE, MAKE SURE THAT TERMTABLE.EXE EXISTS IN YOUR      *)
(*		PRIVATE DIRECTORY AND THAT THE LOGICAL NAME TERM$TABLOC     *)
(*		POINTS TO IT.  MAKE SURE THAT YOU HAVE READ ACCESS TO       *)
(*		THE FILE.                                                   *)
(*--                                                                        *)
 
CONST	SMG$_UNDTERNOS = 1213204;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED TERMINAL DEFINITION WAS NOT FOUND IN          *)
(*		THE CALLER'S PRIVATE TERMTABLE OR THE SYSTEM TERMTABLE.     *)
(*		THE SYSTEM TERMTABLE WAS UNAVAILABLE.                       *)
(* USER ACTION:	THE SYSTEM TERMTABLE SHOULD BE MAPPED AS A GLOBAL           *)
(*		SECTION AND THEREFORE SHOULD ALWAYS BE AVAILABLE.           *)
(*		CONSULT YOUR SYSTEM MANAGER REGARDING THIS.                 *)
(*--                                                                        *)
 
CONST	SMG$_MISFILSPE = 1213212;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE REQUIRE STATEMENT NEEDS A FILE NAME IN ORDER TO         *)
(*		LOCATE EXTERNAL DEFINITIONS WHICH SHOULD BECOME PART        *)
(*		OF YOUR TERMTABLE.                                          *)
(* USER ACTION:	CHECK YOUR TERMTABLE.TXT.  ADD A FILE NAME TO THE           *)
(*		REQUIRE STATEMENT.                                          *)
(*--                                                                        *)
 
CONST	SMG$_UNRECSTA = 1213220;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER ENCOUNTERED A STATEMENT THAT         *)
(*		IT DOES NOT RECOGNIZE.                                      *)
(* USER ACTION:	CHECK YOUR DEFINITION.  MAKE SURE YOU HAVE SPELLED          *)
(*		KEYWORDS CORRECTLY AND THAT STATEMENTS APPEAR ONLY          *)
(*		IN VALID SEQUENCES.                                         *)
(*--                                                                        *)
 
CONST	SMG$_FAIOPEFIL = 1213228;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER COULD NOT OPEN THE FILE              *)
(*		SPECIFIED IN A REQUIRE.                                     *)
(* USER ACTION:	MAKE SURE THAT THE SPECIFIED FILE EXISTS AND THAT           *)
(*		YOU HAVE READ ACCESS TO IT.                                 *)
(*--                                                                        *)
 
CONST	SMG$_NO_ARGS = 1213236;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	YOU REQUESTED A CAPABILITY THAT REQUIRES USER               *)
(*		ARGUMENTS (SUCH AS A ROW AND COLUMN NUMBER) BUT             *)
(*		DID NOT PROVIDE ANY.                                        *)
(* USER ACTION:	CHECK THE NUMBER OF ARGUMENTS REQUIRED BY THIS              *)
(*		CAPABILITY AND CORRECT YOUR CALL TO SMG$GET_TERM_DATA.      *)
(*--                                                                        *)
 
CONST	SMG$_NOTNUMCAP = 1213244;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER EXPECTED A NUMERIC CAPABILITY        *)
(*		NAME BUT ENCOUNTERED SOMETHING ELSE.                        *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITION.  PERHAPS YOU HAVE           *)
(*		MISSPELLED THE NUMERIC CAPABILITY NAME OR OMITTED           *)
(*		ANOTHER STATEMENT SUCH AS STRING OR BOOLEAN WHICH           *)
(*		WOULD HAVE TERMINATED NUMERIC CAPABILITIES.                 *)
(*--                                                                        *)
 
CONST	SMG$_NOTSTRCAP = 1213252;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER EXPECTED A STRING CAPABILITY         *)
(*		NAME BUT ENCOUNTERED SOMETHING ELSE.                        *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITION.  PERHAPS YOU HAVE           *)
(*		MISSPELLED THE STRING CAPABILITY NAME OR OMITTED            *)
(*		ANOTHER STATEMENT SUCH AS BOOLEAN OR NUMERIC WHICH          *)
(*		WOULD HAVE TERMINATED STRING CAPABILITIES.                  *)
(*--                                                                        *)
 
CONST	SMG$_MISENDSTA = 1213260;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER EXPECTED AN END STATEMENT            *)
(*		TO TERMINATE THE CURRENT TERMINAL DEFINITION AND            *)
(*		THIS END STATEMENT WAS MISSING.                             *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITIONS.                            *)
(*--                                                                        *)
 
CONST	SMG$_MISNAMREQ = 1213268;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER EXPECTED TERMTABLE.TXT TO            *)
(*		BEGIN WITH A TERMINAL DEFINITION (NAME STATEMENT)           *)
(*		OR A REQUIRE.  SOMETHING ELSE WAS ENCOUNTERED.              *)
(* USER ACTION:	CHECK YOUR TERMTABLE.TXT.                                   *)
(*--                                                                        *)
 
CONST	SMG$_ILLBATFNC = 1213276;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN ATTEMPT WAS MADE TO PERFORM AN OPERATION THAT            *)
(*		IS NOT PERMITTED WHILE A VIRTUAL DISPLAY IS BEING           *)
(*		BATCHED (FOR EXAMPLE, PERFORMING INPUT OR MOVING            *)
(*		THE VIRTUAL DISPLAY).                                       *)
(* USER ACTION:	TERMINATE BATCHING FIRST BY CALLING                         *)
(*		SMG$END_DISPLAY_UPDATE.                                     *)
(*--                                                                        *)
 
CONST	SMG$_INVDIR = 1213284;
 
(*++                                                                        *)
(* FACILITY:	SMG (TERMTABLE COMPILER)                                    *)
(* EXPLANATION:	THE TERMTABLE COMPILER ACCEPTS ONLY A FEW FAO               *)
(*		DIRECTIVES.  IT ENCOUNTERED ONE THAT IT DOES NOT            *)
(*		HANDLE.                                                     *)
(* USER ACTION:	CHECK YOUR TERMINAL DEFINITION.  MAKE SURE THAT             *)
(* 		YOU USE ONLY VALID FAO DIRECTIVES.                          *)
(*--                                                                        *)
 
CONST	SMG$_TRMNOTANS = 1213292;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A CALL TO SMG$GET_PHYSICAL_CURSOR WAS MADE                  *)
(*		SPECIFYING THAT THE TERMINAL IS TO BE QUERIED               *)
(*		TO DETERMINE WHERE THE CURSOR IS.                           *)
(*		THIS COULD NOT BE DONE BECAUSE THE TERMINAL                 *)
(*		WAS NOT ANSI, AND THIS FEATURE IS AVAILABLE                 *)
(*		ONLY ON ANSI TERMINALS.                                     *)
(* USER ACTION:	RERUN YOUR APPLICATION ON AN ANSI TERMINAL,                 *)
(*		SUCH AS A VT100.                                            *)
(*--                                                                        *)
 
CONST	SMG$_LENNOTEQL = 1213300;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A CALL TO SMG$READ_VERIFY WAS MADE                          *)
(*		SPECIFYING A PICTURE STRING AND AN INITIAL                  *)
(*		STRING OF UNEQUAL LENGTH.  SINCE THE INITIAL                *)
(*		STRING IS USED AS THE DEFAULT VALUE FOR THE                 *)
(*		INPUT, IT MUST MATCH THE PICTURE STRING IN LENGTH.          *)
(* USER ACTION:	MODIFY THE INITIAL STRING TO BE OF THE SAME LENGTH          *)
(*		AS THE PICTURE STRING.                                      *)
(*--                                                                        *)
 
CONST	SMG$_LENMUSONE = 1213308;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A CALL TO SMG$READ_VERIFY WAS MADE                          *)
(*		SPECIFYING A FILL STRING OR A CLEAR                         *)
(*		STRING OF MORE THAN ONE CHARACTER.  SINCE FILL CHARACTER    *)
(*		MATCHING IS DONE ONE CHARACTER AT A TIME, ONLY ONE          *)
(*		CHARACTER MAY BE SPECIFIED FOR THE FILL AND CLEAR STRING.   *)
(* USER ACTION:	MODIFY THE FILL AND/OR CLEAR STRING TO BE ONE CHARACTER     *)
(*		LONG.                                                       *)
(*--                                                                        *)
 
CONST	SMG$_DISREQ = 1213316;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A CALL TO SMG$READ_VERIFY WAS MADE                          *)
(*		SPECIFYING RIGHT-JUSTIFICATION, NO DISPLAY_ID WAS           *)
(*		SPECIFIED AND THE SCROLL_REVERSE SEQUENCE WAS NOT           *)
(*		FOUND FOR THIS TERMINAL IN TERMTABLE.EXE.  SINCE            *)
(*		RIGHT-JUSTIFICATION REQUIRES A CURSOR POSITIONING           *)
(*		SEQUENCE TO WORK PROPERLY, EITHER THE DISPLAY_ID            *)
(*		PARAMETER MUST BE SPECIFIED OR THE SCROLL_REVERSE           *)
(*		SEQUENCE MUST BE ADDED TO TERMTABLE.EXE.                    *)
(* USER ACTION:	ADD THE DISPLAY_ID PARAMETER TO THE SMG$READ_VERIFY         *)
(*		CALL OR ADD THE SCROLL_REVERSE SEQUENCE TO                  *)
(*		TERMTABLE.EXE.                                              *)
(*--                                                                        *)
 
CONST	SMG$_NOTIMP = 1213324;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	YOU TRIED TO USE A ROUTINE THAT IS NOT IMPLEMENTED          *)
(*		IN THIS VERSION OF SMG$.                                    *)
(* USER ACTION:	GET THE APPROPRIATE VERSION OF SMG$.                        *)
(*--                                                                        *)
 
CONST	SMG$_NO_WINASSOC = 1213332;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A VIEWPORT HAS NOT BEEN ASSOCIATED WITH THE VIRTUAL         *)
(*		DISPLAY.                                                    *)
(* USER ACTION:	CREATE A VIEWPORT ON THE VIRTUAL DISPLAY.                   *)
(*--                                                                        *)
 
CONST	SMG$_WINEXISTS = 1213340;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A VIEWPORT EXISTS. THAT IS, A VIEWPORT HAS ALREADY BEEN     *)
(*		ASSOCIATED WITH THE VIRTUAL DISPLAY.                        *)
(* USER ACTION:	1. USE THE VIEWPORT THAT ALREADY ASSOCIATED WITH THE        *)
(*		VIRTUAL DISPLAY. 2. DELETE THE EXISTING VIEWPORT BEFORE     *)
(*		YOU CREATE THE NEW VIEWPORT. OR, 3. USE OTHER VIEWPORT      *)
(*		ROUTINES ON CHANGE/MOVE THE EXISTING VIEWPORT.              *)
(*--                                                                        *)
 
CONST	SMG$_NOSUBEXI = 1213348;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	A SUBPROCESS DOES NOT EXIST FOR THE SPECIFIED VIRTUAL       *)
(*		DISPLAY.                                                    *)
(* USER ACTION:	CALL SMG$CREATE_SUBPROCESS TO CREATE A SUBPROCESS FOR       *)
(*		THIS VIRTUAL DISPLAY.                                       *)
(*--                                                                        *)
 
CONST	SMG$_INSQUOCRE = 1213356;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE CURRENT BYTLM OR PRCLM QUOTA VALUE IS TOO SMALL         *)
(*		TO CREATE A SUBPROCESS.                                     *)
(* USER ACTION:	INCREASE THE ACCOUNTS BYTLM AND/OR PRCLM VALUE.             *)
(*--                                                                        *)
 
CONST	SMG$_INPTOOLON = 1213364;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE LENGTH OF AN INPUT STRING EXCEEDED 255 CHARACTERS.      *)
(* USER ACTION:	SHORTEN THE STRING TO 255 CHARACTERS OR LESS.               *)
(*--                                                                        *)
 
CONST	SMG$_EOF = 1213442;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	END OF FILE CONDITION WAS DETECTED ON INPUT.                *)
(* USER ACTION:	IF NECESSARY, MODIFY YOUR PROGRAM TO RECOGNIZE THIS         *)
(*		CONDITION AND RESPOND TO IT.                                *)
(*--                                                                        *)
 
CONST	SMG$_KEYDEFPRO = 1213450;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE SPECIFIED KEY IS ALREADY DEFINED, AND SINCE ITS         *)
(*		DEFINITION IS PROTECTED, IT CAN NOT BE CHANGED OR           *)
(*		RE-DEFINED.                                                 *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	SMG$_INVDEFATT = 1213458;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	WHEN A KEY IS DEFINED, ATTRIBUTES SUCH AS NOECHO MAY BE     *)
(*		SPECIFIED  WITH THE DEFINITION.  AN ATTEMPT WAS MADE TO     *)
(*		SPECIFY	AN UNKNOWN ATTRIBUTE.                               *)
(* USER ACTION:	CHECK THE LIST OF VALID ATTRIBUTES AND CORRECT YOUR         *)
(*		PROGRAM.                                                    *)
(*--                                                                        *)
 
CONST	SMG$_INVKEYNAM = 1213466;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE SPECIFIED KEY NAME WAS LESS THAN 1 CHARACTER OR         *)
(*		MORE THAN 31 CHARACTERS.                                    *)
(* USER ACTION:	CHOOSE A KEY NAME WITH 1 TO 31 CHARACTERS.                  *)
(*--                                                                        *)
 
CONST	SMG$_INVSTANAM = 1213474;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE SPECIFIED STATE NAME WAS LESS THAN 1 CHARACTER OR       *)
(*		MORE THAN 31 CHARACTERS.                                    *)
(* USER ACTION:	CHOOSE A STATE NAME WITH 1 TO 31 CHARACTERS.                *)
(*--                                                                        *)
 
CONST	SMG$_LINNOTFND = 1213482;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE INPUT LINE REQUESTED IN A CALL TO                       *)
(*		SMG$RETURN_INPUT_LINE WAS NOT FOUND.                        *)
(* USER ACTION:	IF NECESSARY, MODIFY YOUR PROGRAM TO RECOGNIZE THIS         *)
(*		CONDITION AND RESPOND TO IT.                                *)
(*--                                                                        *)
 
CONST	SMG$_KEYNOTDEF = 1216512;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	AN ATTEMPT WAS MADE TO PERFORM AN OPERATION, SUCH AS        *)
(*		SMG$GET_KEY_DEF, ON A KEY WHICH WAS NOT DEFINED.            *)
(* USER ACTION:	CHECK YOUR PROGRAM TO SEE IF THIS KEY SHOULD HAVE           *)
(*		BEEN DEFINED.                                               *)
(*--                                                                        *)
 
CONST	SMG$_NOMOREKEYS = 1216520;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	SMG$LIST_KEY_DEFS, WHEN CALLED REPEATEDLY, RETURNS ALL      *)
(*		THE KEY DEFINITIONS IN A KEY TABLE.  WHEN THERE ARE NO      *)
(*		MORE DEFINITIONS, THIS ERROR IS RETURNED.                   *)
(* USER ACTION:	MODIFY YOUR PROGRAM TO EXPECT THIS CONDITION, IF NECESSARY. *)
(*--                                                                        *)
 
CONST	SMG$_STRTERESC = 1216528;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:  AN SMG OUTPUT ROUTINE WAS PASSED AN ESCAPE SEQUENCE        *)
(*		EMBEDDED IN THE OUTPUT STRING.  THE OUTPUT STRING WAS       *)
(*		WRITTEN TO THE VIRTUAL DISPLAY BUT THE STRING WAS           *)
(*		TERMINATED AT THE ESCAPE SEQUENCE.                          *)
(* USER ACTION:	CHECK YOUR OUTPUT STRING TO DETERMINE WHAT ESCAPE           *)
(*		SEQUENCE WAS BEING PASSED.  IF YOU NEED THE ACTION          *)
(*		PROVIDED BY THIS ESCAPE SEQUENCE, THERE IS MOST             *)
(*		LIKELY A SMG ROUTINE THAT PROVIDES THAT FUNCTIONALITY.      *)
(*--                                                                        *)
 
CONST	SMG$_GBLNOTCRE = 1216536;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE SYSTEM PROGRAM THAT MAPS TERMTABLE.EXE AS A GLOBAL      *)
(*		SECTION WAS UNSUCCESSFUL.                                   *)
(* USER ACTION:	THIS MESSAGE SHOULD BE HANDLED ONLY BY SYSTEM MANAGERS.     *)
(*		IF THERE IS ALREADY AN EXISTING GLOBAL SECTION FOR          *)
(*		TERMTABLE, THE SYSTEM MUST BE REBOOTED IN ORDER TO CREATE   *)
(*		A NEW GLOBAL SECTION.                                       *)
(*--                                                                        *)
(* THE FOLLOWING IS USED WITH GBLNOTCRE                                     *)
 
CONST	SMG$_DELEXIGBL = 1216544;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE SYSTEM PROGRAM WHICH MAPS TERMTABLE AS A GLOBAL         *)
(*		SECTION WAS UNSUCCESSFUL.  BEFORE A NEW GLOBAL SECTION      *)
(*		CAN BE CREATED, THE PREVIOUS ONE MUST BE DELETED.           *)
(* USER ACTION:	THE SYSTEM MUST BE REBOOTED IN ORDER TO MAP A NEW           *)
(*		TERMTABLE.EXE AS THE GLOBAL SECTION.                        *)
(*--                                                                        *)
 
CONST	SMG$_GETDATSTR = 1216552;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THIS MESSAGE IS SIGNALED INTERNALLY TO ALLOW THE HELP       *)
(*		OUTPUT ROUTINE THAT WAS CALLED BY LBR$OUTPUT_HELP TO        *)
(*		ACCESS THE DISPLAY_ID AND KEYBOARD_ID FOR THE SPECIFIED     *)
(*		DISPLAY.  THIS MESSAGE SHOULD NEVER BE RETURNED AS A        *)
(*		STATUS FROM ANY SMG ROUTINE AND MUST NOT BE DOCUMENTED.     *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	SMG$_OPNOTSUP = 1216560;
 
(*++                                                                        *)
(* FACILITY:	SMG                                                         *)
(* EXPLANATION:	THE REQUESTED OPERATION CANNOT BE PERFORMED SINCE           *)
(*		THE TERMINAL HARDWARE DOES NOT SUPPORT THIS FEATURE.        *)
(* USER ACTION:	RUN THE APPLICATION ON A TERMINAL THAT SUPPORTS THIS        *)
(*		FEATURE.                                                    *)
(*--                                                                        *)
 
