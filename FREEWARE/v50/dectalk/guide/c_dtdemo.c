/*
 *
 *			    c_dtdemo.c
 * 
 *  The following file contains a "modifiable" demonstration program 
 *  to be included in the DECtalk Application Development Guide.  This
 *  demonstration program typifies most telephone application programs
 *  written for DECtalk.  It is written in VAX11-C and uses Version 4.5
 *  or later of the VMS/MicroVMS Run-Time Library Support provided for 
 *  DECtalk.
 *
 *  Description:  This program provides a framework to develop other
 *                demonstration programs.  Currently, it provides an
 *                information services demonstration giving the Boston
 *                weather forecast, transportation information, ski
 *                conditions, and mortgage rate information.  The 
 *                program provides dial-in access only.  When a user 
 *                calls in, DECtalk answers the phone and speaks a short
 *                greeting message.  Then, the customer is asked to enter
 *                his access code and password.  The customer is given 
 *                three attempts to enter his access code and three 
 *                attempts to enter his password.  Once access to the 
 *                system has been gained, DECtalk prompts the user to
 *                enter a command.  The six valid commands are given below:
 *
 *                   Key 1 for the Boston weather forecast.
 *                   Key 2 for transportation information.
 *                   Key 3 for mortgage rate information.
 *                   Key 4 for the current ski conditions.
 *                   Key 0 for help.
 *                   Key * to exit.
 * 
 *                Note, the user's access code, password and all commands
 *                except exit must be terminated by the number sign key.  
 * 		  When a valid command is entered, DECtalk speaks the 
 *                appropriate message to the user.  Once the exit key is
 *                entered, a wink is detected, or a user fails to enter a
 *                command in the specified time period, the user session is
 *                ended.  DECtalk speaks a goodbye message to the user and
 *                hangs up the telephone.  Then, DECtalk is re-enabled for
 *                autoanswer and waits for a new telephone call.
 *
 *		  All error messages are logged only to the operator terminals
 *		  that have specified that the "OPER11" type of message will
 *		  be handled.  To have error messages logged to the console
 *		  terminal, or any other terminal, at the DCL prompt ($),
 *		  enter the REPLY/ENABLE command:
 *
 *                    $ reply/enable=oper11
 *
 *                To enter this command, you must have the OPER user privelege.
 *
 *
 *
 *  VMS VERSION 4.5 RUN-TIME LIBRARY RESTRICTIONS FOR DECtalk SUPPORT:
 *
 *
 *  Before the application program is run, the terminal line that the
 *  DECtalk is connected to should be allocated.  On some machines running
 *  VMS, if the terminal line is running at high speed (9600 baud) and the
 *  terminal line is not allocated, the program fails on the call to 
 *  DTK$INITIALIZE with a device timeout error.  Note, the program also may
 *  fail with a device timeout error the first time you run it after the 
 *  terminal line has been allocated.  It should run the second time.  If not, 
 *  lower the baud rate on both the terminal line and the DECtalk module.
 *
 *  There is a problem with the DTKDEF module in STARLET.OLB.  When 
 *  calling DTK$READ_KEYSTROKE, the return values for touch tone keys
 *  0-9 do not match the definitions in DTKDEF.  The current definitions
 *  are currently found in "dtkdef.h".  Note, it is anticipated that 
 *  this will be fixed in the Version 4.6 release of VMS.  Consult 
 *  the release notes for Version 4.6.
 *
 *  The current version of the RTL does not provide a command to check
 *  the status of DECtalk.  With this command, an application program 
 *  can determine whether or not the DECtalk module has power cycled 
 *  since the last time its status has been observed.  If it is detected
 *  that the DECtalk module has power cycled, application specific 
 *  parameters (speaking voice, speaking rate, words loaded into the user 
 *  loadable dictionary) can be reinitialized.  By periodically checking 
 *  the status of the DECtalk module, and reinitializing if the unit
 *  has power cycled, an application may not have to be terminated
 *  to replace failed DECtalk modules.  An alternative solution (used in
 *  this demonstration program) re-initializes application specific 
 *  parameters every time a phone call has not been received in 15 minutes.
 *  While these parameters may not need to be reinitialized every 15
 *  minutes, using this alternative, DECtalk modules can still be "hot
 *  swapped" without terminating the application.  However, the application
 *  will have to be terminated if the DECtalk module failed at some time
 *  other than when it is waiting for a phone call.
 *
 *  Most of the DTK$ RTL functions that read and return a status condition
 *  from the DECtalk do not have a timeout specified on their read from 
 *  DECtalk.  These DTK$ RTL functions include DTK$ANSWER_PHONE, 
 *  DTK$DIAL_PHONE, DTK$HANGUP_PHONE, DTK$LOAD_DICTIONARY, DTK$READ_KEYSTROKE,
 *  DTK$READ_STRING, DTK$RETURN_LAST_INDEX, DTK$SET_KEYPAD_MODE, 
 *  DTK$SPEAK_FILE, DTK$SPEAK_PHONEMIC_TEXT, and DTK$SPEAK_TEXT.
 *  Without a timeout, it is possible for the application
 *  program to hang if the DECtalk module fails, the power cord is 
 *  disconnected or the RS232 cable is disconnected.  In this situation, 
 *  the operator would not be aware that the application program is hung.
 *  However, when a user tried to call the system, either the phone would
 *  ring indefinitely or the user would always get a busy signal.  To
 *  prevent the application program from hanging without notifying the operator
 *  of the problem, a system timer (using SYS$SETIMR) is set before ALL calls
 *  made to the DTK$ facility of the Run-Time Library.  This is done by
 *  calling the subroutine "set_timer()",  At present, the MAXIMUM value that 
 *  a system timer can be set to is 86400 seconds or 24 hours.  If a response 
 *  is received from the DECtalk within the time period specified, the system 
 *  timer is canceled (using SYS$CANTIM) by calling the subroutine 
 *  "cancel_timer()".  Otherwise, if the timer expires, the following warning 
 *  message is written to the operator's terminal:
 *
 *			FATAL ERROR 556 on TXyy
 *
 *  where 556 is the decimal value of the timeout error status returned from 
 *  the RTL routine and TXyy is the name of the physical device that the 
 *  DECtalk is connected to.  This warning message informs the operator that 
 *  there is something wrong with the connection between the DECtalk module
 *  and the physical device (failed DECtalk module, RS232 cable disconnected,
 *  power cord disconnected, etc) and that the error should be corrected.
 *  To correct any error that may occur in the communication between
 *  the DECtalk module and the physical device, terminate the current
 *  job, correct the error, and then restart the job.  Note, the 
 *  application program will hang until the problem has been corrected.
 *
 *  The current version of DTK$READ_STRING does not work correctly.  If
 *  a series of touch tone keys is entered on the touch tone keypad and
 *  a terminating character (number sign key or asterisk) is not entered,
 *  the series of touch tone keys entered is returned after the timeout
 *  period specified along with a terminator code of DTK$K_TRM_TIMEOUT.
 *  On the next call to DTK$READ_STRING, when the series of touch tone
 *  keys is returned, it always includes the last touch tone key returned
 *  in the previous call to DTK$READ_STRING as the first touch tone key in
 *  the series of keys returned.  The subroutine "get_key_string()"
 *  in this demonstration program can be used to read a series of touch
 *  tone keys terminated by the number sign key or the asterisk key.  It
 *  is similar in functionality to the DTK$READ_STRING routine.
 *
 *  DTK$ANSWER_PHONE automatically enables autostop mode on the telephone
 *  keypad and wink detection.  If it is not desirable to have autostop
 *  mode enabled for your application, it can be disabled by turning on
 *  the keypad without autostop (DTK$K_KEYPAD_ON), using the 
 *  DTK$SET_KEYPAD_MODE command.  Wink detection cannot be disabled using
 *  an RTL routine.  However, if it is observed that spontaneous winks
 *  are being detected that DO NOT indicate that the caller has hung up,
 *  the wink status returned from the DTK$ routines can be ignored.
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <descrip.h>
#include <opcdef.h>
#include <signal.h>
#include <ssdef.h>
#include <stsdef.h>
#include "dtkdef.h"

#define LF		  0x0A		/* Linefeed			*/
#define CR		  0x0D		/* Carriage return		*/
#define MAX_ENTRY	  3		/* Max number of tries to login	*/
#define MAXKEYS		  80		/* Max length of keypad input	*/
#define BUFLEN		  80		/* Max len command string param	*/
#define MAXSIZE		  257		/* Max size of dictionary entry	*/
#define ERRMAX		  120		/* Max size of error msg buffer	*/
#define T1SECOND	  1		/* 1 second timeout period	*/
#define T2SECOND	  2		/* 2 second timeout period	*/
#define T20SECOND	  20		/* 20 second timeout period	*/
#define T30SECOND	  30		/* 30 second timeout period	*/
#define T5MINUTE	  300		/* 5 minute watchdog timer	*/
#define T15MINUTE	  900		/* 15 minute watchdog timer	*/
#define T429SECOND	  429		/* 429 second timer (ca. 7 min) */
#define T24HOUR		  86400         /* 24 hour timeout period	*/
#define NUMRINGS	  0		/* Number of rings before answer*/
#define DEF_COMMA_PAUSE   0		/* Use default comma pause time */
#define DEF_PERIOD_PAUSE  0		/* Use default period pause time*/
#define NOT_INIT	  0		/* DECtalk & appl. not initial'd*/
#define APPL_INIT	  1		/* Appl. parameters initialized */

/*
 *  Defines the structure for the message buffer to log
 *  errors to the operator's terminal (using the $SNDOPR command)
 */
typedef struct msg_to_oper {
    unsigned int	opc$b_ms_type:8;
    unsigned int	opc$b_ms_target:24;
    long 		opc$l_ms_rqstid;
    char		opc$l_ms_text[ERRMAX];
} OPER_REQ;

/*
 *  Define variables used to set various DECtalk
 *  features (modes) for the current application.
 */
static int           nrings=0;		        /* # rings before answer*/
static long	     voice=DTK$K_VOICE_MALE;    /* Type of voice	*/
static long  	     rate=180;		        /* Speaking rate	*/
static unsigned long cp=DEF_COMMA_PAUSE;	/* Default comma pause	*/
static unsigned long pp=DEF_PERIOD_PAUSE;	/* Default period pause	*/
static unsigned long speechon=DTK$K_SPEAK;	/* Start speaking	*/     	
static unsigned long mode=DTK$M_SQUARE;   	/* Enable "[" mode	*/
static unsigned long keymode=DTK$K_KEYPAD_AUTO; /* Default keypad mode	*/

/*
 *  Static (application specific) variables
 */			
static int    init_dtk = NOT_INIT;	/* DECtalk not initialized	*/
static char   dict_file[BUFLEN];	/* Dictionary file name		*/
static char   device[BUFLEN]; 		/* DECtalk output device	*/
static char   errbuf[ERRMAX];		/* Error message buffer		*/
static char   ttkeys[MAXSIZE];		/* Touch tone keys typeahead buf*/
static char   term_set[] = "#*";	/* Key string terminators	*/
       int    bad_entry_count;		/* counter for bad entries
					    3 bad entries, speak help msg */
/*
 *  String descriptors for RTL calls.
 */
static struct dsc$descriptor_vs vdesc;	/* Variable string descriptor	*/
static struct dsc$descriptor_s  s1desc; /* Static string descriptor	*/
static struct dsc$descriptor_s  s2desc; /* Static string descriptor	*/

/*
 *  Prompts and demo text
 */
static char rsbracket[]    = "]";

static char msg_welcome[]  = "Welcome to the DECtalk information services \
demonstration.  ";

static char msg_access[]   = "Please enter your access code followed by the \
pound key.  You may enter any number as a test.  ";

static char msg_password[] = "Please enter your password followed by the \
pound key.  You may enter any number as a test.  ";

static char msg_invalid[]  = "Invalid entry.  Please try again.  ";

static char msg_noaccess[] = "Access denied.  Please check your access code \
and password and try again.  ";

static char msg_goodbye[]  = "Thank you for calling the DECtalk demonstration \
program.  Have a nice day.  ";

static char msg_timeout[]  = "No key pressed in the timeout period specified. ";

static char bad_command[]  = "Invalid command.  Please try again.  ";

static char no_terminator[]  = "Please remember to terminate your entry \
with the pound key.  Command accepted.  ";

static char menu_prompt[]  = "Please enter a command.  For help, press key 0 \
followed by the pound key.  To exit, press the star key.  ";

static char msg_help[]     = "To hear the current Boston weather forecast, \
press key 1.  To hear transportation information, press key 2.  To hear \
mortgage rate information, press key 3.  To hear the current ski conditions, \
press key 4.  To repeat this message, press key 0.  Terminate your entry with \
the pound key.  To exit, press the star key.  ";

static char msg_weather[]  = "Welcome to the Boston Area Weather Service.  \
April 1st.  Today will be a day [\"]more like early May.  Current downtown \
Boston temperature is 58 degrees Fahrenheit, 14 Celsius.  It will be a \
[\"]beautiful sunny day, breezy and mild, with a high of 76 degrees \
Fahrenheit.  Humidity will be 76 per cent.  Barometric pressure is currently \
32.5.  Tomorrow will be cooler, with a high of fifty on the coast and \
sixty inland.  Fair weather is expected to continue throughout the week.  " ;
		
static char msg_MBTA[]     = "Welcome to the MBTA Information Line.  All \
MBTA lines are on time.  The B and M line from the North Shore is running a \
limited service, with trains running from Ipswich and Rockport, at 8 A.M. \
and 9 A.M. only.  There will be no red line service between Park Street and \
Harvard, Monday from 9 P.M. to 1 [`ey*'ehm].  Substitute bus transportation \
will be provided.  ";

static char msg_mortgage[] = "Welcome to Hamden National Bank's Mortgage \
Line.  All of the following rates are subject to change.  The application fee \
is $250.  30 year rate with 10 % [']down are 9.9 % with 3 [aen ax] half \
points, 10.2 % with 3 points, 10.5 % with 2 [aen ax] half points, 10.8 % with \
no points.  15 year rates with 10 % [']down are 9.9 % with 2 [aen ax] half \
points, 10.1 % with 2 points, and 10.5% with no points.  Adjustable rates are \
at 8.5 % fixed for 3 years with 2 % a year and 6 % lifetime caps thereafter.  ";

static char msg_ski[]      = "Welcome to the New England Ski Report.  \
January 31st.  Skiing is excellent in Vermont because of yesterday's snowfall \
of 10 inches.  All ski trails are open at Stowe, Sugar-bush, Mt. Snow, \
Stratton, Jay Peak, and Smuggler's Notch, with packed powder and full \
snow-making in operation.  All cross country ski trails are also open, with \
many groomed trails.  Skiing in Maine and [nuw] Hampshire is good, with cold \
temperatures allowing for constant snow-making.  Gun-stock has 15 [aatax] 25 \
trails open, Mt. Cranmore has 15 [aatax] 20 trails open, Attitash and Wildcat \
have all trails open.  Sugar-loaf and Sunday River also have all trails open, \
with packed powder and loose granular.  Expected snowfall in Maine and [nuw] \
Hampshire should improve weekend ski conditions.  ";

/*
 *  External Run-time Library Routines (DECtalk support)
 */
extern int	DTK$ANSWER_PHONE();	/* Wait for phone to ring & ans */
extern int	DTK$HANGUP();		/* Hangup the telephone		*/
extern int	DTK$INITIALIZE();	/* Initialize DECtalk device	*/
extern int	DTK$LOAD_DICTIONARY();	/* Load word in user dictionary	*/
extern int	DTK$READ_KEYSTROKE();	/* Reads a single keypad entry	*/
extern int	DTK$READ_STRING();	/* Read series of keypad entries*/
extern int	DTK$SET_MODE();		/* Set mode settings for DECtalk*/
extern int	DTK$SET_SPEECH_MODE();	/* Stop or start DECtalk speech	*/
extern int	DTK$SET_VOICE();	/* Change DECtalk voice chars.	*/
extern int	DTK$SPEAK_TEXT();	/* DECtalk speaks specified text*/
extern int	DTK$TERMINATE();	/* End initialized DECtalk use	*/

/*
 *  Run-time Library Routines (LIB$) and system service routines (SYS$).
 */
extern int	LIB$SIGNAL();		/* Indicate exception condition	*/
extern int      LIB$GET_FOREIGN();	/* Gets foreign command line	*/
extern int	SYS$CANTIM();		/* Cancel SYS$SETIMR request	*/
extern int	SYS$SETIMR();		/* Schedule AST at future time	*/

main(argc, argv)
int		argc;			/* Number of command line args	*/
char		*argv[];		/* Command line arguments	*/
{
    unsigned long 	vid;		/* Voice I.D. of DECtalk device	*/
    unsigned long       stat;		/* Status of RTL calls		*/
    long 	        timeout;	/* Waiting time for phone answer*/
    long		watch_timeout;  /* Watchdog timer for phone ans	*/
    long 		version;	/* DECtalk I or DECtalk III	*/

    /*
     *  Get the name of the physical device that the DECtalk module is
     *  connected to and the name of the dictionary file (if specified)
     *  that is used to load DECtalk's user loadable dictionary.  After
     *  the device name has been retrieved, create a string descriptor
     *  with this device name and invoke the RTL routine that initializes
     *  the communication channel between the DECtalk and the physical
     *  device.  If the call to DTK$INITIALIZE is unsuccessful, then exit.
     */
    get_param(device, dict_file);	/* Get device & dictionary file	*/
    make_sdesc(&s1desc, device);    
    set_timer(T30SECOND);		/* Set system (watchdog) timer	*/
    stat = DTK$INITIALIZE(&vid, &s1desc, &version);
    cancel_timer();			/* Cancel system timer		*/	
    if (!(stat & STS$M_SUCCESS)) {
	sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
	log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
    timeout = T15MINUTE;		/* Set answer timeout to 15 min	*/
    stat = SS$_NORMAL;
    while (stat == SS$_NORMAL || stat == SS$_TIMEOUT) {		
        /*
         *  Main loop of the demonstration program.
         *  First, check to see if the application specific parameters of
         *  the DECtalk need to be initialized ((init_dtk & APPL_INIT) = 0).
         *  If so, reinitialize them by calling init_dectalk().  Next, set 
         *  a watchdog timer for the DTK$ANSWER_PHONE command.  Then, enable the
         *  DECtalk for autoanswer (by calling DTK$ANSWER_PHONE).  If the
         *  phone does not ring in the timeout period specified, (or the 
         *  watchdog timer expires), reinitialize the application specific 
         *  parameters of DECtalk.  Normally, these parameters would not be 
         *  reinitialized everytime a phone call has not been received, but
         *  ONLY when the DECtalk module had power cycled.  However, in the
         *  current version of the VMS Run-Time Library Support for DECtalk
         *  (Version 4.5), it is not possible to check the status of DECtalk
         *  (detect if it has power cycled).  If a phone call is received 
         *  within the timeout period, the phone is answered, autostop keypad
         *  mode and wink detection are enabled, and a greeting message is
         *  spoken to the user.  Next, the user must be verified as a valid
         *  user of the system.  As soon as the user has successfully gained
         *  access to the system, he can start entering main menu commands.
         *  The code in this loop is continuely executed until a fatal error
         *  occurs or the process is stopped.
         */
	if ((init_dtk & APPL_INIT) == 0) {
            if (!init_dectalk(vid)) {	/* Reinitialize DECtalk params	*/
		/*
		 *  Initialization failed.  It appears that
		 *  the DECtalk module may be dead so exit
		 */
		break;			/* End the program		*/
	    }
	    init_dtk |= APPL_INIT;	/* Appl. initialization complete*/
	}	
        /*  
         *  Set the watchdog timer and enable DECtalk for autoanswer.
         *  The watchdog timer is set by calling the subroutine "set_timer()".
         *  The watchdog timer is always set to a value 30 seconds larger than 
         *  than the timeout parameter passed to the DTK$ANSWER_PHONE RTL 
         *  routine ("timeout").  To change the amount of time DECtalk waits for
         *  a phone call, the value of the parameter "timeout" passed to 
         *  DTK$ANSWER_PHONE must be modified.  The greeting message DECtalk
         *  speaks upon answering the telephone can be modified by changing the
         *  text in the static character string "msg_welcome".  The number of
         *  rings DECtalk waits to answer the telephone ("nrings"), can be
         *  changed but it is recommended that the phone is always answered
         *  on the first ring.
         */
        watch_timeout = timeout + T30SECOND;
 	set_timer(watch_timeout);	/* Set system (watchdog) timer	*/
        make_sdesc(&s1desc, msg_welcome);
        stat = DTK$ANSWER_PHONE(&vid, &nrings, &s1desc, &timeout);
        if (stat == SS$_NORMAL) {	/* Timeout period expired	*/
	    /*
	     *  The telephone has been answered so cancel the system
	     *  timer.  Then, verify that the caller is a valid user 
             *  of the system.  If the caller fails to enter a valid 
             *  access code and password in three attempts, access to
             *  the system is denied.  A warning message is spoken to
             *  the user and the current phone call is ended by invoking
             *  "end_call()".
	     *
	     *  NOTE: DTK$_ANSWER_PHONE automatically enables autostop keypad
	     *	      mode, and wink detection on the telephone keypad.  This
	     *        may not be suitable for all applications.  See note in
	     *        the initial header comment for disabling autostop keypad
             *        mode and ignoring winks received on the phone line.
             */				
            cancel_timer();		/* Cancel the system timer	*/
            if (!verify_user(vid)) {	/* Deny access and hangup	*/
		speak_text(vid, DTK$K_WAIT, msg_noaccess);		
                end_call(vid, msg_goodbye);	  
            }
            else {			
		/*
		 *  The user has successfully gained access to the system.
                 *  Start processing commands from the user.  Note, the 
                 *  menu prompt spoken prior to receiving commands from the
                 *  user, can be modified by changing the text in the static
                 *  character string "menu_prompt"
		 */
        	menu(vid, menu_prompt);
                end_call(vid, msg_goodbye);	  
    	    }
	}
        else if (stat == SS$_TIMEOUT) {	/* Timeout period expired	*/
	    init_dtk &= ~APPL_INIT;	/* Have to reinitialize		*/
	}
        else {
            cancel_timer();		/* Cancel the system timer	*/
	    sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
	    log_error(errbuf);		/* Log error to operator term.	*/
	    exit(stat);			/* Fatal error so exit		*/
	}
    }
    /*
     *  Operator specified to end the current application process.  
     */
    set_timer(T30SECOND);
    stat = DTK$TERMINATE(&vid);         /* End use of DECtalk device	*/
    cancel_timer();
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
}


/*
 *  Sends an error message to the operator (console) terminal
 *  using the $SNDOPR system service.  Note, this routine only
 *  sends a message to an operator terminal that has specified
 *  the OPER11 type of message.  Other types of messages can be
 *  specified by changing the value of "msg.opc$b_ms_target"
 */
int
log_error(buf)
char	*buf;					/* Text to log to oper	*/
{
    unsigned long	stat;			/* System service status*/
    OPER_REQ		msg;			/* Msg to send to oper  */
    
    msg.opc$b_ms_type = OPC$_RQ_RQST;		/* Request to operator	*/
    msg.opc$b_ms_target =  OPC$M_NM_OPER11;	/* Oper11 type message	*/
    sprintf(msg.opc$l_ms_text, "%s", buf);	/* Text to output	*/
    make_opr_sdesc(&s1desc, &msg);
    stat = SYS$SNDOPR(&s1desc, 0);	
    if (stat != SS$_NORMAL) {			/* Send to oper failed	*/
	exit(stat);				/* Fatal error so exit	*/
    }
}

	
/*
 *  Gets the parameters from the foreign command string that 
 *  invoked the program.  If a fatal error occurs, it is reported
 *  and the demo is exitted.  Otherwise, the parameters specified are
 *  returned in "p1name" and "p2name".
 */
int
get_param(p1name, p2name)
char			*p1name;	/* Param 1 from command line	*/
char			*p2name;	/* Param 2 from command line	*/
{
    unsigned long	stat;		/* Status from RTL calls	*/
    short int		len;		/* Actual length of device name	*/
    short int		outlen;		/* Length of command string	*/
    char   		cmd[MAXSIZE];	/* Foreign command line		*/
    register char	*tp;

    make_vdesc(&vdesc, cmd, MAXSIZE);  /* String desc for parameter	*/
    /*
     *  Get contents of the foreign command 
     *  line that activated the image
     */
    stat = LIB$GET_FOREIGN(&vdesc, NULL, &outlen, NULL);
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
    /*
     *  Get the command string from the variable string descriptor.  First,
     *  get the length of the parameter from the first two bytes of the
     *  string.  Then, recopy the parameter to the beginning of the buffer.
     */
    len = (short int)vdesc.dsc$a_pointer[0];
    len += (short int)(vdesc.dsc$a_pointer[1] << 8);
    /*
     *  Extract the first parameter from the foreign command line
     */
    for (tp = vdesc.dsc$a_pointer+2; tp < vdesc.dsc$a_pointer+(len+2); ) {
        if ((isspace(*tp)) || (*tp == NULL))
    	    break;			/* At end of first parameter	*/
	else
	    *p1name++ = *tp++;	
    }
    *p1name = NULL;			/* Terminate first parameter	*/
    /*
     *  Skip over any intermediate whitespace
     */
    while (tp < vdesc.dsc$a_pointer+(len+2)) {
        if (!isspace(*tp))
            break;			
	else
	    tp++;
    }
    /*
     *  Extract the second parameter from the foreign command line
     */
    while (tp < vdesc.dsc$a_pointer+(len+2)) {
        if ((isspace(*tp)) || (*tp == NULL))
	    break;			/* At end of second parameter	*/
        else
	    *p2name++ = *tp++;	
    }
    *p2name = NULL;			/* Terminate second parameter	*/
}

/*
 *  Initializes the application specific parameters of DECtalk.
 *  First, left square bracket ('[') and right square bracket (']') 
 *  are enabled as phonemic delimiters.  All other modes are reset.
 *  To specify other modes to be set, the bit masks for the modes to 
 *  be set should be OR'd together with the DTK$M_SQUARE bit mask and
 *  assigned to the static varible "mode".  Next, the default speaking 
 *  voice and speaking rate are selected for the application.  The comma
 *  pause and period pause are set to DECtalk defaults.  Other voices
 *  can selected for the default speaking voice by modifying the static
 *  variable "voice".  Likewise, a different speaking rate can be specified
 *  by changing the static variable "rate".  Finally, the user dictionary
 *  is loaded (by invoking "load_dict()").  Note, other application specific
 *  parameters should also be initialized in this subroutine if the need arises.
 */
int
init_dectalk(vid)
unsigned long		vid;		/* DECtalk voice identifier	*/
{
    unsigned long	stat;		/* Status from RTL calls	*/

    /*
     *  Set square bracket mode on.  All other modes are reset.
     */
    set_timer(T30SECOND);
    stat = DTK$SET_MODE(&vid, &mode);
    cancel_timer();
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
    /*
     *  Select type of voice and speaking rate for DECtalk.
     */
    set_timer(T30SECOND);
    stat = DTK$SET_VOICE(&vid, &voice, &rate, &cp, &pp);
    cancel_timer();
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
    return(load_dict(vid));		/* Load the user dictionary	*/
}


/*
 *  Verifies that the caller is a valid user of the system.
 *  The caller is given three attempts to enter a valid access code and
 *  three attempts to enter his password.  If the caller fails to enter
 *  a valid access code or a valid password in the timeout period specified,
 *  FALSE is returned.  Otherwise, TRUE is returned.
 *
 *  NOTE: In this demonstration, almost all access codes and passwords
 *	  are detected as valid.  The only ways an access code or a 
 *        password are rejected are if the user fails to enter an access
 *        code or a password in the time period specified, or if the user
 *        terminates his access code or password with the star key ("*")
 *        rather than the pound key ("#").  All access codes and passwords
 *        that are entered correctly (terminated with the pound key)
 *        are verified by calling a dummy access code verification routine
 *        "access_verify()" and a dummy password verification routine
 *        "password_verify()".  Both of these routines always return TRUE.
 */
int
verify_user(vid)
unsigned long		vid;		/* Voice I.D. of DECtalk device	*/
{
    long 	        illegal_entry;	/* Number of illegal entries	*/
    long 		term_code;	/* Key string terminator code	*/
    char                acode[MAXKEYS]; /* Access code array		*/
    char	        pcode[MAXKEYS]; /* Passcode array		*/

    illegal_entry = 0;			/* No bad attempts yet		*/
    do {
        /*
         *  Gets the user's access code entered on the touch tone keypad.
         *  The entered access code is returned in the character buffer 
         *  "acode".  Currently, the maximum size of the buffer is 80 
         *  characters.  If a larger or smaller buffer is required, the
         *  definition of MAXKEYS should be changed.  The user is prompted
         *  for his access code by the text specified in "msg_access".
         *  To have a different prompt spoken, the static character string
         *  "msg_access" should be modified.  Currently, the application
         *  waits 30 seconds for a touch tone key to be entered.  If a longer
         *  or shorter timeout period is desired, the new timeout value (in 
         *  seconds) should be passed to "get_key_string" instead of
         *  "T30SECOND".  The final parameter, "term_code" will contain the
         *  character used to terminate the key string or a timeout upon
         *  return from the "get_key_string" routine.     
         */
        if (get_key_string(vid,acode,MAXKEYS,msg_access,T30SECOND,&term_code)) {
            /*
             *  Check if an invalid key string termination character ("*")
             *  was entered, if the key string was not entered in the timeout
             *  period specified or if an invalid access code was entered.
	     *  If any of these conditions occured, increment the count of
             *  invalid access code entry attempts.  If this count is greater
             *  than the maximum allowed (specified by MAX_ENTRY), return FALSE.
             *  Otherwise, notify the user and prompt him again.  If a valid
             *  access code is entered, then prompt the user for his password.
             */
            if (term_code==DTK$K_TRM_ASTERISK || term_code==DTK$K_TRM_TIMEOUT 
			                  || (access_verify(acode) == FALSE)) { 
	        if (++illegal_entry >= MAX_ENTRY) 
		    return(FALSE);		/* 3 strikes you're out	*/
		else {
		    speak_text(vid, DTK$K_WAIT, msg_invalid);
		}
	    }
  	    else 
	        break;			/* Access code is valid		*/
	}
	else
	    return(FALSE);		/* No key string received	*/
    } while (illegal_entry < MAX_ENTRY);
    /*
     *  Access code has been entered correctly. 
     *  Now prompt the user for his password.
     */
    illegal_entry = 0;			/* No bad attempts yet		*/
    do {
        /*
         *  Gets the user's password entered on the touch tone keypad.
         *  The entered password is returned in the character buffer 
         *  "pcode".  Currently, the maximum size of the buffer is 80 
         *  characters.  If a larger or smaller buffer is required, the
         *  definition of MAXKEYS should be changed.  The user is prompted
         *  for his password by the text specified in "msg_password".
         *  To have a different prompt spoken, the static character string
         *  "msg_password" should be modified.  Currently, the application
         *  waits 30 seconds for a touch tone key to be entered.  If a longer
         *  or shorter timeout period is desired, the new timeout value (in 
         *  seconds) should be passed to "get_key_string" instead of
         *  "T30SECOND".  The final parameter, "term_code" will contain the
         *  character used to terminate the key string or timeout upon
         *  return from the "get_key_string" routine.     
         */
        if (get_key_string(vid,pcode,MAXKEYS,msg_password,T30SECOND,&term_code)) {
            /*
             *  Check if an invalid key string termination character ("*")
             *  was entered, if the key string was not entered in the timeout
             *  period specified or if an invalid password was entered.
	     *  If any of these conditions occured, increment the count of
             *  invalid password entry attempts.  If this count is greater
             *  than the maximum allowed (specified by MAX_ENTRY), return FALSE.
             *  Otherwise, notify the user and prompt him again.  If a valid
             *  password is entered, then return TRUE.
             */
            if (term_code==DTK$K_TRM_ASTERISK || term_code==DTK$K_TRM_TIMEOUT 
           			        || (password_verify(pcode) == FALSE)) { 
	        if (++illegal_entry >= MAX_ENTRY) 
		    return(FALSE);		/* 3 strikes you're out	*/
		else {
		    speak_text(vid, DTK$K_WAIT, msg_invalid);
		}
	    }
  	    else 
	        break;			/* Password is valid		*/
	}
	else
	    return(FALSE);		/* No key string received	*/
    } while (illegal_entry < MAX_ENTRY);
    return(TRUE);
}

/* 
 *  Gets a string of touch tone keys entered on the telephone keypad
 *  terminated by the pound key "#" or the star key "*".
 *  Returns TRUE if the string of touch tone keys was received successfully.
 *  Otherwise, FALSE is returned.
 */
int
get_key_string(vid, keybuf, buflen, prompt, timeout, term_code)
unsigned long	vid;			/* Voice identifier for DECtalk	*/
char	        *keybuf;		/* Buffer for keypad input	*/
long    	buflen;			/* Maximum length of key buffer	*/
char		*prompt;		/* Prompt spoken prior to input	*/
long    	timeout;		/* # of secs to wait for input	*/
long            *term_code;		/* Terminating code - key input */
{
    unsigned long	stat;		/* Status code from RTL call	*/
    long		keycode;	/* Code for touch tone key	*/
    long		watch_time;	/* Watch dog timer value	*/
    int			len;		/* Buffer len before terminator */
    char		*ttkp;		/* Touch tone key buffer pointer*/

    stat = SS$_NORMAL;			/* Status is initially o.k.	*/
    if (strlen(ttkeys) > 0) {		/* Keys in typeahead buffer ???	*/
	/*
	 *  There are Touch Tone keys in the typeahead buffer so 
	 *  check if there is a pound key "#" or an star 
         *  key "*" (both are key string terminators).  If so, return
	 *  the string of touch tone keys up to the "#" or "*" key.
	 */
	len = strcspn(ttkeys, term_set);
	if ((ttkeys[len] == DTK$K_TRM_ASTERISK) ||
            (ttkeys[len] == DTK$K_TRM_NUMBER_SIGN)) 
	    goto ret_string;		/* Found "#" or "*"		*/
    }
    else {				/* No keys in typeahead buffer	*/
	if (prompt != NULL) 		/* Prompt for a command		*/
            speak_text(vid, DTK$K_WAIT, prompt);
    }
    ttkp = &ttkeys[strlen(ttkeys)];	/* Pt to next available space	*/
    while (stat != SS$_TIMEOUT) {
	/*
	 *  Read all keys entered on the touch tone keypad by the user
	 *  and store them in the typeahead buffer "ttkeys".  First a 
	 *  watchdog timer must be set before any keys can be read.
         */
        watch_time = timeout + T30SECOND;
        set_timer(watch_time);		/* Set system (watchdog) timer	*/
	stat = DTK$READ_KEYSTROKE(&vid, &keycode, 0, &timeout);
        cancel_timer();
	if (stat == SS$_NORMAL) {	/* Received touch tone key	*/
	    if ((keycode == DTK$K_TRM_ASTERISK) || 
	 	(keycode == DTK$K_TRM_NUMBER_SIGN)) {
		/*
		 *  Received key string terminator ("#" or "*") so reception
		 *  of keystring is completed.  Use a short timeout to gather
		 *  up any remaining touch tone keys entered by the user.
		 */
		timeout = T2SECOND;
	    }
	    /*
	     *  If there is room in the typeahead buffer ("ttkeys"), store
	     *  the key.  Otherwise, indicate the buffer is full and stop
	     *  receiving keys.
	     */
	    if (ttkp < &ttkeys[MAXSIZE-1])
		*ttkp++ = keycode;
	    else {
	 	*term_code = DTK$K_TRM_BUFFER_FULL;
		break;
	    }
        }
        else if (stat == SS$_TIMEOUT)
	    break;			/* No keys available so exit	*/
	else if (stat == DTK$_WINK) {
	    /*
	     *  DECtalk detected a wink which sometimes indicates that the
	     *  user has hungup.  This demonstration program assumes that a
	     *  WINK does indicate the a user has hungup.  Therefore, return
	     *  so DECtalk can hangup its phone.  If for any reason it is
	     *  noticed that spontaneous winks are occurring and the user
	     *  at the other end of the telephone did not hangup, then DO NOT
	     *  return. Just continue receiving touch tone keys.
	     */
            sprintf(errbuf, "ERROR--DECtalk on %s detected a wink.\n", device);
            log_error(errbuf);		/* Log error to operator term.	*/
	    return(FALSE);
	}
        else {				/* FATAL error so exit program	*/
            sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
            log_error(errbuf);		/* Log error to operator term.	*/
	    exit(stat);
	}
    }
    *ttkp = NULL;			/* Terminate key string		*/
    if (strlen(ttkeys) <= 0) {		/* No keys received so exit	*/
	*term_code = DTK$K_TRM_TIMEOUT; 
	return(TRUE);
    }
    /*
     *  There are Touch Tone keys in the typeahead buffer so check if there
     *  is a pound key "#" or an star key "*" (both are string 
     *  terminators).  If so, return the keystring.
     */
    len = strcspn(ttkeys, term_set); 
ret_string:
    if ((ttkeys[len] == DTK$K_TRM_ASTERISK) || 
        (ttkeys[len] == DTK$K_TRM_NUMBER_SIGN)) {
        /*
         *  Got a keystring terminator ("#" or "*").  Check if the
	 *  keystring will fit in the buffer to be returned.  If not,
	 *  return as many keys as possible (buflen - 1) and set the
	 *  terminating keycode to DTK$K_TRM_TIMEOUT.  Otherwise, return
	 *  the entire keystring up to the terminating keycode and set the 
	 *  terminating code to the keycode entered ("#" or "*").
         */
        if (len >= (buflen - 1)) {	/* Can't return whole keystring	*/
	    len = buflen - 1;
	    *term_code = DTK$K_TRM_TIMEOUT;
	}
	else {				/* Can return whole keystring	*/
	    *term_code = ttkeys[len];	/* Return terminating character	*/
	}
        strncpy(keybuf, ttkeys, len);	/* Copy into string to return	*/
        keybuf[len] = NULL;		/* Terminate the string		*/
	/*
	 *  Recopy the remaining keys in the typeahead buffer (ttkeys) to
	 *  the beginning of the buffer.  If the terminating keycode was a
	 *  "#" or "*", skip over it and start recopying with the next keycode.
	 */
        if (*term_code != DTK$K_TRM_TIMEOUT) { 
	    len = len + 1;		/* "#" or "*" so skip over it	*/
        }
        strcpy(&ttkeys[0], &ttkeys[len]);
    }
    else {
        /*
         *  No keystring terminator ("#" or "*") entered so set the terminating
         *  keycode to DTK$K_TRM_TIMEOUT.  Check if the keystring will fit in
	 *  the buffer to be returned.  If not, return as many keys as possible
	 *  (buflen - 1).  Otherwise, return all touch tone keys entered. 
	 *  Finally, recopy any remaining touch tone keys in the typeahead
	 *  buffer (ttkeys) to the beginning of the buffer.
         */
        *term_code = DTK$K_TRM_TIMEOUT;
	if (strlen(ttkeys) < (buflen-1))
	    len = strlen(ttkeys);	/* Not larger than buffer	*/
	else
	    len = buflen - 1;		
        strncpy(keybuf, ttkeys, len);	/* String of keys to return	*/
	keybuf[len] = NULL;		/* Terminate string		*/
        strcpy(&ttkeys[0], &ttkeys[len]);
    }
    restart(vid);			/* Restart speech		*/
    return(TRUE);
}


/*
 *  Verifies the access code received from the customer.
 *  This routine always returns TRUE for the purpose of
 *  this demonstration.  In a real application, the code
 *  would be verified against the access code in the database.
 */
int
access_verify(acode)
register char	*acode;			/* Access code to verify	*/
{
    return(TRUE);
}


/*
 *  Verifies the password received from the customer.
 *  This routine always returns TRUE for the purpose of
 *  this demonstration.  In a real application, the code
 *  would be verified against the password in the database.
 */
int
password_verify(pcode)
register char	*pcode;			/* Password to verify		*/
{
    return(TRUE);
}


/*
 *  Prompts the user for a command and receives the command (touch tone
 *  key) from the user.  Note, the command will be accepted whether or not
 *  it is terminated with the pound key "#".  However, if the command
 *  is not followed by the pound key, the command will not be processed
 *  until the timeout period has expired and a warning message has been spoken.
 *  Returns TRUE if the exit key is received or if no response is received from
 *  the user in the timeout period specified.  Returns FALSE if a wink is
 *  detected by the DECtalk.
 */
int
menu(vid, prompt)
unsigned long		vid;		/* Voice identifier of DECtalk	*/
char			*prompt;	/* Prompt to speak		*/
{
    unsigned long	stat;		/* Get_key_string status return */
    long 	        term;		/* Command terminator code	*/
    long 		timeout;	/* Time to wait for a keystroke	*/
    long		len;		/* Length of typeahead buffer	*/
    char		buf[MAXKEYS];	/* Touch tone key buffer	*/

    bad_entry_count = 0;
    do {
	/*
	 *  Keep getting commands from the user until the exit key "*",
	 *  is entered, a wink is detected, or the timeout period expires.
	 *  First, check if there are any keys in the typeahead buffer.
	 *  If so, process them.  Otherwise, prompt the user for a command
	 *  and wait for his response.
	 */
	buf[0] = NULL;			/* No touch tone keys received	*/
        stat = get_key_string(vid, buf, MAXKEYS, NULL, T1SECOND, &term);
        if (stat == FALSE) 
	    return(FALSE);		/* Detected wink so exit	*/  
        if (term == DTK$K_TRM_ASTERISK) 
	    return(TRUE);		/* Exit key so exit menu	*/
	else if (term == DTK$K_TRM_NUMBER_SIGN) {
	    process_menu_entry(vid, buf);	/* Valid command so process it	*/
	    continue;
	}
	else if ((term != DTK$K_TRM_TIMEOUT) ||
	         (term == DTK$K_TRM_TIMEOUT && buf[0] == NULL)) {
	    /*
	     *  No keys in typeahead buffer so prompt for a command.
	     */
	    speak_text(vid, DTK$K_WAIT, prompt);
	}
	/*
	 *  Any new keys received should be processed after the keys read
	 *  from the typeahead buffer.  Start storing touch tone keys into
	 *  into the buffer following the keys read from the typeahead buffer.
	 */
	len = strlen(buf);		/* Get number of typeahead keys	*/
        stat=get_key_string(vid,&buf[len],(MAXKEYS-len),NULL,T20SECOND,&term);
        if (stat == FALSE) 
	    return(FALSE);		/* Detected wink so exit	*/
        if (term == DTK$K_TRM_ASTERISK)
	    return(TRUE);		/* Exit key so exit		*/
	else if (term == DTK$K_TRM_TIMEOUT && buf[0] != NULL) {
	    /*
	     *  Touch tone keys entered but no command terminator ("#")
	     *  entered.  Warn the user that commands must be terminated
	     *  by the pound key and then process the entered command.
	     */
	    speakall_text(vid, no_terminator);
	    process_menu_entry(vid, buf);
	}
        else if (term == DTK$K_TRM_TIMEOUT && buf[0] == NULL) {
	    /*
	     *  No touch tone keys received.  Inform the user and exit.
	     */
	    speakall_text(vid, msg_timeout);
	    return(TRUE);
	}
	else
	    process_menu_entry(vid, buf);	/* Command so process it	*/
    } while ((buf[0] != NULL || term != DTK$K_TRM_TIMEOUT));
    return(TRUE);
}

/*
 *  Processes the touch tone key string (pointed to by buf)
 *  received from the user.
 */
int
process_menu_entry(vid, buf)
unsigned long		vid;			/* DECtalk voice id.	*/
char			*buf;			/* Touch tone key buffer*/
{
    if (strlen(buf) > 1) {
	/*
	 *  Only single key commands are valid in this menu.  Inform
	 *  the user that an invalid command was entered and return.
	 */
        speakall_text(vid, bad_command);
	bad_entry_count++;
    }
    else if (buf[0]==DTK$K_TRM_ZERO) {	/* Speak help text for menu	*/
        speak_text(vid, DTK$K_WAIT, msg_help);
	bad_entry_count = 0;
    }
    else if (buf[0]==DTK$K_TRM_ONE) {	/* Give Boston weather forecast	*/
        speak_text(vid, DTK$K_WAIT, msg_weather); /* Speak weather info */
	bad_entry_count = 0;
    }
    else if (buf[0]==DTK$K_TRM_TWO) {	/* Give transportation info	*/
        speak_text(vid, DTK$K_WAIT, msg_MBTA);
	bad_entry_count = 0;
    }
    else if (buf[0]==DTK$K_TRM_THREE) {	/* Give mortgage rate info	*/
        speak_text(vid, DTK$K_WAIT, msg_mortgage);
	bad_entry_count = 0;
    }
    else if (buf[0]==DTK$K_TRM_FOUR) {	/* Give ski condition info	*/
	speak_text(vid, DTK$K_WAIT, msg_ski);
	bad_entry_count = 0;
    }
    else {				/* Invalid entry		*/
        speak_text(vid, DTK$K_WAIT, msg_invalid);
	bad_entry_count++;
    }

    if (bad_entry_count >= 3) {
        speak_text(vid, DTK$K_WAIT, msg_help);
	bad_entry_count = 0;
    }
}


/*
 *  Load the user dictionary with the words and phonemic pronunciations
 *  stored in the sequential file specified in the foreign command line
 *  invoking the program.  Each line of this file contains the word to be
 *  defined in the user dictionary followed by a space, followed by the
 *  phonemic pronunciation of the word.  As each line is read from the file,
 *  the word and phonemic pronunciation are stored in separate arrays.  Then,
 *  the word and phonemic pronunciation are loaded into the user dictionary.
 *  If the load dictionary command fails, the user is notified and processing
 *  is terminated.  Returns TRUE if the dictionary is loaded successfully or
 *  if no user dictionary file name is specified.  Otherwise, FALSE is returned.
 */
int
load_dict(vid)
unsigned long		vid;		/* Voice identifier for DECtalk	*/
{
    unsigned long	stat;		/* Status of RTL call		*/
    char	        word[MAXSIZE];	/* Word to be defined		*/
    char		defn[MAXSIZE];	/* Susstitution (defn) for word	*/
    char	        buf[MAXSIZE];	/* Entry has 256 char maximum	*/
    char	        *bp;		/* Ptr to word/subtitution pair	*/
    char	        *tp;		/* Temporary pointer		*/
    FILE		*fldp;		/* File ptr to word/definitions	*/

    if (dict_file[0] == NULL)
	return(TRUE);			/* No dictionary file specified	*/
    if ((fldp = fopen(dict_file, "r")) == NULL) {
        sprintf(errbuf, "Could not open file %s\n", dict_file);
        log_error(errbuf);		/* Log error to operator term.	*/
        return(FALSE);
    }
    /*
     *  Since there is no timeout associated with the DTK$LOAD_DICTIONARY 
     *  command, it is possible for an application to hang waiting for a
     *  response after issuing the DTK$LOAD_DICTIONARY command.  Therefore,
     *  arm a watchdog timer (5 minutes in this case) to time the loading of
     *  the entire user dictionary.  If all the entries are not loaded into 
     *  the user dictionary before the timer expires, then assume that
     *  something is wrong with the DECtalk module and exit.
     */
    set_timer(T5MINUTE);		/* Set system (watchdog) timer	*/
    while ((fgets(buf, MAXSIZE, fldp)) != NULL) {
	/* 
	 *  Read in all of the words and substitutions from the sequential
	 *  file specified in the command string.  First, get the word from
	 *  the word/substitution pair and store it in the character buffer
	 *  "word".  Then, get the phonemic pronunciation for the word and
         *  store it in the character buffer "defn".
	 */
	for (bp = &buf[0], tp = &word[0]; bp < &buf[MAXSIZE]; bp++) {
	    if ((isspace(*bp)) || (*bp == NULL))
		break;			/* At end of word to define	*/
	    else
	        *tp++ = *bp;		/* Save character of the word	*/
	}
	*tp = NULL;			/* Terminate word to define	*/
	while (isspace(*++bp))
	    ;				/* Skip over any white space	*/
	/*
	 *  Now get the phonemic substitution for the word.
	 */
	for (tp = &defn[0]; bp < &buf[MAXSIZE]; bp++) {
	    if (*bp == CR || *bp == LF) /* Skip over carriage returns	*/
		continue;		/* and linefeeds		*/
	    else if (*bp == NULL)
		break;			/* At end of substitution text	*/
	    else
		*tp++ = *bp;		/* Save character of phonemics	*/
	}
	*tp = NULL;			/* Terminate phonemics		*/
        /*
         *  Create the string descriptors for the word and its phonemic
	 *  pronunciation and then load the word into the user dictionary.
 	 */
	make_sdesc(&s1desc, word);	/* String descriptor for word	*/
	make_sdesc(&s2desc, defn);	/* String descriptor for defn	*/
        stat = DTK$LOAD_DICTIONARY(&vid, &s1desc, &s2desc);
	if (stat == DTK$_TOOLONG || stat == DTK$_NOROOM) {
	    /*
             *  Dictionary entry too long or no room in user dictionary.
	     *  These are not generally fatal errors.  However, in this
             *  demonstration program, they are treated as such.  In creating
             *  a demo program, if words cannot be loaded into the user
             *  dictionary, the programmer should be notified so that the
             *  appropriate corrections can be made.  By treating these 
             *  errors as fatal, the programmer will at least know what words 
             *  will be mispronounced.
 	     */
	    cancel_timer();
	    sprintf(errbuf, "ERROR -- Loading user dictionary.  \n");
            log_error(errbuf);		/* Log error to operator term.	*/
	    sprintf(errbuf, "WORD: %s\n", word);
            log_error(errbuf);		/* Log error to operator term.	*/
	    sprintf(errbuf, "PHONEMIC SUBSTITUTION: %s\n", defn);
            log_error(errbuf);		/* Log error to operator term.	*/
	    fclose(fldp);
	    sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
            log_error(errbuf);		/* Log error to operator term.	*/
	    exit(stat);				/* Fatal error so exit	*/
	}
	else if (stat != SS$_NORMAL) {
	    cancel_timer();
	    fclose(fldp);
            sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
            log_error(errbuf);		/* Log error to operator term.	*/
	    exit(stat);				/* Fatal error so exit	*/
        }
    }
    /*
     *  Entire dictionary is loaded successfully.
     *  Cancel the watchdog timer and close the dictionary file.
     */
    cancel_timer();			/* Cancel system(watchdog) timer*/
    fclose(fldp);			/* Close dictionary file	*/
    return(TRUE);
}

/*
 *  DECtalk stopped speaking because it was in autostop keypad mode when
 *  it received a Touch Tone Key from the user.  First, send DECtalk a 
 *  right square bracket "]" just in case speech was stopped in phonemic
 *  send DECtalk a right sqaure bracket (]) just in case speech was stopped 
 *  mode.  Then, restart speech and reset the speaking voice and rate.
 */
int
restart(vid)
unsigned long		vid;		/* Voice ID for DECtalk		*/
{
    unsigned long	stat;		/* Status of RTL call		*/
    unsigned long	old_mode;	/* Current mode before reset	*/

    /*
     *  Send right square bracket to exit phonemic mode.
     */
    speak_text(vid, DTK$K_WAIT, rsbracket);
    /*
     *  Set speaking on
     */
    set_timer(T30SECOND);
    stat = DTK$SET_SPEECH_MODE(&vid, &speechon, &old_mode);
    cancel_timer();
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
    /*
     *  Reset the voice and speaking rate
     */
    set_timer(T30SECOND);
    stat = DTK$SET_VOICE(&vid, &voice, &rate, &cp, &pp);
    cancel_timer();
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
}

/*
 *  End the current user session.  Since the DTK$HANGUP_PHONE command
 *  does not set a timeout, and it requests DECtalk to send a phone
 *  status, a watchdog timer is set to insure that the application does
 *  not hang (if DECtalk fails).  If a longer timeout period is needed,
 *  change the value of the parameter passed to "set_timer()".  After the
 *  watchdog timer is set, speak a goodbye message to the caller and then
 *  hangup the phone.  The goodbye message spoken can be changed by passing
 *  a different prompt as the second parameter in the call to "end_call()"
 */
int
end_call(vid, prompt)
unsigned long		vid;		/* Voice id for DECtalk channel	*/
char		        *prompt;	/* Prompt to speak		*/
{
    unsigned long	stat;		/* Status of RTL call		*/
    long		temp;
    long		timeout;

    make_sdesc(&s1desc, prompt);
    set_timer(T5MINUTE);		/* Set system (watchdog) timer	*/
    /*
     *  Say goodbye and hangup the phone
     */
    stat = DTK$HANGUP_PHONE(&vid, &s1desc);
    cancel_timer();			/* Cancel system timer		*/
    if (!(stat & STS$M_SUCCESS)) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
    /*
     *  Get rid of any winks that may have been generated by the 
     *  caller by hanging up in the middle of the goodbye message.
     *  This workaround clears the internal WINK flag so that winks
     *  are not propagated to the next phone call.
     */
    timeout = 1;
    for (;;) {
        set_timer(T5MINUTE);		/* Set system (watchdog) timer	*/
	stat = DTK$READ_KEYSTROKE(&vid, &temp, 0, &timeout);
	cancel_timer();			/* Cancel system timer		*/
	if (stat == DTK$_ONHOOK || stat == SS$_TIMEOUT) {
	    /*
	     *  Clear remaining winks that may have been 
	     *  detected before DECtalk hung up the telephone.
	     */
            set_timer(T30SECOND);	/* Set system (watchdog) timer	*/
	    stat = DTK$READ_KEYSTROKE(&vid, &temp, 0, &timeout);
	    cancel_timer();		/* Cancel system timer		*/
	    if (!(stat & STS$M_SUCCESS)) {
		sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
		log_error(errbuf);	/* Log error to operator term.	*/
		exit(stat);		/* Fatal error so exit		*/
	    }
	    break;
	}
	else if (!(stat & STS$M_SUCCESS)) {
	    sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
	    log_error(errbuf);	    /* Log error to operator term.	*/
	    exit(stat);		    /* Fatal error so exit		*/
	}
    }
}


/*
 *  Sends the prompt specified by parameter "prompt" to the DECtalk to
 *  be spoken.  However, before the prompt is sent to the DECtalk, 
 *  autostop keypad mode is disabled (if it is enabled) so that it is
 *  guarenteed that the user hears the entire prompt.  Once the prompt
 *  has been spoken, autostop keypad mode is re-enabled.  
 */
int
speakall_text(vid, prompt)
unsigned long	vid;			/* DECtalk voice identifier	*/
char		*prompt;		/* Text to speak		*/
{
    unsigned long	kmode;		/* Keypad mode to set		*/
    unsigned long	stat;		/* Status code from RTL call	*/

    /*
     *  If autostop keypad mode is enabled (keymode = DTK$K_KEYPAD_AUTO), 
     *  then enable the keypad without autostop mode.
     */
    if (keymode == DTK$K_KEYPAD_AUTO) {	/* Keypad is in autostop mode	*/
	kmode = DTK$K_KEYPAD_ON;	/* Just enable "normal" keypad	*/
	set_timer(T30SECOND);		/* Set the watchdog timer	*/
	stat = DTK$SET_KEYPAD_MODE(&vid, &kmode);
	cancel_timer();			/* Cancel the watchdog timer	*/
        if (!(stat & STS$M_SUCCESS)) {
            sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
            log_error(errbuf);		/* Log error to operator term.	*/
            exit(stat);
        }
    }
    /*
     *  Send text to DECtalk to be spoken.  
     *  Specify the mode as DTK$K_WAIT so that text is completely
     *  spoken before the keypad is re-enabled in autostop mode.
     */
    speak_text(vid, DTK$K_WAIT, prompt);
    /*
     *  If autostop keypad mode was enabled
     *  (keymode = DTK$K_KEYPAD_AUTO), then re-enable it.
     */
    if (keymode == DTK$K_KEYPAD_AUTO) {	/* Keypad was in autostop mode	*/
	kmode = DTK$K_KEYPAD_AUTO;
	set_timer(T30SECOND);		/* Set watchdog timer		*/
	stat = DTK$SET_KEYPAD_MODE(&vid, &kmode);
	cancel_timer();			/* Cancel the watchdog timer	*/
        if (!(stat & STS$M_SUCCESS)) {
            sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
            log_error(errbuf);		/* Log error to operator term.	*/
            exit(stat);
        }
    }
}


/*
 *  Sends the prompt specified by parameter "prompt" to the DECtalk to be
 *  spoken.  If an error occurs, a warning message is displayed on the
 *  console terminal.
 *  NOTE: The watchdog timer (set by calling "set_timer()") is set to a 
 *        fairly large value (15 minutes) because there is no accurate
 *        way to detemine how long it will take to speak the prompt.
 */
int
speak_text(vid, mode, prompt)
unsigned long	vid;			/* DECtalk voice identifier	*/
unsigned long	mode;			/* When to return from speaking	*/
char		*prompt;		/* Text to speak		*/
{
    unsigned long	stat;
    long        	timer;

    make_sdesc(&s1desc, prompt);
    set_timer(T15MINUTE);		/* Set system (watchdog) timer	*/
    stat = DTK$SPEAK_TEXT(&vid, &s1desc, &mode);
    cancel_timer();			/* Cancel system timer		*/	
    if (stat != SS$_NORMAL) {
        sprintf(errbuf, "FATAL ERROR %d on %s\n", stat, device);
        log_error(errbuf);		/* Log error to operator term.	*/
        exit(stat);			/* Fatal error so exit		*/
    }
}


/*
 *  Sets the system (watchdog) timer to expire "sec" seconds in the future
 *  by invoking the "SYS$SETIMR" system service.  The time passed to 
 *  "SYS$SETIMR" is expressed in a unique  64 bit format.  It is a binary
 *  number expressed in 100 nanosecond units offset from the system base 
 *  date and time.  To represent a time value in terms of 100 nanosecond
 *  units, use the conversion:
 *
 *          1 second = 10,000,000 * 100 nanoseconds
 *
 *  The time must also be specified as a delta time, that is, it is an
 *  offset from the current time to a time in the future.  Delta times 
 *  are always expressed as negative values.
 */
int
set_timer(sec)
long    	sec;			/* Number of seconds to wait	*/
{		
			
    unsigned long	stat;
    long 		delta[2];
    long 		op1[2];

    /*
     *  Check if time to wait is more than 86400 seconds (24 hours).  If so, 
     *  print an error message on the screen and exit.  Otherwise, convert 
     *  the number of seconds to a delta time to be passed to the system
     *  service "SYS$SETIMR"
     */
    if (sec > T24HOUR) {		/* Timeout period > 24 hours	*/
	sprintf(errbuf, "ERROR: Timer specified greater than 24 hours\n");
        log_error(errbuf);		/* Log error to operator term.	*/
        exit(SS$_ABORT);
    }
    /*
     *  Convert the number of seconds (passed as parameter "sec")
     *  to a delta time.  In VAX C, the maximum size integer that
     *  arithmetic can be performed on is 32 bits long.  With this
     *  limitation, the maximum timeout period that could be allowed
     *  is 429 seconds (a little more than 7 minutes).  Since the time
     *  passed to the "SYS$SETIMR" routine is specified in a 64 bit 
     *  format, it is possible to specify longer timeout periods (up
     *  to 24 hours for this demonstration program).  However, to compute
     *  the delta time is a bit more complicated.  The algorithm used
     *  loops continuously until the time specified in seconds ("sec")
     *  is less than or equal to zero.  If the time specified in seconds
     *  ("sec") is less than or equal to 429 seconds (that is, it can
     *  be expressed in a longword), convert this time to 100 nanosecond
     *  units by multiplying the number of seconds ("sec") by 10 million.
     *  Then, subtract 429 from "sec".  Finally add the converted time 
     *  value to the computed delta time ("delta").  If the time specified
     *  in seconds ("sec") is larger than 429, multiply 429 by 10 million
     *  to convert time to 100 nanosecond units.  Again, 429 is subtracted
     *  from "sec" and the converted time value is added to the computed
     *  delta time "delta".  Note, the additions are performed using
     *  LIB$ADDX since the two values being added are quadwords (64 bits long).
     */
    delta[0] = 0;			/* Initialize converted time	*/
    delta[1] = 0;		
    while (sec > 0) {			/*  Convert "seconds" to delta	*/
	if (sec <= T429SECOND) {	/* Time * 10000000 fits in long	*/
	    /*
	     *  Value specified by "sec" will fit into longword after 
             *  conversion to 100 nanosecond units occurs. (That is, 
             *  after the multiplication by 10,000,000 is performed).
             */
	    op1[0] = sec * 10000000;	/* Converted value in low order	*/
	    op1[1] = 0;			/* Nothing in high order longwrd*/
	}
	else {	
	    /*
	     *  Value specified by "sec" will not fit into longword 
             *  after conversion to 100 nanosecond units occurs. So, 
             *  convert the maximum number of seconds that will fit
             *  in a longword (429 seconds).  
             */
	    op1[0] = T429SECOND * 10000000;   /* Max value in long word	*/
	    op1[1] = 0;
        }
        sec -= T429SECOND;		/* Decrement timeout by 429 secs*/
	/*
	 *  Add the converted time (in "op1") to the computed sum
         *  for the delta time ("delta").    Use LIB$ADDX to perform
         *  this addition because the sum of two quadwords (64 bit 
         *  integers) is being computed.
         */
        if ((stat = LIB$ADDX(&op1, &delta, &delta)) != SS$_NORMAL) {
            sprintf(errbuf, "ERROR failed computation of delta time\n");
            log_error(errbuf);		/* Log error to operator term.	*/
            exit(stat);
	}
    }
    op1[0] = op1[1] = 0;
    /*
     *  Delta times must be expressed as negative values.  Subtract the
     *  computed time (in variable "delta") from zero to obtain the actual
     *  delta time.  Use LIB$SUBX to perform the subtraction since the
     *  difference between two quadword (64 bit integers) must be found.
     */
    if ((stat = LIB$SUBX(&op1, &delta, &delta)) != SS$_NORMAL) {
	sprintf(errbuf, "ERROR: failed computation of delta time\n");
        log_error(errbuf);		/* Log error to operator term.	*/
        exit(stat);
    }
    stat = SYS$SETIMR(0, delta, &LIB$SIGNAL, SS$_TIMEOUT);
    if (!(stat & STS$M_SUCCESS)) {
	sprintf(errbuf, "ERROR: failed setting system (watchdog) timer\n");
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
}

/*
 *  Cancel the system (watchdog) timer
 */
int
cancel_timer()
{
    unsigned long	stat;

    stat = SYS$CANTIM(SS$_TIMEOUT, 0);
    if (!(stat & STS$M_SUCCESS)) {
	sprintf(errbuf, "ERROR: failed canceling system (watchdog) timer\n");
        log_error(errbuf);		/* Log error to operator term.	*/
	exit(stat);			/* Fatal error so exit		*/
    }
}
	    
/*
 *  Create the static string descriptor
 *  "desc" for the string "text"
 */
int
make_sdesc(desc, text)
struct dsc$descriptor_s *desc;		/* Static string descriptor	*/
char			*text;		/* String of text		*/
{
    desc->dsc$w_length = strlen(text);	/* Length of text string	*/
    desc->dsc$b_dtype = DSC$K_DTYPE_T;	/* Data type			*/
    desc->dsc$b_class = DSC$K_CLASS_S;	/* Descriptor class code	*/
    desc->dsc$a_pointer = text;		/* String of text pointer	*/
}


/*
 *  Create the variable string descriptor
 *  "desc" for the string "text"
 */
int
make_vdesc(desc, buffer, maxlen)
struct dsc$descriptor_vs *desc;		/* Variable string descriptor	*/
char			*buffer;	/* String buffer pointer	*/
long			maxlen;		/* Maximum length of string	*/
{
    desc->dsc$w_maxstrlen = maxlen;	/* Maximum length of string	*/
    desc->dsc$b_dtype = DSC$K_DTYPE_VT;	/* Data type			*/
    desc->dsc$b_class = DSC$K_CLASS_VS;	/* Descriptor class code	*/
    desc->dsc$a_pointer = buffer;	/* String buffer pointer	*/
}


/*
 *  Create the static string descriptor "desc" for the message
 *  buffer "msgbuf" used to send a user request to an operator terminal
 */
int
make_opr_sdesc(desc, msgbuf)
struct dsc$descriptor_s *desc;		/* Static string descriptor	*/
OPER_REQ		*msgbuf;	/* Request to send to oper term	*/
{
    register int	len;
    register int	maxlen;

    maxlen=sizeof(msgbuf->opc$l_ms_text);  /* Max length of text buffer	*/
    len=strlen(msgbuf->opc$l_ms_text);	   /* Actual len of text string	*/
    desc->dsc$w_length = sizeof(OPER_REQ); /* Max length of entire buf	*/
    desc->dsc$w_length -= (maxlen - len);  /* Actual length		*/
    desc->dsc$b_dtype = DSC$K_DTYPE_T;		/* Data type		*/
    desc->dsc$b_class = DSC$K_CLASS_S;	        /* Descriptor class code*/
    desc->dsc$a_pointer = msgbuf;	        /* String of text ptr	*/
}

