/* rolodefs.h */
#define VERSION "2.0.2"

#ifdef VMS
#define CHUNKSIZE 100000        /* storage size for holding data file */
#endif

#ifdef UNIX
#define CHUNKSIZE ((unsigned)100000) /* storage size for holding data file */
#endif

#ifdef MSDOS
#define CHUNKSIZE ((unsigned)50000)  /* storage size for holding data file */
#endif

#define NOLOCKFLAG 'l'
#define SUMMARYFLAG 's'
#define OTHERUSERFLAG 'u'
#define READONLYFLAG 'r'

#define LEGAL_OPTIONS "lsru"
#ifdef VMS
#define USAGE "rolo [ person1 person2 ...] [ -l -s -r -u use_dir ] "
#endif

#ifdef UNIX
#define USAGE "rolo [ person1 person2 ...] [ -l -s -r -u user ] "
#endif

#ifdef MSDOS
#define USAGE "rolo [ person1 person2 ...] [ -l -s -r -u use_dir ] "
#endif
