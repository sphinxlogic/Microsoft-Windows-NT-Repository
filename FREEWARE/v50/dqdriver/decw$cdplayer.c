/*
 *******************************************************************************
 *                                                                             *
 * Copyright © 2000 Compaq Computer Corporation                                *
 *                                                                             *
 * COMPAQ Registered in U.S. Patent and Trademark Office.                      *
 *                                                                             *
 * Confidential computer software. Valid license from Compaq or                *
 * authorized sublicensor required for possession, use or copying.             *
 * Consistent with FAR 12.211 and 12.212, Commercial Computer Software,        *
 * Computer Software Documentation, and Technical Data for Commercial          *
 * Items are licensed to the U.S. Government under vendor's standard           *
 * commercial license.                                                         *
 *                                                                             *
 *******************************************************************************
 */
/*
 *  DECW$CDPLAYER.C
 *
 *      This program acts as a control panel for a SCSI audio CD player. It
 *      creates a workstation window with buttons for playing, pausing,
 *      selecting tracks, etc. The program uses the DECwindows toolkit
 *      routines to create and manage the display and its widgets.
 *
 *
 *  Note:
 *
 *      This program's user interface was created with VUIT.  If the interface
 *      is modified with vuit, then this source will have to be modified.
 *
 *      THIS APPLICATION IS NOT MOTIF STYLE GUIDE COMPLAINT, and should not
 *      be used as an example of how to program a style guide compliant
 *      application.  It should be used ONLY as an example of accessing the
 *      SCSI device.
 *
 *	The logicial name, DECW$CD_PLAYER is used to identify the CD-ROM
 *	drive containing an audio disk.  For example:
 *
 *      If SCSI CD player is at ID 2 on the second SCSI bus, the logical 
 *	DECW$CD_PLAYER is defined:
 *
 *              $ DEFINE DECW$CD_PLAYER DKB200:
 *
 *      If IDE/ADAPI CD player is the master on the first IDE bus, the logical 
 *	DECW$CD_PLAYER is defined:
 *
 *              $ DEFINE DECW$CD_PLAYER DQA0:
 *
 *  Written by:
 *
 *      Rob Lembree and Jeff Otterson of VMS DECwindows Engineering
 *      based on an earlier work by J. Mc., J. K. - VMS engineering.
 *	ATAPI support provided by Paul A. Jacobi.
 *
 *
 *  The commands to build this program on a VAX system are:
 *
 *      $ CC DECW$CDPLAYER
 *      $ LINK DECW$CDPLAYER,SYS$INPUT/OPT
 *      SYS$SHARE:DECW$XLIBSHR.EXE/SHARE
 *      SYS$SHARE:DECW$XMLIBSHR.EXE/SHARE
 *      SYS$SHARE:DECW$DXMLIBSHR.EXE/SHARE
 *	$ UIL/MOTIF DECW$CDPLAYER
 *
 *  The commands to build this program on an Alpha system are:
 *
 * 	$ CC/STANDARD=VAXC/NOWARN/DEFINE=__NEW_STARLET DECW$CDPLAYER
 *	$ LINK DECW$CDPLAYER,SYS$INPUT/OPT
 *        SYS$SHARE:DECW$MRMLIBSHR12/SHARE
 *	  SYS$SHARE:DECW$DXMLIBSHR12/SHARE
 *	  SYS$SHARE:DECW$XMLIBSHR12/SHARE
 *	  SYS$SHARE:DECW$XTLIBSHRR5/SHARE
 *	  SYS$SHARE:DECW$XLIBSHR/SHARE
 *
 *	$ LINK/debug DECW$CDPLAYER,SYS$INPUT/OPT
 *	  SYS$SHARE:DECW$XLIBSHR.EXE/SHARE
 *	  SYS$SHARE:DECW$XMLIBSHR.EXE/SHARE
 *	  SYS$SHARE:DECW$DXMLIBSHR.EXE/SHARE
 *
 *	$ UIL/MOTIF DECW$CDPLAYER
 *
 *  You need PHY_IO and DIAGNOSE privileges to run this program.
 *  If you do not have these privileges, you can link the program
 *  with the "/NOTRACEBACK/NODEBUG" qualifiers and have your system
 *  manager INSTALL the program with the appropriate privileges.
 *
 */

#include <stdio.h>                      /* For printf and so on. */
#include <Xm/Text.h>        
#include <Mrm/MrmAppl.h>                /* Motif Toolkit and MRM */
#include <Xm/Xm.h>
#include <X11/X.h>
#include <Xm/DECWmHints.h>

#include <dvidef.h>
#include <devdef.h>
#include <iledef.h>

/* The vuit generated application include file.				    */
/* If the user does not specify a directory for the file		    */
/* then the vaxc$include logical needs to be defined to point to the	    */
/* directory of the include file.					    */

#include "DECW$CDPLAYER.H"

/*
 *  Forward routine declarations 
 */

void		exit_handler();
void		itoa();
void		reverse();
void		alloc_cd_channel();
void		dealloc_cd_channel();
int		pause_cd();
int		resume_cd();
void		Do_CD_Setup_Work();
void		setup_timer();
void		house_keeping_timer();
void		update_display();
void		update_timer();
 
void		WidgetCreateCallback();
void		ButtonPressCallback();
static void 	set_icons_on_shell();
void	    	set_icons();
static Pixmap	fetch_icon_literal();
static char *	get_icon_index_name();
static void 	set_icon_pixmap();
static void 	set_iconify_pixmap();

static Boolean	xui_winmgr = False;

#define FALSE 0
#define TRUE 1

/*                 
 *  Define the constants used to interface to the SCSI generic class driver.
 */

#define GOOD_SCSI_STATUS 0
#define OPCODE 0
#define FLAGS 1
#define COMMAND_ADDRESS 2       
#define COMMAND_LENGTH 3
#define DATA_ADDRESS 4
#define DATA_LENGTH 5
#define PAD_LENGTH 6
#define PHASE_TIMEOUT 7
#define DISCONNECT_TIMEOUT 8
#define FLAGS_READ 1
#define FLAGS_DISCONNECT 2
#define GK_EFN 1
#define SCSI_STATUS_MASK 0X3E
#define PAUSE_OPCODE 0X4B
#define RESUME_OPCODE 0X4B
#define STOP_OPCODE 1
#define READ_SUBCHAN_OPCODE 0X42
#define READ_TOC_OPCODE 0X43
#define PLAY_TRACK_OPCODE 0X48
#define PAUSE_CMD_LEN 10
#define RESUME_CMD_LEN 10
#define STOP_CMD_LEN 6
#define READ_SUBCHAN_CMD_LEN 10
#define READ_TOC_CMD_LEN 10
#define PLAY_TRACK_CMD_LEN 10
#define READ_TOC_DATA_LEN 4
#define READ_SUBCHAN_DATA_LEN 48

#define ATAPI_READ_TOC_DATA_LEN 804
#define ATAPI_READ_TOC_DATA_LEN_LSB (ATAPI_READ_TOC_DATA_LEN & 0x00FF) 
#define ATAPI_READ_TOC_DATA_LEN_MSB (ATAPI_READ_TOC_DATA_LEN >> 8) 
#define ATAPI_PLAY_MSF_OPCODE 0x47
#define ATAPI_STOP_OPCODE 0x1B
#define ATAPI_READ_SUBCHAN_OPCODE 0X42
#define ATAPI_CMD_LEN 12

#define STOPPED 0
#define PLAYING 1
#define PAUSED  2

globalvalue
	IO$_DIAGNOSE;

/*      
 *  Variables for saving the state of the CD player.
 */     

int	total_tracks=0;                    
int	saved_total_tracks=0;
int	current_track=0;
int	saved_current_track=0;
int	skip_next_poll=FALSE;
int	client_data = 1;

int	cd_mode = 0;
int	last_cd_mode = -1;
int	powerflag = -1;
int	playseconds = 0;

Widget
    *TrackPlayingWidget,
    *StopButtonWidget,
    *PlayPauseButtonWidget,
    *ForwButtonWidget,
    *BackButtonWidget,
    *TrackSelectSliderWidget,
    *StoppedIndicatorWidget,
    *PausedIndicatorWidget,
    *PlayingIndicatorWidget,
    *TotalTracksWidget,
    *TimerLEDWidget;
        
typedef char unsigned UCHAR;
        
#pragma __member_alignment __save
#pragma __nomember_alignment

typedef struct _track_descriptor {
    UCHAR session;
    union _adr_ctrl_u {
        UCHAR adr_ctrl;
        struct {
            unsigned int adr:4;
            unsigned int ctrl:4;
        } adr_ctrl_bits;
    } adr_ctrl_u;
    UCHAR tno;
    UCHAR point;
    UCHAR absolute_time[3];
    UCHAR zero;
    UCHAR start_minute;
    UCHAR start_second; 
    UCHAR start_frame;
} TRACK_DESCRIPTOR;

#define POINT_FIRST_TRACK 0xA0
#define POINT_LAST_TRACK 0xA1
#define POINT_LEAD_OUT 0xA2
#define MAX_TRACK 99

#define CTRL_DIGITAL_DATA 4
typedef struct _atapi_toc {
    UCHAR length_msb;
    UCHAR length_lsb;
    UCHAR first_session;
    UCHAR last_session;
    TRACK_DESCRIPTOR track_descriptor[MAX_TRACK];           
} ATAPI_TOC;

#pragma __member_alignment __restore

typedef struct _track {
    UCHAR start_minute;
    UCHAR start_second;
    UCHAR start_frame;
    UCHAR end_minute;
    UCHAR end_second;
    UCHAR end_frame;
} TRACK;

TRACK track[MAX_TRACK];   


/*
 *  OWN STORAGE 
 *                               
 *  Variables used to interface to the SCSI generic class driver.
 */

short
	gk_chan;

int  
	gk_device_desc[2],
	gk_iosb[2],
	gk_desc[15];

int	atapi;

UCHAR
	pause_command [PAUSE_CMD_LEN] = {PAUSE_OPCODE,0,0,0,0,0,0,0,0,0},
	resume_command [RESUME_CMD_LEN] = {RESUME_OPCODE,0,0,0,0,0,0,0,1,0},
	stop_command [STOP_CMD_LEN] = {STOP_OPCODE,0,0,0,0,0},
	read_subchan_command [READ_SUBCHAN_CMD_LEN] = {READ_SUBCHAN_OPCODE,0,0x40,1,0,0,0,0,READ_SUBCHAN_DATA_LEN,0},
	read_toc_command [READ_TOC_CMD_LEN] = {READ_TOC_OPCODE,0,0,0,0,0,0,0,READ_TOC_DATA_LEN,0},
	play_track_command [PLAY_TRACK_CMD_LEN] = {PLAY_TRACK_OPCODE,0,0,0,0,1,0,0,1,0},
	toc_data[READ_TOC_DATA_LEN],
	subchan_data [READ_SUBCHAN_DATA_LEN],
             
	atapi_play_msf_command [ATAPI_CMD_LEN] = 
            {ATAPI_PLAY_MSF_OPCODE,0,0,0,0,0,0,0,0,0,0,0},
	atapi_read_toc_command [ATAPI_CMD_LEN] = 
            {READ_TOC_OPCODE,0,2,0,0,0,0,
             ATAPI_READ_TOC_DATA_LEN_MSB,
             ATAPI_READ_TOC_DATA_LEN_LSB,0,0,0},
	atapi_toc_data[ATAPI_READ_TOC_DATA_LEN],
	atapi_start_command [ATAPI_CMD_LEN] = {ATAPI_STOP_OPCODE,0,0,0,3,0,0,0,0,0,0,0},
	atapi_stop_command [ATAPI_CMD_LEN] = {ATAPI_STOP_OPCODE,0,0,0,0,0,0,0,0,0,0,0},
	atapi_read_subchan_command [ATAPI_CMD_LEN] = {READ_SUBCHAN_OPCODE,2,0x40,1,0,0,0,0,READ_SUBCHAN_DATA_LEN,0},
                              
	gk_device [] = {"DECW$CD_PLAYER"};
                      

/*
 * Global data
 */
                                               
static MrmType class_id;		/* Place to keep class ID*/
static MrmType *dummy_class;            /* and class variable. */
static char *db_filename_vec[] =        /* Mrm.hierachy file list. */
  {           
"DECW$CDPLAYER.UID"       
  };
static int db_filename_num =
                (sizeof db_filename_vec / sizeof db_filename_vec [0]);
char *vuit_dummy_ident_value = "VUIT dummy identifier value";
int i;
#define hash_table_limit 500
struct HASH_TABLE_STRUCT
    {
    char     	*widget_name;
    Widget	id;
    } hash_table[hash_table_limit + 1];


/*
 * Names and addresses of callback routines to register with Mrm
 */
static MrmRegisterArg reglist [] = {
{"WidgetCreateCallback", (caddr_t)WidgetCreateCallback},
{"ButtonPressCallback", (caddr_t)ButtonPressCallback}};

static int reglist_num = (sizeof reglist / sizeof reglist[0]);

/*
 * Names and addresses of uil identifiers (if any) to register with Mrm.
 * These identifiers are registered with a dummy value to allow the generated 
 * code to run without error.
 * You can avoid the registration of these identifiers by simplying editing
 * this template file (vuit_main_template_c) and removing the following
 * special format comments:
 *	***VUIT ident registration***
 *	***VUIT identlist size***
 *	***VUIT register identifiers***
 * You can provide your own registration of identifiers by calling your own
 * routine in response to a callback (such as the MrmNcreateCallback for your
 * application's main window), or by modifying this template to call your
 * own registration routine.
 */



/*
 * OS transfer point.  The main routine does all the one-time setup and
 * then calls XtAppMainLoop.
 */

unsigned int main(argc, argv)
    unsigned int argc;                  /* Command line argument count. */
    char *argv[];                       /* Pointers to command line args. */
{
    Arg arglist[2];
    int n;

    MrmInitialize();			/* Initialize MRM before initializing */
                                        /* the X Toolkit. */
    DXmInitialize();			/* Initialize additional DEC widgets */

    /* 
     * If we had user-defined widgets, we would register them with Mrm.here. 
     */

    /* 
     * Initialize the X Toolkit. We get back a top level shell widget.
     */
    XtToolkitInitialize();

    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, "DECW$CDPLAYER", "DECW$CDPLAYER",
                            NULL, 0, &argc, argv);
    if (display == NULL) 
	{
        fprintf(stderr, "%s:  Can't open display\n", argv[0]);
        exit(1);
	}

    n = 0;
    XtSetArg(arglist[n], XmNallowShellResize, True);  n++;
    toplevel_widget = XtAppCreateShell(argv[0], NULL, 
                                       applicationShellWidgetClass,
                                       display, arglist, n);

    /* 
     * Open the UID files (the output of the UIL compiler) in the hierarchy
     */
    if (MrmOpenHierarchy(db_filename_num, /* Number of files. */
      db_filename_vec,                    /* Array of file names.  */
      NULL,                               /* Default OS extenstion. */
      &s_MrmHierarchy)                    /* Pointer to returned MRM ID */
      !=MrmSUCCESS)
        s_error("can't open hierarchy");

MrmRegisterNames (reglist, reglist_num);

    n = 0;
    XtSetArg(arglist[n], XtNiconName, "CD Player");  n++;
    XtSetArg(arglist[n], XtNtitle, "CD Player");  n++;
    XtSetValues(toplevel_widget,arglist,n);

VUIT_Manage("MainBulletinBoard");

    /* 
     * Realize the top level widget.  All managed children now become visible
     */


    XtAddEventHandler(toplevel_widget, StructureNotifyMask, False,
                      set_icons_on_shell, None);

    XtRealizeWidget(toplevel_widget);

    Do_CD_Setup_Work();

    /* 
     * Sit around forever waiting to process X-events.  We never leave
     * XtAppMainLoop. From here on, we only execute our callback routines. 
     */
    XtAppMainLoop(app_context);
}


/*
 * All errors are fatal.
 */
void s_error(problem_string)
    char *problem_string;
{
    printf("%s\n", problem_string);
    exit(0);
}

void VUIT_Manage(widget_name)
    char	*widget_name;
{
    Widget		id;
    Window		pop_window;
    XWindowChanges	values;

    if (HashLookup(widget_name, &id))
	if (XtIsManaged(id))
	    {
	    pop_window = XtWindow(XtParent(id));
	    values.x = values.y = values.width = values.height =
		values.border_width = values.sibling = NULL;
	    values.stack_mode = Above;
	    XConfigureWindow(display, pop_window, CWStackMode, &values);
	    }
	else
	    XtManageChild(id);
    else
	{
	MrmFetchWidget(s_MrmHierarchy, widget_name, toplevel_widget, &id, 
	    &class_id);
	XtManageChild(id);
	HashRegister(widget_name, id);
	}
}

void VUIT_Unmanage(widget_name)
    char	*widget_name;
{
    Widget	id;

    if (HashLookup(widget_name, &id))
	XtUnmanageChild(id);
}

int HashRegister (widget_name, id)
    char		*widget_name;
    Widget		id;    
{
    int			ndx;

    for (ndx = HashFunction(widget_name, hash_table_limit);
	((hash_table[ndx].widget_name != NULL) &&
	    (ndx < hash_table_limit));
	ndx++);
    if (hash_table[ndx].widget_name != NULL)
	for (ndx = 0;
	    hash_table[ndx].widget_name != NULL;
	    ndx++);
    if (ndx > hash_table_limit)
	return (FALSE);
    else
	{
	hash_table[ndx].widget_name = XtCalloc(1, strlen(widget_name) + 1);
	strcpy(hash_table[ndx].widget_name, widget_name);
	hash_table[ndx].id = id;
	return (TRUE);
	}
}


int HashLookup (name, id)
    char		*name;
    Widget		*id;
{
    int			ndx;

    for (ndx = HashFunction(name, hash_table_limit);
	((hash_table[ndx].widget_name != NULL) &&
	    (ndx <= hash_table_limit));
	ndx++)
	if (strcmp(name, hash_table[ndx].widget_name) == 0)
	    {
	    *id = hash_table[ndx].id;
	    return (TRUE);
	    }

    if (ndx > hash_table_limit)
	for (ndx = 0;
	    ((hash_table[ndx].widget_name != NULL) &&
		(ndx <= hash_table_limit));
	    ndx++)
	    {
	    if (strcmp(name, hash_table[ndx].widget_name) == 0)
		{
	 	*id = hash_table[ndx].id;
		return (TRUE);
		}
	    }

    return (FALSE);
}

int HashFunction (name, max)
    char		*name;
    int			max;

{
#define HashVecSize		20	/* plenty for 31 character names */
typedef union
    {
    short int		intname[HashVecSize];	 /* name as vector of ints */
    char		charname[2*HashVecSize]; /* name as vector of chars */
    } HashName;

    HashName		locname;	/* aligned name */
    int			namelen;	/* length of name */
    int			namelim;	/* length limit (fullword size) */
    int			namextra;	/* limit factor remainder */
    int			code = 0;	/* hash code value */
    int			ndx;		/* loop index */


    /*
     * Copy the name into the local aligned union.
     * Process the name as a vector of integers, with some remaining characters.
     * The string is copied into a local union in order to force correct
     * alignment for alignment-sensitive processors.
     */
    strcpy (locname.charname, name);
    namelen = strlen (locname.charname);
    namelim = namelen >> 1;		/* divide by 2 */
    namextra = namelen & 1;		/* remainder */

    /*
     * XOR each integer part of the name together, followed by the trailing
     * 0/1 character
     */
    for ( ndx=0 ; ndx<namelim ; ndx++ )
        code = code ^ ((locname.intname[ndx])<<ndx);
    if ( namextra > 0 )
        code = code ^ ((locname.intname[ndx])&0x00FF);

    return (code&0x7FFF) % max;
}


void WidgetCreateCallback (w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;

{
    switch (*tag)
    {
        case StopButtonWidgetID:
            StopButtonWidget = w;
            break;
        case PlayPauseButtonWidgetID:
            PlayPauseButtonWidget = w;
            break;
        case ForwButtonWidgetID:
            ForwButtonWidget = w;
            break;
        case BackButtonWidgetID:
            BackButtonWidget = w;
            break;
        case TrackSelectSliderWidgetID:
            TrackSelectSliderWidget = w;
            break;
        case StoppedIndicatorWidgetID:
            StoppedIndicatorWidget = w;
            break;
        case PausedIndicatorWidgetID:
            PausedIndicatorWidget = w;
            break;
        case PlayingIndicatorWidgetID:
            PlayingIndicatorWidget = w;
            break;
        case TotalTracksWidgetID:
            TotalTracksWidget = w;
            break;
        case TrackPlayingWidgetID:
            TrackPlayingWidget = w;
            break;
        case TimerLEDWidgetID:
            TimerLEDWidget = w;
            break;
        default:
            printf("bad widget value, widget is 0x%lx, \
                  tag is 0x%lx, reason is 0x%lx\n", w, *tag, *reason);
    }
}
                   
void ButtonPressCallback (w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
    int last_mode;
    XmScaleCallbackStruct *s;
    last_mode = cd_mode;
    switch (*tag)
    {
	case StopButtonWidgetID:
	    if (cd_mode != STOPPED) stop_unit();
	    update_display();
	    break;
	case PlayPauseButtonWidgetID:
            switch (last_mode)
            {
                case STOPPED:
                    play_track(current_track);
                    break;
                case PLAYING:
                    pause_cd();
                    break;
                case PAUSED:
                    resume_cd();
                    break;
                default:
                    fprintf(stderr,"Something is not right!\n");
            }
	    update_display();
	    break;
	case ForwButtonWidgetID:
            if (current_track < total_tracks) {
                play_track(++current_track);
                update_display();
                }
	    break;
	case BackButtonWidgetID:
            if (current_track > 1) {
                play_track(--current_track);
                update_display();
                }
	    break;
	case TrackSelectSliderWidgetID:
            s = reason;
            current_track = s->value;
            play_track(current_track);
	    update_display();
	    break;
        case OffButtonWidgetID:
	    if (cd_mode != STOPPED) stop_unit();
	    update_display();
            powerflag = 2;
            break;
    	default:
	    printf("bad widget value, widget is 0x%lx, \
		tag is 0x%lx, reason is 0x%lx\n", w, *tag, *reason);
    }
}

void Do_CD_Setup_Work()
{

/*
 *  Go assign a channel to the device.
 */

	alloc_cd_channel();

       	stop_unit ();
	update_display();
	setup_timer ();
}

void itoa (int n, char s[])
/*
 *  This routine changes an integer to an ascii character.
 */
{
	int t=0;

	do {
	   s[t++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	s[t] = '\0';
	reverse(s);
}

void reverse(char s[])
/*
 *  This is a subroutine for ITOA.
 */
{
	int t, j;
	char c;

	for (t=0, j=strlen(s)-1; t < j; t++, j--)
	{
	c = s[t];
	s[t] = s[j];
	s[j] = c;
	}
}

void exit_handler ()
/*
 *  Callback routine for exit button. Stop the CD from playing, deassign
 *  the channel to the player, and exit.
 */
{
	stop_unit ();           /* spin down the unit */
	dealloc_cd_channel();	/* blow away the channel */
	sys$exit(1);		/* go away */
}
	
void alloc_cd_channel ()
/* 
 *  This routine assigns a channel to the CD player.
 */
{
	int status;
        int return_length;
        unsigned int devchar2;
        ILE3 item_list[2];

	gk_device_desc[0] = strlen (gk_device);
	gk_device_desc[1] = &gk_device[0];

	status = sys$assign (&gk_device_desc[0], &gk_chan, 0, 0);
	if (!(status & 1)) {
		fprintf (stderr,"Unable to assign channel to %s", &gk_device[0]);
		sys$exit (status);
	}

        item_list[0].ile3$w_code = DVI$_DEVCHAR2;
        item_list[0].ile3$w_length = sizeof(devchar2);
        item_list[0].ile3$ps_bufaddr = &devchar2;
        item_list[0].ile3$ps_retlen_addr = &return_length;
        item_list[1].ile3$w_code = 0;
        item_list[1].ile3$w_length = 0;

        status = sys$getdvi(0, 0, &gk_device_desc[0], item_list, 0, 0, 0, 0);

        if (devchar2 & DEV$M_SCSI)
            atapi = FALSE;
        else
            atapi = TRUE;
}

void dealloc_cd_channel ()
/*
 *  This routine deassigns a channel to the CD player 
 */
{
	int status;

	status = sys$dassgn (gk_chan);
	if (!(status & 1)) {
		fprintf (stderr,"Unable to deassign channel from %s", &gk_device[0]);
		sys$exit (status);
	}
}

void setup_timer ()
/*
 *  This routine sets up the time to run one second from the current time.
 */
{
        /* set to 998 ms, takes about 2 ms to reschedule... */ 
 	XtAppAddTimeOut(app_context, 998, &house_keeping_timer, &client_data);
}

/*
 *  This is the timer routine which runs once a second. It reads the current
 *  state of the CD player and updates the display.
 */

void house_keeping_timer ()
/* 
 *  this routine is acitivated every 1 second.  It will re-schedule the 
 *  timer, then increment the number of seconds that this track has been
 *  playing
 */
{
    setup_timer ();
    if (cd_mode == PLAYING) {
        update_timer();
        playseconds++;
    }
    if (!skip_next_poll) get_status ();
    skip_next_poll = FALSE;
}

void update_timer()
/* this routine updates the fake track playing time */
{
    int minutes,seconds;
    char tstr[100];
    XmString timestring;
    Arg argList[2];

    minutes = playseconds / 60;
    seconds = playseconds - minutes * 60;
    sprintf(tstr,"%02d:%02d",minutes,seconds);
    timestring = XmStringCreateSimple(tstr);
    XtSetArg(argList[0],XmNlabelString,timestring);
    XtSetValues(TimerLEDWidget,argList,1);
    XmStringFree(timestring);
}

int pause_cd ()
/*
 *  This routine sends a pause command to the CD player. 
 */
{
	cd_mode=PAUSED;
	return execute_command (pause_command, PAUSE_CMD_LEN, 0, 0);
}

int resume_cd ()
/*
 *  This routine sends a resume command to the CD player. 
 */
{
	cd_mode=PLAYING;
	return execute_command (resume_command, RESUME_CMD_LEN, 0, 0);
}

int read_toc()  
/*
 *  This routine reads the table of contents on the CD to determine the
 *  total number of tracks.
 */
{
	int track, status, length;
                                                                               
	total_tracks = 0;
        if (atapi) {
            if (!execute_command (atapi_read_toc_command, ATAPI_CMD_LEN, atapi_toc_data, ATAPI_READ_TOC_DATA_LEN)) return FALSE;
            atapi_parse_toc(atapi_toc_data);
        } else {
            if (!execute_command (read_toc_command, READ_TOC_CMD_LEN, toc_data, READ_TOC_DATA_LEN)) return FALSE;
   	    total_tracks = toc_data[3] - toc_data[2] + 1;
        }

/*	printf("This CD has %d tracks\n",total_tracks); */
	return TRUE;
}                                                         

void atapi_parse_toc()
{
    int toc_length;
    int number_toc_descriptors;
    ATAPI_TOC *atapi_toc;                                                      
    int i;
    int cnt;
    int first_track, last_track;
    UCHAR lead_out_minute, lead_out_second, lead_out_frame;

    atapi_toc = &atapi_toc_data;                                               

    toc_length = (atapi_toc->length_msb << 16) | atapi_toc->length_lsb;
    number_toc_descriptors = (toc_length - 2) / sizeof(TRACK_DESCRIPTOR);
    cnt = 0;
    last_track = MAX_TRACK;

    for (i=0; i< number_toc_descriptors; i++) {
        switch (atapi_toc->track_descriptor[i].point) {
            case POINT_FIRST_TRACK:
                first_track = (int) atapi_toc->track_descriptor[i].start_minute;
                break;                        
            case POINT_LAST_TRACK:
                last_track = (int) atapi_toc->track_descriptor[i].start_minute;
                break;
            case POINT_LEAD_OUT:
                lead_out_minute = atapi_toc->track_descriptor[i].start_minute;
                lead_out_second = atapi_toc->track_descriptor[i].start_second;
                lead_out_frame = atapi_toc->track_descriptor[i].start_frame;
                break;
            default:
                if ((atapi_toc->track_descriptor[i].point > 0) &&
                    (atapi_toc->track_descriptor[i].point < 100) &&
                    (atapi_toc->track_descriptor[i].adr_ctrl_u.adr_ctrl_bits.ctrl < CTRL_DIGITAL_DATA))  {
                    track[cnt].start_minute = atapi_toc->track_descriptor[i].start_minute;
                    track[cnt].start_second = atapi_toc->track_descriptor[i].start_second;
                    track[cnt].start_frame = atapi_toc->track_descriptor[i].start_frame;
                    cnt++;
                }
                break;
        }
        
        /* If this was the last track, then break out of the loop.  This
         * assumes only a single-session disk.  For multi-session audio, more 
         * code is needed here to parse the second session descriptors.
         */
        if (cnt == last_track) break;
    }

    total_tracks = cnt;

    for (i=0; i< total_tracks-1; i++) {
        track[i].end_minute = track[i+1].start_minute;
        track[i].end_second = track[i+1].start_second;
        track[i].end_frame = track[i+1].start_frame;
    }

    track[total_tracks-1].end_minute = lead_out_minute;
    track[total_tracks-1].end_second = lead_out_second;
    track[total_tracks-1].end_frame = lead_out_frame;


}


int play_track (track_num)
/*
 *  This routine plays the specified track on the CD.
 */
char track_num;
{                                              
	int status;
                                        
        if (atapi) {
	    atapi_play_msf_command [3] = track[track_num - 1].start_minute;
	    atapi_play_msf_command [4] = track[track_num - 1].start_second;
	    atapi_play_msf_command [5] = track[track_num - 1].start_frame;
	    atapi_play_msf_command [6] = track[total_tracks - 1].end_minute;
	    atapi_play_msf_command [7] = track[total_tracks - 1].end_second;
	    atapi_play_msf_command [8] = track[total_tracks - 1].end_frame;
	    status = execute_command (atapi_play_msf_command, ATAPI_CMD_LEN, 0, 0);
        } else {
	    play_track_command [4] = track_num;
	    play_track_command [7] = total_tracks;       
	    status = execute_command (play_track_command, PLAY_TRACK_CMD_LEN, 0, 0);
        }
	skip_next_poll = TRUE;
	cd_mode = PLAYING;
	return status;	                  
}                      

int stop_unit()
/*
 *  This routine sends a stop command the the CD player.
 */
{
	cd_mode = STOPPED;      
        if (atapi) {
            return execute_command (atapi_stop_command, ATAPI_CMD_LEN, 0, 0);
        } else {
            return execute_command (stop_command, STOP_CMD_LEN, 0, 0);
        }
}

int execute_command (command_addr, command_len, data_addr, data_len)
/*
 *  This routine sends the specified command to the CD player. It does
 *  so by filling in the generic class driver descriptor and issuing
 *  an IO$_DIAGNOSE QIO to GKDRIVER.
 */
int *command_addr, command_len, *data_addr, data_len;
{
	int i, status;
	char scsi_status;

	gk_desc[OPCODE] = 1;
	gk_desc[FLAGS] = FLAGS_READ + FLAGS_DISCONNECT;
	gk_desc[COMMAND_ADDRESS] = command_addr;
	gk_desc[COMMAND_LENGTH] = command_len;
	gk_desc[DATA_ADDRESS] = data_addr;
	gk_desc[DATA_LENGTH] = data_len;
	gk_desc[PAD_LENGTH] = 0;
	gk_desc[PHASE_TIMEOUT] = 0;
	gk_desc[DISCONNECT_TIMEOUT] = 60;
	for (i=9; i<15; i++) gk_desc[i] = 0;	/* Clear reserved fields */

/*
 *  Issue the QIO to send the inquiry command and receive the inquiry data.
 */

	status = sys$qiow (GK_EFN, gk_chan, IO$_DIAGNOSE, gk_iosb, 0, 0, 
			   &gk_desc[0], 15*4, 0, 0, 0, 0);

/*
 *  Check the various returned status values.
 */

	if (!(status & 1)) sys$exit (status);
	if (!(gk_iosb[0] & 1)) sys$exit (gk_iosb[0] & 0xffff);
	scsi_status = (gk_iosb[1] >> 24) & SCSI_STATUS_MASK;
	if (scsi_status == GOOD_SCSI_STATUS) return TRUE;
	return FALSE;
}

int get_status ()
/*
 *  This routine reads the current status of the CD player to determine the
 *  total number of tracks, whether the player is playing, and, if so, the
 *  current track number being played. If any of this information has changed
 *  since the last time the screen was updated, then the screen is updated
 *  again.
 */
{
	int a;
        int read_toc_status;
        int read_subchan_status;

        read_toc_status = read_toc();
        if (atapi)
            read_subchan_status = execute_command(atapi_read_subchan_command, ATAPI_CMD_LEN, subchan_data, READ_SUBCHAN_DATA_LEN);
        else
            read_subchan_status = execute_command(read_subchan_command, READ_SUBCHAN_CMD_LEN, subchan_data, READ_SUBCHAN_DATA_LEN);
 
	if (!read_toc_status || !read_subchan_status) {
		cd_mode = STOPPED;
		current_track = 0;
		total_tracks = 0;
	} else {
                if (atapi)
                    current_track = atapi_find_track(subchan_data[9],subchan_data[10], subchan_data[11]);
                else
		    current_track = subchan_data[6];
		if (subchan_data[1] == 0x13) stop_unit ();		
	}
	if (current_track != saved_current_track || 
		cd_mode != last_cd_mode ||
		total_tracks != saved_total_tracks) update_display ();

        if (powerflag > -1) {
            powerflag--;
            if (powerflag < 1) exit(1);
        }
}



int atapi_find_track(UCHAR minute, UCHAR second, UCHAR frame)
{
    int i;
    int current_second, start_second, end_second;

    current_second =  (((int) minute * 60) + (int) second);

    for (i=0; i < total_tracks; i++) {
 
        start_second = (((int)track[i].start_minute * (int) 60) + (int)track[i].start_second);
        end_second = (((int)track[i].end_minute * (int) 60) + (int)track[i].end_second);

        if ((current_second >= start_second) && (current_second < end_second)) {
            return(i+1);
        }
    }

    return(1);
}



void update_display()
{
        Arg argList[3];
        int min=1, max=total_tracks;
        char total_tracks_char[4], track_num_char[4];
	XmString track_str, total_track_str;
/*
 *  If a new CD has been inserted which contains a different number of
 *  tracks, update the slider to indicate the total number of tracks on
 *  the new CD.
 */
                             
        if (saved_total_tracks != total_tracks) {
                saved_total_tracks = total_tracks;

                itoa (total_tracks, total_tracks_char);
                total_track_str = XmStringCreateSimple(total_tracks_char);
                XtSetArg(argList[0],XmNlabelString,total_track_str);
                XtSetValues(TotalTracksWidget,argList,1);
		XmStringFree(total_track_str);
                if (total_tracks == 0) {
                        min=0;
                        max=1;
                }
                XtSetArg(argList[0], XmNminimum, min);
                XtSetArg(argList[1], XmNmaximum, max);
                XtSetArg(argList[2],XmNvalue, current_track);
                XtSetValues(TrackSelectSliderWidget, argList, 3);
        }

/*
 *  If the player is now playing a different track than before, update
 *  the slider to indicate this.
 */

        if (saved_current_track != current_track) {
                saved_current_track = current_track;

                playseconds = 0;
                update_timer();

                itoa(current_track,track_num_char);
                track_str = XmStringCreateSimple(track_num_char);
                XtSetArg(argList[0],XmNlabelString,track_str);
                XtSetValues(TrackPlayingWidget,argList,1);
                XmStringFree(track_str);

                XtSetArg(argList[0],XmNvalue, current_track);
                XtSetValues(TrackSelectSliderWidget,argList,1);
        }

        if (cd_mode != last_cd_mode) {
            switch (cd_mode) {
                case STOPPED:
                    if (!XtIsManaged(StoppedIndicatorWidget)) 
                        XtManageChild(StoppedIndicatorWidget);
                    if (XtIsManaged(PlayingIndicatorWidget))
                        XtUnmanageChild(PlayingIndicatorWidget);
                    if (XtIsManaged(PausedIndicatorWidget))
                        XtUnmanageChild(PausedIndicatorWidget);
                    break;
                case PLAYING:
                    if (XtIsManaged(StoppedIndicatorWidget)) 
                        XtUnmanageChild(StoppedIndicatorWidget);
                    if (!XtIsManaged(PlayingIndicatorWidget))
                        XtManageChild(PlayingIndicatorWidget);
                    if (XtIsManaged(PausedIndicatorWidget))
                        XtUnmanageChild(PausedIndicatorWidget);
                    break;
                case PAUSED:
                    if (XtIsManaged(StoppedIndicatorWidget)) 
                        XtUnmanageChild(StoppedIndicatorWidget);
                    if (XtIsManaged(PlayingIndicatorWidget))
                        XtUnmanageChild(PlayingIndicatorWidget);
                    if (!XtIsManaged(PausedIndicatorWidget))
                        XtManageChild(PausedIndicatorWidget);
                    break;
                default:
                    fprintf(stderr,"Unknown CD_MODE: %d\n",cd_mode);
                    break;
            }
            last_cd_mode = cd_mode;
        }
}

/*
 *  Callback routine which sets the icon pixmaps for Reparenting
 *  window managers.
 */
static void set_icons_on_shell( shell, user_data, event )
Widget		shell;
caddr_t		user_data;	/* unused */
XEvent		*event;
{
    XIconSize       *size_list;
    int	    	    num_sizes;
    Display 	    *dpy = XtDisplay( shell );
    Window  	    root_window = XDefaultRootWindow( dpy );
    XReparentEvent  *reparent = (XReparentEvent *) &event->xreparent;

    if ( event->type != ReparentNotify )
    	return;

    /* Ignore reparents back to the root window.
     */
    if ( reparent->parent == root_window )
    	return;

    /* Only take the performance hit to see which window manager is now
     * running when we get the reparent event.  XmIsMotifWMRunning 
     * fetches several properties and does a XQueryTree which makes
     * it an 'expensive' routine to call.
     */
    if ( XmIsMotifWMRunning( shell ) )
    	xui_winmgr = False;
    else
    	xui_winmgr = True;

    /*  Set the icons for this shell.
     */
    if ( ! XGetIconSizes( dpy, root_window, &size_list, &num_sizes ) )
        return;
    else
    {
    	XFree( size_list );
    	set_icons( shell, s_MrmHierarchy );
    }

};  /* end of set_icons_on_shell */


/*
 *  Set the icon and iconify pixmaps for the given shell widget.
 */
void set_icons( shell, hierarchy_id )
Widget		shell;
MrmHierarchy	hierarchy_id;
{
    Display 	    	*dpy = XtDisplay( shell );
    Screen  	    	*scr = XtScreen( shell );
    unsigned int    	icon_size;
    char	    	*icon_name;
    static char     	*shell_icon_sizes[] = { "75", "50", "32", "17" };
    static int	    	num_sizes = XtNumber( shell_icon_sizes );
    static unsigned int	current_icon_size = 0;
    static Pixmap   	icon_pixmap = 0;
    static Pixmap   	iconify_pixmap = 0;

    /* Determine the icon pixmap name and size to fetch.
     */
    icon_name = get_icon_index_name( dpy, "ICON_PIXMAP", &icon_size, 
    	    	    	    	     shell_icon_sizes, num_sizes );
    if ( icon_name != NULL )
    {
    	/*  If the icon sizes are different we need to free the current
    	 *  ones, and re-fetch new icons.  We assume that re-fetching
    	 *  new icons is an infrequent operation, so we don't cache the 
    	 *  old icons.
    	 */
    	if ( ( current_icon_size != 0 )	    	    	/* Icon exists.     */
    	     && ( current_icon_size != icon_size ) )	/* New icon needed. */
    	{
    	    if ( icon_pixmap )
    	    	XFreePixmap( dpy, icon_pixmap );
    	    if ( ( iconify_pixmap ) && ( iconify_pixmap != icon_pixmap ) )
    	    	XFreePixmap( dpy, iconify_pixmap );
    	    icon_pixmap = 0;
    	    iconify_pixmap = 0;
    	    current_icon_size = 0;
    	}
    	if ( current_icon_size == 0 )
    	{
    	    current_icon_size = icon_size;
    	    icon_pixmap = fetch_icon_literal( hierarchy_id, dpy, scr, icon_name );
    	}
    	XtFree( icon_name );
    	icon_name = NULL;
    }

    /* Fetch the iconify pixmap for compatibility with the XUI window manager.
     */
    if (xui_winmgr)
    {
    	if ( icon_size == 17 )  	    /* Don't fetch icon twice */
    	    iconify_pixmap = icon_pixmap;
    	else if ( icon_size > 17 )
    	{
    	    if (iconify_pixmap == 0)
    	    	iconify_pixmap = fetch_icon_literal( hierarchy_id, dpy, scr, 
    	    	    	     	    "ICON_PIXMAP_17X17" );
    	}
    }

    /* Set the icon pixmap on the shell.
     */
    if ( icon_pixmap )
    	set_icon_pixmap(shell, icon_pixmap);

    /* Set the iconify pixmap for the XUI window manager 
     */
    if ( iconify_pixmap )
    	set_iconify_pixmap( shell, iconify_pixmap );

};  /* end of set_icons */


/*
 *  Fetches a bitmap from a UID hierachy to be used as an icon.
 */
Pixmap fetch_icon_literal( hierarchy_id, dpy, scr, index_string )
MrmHierarchy	hierarchy_id;
Display		*dpy;
Screen		*scr;
String		index_string;
{
    int	    	    status;
    Pixmap  	    pixmap_rtn;
    Dimension       width, height;	

    if ( MrmFetchBitmapLiteral( 
    	    	hierarchy_id,
    	    	index_string,	    /* name of icon literal */
    	    	scr,	    	    /* screen pointer 	    */
    	    	dpy,
    	    	&pixmap_rtn,
    	    	&width, 		
    	    	&height ) !=  MrmSUCCESS )
    	return  (Pixmap) 0;

    return (Pixmap) pixmap_rtn;

};   /* end of fetch_icon_literal */


/*
 *  Finds the largest icon supported by the window manager and returns
 *  a string which represents that icon in UIL.
 */
static char * get_icon_index_name( dpy, root_index_name, icon_size_rtn, 
    	    	supported_icon_sizes, num_supported_sizes )
Display 	    *dpy;
char    	    *root_index_name;
unsigned int    *icon_size_rtn;
char    	    **supported_icon_sizes;
int	    	    num_supported_sizes;
{
    XIconSize	*size_list;
    int	    	num_sizes;
    int	    	cursize;
    int	    	i;
    char    	*icon_index = NULL;
    int	    	icon_size;
    char    	*icon_size_ptr;
    Boolean 	found_icon_size = False;

    *icon_size_rtn = 0;	    /* Initial value */

    if ( XGetIconSizes( dpy, XDefaultRootWindow( dpy ), 
    	    	    	  &size_list, &num_sizes ) )
    {
    	/* Find the largest icon supported by the window manager.
     	 */
    	cursize = 0;
    	for ( i = 1; i < num_sizes; i++ )
    	{
    	    if ( ( size_list[i].max_width >= size_list[cursize].max_width )
    	      	&& ( size_list[i].max_height >= size_list[cursize].max_height ) )
    	    	cursize = i;
    	}
    	/* Find the largest icon we can support.
    	 */
    	if ( ( size_list[cursize].max_width > 0 ) 
    	    || ( size_list[cursize].max_height > 0 ) )
    	{
    	    for ( i = 0; i < num_supported_sizes; i++ )
    	    {
    	    	icon_size = atoi( supported_icon_sizes[i] );
    	    	if ( ( icon_size <= size_list[cursize].max_width )
    	    	    && ( icon_size <= size_list[cursize].max_height ) )
    	    	{
    	    	    icon_size_ptr = supported_icon_sizes[i];
    	    	    found_icon_size = True;
    	    	    break;
    	    	}
    	    }
    	}
    	XFree( size_list );
    }

    /* Default to something that both XUI and Mwm can except.
     */
    if ( !found_icon_size )
    {
    	icon_size = 32;
    	icon_size_ptr = "32";
    }

    /*  Build the icon index name
     *  
     *     format: root_index_name + "_" + icon_size_ptr + "X" + icon_size_ptr
     */
    icon_index = (char *) XtMalloc( strlen( root_index_name ) 	    +
    	    	    	    	    sizeof( "_" )	    	    +
    	    	    	    	    ( 2 * sizeof( icon_size_ptr ) ) +
    	    	    	    	    1 );    /* for \0 char */
    strcpy( icon_index, root_index_name );
    strcat( icon_index, "_" );
    strcat( icon_index, icon_size_ptr );
    strcat( icon_index, "X" );
    strcat( icon_index, icon_size_ptr );
    *icon_size_rtn = (unsigned int) icon_size;

    return( icon_index );

};  /* end of get_icon_index_name */


static void set_icon_pixmap( shell, pixmap)
Widget  shell;
Pixmap  pixmap;
{
    if ( XtWindow(shell) != 0 )
    {
	XWMHints    *wmhints = NULL;
    	Display     *dpy = XtDisplay( shell );
    	Window      win = XtWindow( shell );

	/* HACK: Under Motif 1.1 changing iconPixmap will cause the window 
	*  	 to go to its intial state.  This appears to be a side-effect 
	*  	 of ICCCM-compliant behavior, and doing XtSetValues in the
	*  	 X toolkit, so we need to call Xlib directly instead of 
	*  	 setting XtNiconPixmap. 
	*/
	wmhints = XGetWMHints( dpy, win );
	if ( wmhints != NULL )
	{
	    if (xui_winmgr)
		wmhints->flags &= ~StateHint;   /* clear it */
	    else
		wmhints->flags |= StateHint;    /* reset it */
	    wmhints->flags |= IconPixmapHint;
	    wmhints->icon_pixmap = pixmap;
	    XSetWMHints( dpy, win, wmhints );
	    XFree( wmhints );
	} 
	else
	{
	    wmhints = (XWMHints *)XtCalloc(1, sizeof(XWMHints));
	    wmhints->flags &= ~StateHint;
	    wmhints->flags |= IconPixmapHint;
	    wmhints->icon_pixmap = pixmap;
	    XSetWMHints( dpy, win, wmhints );
	    XtFree( wmhints );
	}
    }
    else
    {
	Arg	arglist[1];
	XtSetArg(arglist[0], XmNiconPixmap, pixmap);
	XtSetValues(shell, arglist, 1);
    }

}   /* end of set_icon_pixmap */


/*
 *  Sets the iconify pixmap in the DEC_WM_HINTS property for the 
 *  given shell widget.
 */
static void set_iconify_pixmap( shell, iconify_pixmap )
    Widget shell;
    Pixmap iconify_pixmap;
{
typedef unsigned long int   INT32;
typedef struct {
        INT32 value_mask;
        INT32 iconify_pixmap;
        INT32 icon_box_x;
        INT32 icon_box_y;
        INT32 tiled;
        INT32 sticky;
        INT32 no_iconify_button;
        INT32 no_lower_button;
        INT32 no_resize_button;
} internalDECWmHintsRec, *internalDECWmHints;

#define WmNumDECWmHintsElements ( sizeof( internalDECWmHintsRec ) / sizeof( INT32 ) )

    internalDECWmHintsRec   prop;
    static Atom	    	    decwmhints = None;

    prop.value_mask 	    = DECWmIconifyPixmapMask;
    prop.icon_box_x 	    = -1;
    prop.icon_box_y 	    = -1;
    prop.tiled 	    	    = False;
    prop.sticky     	    = False;
    prop.no_iconify_button  = False;
    prop.no_lower_button    = False;
    prop.no_resize_button   = False;
    prop.iconify_pixmap     = iconify_pixmap;

    if (decwmhints == None)
    	decwmhints = XmInternAtom( XtDisplay( shell ), "DEC_WM_HINTS", False );
    if (decwmhints != None)
    	XChangeProperty( XtDisplay( shell ), XtWindow( shell ), decwmhints, 
    	    	     	decwmhints, 32, PropModeReplace, 
    	    	     	(unsigned char *) &prop, WmNumDECWmHintsElements );

};  /* end of set_iconify_pixmap */

