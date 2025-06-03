/* rolofilz.h */
#ifdef UNIX
#define ROLOBAK ".rolodex~"
#define ROLODATA ".rolodex.dat"
#define ROLOPRINT "roloprint.text"
#define ROLOLOCK ".rolodexdata.lock"
#define ROLOTEMP ".rolotemp"
#define ROLOCOPY ".rolocopy"
#endif

#ifdef VMS
#define ROLOBAK "rolodex.bak"
#define ROLODATA "rolodex.data"
#define ROLOPRINT "roloprint.text"
#define ROLOLOCK "rolodexdata.lock"
#define ROLOTEMP "rolotemp.data"
#define ROLOCOPY "rolocopy.data"
#endif

#ifdef MSDOS
#define ROLOBAK "rolodex.bak"
#define ROLODATA "rolodex.dat"
#define ROLOPRINT "roloprnt.txt"
#define ROLOLOCK "rolodata.lck"
#define ROLOTEMP "rolotemp.dat"
#define ROLOCOPY "rolocopy.dat"
#endif

/* This is now defined in the Makefile (except for VMS) */

/*  #define ROLOLIB "/usr/local/lib/rolo"  */
#ifdef VMS
#define	ROLOLIB "gal_usrdisk:[ignatz.lib.rololib]"
#endif

/*
 * These definitions go with the hlpfiles[] array in IO.C; do *not* change
 * the value any of these definitions, or funny things will happen to your
 * help definitions!  These values are actually array index values to that
 * array, and as such, modification will cause the help routine to point
 * the wrong file...
 *
 * Adding help files consists of both defining the offset here, and entering
 * the filename in hlpfiles[] in IO.C.
 */
#define	ADDHELP			0
#define ADDINFO 		(ADDHELP + 1)
#define	CONFIRMHELP		(ADDINFO + 1)
#define ENTRYMENU		(CONFIRMHELP + 1)
#define	ESCANHELP		(ENTRYMENU + 1)
#define	ESEARCHHELP		(ESCANHELP + 1)
#define	FIELDSEARCHHELP		(ESEARCHHELP + 1)
#define	LOCKINFO		(FIELDSEARCHHELP + 1)
#define	MAINMENU    		(LOCKINFO + 1)
#define	MANYMATCHHELP		(MAINMENU + 1)
#define	MOPTIONHELP		(MANYMATCHHELP + 1)
#define	MOPTIONSHELP		(MOPTIONHELP + 1)
#define	MOREFIELDSHELP		(MOPTIONSHELP + 1)
#define	NEWADDHELP		(MOREFIELDSHELP + 1)
#define	OTHERFORMATHELP		(NEWADDHELP + 1)
#define	PICKENTRYHELP  		(OTHERFORMATHELP + 1)
#define	PICKENTRYMENU		(PICKENTRYHELP + 1)
#define	POPTIONMENU		(PICKENTRYMENU + 1)
#define	POPTIONSHELP		(POPTIONMENU + 1)
#define	SEARCHSTRINGHELP	(POPTIONSHELP + 1)
#define	UPDATEHELP		(SEARCHSTRINGHELP + 1)
#define	UPDATEMENU		(UPDATEHELP + 1)
#define	USERFIELDHELP		(UPDATEMENU + 1)

/* Use this to validate given index values */
#define LAST_HELP ((sizeof(hlpfiles) / sizeof(char *)) - 1)
