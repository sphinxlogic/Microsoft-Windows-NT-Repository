
/*
 *	D e f i n i t i o n s   a n d   G l o b a l s
 *
 * This file contains symbolic definitions of the structures
 * and characters used by DECtalk application programs,
 * including all DECtalk escape sequence parameters.
 *
 * Note: on RSX-11M, your program must first #include <stdio.h>
 */

#ifdef	DOCUMENTATION

title	dectlk.h	DECtalk Library Header File
index			DECtalk library header file

synopsis
	.s.nf
	#include "dectlk.h"
	.s.f
description

	This file is included in the compilation of all
	modules that use the DECtalk applications library.
	It defines common ASCII characters, DECtalk
	escape sequence parameters, library globals,
	and the DECTALK buffer structure.

configuration

	You can edit dectlk.h to enable debugging code
	by defining the DT_DEBUG flag as follows:

	    #define DT_DEBUG 1

	This changes the primary input and output routines
	so that they become capable of logging all characters
	transmitted to and from the DECtalk device.

globals

	The library provides two global flags which are used
	as follows:
	.lm +16
	.s.i-16;dt__abort########This is set non-zero by
	an intercepted CTRL/C trap (if you have called
	dt_trap()).  When set, no I/O will be performed, and
	library subroutines will exit as quickly as possible.
	.s.i-16;dt__debug########This may be set non-zero
	by an applications program to enable debug printouts.
	Note that the library must have been compiled with
	DT_DEBUG defined in order to compile in the necessary
	print calls.
	.s.lm -16
error codes

	The library may return the following error codes.
	These are all less than zero, and consequently
	cannot be part of the ASCII character set:
	.s.lm +16.nj
	.i-16;DT__ERROR########An operating-system error.
	.i-16;DT__TIMEOUT######An input operation did not
	complete in the required (operating-system) time.
	.i-16;IO__ERROR########An error exit code for
	the exit() library routine.  The value is selected
	as appropriate for the particular operating system.
	.s.lm-16.j.tp 6
Routines implemented as macros

	Certain frequently routines may be implemented as
	macros (if macro expansion is supported by the
	particular C compiler).  These are as follows:
	.s.lm +20.nj
	.i-20;dt__iskey(dt)########TRUE if data is currently
	stored in the keypad type-ahead buffer.
	.i-20;dt__isvalid(c)#######TRUE if the character is
	a valid keypad character.  Note: evaluation of
	the argument must not have side-effects.  I.e.,
	you must not write dt_isvalid(*p++).
	.i-20;dt__ptest(dt,r3)#####Phone test, TRUE if the
	current reply is R2_PHONE, R3.
	.i-20;dt__offhook(dt)######Phone test, TRUE if the
	current reply is R2_PHONE, R3_PH_OFFHOOK.
	.i-20;dt__onhook(dt)#######Phone test, TRUE if the
	current reply is R2_PHONE, R3_PH_ONHOOK.
	.i-20;dt__istimeout(dt)####Phone test, TRUE if the
	current reply is R2_PHONE, R3_PH_TIMEOUT.
	.i-20;dt__phone(dt,p3,p4)##Send a phone message.
	.i-20;dt__eol(dt)##########Send "end of line" and
	force output to DECtalk.
	.s.lm -20;.j.tp 6
general definitions

	The following variables are defined:
	.s.lm +16.nj
	.i-16;EOS#############End of string
	.i-16;FALSE###########For TRUE/FALSE testing
	.i-16;TRUE############For TRUE/FALSE testing
	.s.lm-16.j.tp 6	
ascii characters

	The following C0 control characters are defined:
	.s.nf
	.br;##NUL###STX###ETX###BEL####BS####VT###LS1
	.br;##LS0###XON##XOFF###CAN###SUB###ESC###DEL
	.s.f
	The following C1 control characters are defined:
	.s.nf
	.br;##SS2###SS3###DCS#OLDID###CSI####ST###OSC
	.br;###PM###APC##RDEL
	.s.f
	The following DECtalk-specific parameters are
	also defined.
	.s.lm +16.nj
	.i-16;CSI__DA__PRODUCT##The DECtalk product
	identification code.
	.i-16;DCS__F__DECTALK###The DECtalk specific device
	control sequence (DCS) final character.
	.i-16;P1__DECTALK######All DCT01 DCS sequences
	transmit this for their first (P1) parameter.
	.i-16;R1__DECTALK######All DCT01 DCS replies transmit
	this for the first R1 reply parameter.
	.s.lm-16.j
	The P2 and P3 parameters select the specific DECtalk command.
	.s.lm+16.nj
	.i-16;P2__PHOTEXT######Speak phonemic text.
	.i-16;P2__STOP#########Stop speaking.
	.i-16;P2__SYNC#########Synchronize.
	.i-16;P2__SPEAK########Enable/disable speech.
	.i-16;P2__INDEX########Index text.
	.i-16;P2__IX__REPLY#####Index with reply.
	.i-16;P2__IX__QUERY#####Return last spoken index.
	.i-16;P2__DICT#########Load user dictionary.
	.i-16;P2__PHONE########Telephone control.
	.i-16;P2__MODE#########Synthesis mode control.
	.i-16;P2__LOG##########Local terminal log control.
	.i-16;P2__TERMINAL#####Local terminal control.
	.s.lm-16.j
	The telephone control command takes an additional
	parameter to specify the specific telephone action.
	.s.lm+16.nj
	.i-16;P3__PH__STATUS####Return a status report.
	.i-16;P3__PH__ANSWER####Answer on P4 rings.
	.i-16;P3__PH__HANGUP####Hangup the phone.
	.i-16;P3__PH__KEYPAD####Enable keypad data entry.
	.i-16;P3__PH__NOKEYPAD##Disable keypad data entry.
	.i-16;P3__PH__TIMEOUT###Send a timeout report if
	no data entered in P4 seconds if P4 is greater than
	zero; disable timeouts if P4 is zero.
	.i-16;P3__PH__TONE######Dial out using tones.
	.i-16;P3__PH__PULSE#####Dial out using pulses.
	.s.lm-16.j
	Several P2 commands return messages to the host:
	.s.lm+16.nj
	.i-16;R2__IX__REPLY#####Reply to P2__IX__REPLY.
	R3 contains the last index processed.
	.i-16;R2__IX__QUERY#####Reply to P2__IX__QUERY.
	R3 contains the last index processed.
	.i-16;R2__DICT#########Reply to P2__DICT. R3
	contains the dictionary entry status code.
	.i-16;R2__PHONE########Reply to P2__PHONE.
	R3 contains the telephone status.
	.s.lm-16.j
	The following R3 parameters are returned after a
	P2_PHONE command.
	.s.lm+16.nj
	.i-16;R3__PH__ONHOOK####Telephone is hung up (inactive).
	.i-16;R3__PH__OFFHOOK###Telephone is answered (active).
	.i-16;R3__PH__TIMEOUT###No data was entered by the
	telephone user within the required number of seconds.
	.i-16;R3__PH__TOOLONG###A telephone number to dial is
	too long.
	.s.lm-16.j
	The following R3 parameters are returned after a P2_DICT
	command:
	.s.lm+16.nj
	.i-16;R3__DI__LOADED####Dictionary entry was loaded.
	.i-16;R3__DI__NOROOM####The user dictionary is full.
	.i-16;R3__DI__TOOLONG###The dictionary entry is too long.
	.s.lm-16.j
	The following codes are used to control host-requested
	self test (DECTST):
	.s.lm+16.nj
	.i-16;TEST__POWER######Rerun power up test.
	.i-16;TEST__HDATA######Host data link loopback test.
	.i-16;TEST__HCONTROL###Host line control test.
	.i-16;TEST__LDATA######Local line data test.
	.i-16;TEST__SPEAK######Speak a canned message.
	.s.lm-16.j
	The following status codes are returned by the extended
	DSR sequence:
	.s.lm+16.nj
	.i-16;DSR__OK##########No errors detected.
	.i-16;DSR__COMFAIL#####Communication failure.
	.i-16;DSR__INBUFOVER###Input buffer overflow.
	.i-16;DSR__DECNVRFAIL##Last restore from non-volatile
	memory failed.
	.i-16;DSR__PHONEME#####Incorrect phoneme entered.
	.i-16;DSR__PRIVATE#####DECtalk DCS parameter error.
	.i-16;DSR__DECTSTFAIL##Last DECTST self-test failed.
	.s.lm-16.j
	The following flags configure the P2_LOG command:
	.s.lm+16.nj
	.i-16;LOG__TEXT########Log spoken text.
	.i-16;LOG__PHONEME#####Log generated phonemes.
	.i-16;LOG__RAWHOST#####Log all characters received
	from host without change.
	.i-16;LOG__INHOST######Log all characters received
	from host in "visible" format.
	.i-16;LOG__OUTHOST#####Log all output to host in
	visible format.
	.i-16;LOG__ERROR#######Log error messages.
	.i-16;LOG__TRACE#######Log commands in mnemonic form.
	.s.lm-16.j
	The following flags are for the P2_TERMINAL command:
	.s.lm+16.nj
	.i-16;TERM__HOST#######Send text entered from the
	local terminal to the host.
	.i-16;TERM__SPEAK######Speak text entered from the
	local terminal.
	.i-16;TERM__EDITED#####Line-edit text entered from
	the local terminal.
	.i-16;TERM__HARD#######Use hard-copy edit conventions.
	.i-16;TERM__SETUSPEAK##Speak SETUP dialog.
	.s.lm-16.j
	The following flags are for the P2_MODE command:
	.s.lm+16.nj
	.i-16;MODE__SQUARE#####[ ] bracket phonemic text.
	.i-16;MODE__ASKY#######Use single-letter phonemic
	alphabet.
	.i-16;MODE__MINUS######Pronounce '-' as "minus".
	.s.lm-16.j
Escape sequence data buffer

	All information needed to generate and parse
	escape sequences is contained in the SEQUENCE
	data structure. It is configured by the following
	size constants:
	.s.lm+16.nj
	.i-16;SEQ__INTMAX######Maximum number of intermediate
	characters.
	.i-16;SEQ__PARMAX######Maximum number of parameters.
	.s.lm-16.j
	It contains the following components:
	.s.lm+16.nj
	.i-16;short#state#####Processing state or introducer
	character to send.
	.i-16;char#final######Final character in sequence.
	.i-16;char#private####Private introducer character
	or 'X' to indicate an error.
	.i-16;short#param[]###Private parameters (unsigned);
	param[0] contains the number of parameters.
	.i-16;char#inter[]####Intermediate characters;
	inter[0] contains the number of intermediates.
	.s.lm-16.j
DECTALK data buffer definition

	All information needed by the DECtalk applications
	library is contained in the DECTALK data structure
	which is created by dt_open() and freed by dt_close().
	It is configured by the following parameters:
	.s.lm+16.nj
	.i-16;PEND__SIZE#######Maximum number of keypad
	characters that may be typed-ahead.  Additional
	characters are discarded.
	.i-16;IN__BUFLEN#######Size of the operating
	system input buffer.
	.i-16;OUT__BUFLEN######Size of the operating
	system output buffer.
	.s.lm-16.j
	The data buffer contains the following information:
	.s.lm+16.nj
	.i-16;DECTALK#*link###Chains together all active units.
	.i-16;int#unit########Operating system I/O channel.
	.i-16;short#timeout###TRUE if timeouts enabled.
	.i-16;short#pend__fc###Bytes in pending buffer.
	.i-16;short#pend__fp###Index to free byte in pending
	buffer.
	.i-16;short#pend__ep###Index to next byte to return
	from pending buffer.
	.i-16;char#*in__ptr####Input buffer pointer.
	.i-16;char#*in__end####Input buffer end.
	.i-16;char#*out_ptr####Output buffer free pointer.
	.i-16;SEQUENCE#send###Last DCS sequence sent.
	.i-16;SEQUENCE#reply##Last DECtalk reply received.
	.i-16;SEQUENCE#seq####Look-ahead for string terminator
	processing.
	.i-16;char#*device####Remember dt_open() device name
	for debug printouts.
	.i-16;char#pend[]#####Type-ahead buffer.
	.i-16;char#in_buff[]###Input buffer.
	.i-16;char#out_buff[]##Output buffer.
	.i-16;struct#sgtty#stty__save#Terminal characteristics
	block (Unix only).
	.i-16;FILE#*fildes####File descriptor (RSX only).
	.i-16;struct#iosb#iosb#I/O status block (RSX only).
	.i-16;struct#qioparm#parm#QIO parameter block
	(RSX only).
	.s.lm-16.j
#endif

/*
 * Define DT_DEBUG to enable debug printouts of transmitted
 * characters.
 */

#define	DT_DEBUG

#define	FALSE	0
#define	TRUE	1
#define	EOS	'\0'

#ifdef	unix
#include	<sgtty.h>
#endif

/*
 * These error codes may not be in the Ascii range.
 */

#define	DT_ERROR	(-1)
#define	DT_TIMEOUT	(-2)

/*
 * C0 control characters
 */

#define	NUL		0x00	/* NUL code			*/
#define	STX		0x02	/* Start of text		*/
#define	ETX		0x03	/* End of text			*/
#define	BEL		0x07	/* Bell				*/
#define	BS		0x08	/* Backspace			*/
#define	VT		0x0B	/* Vertical tab ('\013')	*/
#define	LS1		0x0E	/* LS1 (SO)			*/
#define	LS0		0x0F	/* LS0 (SI)			*/
#define	XON		0x11	/* DC1				*/
#define	XOFF		0x13	/* DC3				*/
#define	CAN		0x15	/* Cancel			*/
#define	SUB		0x1A	/* Substitute			*/
#define	NUL		0x00	/* Null code			*/
#define	ESC		0x1B	/* Escape			*/
#define	DEL		0x7F	/* Delete			*/

/*
 * C1 control characters
 */

#define	SS2		0x8E	/* Single shift 2		*/
#define	SS3		0x8F	/* Single shift 3		*/
#define	DCS		0x90	/* Device control sequence	*/
#define	OLDID		0x9A	/* ESC Z			*/
#define	CSI		0x9B	/* Control Sequence Introducer	*/
#define	ST		0x9C	/* String terminator		*/
#define	OSC		0x9D	/* Operating System sequence	*/
#define	PM		0x9E	/* Privacy Message		*/
#define	APC		0x9F	/* Application Program Control	*/
#define	RDEL		0xFF	/* Delete in right side		*/

#define	CSI_DA_PRODUCT	19	/* Dectalk DA product code	*/

/*
 * Basic definitions for DECtalk device control
 * strings. All DECtalk sequences have a first parameter of
 * P1_DECTALK. This provides an easy place for future DECtalk
 * products to fit into the scheme of things.
 */
#define	DCS_F_DECTALK	'z'	/* DECtalk final		*/
#define	P1_DECTALK	0	/* DECtalk param 1		*/
#define	R1_DECTALK	0	/* DECtalk reply param 1	*/

/*
 * The second parameter selects the basic command.
 */

#define	P2_PHOTEXT	0	/* Speak phonemic text		*/
#define	P2_STOP		10	/* Stop speaking		*/
#define	P2_SYNC		11	/* Synchronize			*/
#define	P2_SPEAK	12	/* Enable or disable speaking	*/
#define	P2_INDEX	20	/* INDEX			*/
#define	P2_IX_REPLY	21	/* INDEX_REPLY			*/
#define	P2_IX_QUERY	22	/* INDEX_QUERY			*/
#define	P2_DICT		40	/* Dictionary control		*/
#define	P2_PHONE	60	/* Phone control		*/
#define	P2_MODE		80	/* Synthesis mode control	*/
#define	P2_LOG		81	/* LOG information on local tty	*/
#define	P2_TERMINAL	82	/* Local terminal control	*/

/*
 * Additional parameters for the phone command.
 */
#define	P3_PH_STATUS	0	/* Send a status report		*/
#define	P3_PH_ANSWER	10	/* Answer (P4 has ring number)	*/
#define	P3_PH_HANGUP	11	/* Hangup			*/
#define	P3_PH_KEYPAD	20	/* Raw keypad			*/
#define	P3_PH_NOKEYPAD	21	/* Disable keypad		*/
#define	P3_PH_TIMEOUT	30	/* Status report on timeout	*/
#define	P3_PH_TONE	40	/* Dial out			*/
#define	P3_PH_PULSE	41	/* Dial out			*/

/*
 * The second parameter in a reply specifies the general class
 * of the reply sequence.
 */

#define	R2_IX_REPLY	31	/* Sent after INDEX_REPLY	*/
#define	R2_IX_QUERY	32	/* Sent after INDEX_QUERY	*/
#define R2_DICT		50	/* Sent after DICT		*/
#define	R2_PHONE	70	/* Telephone status report	*/

/*
 * Additional reply information is passed in the third parameter.
 */

#define	R3_PH_ONHOOK	0	/* Hung up			*/
#define	R3_PH_OFFHOOK	1	/* Phone is lifted		*/
#define	R3_PH_TIMEOUT	2	/* No reply in N seconds	*/
#define	R3_PH_TOOLONG	3	/* Telephone # text too long	*/
#define R3_DI_LOADED	0	/* Dictionary entry loaded ok	*/
#define R3_DI_NOROOM	1	/* No room in dictionary	*/
#define R3_DI_TOOLONG	2	/* String too long		*/

/*
 * Test specification codes for the request self test
 * (DECTST) sequence.
 */

#define	TEST_POWER	1	/* Rerun power up tests		*/
#define	TEST_HDATA	2	/* Host line data loopback test	*/
#define	TEST_HCONTROL	3	/* Host line control test	*/
#define	TEST_LDATA	4	/* Local line data test		*/
#define	TEST_SPEAK	5	/* Speak a canned message	*/

/*
 * Error (and success) codes for the extended DSR sequence.
 */

#define	DSR_OK		20	/* All OK			*/
#define	DSR_COMMFAIL	22	/* Communication failure	*/
#define	DSR_INBUFOVER	23	/* Input buffer overflow	*/
#define	DSR_DECNVRFAIL	24	/* Last DECNVR failed		*/
#define	DSR_PHONEME	25	/* Error in phonemic text	*/
#define	DSR_PRIVATE	26	/* Error in DECtalk private DCS	*/
#define DSR_DECTSTFAIL	27	/* Last DECTST failed		*/

/*
 * Local logging flags for the P2_LOG command.
 */

#define	LOG_TEXT	0x0001	/* Log text sent to LSTASK	*/
#define	LOG_PHONEME	0x0002	/* Log generated phonemes	*/
#define	LOG_RAWHOST	0x0004	/* Log raw host input		*/
#define	LOG_INHOST	0x0008	/* Log host input		*/
#define	LOG_OUTHOST	0x0010	/* Log host output		*/
#define	LOG_ERROR	0x0020	/* Log errors			*/
#define	LOG_TRACE	0x0040	/* Log sequence trace info.	*/

/*
 * Local terminal flags for the P2_TERMINAL command.
 */

#define	TERM_HOST	0x0001	/* Send text to host		*/
#define	TERM_SPEAK	0x0002	/* Speak local terminal input	*/
#define	TERM_EDITED	0x0004	/* Edited			*/
#define	TERM_HARD	0x0008	/* Local terminal is hardcopy	*/
#define TERM_SETUSPEAK	0x0010	/* Spoken setup mode		*/

/*
 * Mode flags for the P2_MODE command.
 */

#define	MODE_SQUARE	0x0001	/* [ ] are phonemic brackets	*/
#define	MODE_ASKY	0x0002	/* Use ASKY alphabet		*/
#define	MODE_MINUS	0x0004	/* "-" is pronounced "minus"	*/

/*
 * These macros and structure definitions are used by the escape
 * sequence parser.
 */

#define	SEQ_INTMAX	2	/* Max. # of intermediates	*/
#define	SEQ_PARMAX	16	/* Max. # of parameters		*/

/*
 * dt_gesc() (get escape sequence) and dt_pesc() (put escape
 * sequence) use this structure for all processing.
 */

typedef	struct	{
    short	state;		/* Processing state or intro	*/
    char	final;		/* Final character in seq.	*/
    char	private;	/* Private introducer		*/
#ifdef	decus
    unsigned		param[SEQ_PARMAX+1];
#else
    unsigned short	param[SEQ_PARMAX+1];
#endif
				/* Intermediate count, values	*/
    char	inter[SEQ_PARMAX+1];
} SEQUENCE;

/*
 * The DECTALK structure is used to maintain all information
 * needed to process a DECtalk device.  It is allocated by
 * dt_open(), freed by dt_close() and a required parameter
 * by essentially all routines.
 */

#ifndef	PEND_SIZE
#define	PEND_SIZE	32		/* Pending buffer size	*/
#endif
#ifndef	IN_BUFLEN
#define	IN_BUFLEN	32
#endif
#ifndef	OUT_BUFLEN
#define	OUT_BUFLEN	128
#endif
#if	(IN_BUFLEN < 1 || OUT_BUFLEN < 1 || PEND_SIZE < 1)
	<< error, mandatory parameters aren't correct >>
#endif

typedef struct DECtalk {
    struct DECtalk *link;	/* Chain all units together	*/
    int		unit;		/* I/O channel			*/
    short	timeout;	/* TRUE if timeouts enabled	*/
    short	pend_fc;	/* Bytes in pending buffer	*/
    short	pend_fp;	/* Pending buffer fill index	*/
    short	pend_ep;	/* Pending buffer empty index	*/
    char	*in_ptr;	/* I/O input buffer pointer	*/
    char	*in_end;	/* -> end of input buffer	*/
    char	*out_ptr;	/* -> free spot in output buff.	*/
    SEQUENCE	send;		/* Last sequence sent		*/
    SEQUENCE	reply;		/* Last sequence read		*/
    SEQUENCE	seq;		/* Sequence look-ahead		*/
    char	*device;	/* DECtalk hardware device	*/
    char	pend[PEND_SIZE]; /* Type-ahead ring buffer	*/
    char	in_buff[IN_BUFLEN];	/* I/O input buffer	*/
    char	out_buff[OUT_BUFLEN];	/* I/O output buffer	*/
    /*
     * The following entries are operating-system specific.
     */
#ifdef	unix
    struct sgttyb stty_save;	/* Terminal flags		*/
#endif
#ifdef	rsx
    FILE	*fildes;	/* File descriptor		*/
    struct iosb {		/* I/O status block		*/
	char	status;		/* Operation status		*/
	char	terminator;	/* Input terminator byte	*/
	int	count;		/* Bytes read from device	*/
    } iosb;
    struct qioparm {		/* QIO parameter block	*/
        char	*buffer;	/* Buffer location	*/
        int	size;		/* Bytes to transfer	*/
        char	*p3;		/* For ctrl/c ast	*/
        char	*table;		/* Terminator table	*/
        int	unused[2];	/* Not used here	*/
    } parm;
#endif
} DECTALK;
    
/*
 * Certain short routines and common tests are expressed as
 * macros.  In all instances, 'dd' is a DECtalk I/O descriptor
 * as returned by dt_open().  Note that the arguments should
 * not have "side-effects".
 *
 *   dt_iskey(dd)	TRUE if something in type-ahead buffer.
 *   dt_isvalid(c)	TRUE if argument is a valid keypad key.
 *
 * The following are only useful after executing dt_phone().
 *
 *   dt_ptest(dd, r3)	TRUE if specific phone reply.
 *   dt_offhook(dd)	TRUE if last DECtalk reply is OFFHOOK.
 *   dt_onhook(dd)	TRUE if last DECtalk reply is ONHOOK.
 *   dt_istimeout(dd)	TRUE if last DECtalk reply is TIMEOUT.
 *
 * The following simple commands may be written as macros:
 *
 *   dt_phone(dd,p3,p4)	Send a phone message.
 *   dt_get(dd, sec)	Read a character (with timeout)
 *   dt_put(dd, c)	Send a character to DECtalk
 *   dt_eol(dd, c)	Send "end of line", flush output buffers
 *
 * If DT_DEBUG is #defined, dt_get() and dt_put() are functions
 * which may log all characters to the standard output stream.
 */

#ifndef	DT_DEBUG
#define	dt_get		dt_ioget
#define	dt_put		dt_ioput
#endif
#ifndef	nomacarg
#define	dt_iskey(dd)	(dd->pend_fc != 0)
#define	dt_isvalid(c)	((  (c >= '0' && c <= '9')	\
			 || c == '#' || c == '*'	\
			 || (c >= 'A' && c <= 'D')))
#define dt_ptest(dd,r3)	(dt_test(dd, R2_PHONE, r3))
#define	dt_offhook(dd)	(dt_ptest(dd, R3_PH_OFFHOOK))
#define	dt_onhook(dd)	(dt_ptest(dd, R3_PH_ONHOOK))
#define	dt_istimeout(dd) (dt_ptest(dd, R3_PH_TIMEOUT))
#define	dt_phone(dd,p3,p4) (dt_msg(dd, \
		P2_PHONE, p3, p4, R2_PHONE, -1))
#ifdef	unix
#define	dt_eol(dd)	(dt_put(dd, '\n'), dt_put(dd, 0))
#else
#define	dt_eol(dd)	(dt_put(dd, '\r'), \
		dt_put(dd, '\n'), dt_put(dd, 0))
#endif
#endif
#ifdef	decus
#ifdef	DT_DEBUG
/*
 * This forces traceback on Decus C systems.
 */
#define	exit		error
#define	IO_ERROR	"fatal DECtalk I/O error"
#endif
#endif

#ifndef	IO_ERROR
#ifdef	vms
#include		<ssdef.h>
#define	IO_ERROR	SS$_ABORT
#else
#define	IO_ERROR	2
#endif
#endif
/*
 * dt_abort may be set by a user program at any time to
 * stop DECtalk.  Typically, it would be set by dt_trap()
 * when a <CNTL/C> (Unix INTERRUPT signal) is typed by the
 * terminal user.
 */
extern int	dt_abort;		/* Set TRUE to stop	*/
extern DECTALK	*dt_root;		/* Root of device chain	*/
#ifdef	DT_DEBUG
extern int	dt_debug;		/* TRUE if debug log	*/
#endif
