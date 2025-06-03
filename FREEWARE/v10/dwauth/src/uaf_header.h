/*module uaf_header "X-1" */


/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the header file information for
**		all (C) modules used by this utility.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:	26-Feb-1993
**
**  MODIFICATION HISTORY:
**
**--
*/

/* To enable debugging code, uncomment the following line.	*/
/*								*/
/* #define	DEBUGGING	1				*/

/* To enable the opening of the authorization file using the	*/
/* logical SYSUAF, uncomment the following line.  This is	*/
/* uaually used in conjuntion with the INITIAL_OPEN symbol.	*/
/*								*/
/* #define	SYSUAF_LOGICAL	1				*/

/* To enable the initial opening of the authorization file,	*/
/* uncomment the following line.  Initially opening the file	*/
/* follows the convention of the normal AUTHORIZE utility, but	*/
/* is not really consistent with MOTIF file handling.  For this	*/
/* reason, there is the option.					*/
/*								*/
/* #define	INITIAL_OPEN	1				*/

/* There was a change made to the way the context argument is	*/
/* used for Version 6.  Defining this variable will allow this	*/
/* utility to operate on a Version 5 system (and OpenVMS/AXP	*/
/* Version 1.5 and earlier), but at a slight performance cost.	*/
/* This is done by not saving the $xxxUAI context between	*/
/* calls.  This is necessary to allow the authorization file	*/
/* record to be unlocked between calls.  It is also necessary	*/
/* because of the Version 6 added EXEC mode protection to the	*/
/* system authorization file when being accessed with the	*/
/* $GETUAI and $SETUAI system services.				*/
/*								*/

#define	NONSHARED_UAI_CONTEXT	1

/*								*/
/*			SPECIAL NOTE				*/
/*								*/
/* If NONSHARED_UAI_CONTEXT is defined, INITIAL_OPEN and	*/
/* SYSUAF_LOGICAL must also be defined.  This is necessary	*/
/* because, unless the UAI context is shared, the $xxxUAI	*/
/* system services cannot deal with any file other than the	*/
/* normal system authorization file.  (Via the SYSUAF logical	*/
/* or SYS$SYSTEM:SYSUAF.DAT file specification.			*/

#ifdef NONSHARED_UAI_CONTEXT

/* First eliminate any defined values. */

#undef		INITIAL_OPEN
#undef		SYSUAF_LOGICAL

/* Now define the necessary values. */

#define		SYSUAF_LOGICAL	1
#define		INITIAL_OPEN	1

#endif /* NONSHARED_UAI_CONTEXT */

/*
**
**  INCLUDE FILES
**
*/

#include	<SSDEF>
#include	"UAFDEF"
#include	<UAIDEF>

#include	"uafpvtdef"
#include	"uafuidef"
#include	"uaf_messages"

/* Because of the lag between the release of the C compiler, and the associated
** header files, and the VMS operating system two of the UAI$_xxx symbols may not
** be defined.  If they are not defined, define them here.  Note the $xxxUAI
** system services are prepared to deal with these code. */

#ifndef		UAI$_PASSWORD
#define		UAI$_PASSWORD	73
#endif /* UAI$_PASSWORD */

#ifndef		UAI$_PASSWORD2
#define		UAI$_PASSWORD2	74
#endif /* UAI$_PASSWORD2 */

/*
**
**  MACRO DEFINITIONS
**
*/

#define		TRUE(expression)	((_tfstatus = (expression))&1)			/* Generic success test */
#define		FALSE(expression)	(!((_tfstatus = (expression))&1))		/* Generic failure test */

/* Define the macro used to signal a failure status and continue on if
** possible. */

#define		SIGNAL(error_code, expression)							\
		    {										\
		    if (FALSE (_cstatus = expression)) LIB$SIGNAL (error_code, _cstatus);	\
		    }

/* Define the macro used to signal a failure and return to the caller
** of the routine. */

#define		SIGNAL_RETURN(error_code, expression)						\
		    {										\
		    if (FALSE (_cstatus = expression))						\
			{									\
			LIB$SIGNAL (error_code, _cstatus);					\
			return error_code;							\
			}									\
		    }

/* Define the macro used to check the returned status.  If it is a failure
** status, signal the error and return to the caller of the routine. */

#define		CHECK_RETURN(expression)							\
		    {										\
		    if (FALSE (_crstatus = expression)) return _crstatus;			\
		    }

/* Define the macro used to complement a byte stream. */

#define		COMPLEMENT(size, storage)							\
		    {										\
		    for (_cindex = 0; _cindex < size; _cindex++)				\
			storage[_cindex] = ~storage[_cindex];					\
		    }

/* Storage needed by various macros. */

unsigned int		_cstatus;		/* Status storage used by CHECK macro */
unsigned int		_crstatus;		/* Status storage used by CHECK_RETURN macro */
int			_cindex;		/* Index storage for COMPLEMENT macro */
unsigned int		_tfstatus;		/* Status storage use by TRUE and FALSE macros */

/*
**
**  USEFUL CONSTANTS
**
*/

#define	MAX_ACCOUNT		32		/* Size of largest account string - UAF$S_ACCOUNT */
#define	MAX_CLITABLE		32		/* Size of largest CLI table name string - UAF$S_CLITABLES */
#define	MAX_DEFCLI		32		/* Size of largest default CLI name string - UAF$S_DEFCLI */
#define	MAX_DEFDEV		32		/* Size of largest default device string - UAF$S_DEFDEV */
#define	MAX_DEFDIR		64		/* Size of largest default directory string - UAF$S_DEFDIR */
#define	MAX_FAO_SIZE		32		/* Size of largest FAO control string */
#define	MAX_ID_SIZE		32		/* Size of largest identifier name */
#define	MAX_LGICMD		64		/* Size of largest login command file string - UAF$S_LGICMD */
#define	MAX_OWNER		32		/* Size of largest owner string - UAF$S_OWNER */
#define	MAX_TEMP_STRING		512		/* Size of temp string storage */
#define	MAX_UIC			14		/* Size of largest octal UIC */
#define	MAX_UIC_ID		67		/* Size of largest identifier UIC */
#define	MAX_USERNAME		32		/* Size of largest username string - UAF$S_USERNAME */
#define	ACCESS_SIZE		3		/* Size of access bit stream (in bytes) */

#define	FIND_NAME_LESS		1		/* Candidate "less than" target name */
#define	FIND_NAME_EQUAL		2		/* Candidate "equal to" target name */
#define	FIND_NAME_GREATER	3		/* Candidate "greater than" target name */

/* Normal storage quantities (in bytes). */

#define	BYTE			1			/* Byte storage unit */
#define	WORD			2			/* Word storage unit */
#define	LONGWORD		4			/* Longword storage unit */
#define	QUADWORD		8			/* Quadword storage unit */

/*
**
**  STRUCTURE DEFINITIONS
**
*/

/* Define the structure used to build the format table used to display
** information in the authorization entry. */

struct	FMTABL {
    unsigned long	item_l_widget;			/* Widget code */
    union {
	unsigned char	item_b_flags;			/* Miscellaneous item flags */
	struct {
	    unsigned	item_v_flag_cstr : 1;		/*    Counted ASCII string */
	    unsigned	item_v_flag_asc : 1;		/*    Padded ASCII string (zero or space) */
	    unsigned	item_v_flag_val : 1;		/*    Value (to convert) */
	    unsigned	item_v_flag_spc : 1;		/*    Requires special handling */
	    unsigned	item_v_flag_dat : 1;		/*    Date/time quadword */
	    unsigned	item_v_flag_max : 1;		/*    Maximize size */
	    } item_r_flag_bits;
	} item_r_flag_overlay;
    unsigned char	item_b_pad_char;		/* Pad character */
    unsigned short int	item_w_byte_cnt;		/* Byte count for local value */
    unsigned int	item_l_fao_ctrs_len;		/* FAO control */
    unsigned int	item_a_fao_ctrs_ptr;		/*   stringdescriptor */
    char		item_r_fao_ctrs [MAX_FAO_SIZE];	/* FAO control string text */
    };

#define	ITEM_M_FLAG_CSTR	1
#define	ITEM_M_FLAG_ASC		2
#define	ITEM_M_FLAG_VAL		4
#define	ITEM_M_FLAG_SPC		8
#define	ITEM_M_FLAG_DAT		16
#define ITEM_M_FLAG_MAX		32

/* Define the structure used to contain the miscellaneous flags needed by
** the DECwindows Authorization utility. */

struct	UAFLAG {
    unsigned	uaf_v_mod_auth_privs		: 1;	/* Authorized privilege list has been modified */
    unsigned	uaf_v_mod_def_privs		: 1;	/* Default privilege list has been modified */
    unsigned	uaf_v_mod_login_flags		: 1;	/* Login flag list has been modified */
    unsigned	uaf_v_mod_prime_days		: 1;	/* Primary days list has been modified */
    unsigned	uaf_v_mod_pri_local_access	: 1;	/* Primary local access time list has been modified */
    unsigned	uaf_v_mod_pri_batch_access	: 1;	/* Primary batch access time list has been modified */
    unsigned	uaf_v_mod_pri_dialup_access	: 1;	/* Primary dialup access time list has been modified */
    unsigned	uaf_v_mod_pri_remote_access	: 1;	/* Primary remote access time list has been modified */
    unsigned	uaf_v_mod_pri_network_access	: 1;	/* Primary network access time list has been modified */
    unsigned	uaf_v_mod_sec_local_access	: 1;	/* Secondary local access time list has been modified */
    unsigned	uaf_v_mod_sec_batch_access	: 1;	/* Secondary batch access time list has been modified */
    unsigned	uaf_v_mod_sec_dialup_access	: 1;	/* Secondary dialup access time list has been modified */
    unsigned	uaf_v_mod_sec_remote_access	: 1;	/* Secondary remote access time list has been modified */
    unsigned	uaf_v_mod_sec_network_access	: 1;	/* Secondary network access time list has been modified */
    };

/*
**
**  EXTERNAL ROUTINES
**
*/

extern void		LIB$SIGNAL ();

/*
**
**  EXTERNAL REFERENCES
**
*/
