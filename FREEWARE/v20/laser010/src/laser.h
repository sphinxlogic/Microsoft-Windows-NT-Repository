	/* system items */

#define MAXITEMS 60			/* max item number from jobctl */
#define TTY$L_WB_DATA 48		/* GG magic ! (see $TTYDEF) */
#define MAXSERVICESIZE 80		/* GG max length of LAT service name */

#define	CONFIG_RESOURCE_SIZE 32		/* GG max size of resource name */
#define	CONFIG_PASSWORD_SIZE 32		/* GG max pw's size */
#define	CONFIG_INIT_SIZE 255		/* GG max init string size */
#define	CONFIG_DEFAULT_INIT  "TEXAS_INIT_R=md"	/* GG max init string size */
#define	CONFIG_FILE	     "laser$database"	/* GG name of config file */
#define CONFIG_DEFAULT_FILE  "dna=sys$system:.dat" /* GG default fields */
#define	CONFIG_LINESIZE	80		/* GG cfg max line length */
#define CONFIG_SEP	'/'		/* GG item separator on cfg line */

	/* print parameters */
#define IBUFSIZE 10000			/* max input file line size */
			/* time left for printer to respond to questions */
#define MAXBUSY	10			/* 10* 30 sec = 5 minutes */
#define MAXPRODTIME		/* twice this value is the max. time */
				/* spent guessing the printer name/version */

#define MAXLOGLINE	10	/* nb of log lines to print */

	/* bug correcting features */

/* #define QMS1700_BUG			/* ^T won't work with QMS 1700 */
					/* firmware 22*/
typedef	short	int16;
typedef int	int32;
typedef void *	ptr32;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
