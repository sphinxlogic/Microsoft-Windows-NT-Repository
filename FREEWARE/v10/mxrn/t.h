/*#define	DEBUG	1						*/
/*
 *				T . H
 *
 * Definitions for T.
 */

#define	EOS		0		/* End of string		*/
#ifndef	vax11c
#define	MAXMEM		100		/* Remember one hundred pages	*/
#else
#define	MAXMEM		1000		/* Big memory */
#endif
#define	ROWS		24		/* Screen size rows (up/down)	*/
#define	COLS		80		/* Screen size columns (across)	*/
#define	HUGE		32767		/* A very large number		*/
#define	MASK		0100000		/* High bit for short int's	*/

#define	rsx_or_vax11c	1		/* Hack to allow RSX code for VMS */
#ifndef	rsx
#ifndef	vax11c
#undef	rsx_or_vax11c
#endif
#endif

#ifdef	vms
/*
 * Special definitions for RMS I/O package
 */
typedef struct rfa_struct {
	unsigned short int	word[3];
} RFAVALUE;
#define	FILETYPE	char
#define	FWILD		rms_fwild
#define	FNEXT		rms_fnext
#define	FGETNAME	rms_getname
#define	FWILDMODE	"r"
#define	kbin		kbgetc
#else
/*
 * Normal I/O
 */
#define	RFAVALUE	long
#define	FILETYPE	FILE
#define	FWILD		fwild
#define	FNEXT		fnext
#define	FGETNAME	fgetname
#define	FWILDMODE	"run"
#endif
/*
 * Define information to seek within records
 */

typedef struct {
	RFAVALUE record_rfa;		/* Returned by ftell()		*/
	short int buff_offset;		/* Offset in buff[] of record	*/
					/*   < 0 if <Form-Feed> start	*/
	short int line_offset;		/* Offset in textline[]		*/
					/*   < 0 if saved character	*/
} RFA;

/*
 * Things for screen handling
 */

#define	IS_VT52		(64+1)
#ifndef vax11c
#define	IS_VT100	(96+1)
#else
#define	IS_VT100_START	(96+1)
#define IS_VT100_END	(101+1)
#endif

#define	HELPROW	3

extern	char		rx_pattern[];	/* Grep pattern stored here	*/

#ifdef	DEBUG
extern int		debug;		/* Set for debugging		*/
#ifdef	vax11c
#define	_tracef		printf
#endif
#endif
