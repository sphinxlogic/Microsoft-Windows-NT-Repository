
/*  @(#)extern.h 1.24 91/11/19
 *
 *  Contains the external variable definitions used by faces.
 *
 *  Copyright (c) Rich Burridge.
 *                Sun Microsystems, Australia - All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 *
 *  No responsibility is taken for any errors or inaccuracies inherent
 *  either to the comments or the code of this program, but if
 *  reported to me then an attempt will be made to fix them.
 */

extern struct comminfo *communities ;  /* Community alias/username chain. */
extern struct comminfo *clast ;        /* End of chain of community records. */
extern struct machinfo *machines ;     /* Known machine/communities. */
extern struct machinfo *mlast ;        /* End of chain of machine records. */
extern struct psinfo *psrecs ;   /* List of news.ps animation files. */
extern struct psinfo *plast ;    /* End of chain of NeWS animation files. */
extern struct recinfo *recs ;    /* Mail messages being monitored. */
extern struct recinfo *last ;    /* End of the chain of mail records. */
extern struct stat buf ;         /* Buffer for stat call for file existence. */
extern long lastsize ;           /* Last known size of the mail folder. */

extern enum disp_type wdtype ;   /* Current window display option. */
extern enum gr_type gtype ;      /* what graphics system is being used. */
extern enum min_type found ;     /* Result from make_iconname(). */
extern enum mon_type mtype ;     /* What type of monitoring we should do. */

#ifdef AUDIO_SUPPORT
extern char *audio_cmd ;   /* Command to play sound files. */
extern char *bell_sound ;  /* "Bell" audio filename. */
extern int do_audio ;      /* Set if the machine has audio support. */
#endif /*AUDIO_SUPPORT*/

extern char *bgcolor ;     /* X11 background color. */
extern char *bgicon ;      /* Alternate background pattern. */
extern char community[] ;  /* Community name ("real" host name). */
extern char defdir[] ;     /* The default face directory. */
extern char *display ;     /* X11 display information. */
extern char face_buf[] ;   /* Buffer for "X-Face" face images. */
extern char face_host[] ;  /* Pointer to host name from the "From" line. */
extern char face_ts[] ;    /* Pointer to time stamp from the "From" line. */
extern char face_user[] ;  /* Pointer to user name from the "From" line. */
extern char *facedirs ;    /* Colon separated list of face directories. */
extern char *facepath[] ;  /* Array of face image directories. */
extern char *faces_label ; /* Label for the faces window. */
extern char *fgcolor ;                  /* X11 foreground color. */
extern char fname[MAXTYPES][MAXLINE] ;  /* Array of various face name types. */
extern char *fontname ;                 /* X11 font name. */
extern char *geometry ;    /* X11 geometry information. */
extern char *hostname ;    /* Machine name to monitor. */
extern char iconname[] ;   /* Name of the icon file for this person. */
extern char *icon_geometry ;    /* X11 geometry information. */
extern char *line ;        /* Next RFC822 folded line from users spool file. */
extern char machfile[] ;   /* Name of the machine/community file. */
extern char nextline[] ;   /* Input/conversion buffer for various routines. */
extern char peopfile[] ;   /* Name of the people/username file. */
extern char *printer ;     /* Printer name to monitor. */
extern char *progname ;    /* Name of this program. */
extern char realcomm[] ;   /* Real community for this user. */
extern char realname[] ;   /* Real username for this user. */
extern char *resources[] ; /* Faces X resources read. */
extern char revtable[] ;   /* Table for reversing the bits in a byte. */
extern char *rhostname ;   /* Remote host name to monitor (-H option). */
extern char *spoolfile ;   /* Full pathname of users current mail. */
extern char *update_alias ;    /* Name of mail alias for database updates. */
extern char *username ;        /* This users name. */
extern char *userprog ;        /* User supplied program to run. */

extern int beeps ;         /* Number of beeps for arrival of new mail. */
extern int button1clear ;  /* Set if button 1 should clear the window. */
extern int column ;        /* Column number for next icon. */
extern int debug ;         /* Set with the -D command line option. */
extern int disphost ;      /* Display hostname if icon represents user. */
extern int dontshowno ;    /* Set if no. of messages shouldn't  be shown. */
extern int dontshowtime ;  /* Set if timestamp shouldn't be shown. */
extern int dontshowuser ;  /* Set if username shouldn't be shown. */
extern int facetype ;      /* Type of face file found. */
extern int firsttime ;     /* Zeroised after first mail/printer check. */
extern int flashes ;       /* Number of flashes for arrival of new mail. */
extern int fromc_found ;   /* Set if "From:" line found during processing. */
extern int froms_found ;   /* Set if "From " line found during processing. */
extern int height ;        /* Height in pixels of faces display. */
extern int iconheight ;    /* Height of an individual icon. */
extern int iconpos ;       /* Set if icon position given (-WP option). */
extern int iconwidth ;     /* Width of an individual icon. */
extern int iconic ;        /* Start as an icon if set. */
extern int imageheight ;   /* Height of an individual face image. */
extern int imagewidth ;    /* Width of an individual face image. */
extern int invert ;        /* Set if to use reverse video. */
extern int ix ;            /* Initial X position of the icon. */
extern int iy ;            /* Initial Y position of the icon. */
extern int lowerwindow ;   /* Set to lower window when no mail. */
extern int maxcols ;       /* Maximum number of columns to display. */
extern int maxline ;       /* Current size of RFC822 folded line buffer */
extern int maxtypes ;      /* Max. no. of icon types for graphics target. */
extern int mboxshrunk ;    /* Set when the spoolfile size has shrunk. */
extern int mhflag ;        /* Set if this user uses MH to read mail. */
extern int newmail ;       /* Set if there is new mail this time around. */
extern int nolabel ;       /* Whether the faces window has a frame label. */
extern int noicons ;       /* Number of faces this time around. */
extern int old_style ;     /* Set when "old" style face ikon is found. */
extern int period ;        /* Period in seconds for checking new mail. */
extern int posspec ;       /* Set if -Wp or -g option is present (for X11) */
extern int raisewindow ;   /* Set if we are to raise window on update */
extern int row ;           /* Row number for next icon. */
extern int toclear ;       /* Set if faces memory area needs clearing. */
extern int update ;        /* If set, send mail to update faces database. */
extern int width ;         /* Width in pixels of faces display. */
extern int wx ;            /* Initial X position of the window. */
extern int wy ;            /* Initial Y position of the window. */
extern int x_face ;        /* Set if we've extracted an "on-the-fly" face. */
