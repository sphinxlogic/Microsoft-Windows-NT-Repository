{************************************************************************
*									*
*  DTKMSGDEF								*
*									*
*  Module DTKMSG (UNsupported AND UNdocumented) from STARLETSD		*
*									*
************************************************************************}


CONST	DTK$_FACILITY = 419;
	DTK$_NORMAL = 27492353;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	NORMAL SUCCESSFUL COMPLETION.                               *)
(* USER ACTION:	NONE                                                        *)
(*--                                                                        *)
 
CONST	DTK$_VOIALREXI = 27492369;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A USER TRIED TO CREATE A NEW VOICE_ID FOR A DECTALK         *)
(*		DEVICE, WHEN ONE ALREADY EXISTS.  THIS IS NORMAL IN         *)
(*		A MODULAR ENVIRONMENT.  THE EXISTING VOICE ID               *)
(*		IS RETURNED.                                                *)
(* USER ACTION:	NONE. MAKE SURE YOU DON'T DELETE A VOICE_ID                 *)
(*		THAT YOU ACTUALLY DIDN'T CREATE.                            *)
(*--                                                                        *)
 
CONST	DTK$_NOT_A_TRM = 27492377;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		THE OUTPUT DEVICE IS NOT A TERMINAL.                        *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
(*+                                                                         *)
(* NOTE::	DO NOT ADD ANY ADDITIONAL SUCCESS MESSAGES HERE.            *)
(*		ADD THEM BELOW AFTER THE REMAINING .SUCCESS MESSAGES.       *)
(*-                                                                         *)
 
CONST	DTK$_BUSY = 27492387;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE ROUTINE DTK$DIAL_PHONE WAS CALLED TO DIAL A PHONE       *)
(*		NUMBER AND THE LINE WAS BUSY.                               *)
(* USER ACTION:	TRY CALLING AGAIN.                                          *)
(*--                                                                        *)
 
CONST	DTK$_NOANSWER = 27492395;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE ROUTINE DTK$DIAL_PHONE WAS CALLED TO DIAL A PHONE       *)
(*		NUMBER AND NO ONE ANSWERED THE CALL.                        *)
(* USER ACTION:	TRY CALLING AGAIN.                                          *)
(*--                                                                        *)
 
CONST	DTK$_NODIATONE = 27492403;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE ROUTINE DTK$DIAL_PHONE WAS CALLED TO DIAL A PHONE       *)
(*		NUMBER AND NO DIAL TONE WAS DETECTED ON THE                 *)
(*		PHONE LINE.                                                 *)
(* USER ACTION:	CORRECT THE TELEPHONE LINE PROBLEM AND CALL AGAIN.          *)
(*--                                                                        *)
 
CONST	DTK$_OFFHOOK = 27492411;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		THE PHONE IS OFFHOOK (A CALL IS ACTIVE).                    *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	DTK$_ONHOOK = 27492419;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		THE PHONE IS ONHOOK (NO CALL IS ACTIVE).                    *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	DTK$_TIMEOUT = 27492427;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		THE OUTPUT DEVICE HAS TIMED OUT.                            *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	DTK$_WINK = 27492435;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A WINK (POSSIBLE TIMEOUT) WAS DETECTED ON THE               *)
(*		PHONE LINE.  THE CALLER MAY HAVE HUNG UP THE PHONE.         *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	DTK$_COMFAIL = 27492443;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A COMMUNICATIONS MALFUNCTION HAS OCCURRED SINCE THE         *)
(*		DECTALK UNIT WAS TURNED ON.                                 *)
(* USER ACTION:	CHECK THE DECTALK HARDWARE.                                 *)
(*--                                                                        *)
 
CONST	DTK$_INPBUFOVR = 27492451;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	AN INPUT BUFFER OVERFLOW MALFUNCTION HAS OCCURRED SINCE     *)
(*		THE DECTALK UNIT WAS TURNED ON.                             *)
(* USER ACTION:	CHECK THE DECTALK HARDWARE.                                 *)
(*--                                                                        *)
 
CONST	DTK$_NVROPRFAI = 27492459;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A NVR OPERATION FAILURE HAS OCCURRED SINCE                  *)
(*		THE DECTALK UNIT WAS TURNED ON.                             *)
(* USER ACTION:	CHECK THE DECTALK HARDWARE.                                 *)
(*--                                                                        *)
 
CONST	DTK$_ERRPHOTRA = 27492467;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A PHONEMIC TRANSCRIPTION MALFUNCTION HAS OCCURRED SINCE     *)
(*		THE DECTALK UNIT WAS TURNED ON.                             *)
(* USER ACTION:	CHECK THE DECTALK HARDWARE.                                 *)
(*--                                                                        *)
 
CONST	DTK$_CONSEQERR = 27492475;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A DECTALK PRIVATE CONTROL SEQUENCE MALFUNCTION HAS          *)
(*		OCCURRED SINCE THE DECTALK UNIT WAS TURNED ON.              *)
(* USER ACTION:	CHECK THE DECTALK HARDWARE.                                 *)
(*--                                                                        *)
 
CONST	DTK$_DECTSTFAI = 27492483;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A LOCAL SELF-TEST MALFUNCTION HAS OCCURRED SINCE            *)
(*		THE DECTALK UNIT WAS TURNED ON.                             *)
(* USER ACTION:	CHECK THE DECTALK HARDWARE.                                 *)
(*--                                                                        *)
 
CONST	DTK$_FATERRLIB = 27492868;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	AN INTERNAL ERROR HAS OCCURED WITHIN DTK.                   *)
(* USER ACTION:	SUBMIT AN SPR DESCRIBING THE PROBLEM.                       *)
(*--                                                                        *)
 
CONST	DTK$_FILTOOLON = 27492876;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE LENGTH OF THE SPECIFIED STRING                          *)
(*		WAS TOO LONG.                                               *)
(* USER ACTION:	CORRECT YOUR PROGRAM SO THAT IT REQUESTS A FILE             *)
(*		NAME OF 255 CHARACTERS OR LESS.                             *)
(*--                                                                        *)
 
CONST	DTK$_INVARG = 27492884;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE VALUE OF SOME ARGUMENT IS NOT IN THE CORRECT RANGE.     *)
(* USER ACTION:	EXAMINE EACH ARGUMENT. DETERMINE WHICH ONE IS               *)
(*		OUT OF RANGE AND CORRECT YOUR PROGRAM.                      *)
(*--                                                                        *)
 
CONST	DTK$_INVMODE = 27492892;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE VALUE OF THE MODE ARGUMENT IS NOT IN THE                *)
(*		CORRECT RANGE.                                              *)
(* USER ACTION:	EXAMINE THE ARGUMENT. DETERMINE WHAT IS                     *)
(*		OUT OF RANGE AND CORRECT YOUR PROGRAM.                      *)
(*--                                                                        *)
 
CONST	DTK$_INVVOI_ID = 27492900;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	AN INVALID VOICE-ID WAS SPECIFIED.                          *)
(* USER ACTION:	CHECK THAT YOU ARE PASSING THE VOICE ID BY REFERENCE        *)
(*		NOT BY VALUE. ENSURE THAT THE VOICE ID WAS                  *)
(*		PREVIOUSLY SET UP BY A CALL TO DTK$INITIALIZE.              *)
(*--                                                                        *)
 
CONST	DTK$_NOROOM = 27492908;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THERE IS NO ROOM LEFT IN THE DICTIONARY FOR THIS            *)
(*		DEFINITION.                                                 *)
(* USER ACTION:	CORRECT YOUR PROGRAM SO THAT IT REQUESTS FEWER              *)
(*		DICTIONARY ENTRIES OR SO THAT EACH ENTRY IS SMALLER.        *)
(*--                                                                        *)
 
CONST	DTK$_NOTIMP = 27492916;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	YOU TRIED TO USE A FEATURE THAT IS NOT IMPLEMENTED          *)
(*		IN THIS VERSION OF DTK$.                                    *)
(* USER ACTION:	GET THE APPROPRIATE VERSION OF DTK$.                        *)
(*--                                                                        *)
 
CONST	DTK$_PROTOOLON = 27492924;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE LENGTH OF A PROMPT STRING EXCEEDED 512 CHARACTERS.      *)
(* USER ACTION:	SHORTEN THE PROMPT TO 512 CHARACTERS OR LESS.               *)
(*--                                                                        *)
 
CONST	DTK$_STRTERESC = 27492932;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:  AN DTK OUTPUT ROUTINE WAS PASSED AN ESCAPE SEQUENCE        *)
(*		EMBEDDED IN THE OUTPUT STRING.                              *)
(* USER ACTION:	CHECK YOUR OUTPUT STRING TO DETERMINE WHAT ESCAPE           *)
(*		SEQUENCE WAS BEING PASSED.  IF YOU NEED THE ACTION          *)
(*		PROVIDED BY THIS ESCAPE SEQUENCE, THERE IS MOST             *)
(*		LIKELY A DTK ROUTINE THAT PROVIDES THAT FUNCTIONALITY.      *)
(*--                                                                        *)
 
CONST	DTK$_TOOLONG = 27492940;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THE LENGTH OF THE SPECIFIED STRING WAS TOO LONG.            *)
(* USER ACTION:	CORRECT YOUR PROGRAM SO THAT IT REQUESTS A PHONE NUMBER     *)
(*		OR DICTIONARY SUBSTITUTION OF 255 CHARACTERS OR LESS.       *)
(*--                                                                        *)
 
CONST	DTK$_UNKESCSEQ = 27492948;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:  A DTK INPUT ROUTINE WAS PASSED AN ESCAPE SEQUENCE          *)
(*		THAT IT WAS UNABLE TO UNDERSTAND.                           *)
(* USER ACTION:	????                                                        *)
(*--                                                                        *)
 
CONST	DTK$_UNKREPLY = 27492956;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:  A DTK INPUT ROUTINE WAS PASSED AN ESCAPE SEQUENCE          *)
(*		THAT IT COULD NOT PARSE CORRECTLY.                          *)
(* USER ACTION:	????                                                        *)
(*--                                                                        *)
 
CONST	DTK$_WRONUMARG = 27492964;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A DTK ROUTINE WAS CALLED WITH TOO FEW ARGUMENTS             *)
(*		(LESS THAN THE NUMBER OF REQUIRED ARGUMENTS)                *)
(*		OR TOO MANY ARGUMENTS.                                      *)
(* USER ACTION:	CORRECT THE CODE TO CALL THE ROUTINE WITH                   *)
(*		THE PROPER NUMBER OF ARGUMENTS.                             *)
(*--                                                                        *)
 
CONST	DTK$_TLKINUSE = 27492972;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	A DTK ROUTINE WAS CALLED FROM AN AST ROUTINE                *)
(*		THAT INTERRUPTED A DTK ROUTINE WITH THE SAME                *)
(*		VOICE_ID.                                                   *)
(* USER ACTION:	CORRECT THE CODE TO CALL THE ROUTINE WITH                   *)
(*		THE PROPER SYNCHRONIZATION.                                 *)
(*--                                                                        *)
 
CONST	DTK$_NOMALFUN1 = 27494401;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		NO MALFUNCTIONS HAVE OCCURRED SINCE THE DECTALK             *)
(*		UNIT WAS TURNED ON.  THIS IS THE FIRST REPLY.               *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
CONST	DTK$_NOMALFUN2 = 27494409;
 
(*++                                                                        *)
(* FACILITY:	DTK                                                         *)
(* EXPLANATION:	THIS IS AN INFORMATIONAL MESSAGE TELLING YOU THAT           *)
(*		NO MALFUNCTIONS HAVE OCCURRED SINCE THE DECTALK             *)
(*		UNIT WAS TURNED ON.  THIS IS THE SECOND OR LATER REPLY.     *)
(* USER ACTION:	NONE.                                                       *)
(*--                                                                        *)
 
