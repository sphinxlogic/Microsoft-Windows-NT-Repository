/*
 * Define common struction for hold directory browse options.
 *
 * Note that some of the structure elements are for features that the
 * current browser does not implement.
 */
typedef struct {				/* Hold browsing options */
    int access;				/* 0-on, 1-selective, 2-off */
    int readme;				/* 0-off, 1-top, 2-bottom */
    int showminlength;
    int showmaxlength;
    int showicons;			/* add icon images if true */
    int showdate;			/* Show last modified data */
    int showsize;			/* Show size of file */
    int showbytes;			/* show detailed byte count */
    int showdescription;		/* extract title from contents */
    int showhtmltitles;			/* Extract titles from HTML files */
    int descriptionlength;
    int showbrackets;
    int showhidden;
    int showowner;
    int showgroup;
    int showprot;
    int addhref;
    char control_file[80];		/* Name of control-file */
    char readme_file[80];		/* Name of readme file */
    char *iconpath;
    char *iconblank;
    char *iconunknown;
    char *icondir;
    char *iconparent;
} dir_opt;
