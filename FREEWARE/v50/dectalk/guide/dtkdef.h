/*
 *				d t k d e f . h
 *
 *  "C" header file for the DTK$ facility Run-Time Library constants.
 */

/*
 *  Touch Tone Telephone Keypad values.
 *  NOTE:  There is a problem with the DTKDEF module in STARLET.OLB. 
 *         When calling DTK$READ_KEYSTROKE, the returns for phone keypad
 *         numbers 0-9 do not match the definitions in DTKDEF.  The definitions
 *         in this file represent the "actual" values returned, not the values
 *         specified in DTKDEF.
 */
#define DTK$K_TRM_ZERO 			48	/* Touch Tone key 0	*/
#define DTK$K_TRM_ONE 			49	/* Touch Tone key 1	*/
#define DTK$K_TRM_TWO 			50	/* Touch Tone key 2	*/
#define DTK$K_TRM_THREE 		51	/* Touch Tone key 3	*/
#define DTK$K_TRM_FOUR 			52	/* Touch Tone key 4	*/
#define DTK$K_TRM_FIVE 			53	/* Touch Tone key 5	*/
#define DTK$K_TRM_SIX 			54	/* Touch Tone key 6	*/
#define DTK$K_TRM_SEVEN 		55	/* Touch Tone key 7	*/
#define DTK$K_TRM_EIGHT 		56	/* Touch Tone key 8	*/
#define DTK$K_TRM_NINE 			57	/* Touch Tone key 9	*/
#define DTK$K_TRM_A 			65	/* Touch Tone key A	*/
#define DTK$K_TRM_B 			66	/* Touch Tone key B	*/
#define DTK$K_TRM_C 			67	/* Touch Tone key C	*/
#define DTK$K_TRM_D 			68	/* Touch Tone key D	*/
#define DTK$K_TRM_NUMBER_SIGN 		35	/* Touch Tone key #	*/
#define DTK$K_TRM_ASTERISK 		42	/* Touch Tone key *	*/
/*
 *  Conditions 
 */
#define DTK$K_TRM_CANCELLED 		508	/* I/O cancelled	*/
#define DTK$K_TRM_TIMEOUT 		509	/* Timeout period expire*/
#define DTK$K_TRM_BUFFER_FULL 		510	/* No room in key buffer*/
#define DTK$K_TRM_UNKNOWN 		511	/* Unknown terminator	*/

/*
 * Modes to use when dialing the telephone (Phone dialing constants)
 */
#define DTK$K_DIAL_TONE 		0	/* Use tone dialing	*/
#define DTK$K_DIAL_PULSE 		1	/* Use pulse dialing	*/

/*
 *  Device types of DECtalk
 */
#define DTK$K_DTC_UNKNOWN 		0	/* Unknown device type	*/
#define DTK$K_DTC_01 			1	/* DECtalk I		*/
#define DTK$K_DTC_03 			2	/* DECtalk III		*/

/* 
 *  Modes determining the status of the telephone keypad (keypad modes)
 */
#define DTK$K_KEYPAD_OFF 		0	/* Turns the keypad off	*/
#define DTK$K_KEYPAD_ON 		1	/* Turns the keypad on	*/
#define DTK$K_KEYPAD_AUTO 		2	/* Enables autostop mode*/

/* 
 *  Types of DECtalk voices (for DTK$SET_VOICE command).
 */
#define DTK$K_VOICE_MALE 		0	/* Standard male voice	*/
#define DTK$K_VOICE_FEMALE 		1	/* Standard female voice*/
#define DTK$K_VOICE_CHILD 		2	/* Standard child voice	*/
#define DTK$K_VOICE_DEEP_MALE 		3	/* Deep male voice	*/
#define DTK$K_VOICE_DEEP_FEMALE 	4	/* Deep female voice	*/
#define DTK$K_VOICE_OLDER_MALE 		5	/* Older male voice	*/
#define DTK$K_VOICE_LIGHT_FEMALE 	6	/* Light female voice	*/

/*
 *  Modes to be set using DTK$SET_SPEECH_MODE command (speaking modes).
 */
#define DTK$K_SPEAK 			0	/* Start speaking	*/
#define DTK$K_STOP 			1	/* Stop speech when done*/
#define DTK$K_HALT 			2	/* Stop talk immediately*/

/*
 *  Modes to be set using DTK$SPEAK_FILE, DTK$SPEAK_TEXT, and
 *  DTK$SPEAK_PHONEMIC_TEXT.
 */
#define DTK$K_IMMED 			0	/* Return immediately	*/
#define DTK$K_WAIT 			1	/* Wait til text spoken	*/
#define DTK$K_STATUS 			2	/* Return phone status	*/

/* 
 *  Modes to be set using DTK$SET_MODE command
 */									
#define DTK$M_SQUARE 			1	/* [,] delimit phonemics*/
#define DTK$M_ASCII 			2	/* Use ASKY phonemics	*/
#define DTK$M_MINUS 			4	/* Say '-' as minus	*/
#define DTK$M_EUROPE 			8	/* European digit string*/
#define DTK$M_SPELL 			16	/* Spell all words	*/

/*
 *  Current mode settings returned by DTK$SET_MODE command.
 */
#define DTK$M_NORMAL 			0	/* Powerup mode setting	*/
#define DTK$K_SQUARE 			0	/* Sq. brkt. mode	*/
#define DTK$K_ASCII 			1	/* Using ASKY phonemics	*/
#define DTK$K_MINUS 			2	/* Says '-' as minus	*/
#define DTK$K_EUROPE 			3	/* Europe pronunciation	*/
#define DTK$K_SPELL 			4	/* Spell all words	*/

/*
 *  Modes to be set using DTK$SET_TERMINAL_MODE command (DECtalk I only).
 */	
#define DTK$M_HOST 			1	/* All chars to hostline*/
#define DTK$M_SPEAK_ALL 		2	/* Speak all characters	*/
#define DTK$M_EDITED			4       /* Line edit characters	*/
#define DTK$M_HARD 			8	/* Echo in hardcopy form*/
#define DTK$M_SETUP 			16	/* Speak setup mode text*/
#define DTK$M_FILTER 			32	/* Filter DECtalk seqs.	*/

/*
 *  Current modes settings returned by DTK$SET_TERMINAL_MODE command.
 */
#define DTK$K_HOST 			0	/* All chars to hostline*/
#define DTK$K_SPEAK_ALL 		1	/* Speak all characters	*/
#define DTK$K_EDITED 			2	/* Line edit characters	*/
#define DTK$K_HARD 			3	/* Echo in hardcopy form*/
#define DTK$K_SETUP 			4	/* Speak setup mode text*/
#define DTK$K_FILTER 			5	/* Filter DECtalk seqs.	*/

/*
 *  Modes to be set using DTK$SET_LOGGING_MODE command.
 */
#define DTK$M_TEXT 			1	/* ASCII text logging	*/
#define DTK$M_PHONEME 			2	/* Phonemic text logging*/
#define DTK$M_RAWHOST 			4	/* Log "raw" input text	*/
#define DTK$M_INHOST 			8	/* Log input text	*/
#define DTK$M_OUTHOST 			16	/* Log output text	*/
#define DTK$M_ERROR 			32	/* Log error messages	*/
#define DTK$M_TRACE 			64	/* Log escape sequences	*/
#define DTK$M_DEBUG 			128	/* Reserved for DIGITAL	*/

/*
 * Current logging mode settings of DECtalk
 * returned by the DTK$SET_LOGGING_MODE command.
 */
#define DTK$K_TEXT 			0	/* ASCII text logged	*/
#define DTK$K_PHONEME 			1	/* Phonemic text logged	*/
#define DTK$K_RAWHOST 			2	/* Log "raw" input text	*/
#define DTK$K_INHOST 			3	/* Input text logged	*/
#define DTK$K_OUTHOST 			4	/* Output text logged	*/
#define DTK$K_ERROR 			5	/* Error messages logged*/
#define DTK$K_TRACE 			6	/* Esc sequences logged	*/
#define DTK$K_DEBUG 			7	/* Reserved for DIGITAL	*/

/*
 *  Miscellaneous DTK$ Run-Time Library Constants
 */
#define DTK$S_DTKDEF 			1
#define DTK$R_DTKMODE_BITS 		0
#define DTK$V_SQUARE 			0
#define DTK$V_ASCII 			1
#define DTK$V_MINUS 			2
#define DTK$V_EUROPE 			3
#define DTK$V_SPELL 			4
#define DTK$R_DTKTERM_BITS 		0
#define DTK$V_HOST 			0
#define DTK$V_SPEAK_ALL 		1
#define DTK$V_EDITED 			2
#define DTK$V_HARD 			3
#define DTK$V_SETUP 			4
#define DTK$V_FILTER 			5
#define DTK$R_DTKLOG_BITS 		0
#define DTK$V_TEXT 			0
#define DTK$V_PHONEME 			1
#define DTK$V_RAWHOST 			2
#define DTK$V_INHOST 			3
#define DTK$V_OUTHOST 			4
#define DTK$V_ERROR 			5
#define DTK$V_TRACE 			6
#define DTK$V_DEBUG 			7

/*
 *  DTK$ constants defined elsewhere.
 */
globalvalue     DTK$_INVARG;		/* Invalid argument		*/
globalvalue     DTK$_INVMODE;		/* Invalid mode specified	*/
globalvalue	DTK$_NOROOM;		/* No room in dict. to add word	*/
globalvalue	DTK$_OFFHOOK;		/* Phone offhook-already active */
globalvalue	DTK$_ONHOOK;		/* Telephone is onhook		*/
globalvalue	DTK$_TOOLONG;		/* Dict. definition too long	*/
globalvalue     DTK$_BUSY;		/* Dialed number was busy	*/
globalvalue     DTK$_NOANSWER;		/* No one answered the phone	*/
globalvalue     DTK$_NODIATONE;		/* No dialtone detected		*/
globalvalue	DTK$_VOIALREXI;		/* DECtalk exists for device	*/
globalvalue 	DTK$_WINK;		/* A wink was received		*/
globalvalue	DTK$_WRONUMARG;		/* Wrong number of arguments	*/
