
static char rcsid[] = "@(#)$Id: listalias.c,v 5.5 1992/11/15 01:24:34 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.5 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: listalias.c,v $
 * Revision 5.5  1992/11/15  01:24:34  syd
 * The situation is that the .elm/aliases file is missing, but
 * .elm/aliases.dir and .elm/aliases.pag exist (isn't serendipity
 * wonderful?).  The ndbz functions tolerate this and just put a NULL
 * pointer in the db structure for the data file FILE pointer.  However,
 * get_one_alias() in listalias and elm doesn't account for the db_open()
 * succeeding but the dbz_basef field being NULL, so it passes the NULL
 * pointer to fread().  Detect null and return 0
 * From: dwolfe@pffft.sps.mot.com (Dave Wolfe)
 *
 * Revision 5.4  1992/11/02  20:49:19  syd
 * Resolve a linking error in listalias when DEBUG is enabled.
 * From cs.utexas.edu!chinacat!chip Sun Nov  1 22:04:02 1992
 *
 * Revision 5.3  1992/10/30  22:13:43  syd
 * Apply same mapin/mapout changes to listalias as were used by
 * alias in elm
 * From: Syd
 *
 * Revision 5.2  1992/10/11  01:10:31  syd
 * Add missing setlocale and catopen (just forgotten)
 * From: Syd
 *
 * Revision 5.1  1992/10/04  00:46:45  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** Program that lists all the available aliases.  This one uses the pipe 
    command, feeding the stuff to egrep then sort, or just sort.

**/

#include "elmutil.h"
#include "s_newalias.h"
#include "sysdefs.h"
#include "ndbz.h"

#ifdef BSD
  FILE *popen();
#endif

/*
 * These are used by the "dbz" routines.
 */
#ifdef DEBUG
int debug = 0;
FILE *debugfile = stderr;
#endif

main(argc, argv)
int argc;
char *argv[];
{
	FILE *fd_pipe, *datafile;
	DBZ *db;
	struct alias_rec alias_entry;
	int count = 0, len;
	char buffer[VERY_LONG_STRING], fd_hash[SLEN], 
	     fd_data[SLEN], *home;

#ifdef I_LOCALE
	setlocale(LC_ALL, "");
#endif

	elm_msg_cat = catopen("elm2.4", 0);

	if (argc > 2) {
	  fprintf(stderr, catgets(elm_msg_cat, NewaliasSet, NewaliasListUsage,
		"Usage: listalias <optional-regular-expression>\n"));
	  exit(1);
	}

	home = getenv("HOME");

	sprintf(fd_data, "%s/%s", home, ALIAS_DATA);

	if (argc > 1)
	  sprintf(buffer, "egrep \"%s\" | sort", argv[1]);
	else
	  sprintf(buffer, "sort");

	if ((fd_pipe = popen(buffer, "w")) == NULL) {
	  if (argc > 1) 
	    fprintf(stderr, catgets(elm_msg_cat, NewaliasSet, NewaliasNoPipe,
		"cannot open pipe to egrep program for expressions!\n"));
	  fd_pipe = stdout;
	}

	do {
	    if ((db = dbz_open(fd_data, O_RDONLY, 0)) == NULL) {
	        fprintf(stderr, catgets(elm_msg_cat, NewaliasSet,
		NewaliasNoOpenData, "Could not open %s data file!\n"),
	count? catgets(elm_msg_cat, NewaliasSet, NewaliasSystem, "system")
	     : catgets(elm_msg_cat, NewaliasSet, NewaliasUser, "user"));
	      goto next_file;
	    }
	
	    /** Otherwise let us continue... **/

	    datafile = db->dbz_basef;
	    while (get_one_alias(db, &alias_entry)) {
		len = alias_entry.length;
		fread(buffer, len > sizeof(buffer) ? sizeof(buffer) : len, 1, datafile);
		alias_entry.alias += (int) buffer;
		alias_entry.name += (int) buffer;
		alias_entry.address += (int) buffer;
		fprintf(fd_pipe, "%-20.20s %s (%s)\n", alias_entry.alias, alias_entry.address, alias_entry.name);
		for (len -= sizeof(buffer); len > 0; len -= sizeof(buffer))
		    fread(buffer, len > sizeof(buffer) ? sizeof(buffer) : len, 1, datafile);
	    }

next_file: strcpy(fd_data, system_data_file);

	} while (++count < 2);

	pclose(fd_pipe);

	exit(0);
}

/* byte-ordering stuff */
#define	MAPIN(o)	((db->dbz_bytesame) ? (of_t) (o) : bytemap((of_t)(o), db->dbz_conf.bytemap, db->dbz_mybmap))
#define	MAPOUT(o)	((db->dbz_bytesame) ? (of_t) (o) : bytemap((of_t)(o), db->dbz_mybmap, db->dbz_conf.bytemap))

static of_t			/* transformed result */
bytemap(ino, map1, map2)
of_t ino;
int *map1;
int *map2;
{
	union oc {
		of_t o;
		char c[SOF];
	};
	union oc in;
	union oc out;
	register int i;

	in.o = ino;
	for (i = 0; i < SOF; i++)
		out.c[map2[i]] = in.c[map1[i]];
	return(out.o);
}

get_one_alias(db, ar)
DBZ *db;
struct alias_rec *ar;
{
/*
 *	Get an alias (name, address, etc.) from the data file
 */

	FILE *data_file = db->dbz_basef;

	if (data_file == NULL)
	    return(0);	/* no alias file, but hash exists, error condition */

	if (fread((char *) ar, sizeof(struct alias_rec), 1, data_file) <= 0)
	    return(0);

	ar->status = (int) MAPIN(ar->status);
	ar->alias = (char *) MAPIN(ar->alias);
	ar->last_name = (char *) MAPIN(ar->last_name);
	ar->name = (char *) MAPIN(ar->name);
	ar->comment = (char *) MAPIN(ar->comment);
	ar->address = (char *) MAPIN(ar->address);
	ar->type = (int) MAPIN(ar->type);
	ar->length = (long) MAPIN(ar->length);

	return(1);
}
