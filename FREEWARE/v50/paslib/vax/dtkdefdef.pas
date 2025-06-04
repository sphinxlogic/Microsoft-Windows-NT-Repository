{************************************************************************
*									*
*  DTKDEFDEF								*
*									*
*  Module DTKDEF (UNsupported AND UNdocumented) from STARLETSD		*
*									*
************************************************************************}


(* Definitions for RTL DECtalk Management                                   *)
 
(*                                                                          *)
(*	Phone keypad keys.                                                  *)
(*                                                                          *)
 
CONST	DTK$K_TRM_ZERO = 48;
	DTK$K_TRM_ONE = 49;
	DTK$K_TRM_TWO = 50;
	DTK$K_TRM_THREE = 51;
	DTK$K_TRM_FOUR = 52;
	DTK$K_TRM_FIVE = 53;
	DTK$K_TRM_SIX = 54;
	DTK$K_TRM_SEVEN = 55;
	DTK$K_TRM_EIGHT = 56;
	DTK$K_TRM_NINE = 57;
	DTK$K_TRM_A = 65;
	DTK$K_TRM_B = 66;
	DTK$K_TRM_C = 67;
	DTK$K_TRM_D = 68;
	DTK$K_TRM_NUMBER_SIGN = 35;
	DTK$K_TRM_ASTERISK = 42;
 
(* Conditions                                                               *)
 
CONST	DTK$K_TRM_CANCELLED = 508;      (* I/O cancelled                    *)
	DTK$K_TRM_TIMEOUT = 509;        (* Timeout period expired           *)
	DTK$K_TRM_BUFFER_FULL = 510;    (* Buffer is full                   *)
	DTK$K_TRM_UNKNOWN = 511;        (* Unknown terminator               *)
 
(*                                                                          *)
(*	Self-test constants.                                                *)
(*                                                                          *)
 
CONST	DTK$K_TEST_POWER = 1;
	DTK$K_TEST_HDATA = 2;
	DTK$K_TEST_HCONTROL = 3;
	DTK$K_TEST_DATA = 4;
	DTK$K_TEST_SPEAK = 5;
 
(*                                                                          *)
(*	Phone dialing constants.                                            *)
(*                                                                          *)
 
CONST	DTK$K_DIAL_TONE = 0;
	DTK$K_DIAL_PULSE = 1;
 
(*                                                                          *)
(*	DECtalk types.                                                      *)
(*                                                                          *)
 
CONST	DTK$K_DTC_UNKNOWN = 0;
	DTK$K_DTC_01 = 1;
	DTK$K_DTC_03 = 2;
 
(*                                                                          *)
(*	Keypad modes.                                                       *)
(*                                                                          *)
 
CONST	DTK$K_KEYPAD_OFF = 0;
	DTK$K_KEYPAD_ON = 1;
	DTK$K_KEYPAD_AUTO = 2;
 
(*                                                                          *)
(*	Voices.                                                             *)
(*                                                                          *)
 
CONST	DTK$K_VOICE_MALE = 0;
	DTK$K_VOICE_FEMALE = 1;
	DTK$K_VOICE_CHILD = 2;
	DTK$K_VOICE_DEEP_MALE = 3;
	DTK$K_VOICE_DEEP_FEMALE = 4;
	DTK$K_VOICE_OLDER_MALE = 5;
	DTK$K_VOICE_LIGHT_FEMALE = 6;
 
(*                                                                          *)
(*	Speaking modes.                                                     *)
(*                                                                          *)
 
CONST	DTK$K_SPEAK = 0;
	DTK$K_STOP = 1;
	DTK$K_HALT = 2;
 
(*                                                                          *)
(*	Return modes.                                                       *)
(*                                                                          *)
 
CONST	DTK$K_IMMED = 0;    (* Return immedately after sending text. *)
	DTK$K_WAIT = 1;     (* Wait til text completely spoken b4 returning *)
	DTK$K_STATUS = 2;   (* Wait til text spoken & return phone status. *)
 
(*                                                                          *)
(*        DEFINE BIT MASKS AND VALUES FOR MODE ATTRIBUTES                   *)
(*                                                                          *)
 
CONST	DTK$M_SQUARE = 1;
	DTK$M_ASCII = 2;
	DTK$M_MINUS = 4;
	DTK$M_EUROPE = 8;
	DTK$M_SPELL = 16;
	DTK$M_NORMAL = 0;               (* no bits set                      *)
	DTK$K_SQUARE = 0;               (* Mode square bitvector index      *)
	DTK$K_ASCII = 1;                (* asky mode                        *)
	DTK$K_MINUS = 2;                (* Say dash as minus bitvector index *)
	DTK$K_EUROPE = 3;               (* European type numbers            *)
	DTK$K_SPELL = 4;                (* Spell all words                  *)
	DTK$M_HOST = 1;
	DTK$M_SPEAK_ALL = 2;
	DTK$M_EDITED = 4;
	DTK$M_HARD = 8;
	DTK$M_SETUP = 16;
	DTK$M_FILTER = 32;
	DTK$K_HOST = 0;           (* Send all chars typed to host.    *)
	DTK$K_SPEAK_ALL = 1;      (* Speak all chars typed.           *)
	DTK$K_EDITED = 2;         (* Line edit all chars typed.       *)
	DTK$K_HARD = 3;           (* Do local echo in hardcopy format. *)
	DTK$K_SETUP = 4;          (* Speak all chars displayed in setup mode. *)
	DTK$K_FILTER = 5;         (* Don't send DECtalk specific sequences. *)
	DTK$M_TEXT = 1;
	DTK$M_PHONEME = 2;
	DTK$M_RAWHOST = 4;
	DTK$M_INHOST = 8;
	DTK$M_OUTHOST = 16;
	DTK$M_ERROR = 32;
	DTK$M_TRACE = 64;
	DTK$M_DEBUG = 128;
	DTK$K_TEXT = 0;                 (* Log ASCII text.                  *)
	DTK$K_PHONEME = 1;              (* Log phonemic text.               *)
	DTK$K_RAWHOST = 2;              (* Log all chars without inspection. *)
	DTK$K_INHOST = 3;               (* Log text read from host.         *)
	DTK$K_OUTHOST = 4;              (* Log text sent to host.           *)
	DTK$K_ERROR = 5;                (* Log error messages.              *)
	DTK$K_TRACE = 6;                (* Log sequences symbolically       *)
	DTK$K_DEBUG = 7;                (* Reserved                         *)
 
TYPE	DTK$TYPE = RECORD CASE INTEGER OF
(*                                                                          *)
(*	DEFINE BITS, MASKS, AND FIELDS FOR THE CONTROL MODES.               *)
(*                                                                          *)
	1: (DTK$R_DTKMODE_BITS : [BYTE(1)] RECORD END;
	    );
	2: (DTK$V_SQUARE : [POS(0)] $BOOL; (* Set mode square on            *)
	    DTK$V_ASCII : [POS(1)] $BOOL; (* asky mode                      *)
	    DTK$V_MINUS : [POS(2)] $BOOL; (* Say dash as minus              *)
	    DTK$V_EUROPE : [POS(3)] $BOOL; (* Europe                        *)
	    DTK$V_SPELL : [POS(4)] $BOOL; (* Spell all words                *)
	    );
	3: (DTK$R_DTKTERM_BITS : [BYTE(1)] RECORD END;
	    );
	4: (DTK$V_HOST : [POS(0)] $BOOL; (* Send all chars typed to host.   *)
	    DTK$V_SPEAK_ALL : [POS(1)] $BOOL; (* Speak all chars typed.     *)
	    DTK$V_EDITED : [POS(2)] $BOOL; (* Line edit all chars typed.    *)
	    DTK$V_HARD : [POS(3)] $BOOL; (* Do local echo in hardcopy format. *)
	    DTK$V_SETUP : [POS(4)] $BOOL; (* Speak all chars in setup mode *)
	    DTK$V_FILTER : [POS(5)] $BOOL; (* Don't send DECtalk specific seqs*)
	    );
	5: (DTK$R_DTKLOG_BITS : [BYTE(1)] RECORD END;
	    );
	6: (DTK$V_TEXT : [POS(0)] $BOOL; (* Log ASCII text.                 *)
	    DTK$V_PHONEME : [POS(1)] $BOOL; (* Log phonemic text.           *)
	    DTK$V_RAWHOST : [POS(2)] $BOOL;(* Log all chars without inspection*)
	    DTK$V_INHOST : [POS(3)] $BOOL; (* Log text read from host.      *)
	    DTK$V_OUTHOST : [POS(4)] $BOOL; (* Log text sent to host.       *)
	    DTK$V_ERROR : [POS(5)] $BOOL; (* Log error messages.            *)
	    DTK$V_TRACE : [POS(6)] $BOOL; (* Log sequences symbolically     *)
	    DTK$V_DEBUG : [POS(7)] $BOOL; (* Reserved                       *)
	    )
	END;
 
