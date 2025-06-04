#define	CONVENIENCE_ROUTINES	0	/* Compile routines = 1, else 0	     */
/*============================================================================= 
!
! Program:	CD_PLAYER.C	(Version 1.0 revision 21)
!
! Author(s)	Gary Boyles (gpb - DEC, Englewood, CO  USA) ! Motif code
!		Steve Davis (snd - DEC, Greenbelt, MD  USA) ! device control,
!							    ! icons
! Date:		15-JUN-1992				  
!
!
! Modification History:
!
!	Revision	Reason					  Date
!	--------	-------------------------------------	--------
!	V1.0 -17	Initially finished		(gpb)	11/21/91
!	     -18	Plugged some memory-leaks	(gpb)	01/03/92
!	     -19	Modularized things a bit more	(gpb)	06/05/92
!	     -20	Added the following:		(gpb)	06/15/92
!			a) Descending time-remaining scale.
!			b) track-max label to "Track Now Playing"
!			c) Total play-time printf statement.
!			d) MSF-related convenience routines
!			   (present for example -- not used).
!	     -21	OpenVMS Alpha Port	        (snd)	04/19/95
!			 - fix minor code & syntax
!			 - removed the printf
!			 - changed create-scale widget to
!			   allow for size on both x & y axes.
!			 - adjusted window and widget geometry
!                          for proper appearance @1280x1024x100dpi.          
!			 - retest on DEC3000-M400 OpenVMS AXP V6.1
!			 - retest on VS4000-M60 OpenVMS VAX V6.1
!			 - re-kit, add VUE$CD command procedure
!             
! Description:
!
!	This program acts as a control panel for a SCSI audio CD player. The 
!	program uses MOTIF toolkit routines to create and manage the display 
!	and its widgets. It creates a workstation window panel with buttons 
!	for:           
!
!		eject	= stop playing/enable-eject/eject-caddy from player
!		stop	= stop the player from playing & enable-eject
!	    	play	= start playing CD on current track & disable-eject
!	   	pause	= pause/resume the CD-player      
!		replay  = replay the current selection
!		shuffle = play random selections
!		exit	= stop playing/enable eject(but don't)/exit program
!		incr	= increment track playing 
!		decr	= decrement track playing
!
!	The program also creates the following scales/sliders/displays:
!
!		current-track playing
!		track-time display
!		track-time remaining (seconds) display
!		volume control/display
!		channel-balance control/display
!                                 
!  	Note that the volume thumb-wheel on your CD player should be on 
!	full-volume, because the slider will only control volume upto what 
!	the thumb-wheel is set to. Note that the volume control slider is only
!	for the headphone jack on the front of the drive. The audio output
!	RCA phono jacks on tabletop CD drives are fixed output.
!                             
!	NOTE: ALL TESTING WAS PERFORMED ON A DEC RRD42 (Sony CDU-541) drive!
!
!	Currently the following improvements are necessary:
!	  1)  Better random # generator for shuffle (currently not so random)
!	  2)  Some error-trapping code		    (currently quite minimal)
!
!	In the program listing (frontend) refers for the most part to MOTIF
!	routines, and (backend) refers to CD-control routines.  Routines
!	starting with "handle_" are the button/arrow/slider callback
!	routines.
!                                    
!	In regard to routines getting executed... it happens via two methods:
!	  1)  Pushing a button or moving the slider (activating a callback)
!	  2)  Via the timer routine "the_timer".
!
!	Build Instructions:
!		A command procedure DECW$CD_PLAYER.BLD is used to build
!	this product on both VAX and AXP. The com file is included with this
!	kit.
!
!	The program interfaces to the CD player through the SCSI disk driver.
!	If you assume that the CD-player is device DKB400 then you would
!	have to do the following before running the program (with privs):
!                                                            
!	$ DEFINE DECW$CD_PLAYER DKB400:
!
!	Note:
!	To change the icon resources for the CD-player, edit your
!	"mwm" file (on VMS this is DECW$MWM.DAT).  For example:
!	
!		Mwm*CD*iconImageForeground:	Firebrick
!		Mwm*CD*iconImageBackground:	Yellow
!
!===========================================================================*/

#include <stdio.h>                                  
#include <Xm/MainW.h>	    	    	/* MainWindow Class	    	*/
#include <Xm/BulletinB.h>		/* BulletinBoard Class		*/
#include <Xm/Frame.h>			/* Frame widget class		*/
#include <Xm/Label.h>			/* Label class			*/
#include <Xm/PushB.h> 			/* Push-button Class		*/
#include <Xm/RowColumn.h>		/* RowColumn Class		*/
#include <Xm/Scale.h>	    	  	/* Scale class			*/   

/*===============================(Frontend)===================================
! Symbolic Constants                           
!====================*/ 
#define SHOW_CD_TIME		0	/* 1 = show ttl play time on CD	*/
                                            
#define ICON_LABEL		"Audio"	/* Cd-player Icon label		*/

#define TIMER_INTERVAL		1000	/* How often?  1000 = 1/second	*/
        
#define CD_TRACK_MIN		0	/* Lower limit for track select	*/
#define CD_TRACK_MAX   		99	/* Upper limit for track select	*/

#define BALANCE_SCALE_MAX	10	/* Balance max (min = -max)	*/

#define VOLUME_OFF		  0	/* Volume Completely Off	*/
#define VOLUME_DEFAULT		200	/* Initial volume-setting	*/
#define VOLUME_MINIMUM		155 	/* Lower limit for volume level	*/
#define VOLUME_MAXIMUM		255	/* Upper limit for volume level	*/
          
#define PUSH_BUTTON		0	/* Used in Create_Button	*/
#define ARROW_LEFT		1	/* Used in Create_Button	*/
#define ARROW_RIGHT		2	/* Used in Create_Button	*/
                                                          
#define SHADOW_THICKNESS	4
#define BOTTOM_SHADOW_COLOR		"DimGrey"
#define TOP_SHADOW_COLOR		"White"
                
#define DEFAULT_FOREGROUND		"Firebrick"
#define DEFAULT_BACKGROUND		"AntiqueWhite3"

#define DEF_PUSHBUTTON_BACKGROUND	"Black"		/* Defaults	*/
#define DEF_PUSHBUTTON_FOREGROUND	"White"
#define ALT_PUSHBUTTON_BACKGROUND	"Firebrick"	/* Alternate	*/
#define ALT_PUSHBUTTON_FOREGROUND	DEF_PUSHBUTTON_FOREGROUND

#define TRACK_TIME_BACKGROUND		"NavyBlue"                 
#define TRACK_TIME_FOREGROUND		DEF_PUSHBUTTON_FOREGROUND

#define BUTTON_HEIGHT	  	40
#define BUTTON_WIDTH		100
#define LABEL_HEIGHT		29

#define ARROW_FOREGROUND		DEFAULT_BACKGROUND
#define ARROW_BACKGROUND		DEFAULT_FOREGROUND

#define SCALE_FOREGROUND		DEFAULT_FOREGROUND
#define SCALE_BACKGROUND		DEFAULT_BACKGROUND
#define SCALE_TOP_SHADOW		DEFAULT_FOREGROUND
#define SCALE_BOTTOM_SHADOW 		DEFAULT_FOREGROUND


#define	FALSE			0
#define TRUE			1
#define NOT_USED		-1
#define	VERTICAL_SCALE		1
#define HORIZONTAL_SCALE	2

/*===========================================================================
! Define bitmap for the icon used for the cd-player.
!===========================================================================*/
             
#define SMALL_ICON_WIDTH	32
#define SMALL_ICON_HEIGHT	32
static	char small_icon_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xf8, 0x00,
   0x00, 0x01, 0x08, 0x01, 0x80, 0x00, 0x08, 0x02, 0x80, 0x00, 0x08, 0x04,
   0x80, 0x00, 0x08, 0x04, 0x80, 0x00, 0x08, 0x04, 0x80, 0x00, 0x08, 0x04,
   0x80, 0x00, 0x08, 0x04, 0x80, 0x00, 0x08, 0x04, 0x80, 0x00, 0x08, 0x04,
   0x80, 0x00, 0x08, 0x04, 0x80, 0x00, 0x08, 0x02, 0x00, 0x01, 0x08, 0x01,
   0x00, 0x7e, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x00, 0x00, 0x08, 0x80, 0x00,
   0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x87, 0x00, 0x00, 0x88, 0x8f, 0x00,
   0x00, 0xc8, 0x9f, 0x00, 0x00, 0xc8, 0x9f, 0x00, 0x00, 0xc8, 0x9f, 0x00,
   0x00, 0x88, 0x8f, 0x00, 0x00, 0x08, 0x87, 0x00, 0x00, 0x08, 0x80, 0x00,
   0x00, 0xf8, 0xff, 0x00, 0x00, 0xf8, 0xff, 0x00};
             
#define MEDIUM_ICON_WIDTH	50
#define MEDIUM_ICON_HEIGHT	50
static	char medium_icon_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0x0f, 0x7c, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07,
   0x7c, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xe3, 0x7f, 0x8e, 0xff, 0xff, 0xff,
   0xff, 0xf3, 0x7f, 0x9e, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x7f, 0x9e, 0xff,
   0xff, 0xff, 0xff, 0xf3, 0x7f, 0x9e, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x7f,
   0x9e, 0xff, 0xff, 0xff, 0xff, 0xf3, 0x7f, 0x9e, 0xff, 0xff, 0xff, 0xff,
   0xe3, 0x7f, 0x8e, 0xff, 0xff, 0xff, 0xff, 0x07, 0x7c, 0xc0, 0xff, 0xff,
   0xff, 0xff, 0x0f, 0x7c, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x07, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0xc0,
   0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xfc, 0xff, 0xff,
   0xff, 0xc9, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0xff, 0xcc, 0xff, 0x3f, 0xff,
   0xff, 0xff, 0x7f, 0xce, 0xff, 0x9f, 0xff, 0xff, 0xff, 0x3f, 0xcf, 0xff,
   0xcf, 0xff, 0xff, 0xff, 0x9f, 0xcf, 0xff, 0x07, 0x00, 0x00, 0x00, 0xc0,
   0xcf, 0xff, 0x07, 0x00, 0x00, 0x00, 0xe0, 0xcf, 0xff, 0xe7, 0xff, 0xff,
   0xff, 0xe7, 0xcf, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xe7, 0xcf, 0xff, 0xe7,
   0xff, 0xff, 0xff, 0xe7, 0xcf, 0xff, 0x67, 0x00, 0x00, 0x00, 0xe7, 0xcf,
   0xff, 0x67, 0xff, 0xff, 0x7f, 0xe7, 0xcf, 0xff, 0x67, 0xff, 0xff, 0x7f,
   0xe7, 0xcf, 0xff, 0x67, 0x00, 0x00, 0x00, 0xe7, 0xcf, 0xff, 0xe7, 0xff,
   0xff, 0xff, 0xe7, 0xe7, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0xff,
   0xe7, 0xf9, 0xff, 0x1f, 0xe7, 0xf9, 0xff, 0xe7, 0xf9, 0xff, 0x1c, 0xe7,
   0xfc, 0xff, 0xe7, 0xff, 0xff, 0xff, 0x67, 0xfe, 0xff, 0xe7, 0xff, 0xff,
   0xff, 0x27, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x07,
   0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff};
             
#define LARGE_ICON_WIDTH	75
#define LARGE_ICON_HEIGHT	75
static	char large_icon_bits[] = {
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xfd, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xfa, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xfa, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x55, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xfa,
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xfd, 0xaa, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfa, 0xd5, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x60, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xfa, 0xd5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x65, 0xfd, 0xaa, 0xea, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xaa, 0xfa,
   0xd5, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x64, 0xfd, 0xaa, 0x2a,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xaa, 0xfa, 0xd5, 0x25, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x80, 0x64, 0xfd, 0xaa, 0xea, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xaa, 0xfa, 0xd5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x65, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xfa,
   0xd5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x65, 0xfd, 0xaa, 0x0a,
   0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x80, 0xaa, 0xfa, 0xd5, 0x65, 0x05, 0x40,
   0x55, 0x55, 0x05, 0x00, 0x65, 0xfd, 0xaa, 0x6a, 0x0a, 0x80, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xfa, 0xd5, 0x05, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x65, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xfa,
   0xd5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfd, 0xaa, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xfa, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xfa, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x55, 0xfd, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xfa,
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xfd, 0xaa, 0xaa,
   0xfe, 0xff, 0x01, 0xf0, 0xff, 0xaf, 0xaa, 0xfa, 0x55, 0x55, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x5f, 0x55, 0xfd, 0xaa, 0xaa, 0x9f, 0xaa, 0xaa, 0xaa,
   0xaa, 0xbe, 0xaa, 0xfa, 0x55, 0x55, 0x4f, 0x55, 0x01, 0x50, 0x55, 0x3d,
   0x55, 0xfd, 0xaa, 0xaa, 0xa7, 0x2a, 0xbe, 0x82, 0xaa, 0xba, 0xaa, 0xfa,
   0x55, 0x55, 0x53, 0x85, 0x5f, 0x15, 0x54, 0x35, 0x55, 0xfd, 0xaa, 0xaa,
   0xab, 0xea, 0xbf, 0xaa, 0xaa, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0x57, 0xf1,
   0x5f, 0x55, 0x51, 0x35, 0x55, 0xfd, 0xaa, 0xaa, 0xab, 0xfa, 0xbf, 0xaa,
   0xaa, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0x57, 0xfc, 0x5f, 0x55, 0x45, 0x35,
   0x55, 0xfd, 0xaa, 0xaa, 0x2b, 0xfe, 0xbf, 0xaa, 0xaa, 0xba, 0xaa, 0xfa,
   0x55, 0x55, 0x57, 0xff, 0x5f, 0x55, 0x55, 0x35, 0x55, 0xfd, 0xaa, 0xaa,
   0x2b, 0xff, 0xbf, 0xaa, 0x8a, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0x97, 0xff,
   0x5f, 0x55, 0x15, 0x35, 0x55, 0xfd, 0xaa, 0xaa, 0xab, 0xff, 0x07, 0xa8,
   0x2a, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0x97, 0xff, 0xf3, 0x51, 0x55, 0x35,
   0x55, 0xfd, 0xaa, 0xaa, 0xcb, 0xff, 0xf9, 0xa3, 0x2a, 0xba, 0xaa, 0xfa,
   0x55, 0x54, 0xd7, 0xff, 0xfc, 0x47, 0x55, 0x35, 0x55, 0xfd, 0xaa, 0xaa,
   0xcb, 0xff, 0xfe, 0xaf, 0x2a, 0xba, 0xaa, 0xfa, 0x55, 0x54, 0xd7, 0xff,
   0xfe, 0x4f, 0x55, 0x35, 0x55, 0xfd, 0xaa, 0xa8, 0x8b, 0xaa, 0xfe, 0xaf,
   0x3f, 0xba, 0xaa, 0xfa, 0x55, 0x40, 0x57, 0x55, 0xfe, 0xcf, 0x7f, 0x35,
   0x55, 0xfd, 0xaa, 0xaa, 0x8b, 0xaa, 0xfe, 0xef, 0x7f, 0xba, 0xaa, 0xfa,
   0x55, 0x54, 0x57, 0x55, 0xfc, 0xe7, 0x7f, 0x35, 0x55, 0xfd, 0xaa, 0xaa,
   0x8b, 0xaa, 0xf8, 0xf3, 0x7f, 0xba, 0xaa, 0xfa, 0x55, 0x54, 0x17, 0x55,
   0xf1, 0xf9, 0x7f, 0x35, 0x55, 0xfd, 0xaa, 0xaa, 0xab, 0xaa, 0x02, 0xfc,
   0xbf, 0xba, 0xaa, 0xfa, 0x05, 0x54, 0x17, 0x55, 0x55, 0xff, 0x3f, 0x35,
   0x55, 0xfd, 0x02, 0xaa, 0x2b, 0xaa, 0xaa, 0xff, 0x9f, 0xba, 0xaa, 0xfa,
   0x01, 0x54, 0x57, 0x55, 0x55, 0xff, 0x5f, 0x35, 0x55, 0xfd, 0x02, 0xaa,
   0x2b, 0xaa, 0xaa, 0xff, 0xaf, 0xba, 0xaa, 0xfa, 0x05, 0x55, 0x57, 0x55,
   0x55, 0xff, 0x47, 0x35, 0x55, 0xfd, 0xaa, 0xaa, 0xab, 0xa8, 0xaa, 0xff,
   0xab, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0x57, 0x51, 0x55, 0xff, 0x55, 0x35,
   0x55, 0xfd, 0xaa, 0xaa, 0xab, 0xa2, 0xaa, 0xfe, 0xa8, 0xba, 0xaa, 0xfa,
   0x55, 0x55, 0x57, 0x15, 0x55, 0x3f, 0x55, 0x35, 0x55, 0xfd, 0xaa, 0xaa,
   0xab, 0x2a, 0xa8, 0x82, 0xaa, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0x57, 0x55,
   0x01, 0x50, 0x55, 0x35, 0x55, 0xfd, 0xaa, 0xaa, 0xab, 0xaa, 0xaa, 0xaa,
   0xaa, 0xba, 0xaa, 0xfa, 0x55, 0x55, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f,
   0x55, 0xfd, 0xaa, 0xaa, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xaa, 0xfa,
   0x55, 0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x55, 0xfd, 0xaa, 0xaa,
   0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xfa, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0xfd};


Pixmap 					/* The CD-player icon		*/
	cd_icon;

             
globalvalue
	IO$_AVAILABLE,
	IO$_PACKACK,
	IO$_DIAGNOSE;


/*================================(Backend)===================================
!  SCSI Generic Class Driver #Defines & Variables
!===============================================*/

static char	gk_device [] = {"DECW$CD_PLAYER"};                          

short    gk_chan;
              
/*------------------------------------
! CD-ROM SCSI operation command codes           
!-------------------------------------*/
#define SCSI_STATUS_MASK	0X3E
#define MEDIA_OPCODE		0x1E 
#define MODE_SELECT_OPCODE	0X15
#define PAUSE_OPCODE		0X4B
#define RESUME_OPCODE		0X4B
#define STOP_OPCODE		0x1B
#define READ_SUBCHAN_OPCODE	0X42
#define READ_TOC_OPCODE		0X43                             
#define PLAY_TRACK_OPCODE	0X48
#define PLAYBACK_CONTROL_OPCODE 0XC9
#define PLAYBACK_STATUS_OPCODE	0XC4

#define READ 1
#define WRITE 0

                                                                             
/*  Varibles for saving CD-player state
!======================================*/                               
                                                 
static int
	saved_track_time_sec,
	track_time_sec,			/* Playing time for this track	*/
	total_tracks	= 0,		/* Total track read off this CD	*/
	saved_total_tracks = 0,
	current_track	= 0,		/* Track now playing		*/
	saved_current_track = 0,	/* Track we thought was playing	*/
	random_track	= 0,		/* Track # generated for SHUFFLE*/
	old_random_track= 0,		/* Saved (old) random_track	*/ 
	random_total 	= 0,		/* # of random tracks to SHUFFLE*/
	current_volume	= 0,		/* Current volume-level		*/
	left_volume	= 0,		/* Current left-channel setting	*/
	right_volume	= 0,		/* Current right-channel setting*/
	play_flag 	= FALSE,	/* Flags set/reset by pressing	*/
	cd_paused	= FALSE,	/* the various pushbuttons	*/
	saved_cd_paused	= FALSE,
      	shuffle_flag	= FALSE;
                    
static	unsigned int Track_Address[CD_TRACK_MAX];
			/* Save the start address of each CD track	*/

static	int	Track_Seconds [CD_TRACK_MAX];                           
		      	/* Track time (seconds) for each track on CD	*/

                            
/*============================================================================ 
!  (Frontend) Routines  
!=====================*/

void	init_icon();			/* Setup CD icon		*/
                                                                          
int	String_To_Pixel(),		/* String to Pixel Conversion	*/
	get_icon_size();		/* Which icon-size is needed	*/

Widget	Create_Button(),		/* Create button or arrow	*/
    	Create_Scale(),			/* Create a horizontal scale	*/
	Create_Label();			/* Create a label		*/
        
/*===========================================================================
!  (Frontend to Backend) Routines  
!================================*/

void
	handle_eject_button(),		/* EJECT   button callback	*/
	handle_stop_button(),		/* STOP    button callback	*/
	handle_play_button(),		/* START   button callback	*/
	handle_pause_button(),		/* PAUSE   button callback	*/
	handle_replay_button(),		/* REPLAY  button callback	*/
	handle_shuffle_button(),	/* SHUFFLE botton callback	*/
	handle_exit_button(),		/* EXIT    button callback	*/
   	handle_track_decr(),		/* DECRement CD track callback 	*/
	handle_track_incr(),		/* INCRement CD track callback	*/
	handle_track_scale(),		/* Track-input slider callback	*/
	handle_time_scale(),		/* Track-time  slider callback	*/
	handle_volume_scale(),	   	/* Volume-input slider callback	*/
	handle_balance_scale();		/* Balance-input slider callback*/

/*===========================================================================
!  Backend Routines
!==================*/
        
void
	the_timer(),		  	/* Master program timer		*/
	update_track_time(),		/* Update Track-Time Display	*/
	update_time_scale_max(),	/* Size time-scale to tracktime	*/
    	update_track_info(),	 	/* Update track scale and data	*/
	update_track_max(),		/* Update track-max label	*/
	init_track_scale(),		/* Init track-scale  display	*/
	init_volume_and_balance(),	/* Init volume/balance display	*/
	set_volume_and_balance(),	/* Set CD volume/balance levels	*/
	reset_pause_button(),		/* Reset label to PAUSE		*/
	toggle_pause_label(),		/* Toggle label PAUSE/RESUME	*/
	do_shuffle();			/* Do the actual track shuffle	*/
                              
int	get_random_track();		/* Random number generator	*/


/*===========================================================================
!  (Backend) CD-player specific Routines
!=======================================*/
unsigned int	read_CD_capacity(),	/* # of blocks on current CD	*/
		read_toc_track();	/* Returns start-addr for track	*/

int	read_toc(),	       		/* Get CD table-of-contents	*/
	setup_default_block_size(),	/* Set block size = 512		*/
	play_track(),			/* Play a track on the CD	*/
	play_MSF(),                     /* Mins/sec/frame format play	*/
	play_partial_track(),		/* Plays portion of a track	*/
	pause_cd(),			/* Suspend playing		*/
	resume_cd(),			/* Resume  playing		*/
	stop_unit(),			/* Stop the CD player		*/
	eject_cd(),			/* Eject the CD			*/
	enable_eject(),			/* Allow manual removal of CD	*/
   	disable_eject();		/* Don't allow manual removal	*/

void	get_volume(),			/* Get current volume of player	*/
	set_volume(),			/* Set the volume of the player	*/
	get_status(),			/* Read CD table-of-contents	*/
	save_track_time(),		/* Save track-time of CD tracks	*/
    	bpt();				/* Null Routine			*/

/*===========================================================================
! VMS specific routines
!======================*/

int	execute_command();	       	/* Sends the actual SCSI cmds	*/
		  			/* to the CD-player		*/
void    exit_handler(),			/* Stop CD/Deassign Channel/etc	*/
	alloc_cd_channel(),    		/* Assigns a channel to CD	*/
    	dealloc_cd_channel();		/* Deassign CD channel		*/


/*===========================================================================
!  List of Widgets (Frontend)
!============================*/
                               
Widget                
	toplevel,
	main_window,
	work_area,
	rc,
	eject_button,
	stop_button,
	play_button,
	pause_button,
	replay_button,
	shuffle_button,
	exit_button,
	decrArrow,
	incrArrow,
	volume_scale,
	balance_scale,
	track_scale,
	time_scale,
	trackMax,
	trackTime,
	trackFrame,
	trackLabel,
	timeLabel,
	arrowsLabel;

                                            
/*===========================================================================*/
/*===========================================================================*/
/* Other Declarations       
!====================*/
static	Display	*display;		/* Holds display id		*/

XtAppContext ctx;
  
Pixel	color;
       
Arg	args[20];

int	n;		       		/* Generic arg counter		*/

unsigned long int random_number = 1;	/* Holds random numbers required*/
					/* for SHUFFLE operation	*/
                                                             
/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/
/*===========================================================================*/
/*                             BEGIN MAIN ROUTINE                            */
                     
main(argc, argv)        
  int argc;                     
                                                                          
  char **argv;

{
  
  /*=======================================================================
  ! Create our application shell, and our main window.
  !=======================================================================*/
  toplevel = XtInitialize(		/* Create ApplicationShell	*/
	"Audio",			/* Name				*/
	"CD",				/* Class			*/
	NULL,			  	/* Options			*/
	0,	       			/* Number of options		*/
	&argc,				/* Address of argc		*/
	argv);				/* Arg value			*/


  main_window = XmCreateMainWindow (	/* Create main-window		*/
      	toplevel,			/* Parent			*/
	"Main",	 			/* name				*/
	args, 0);	  		/* args, # of args		*/

  XtManageChild(main_window);

  display = XtDisplay(toplevel);


  /*=======================================================================
  ! Create a bulletin-board as our work-area, and manage via a row-column
  !=======================================================================*/
  n = 0;				/* Set bulletin-board resources	*/
  XtSetArg( args[n], XmNwidth,  745); ++n;
  XtSetArg( args[n], XmNheight, 160); ++n;
  XtSetArg( args[n], XmNresizePolicy, XmRESIZE_NONE); ++n;  

  color = String_To_Pixel (DEFAULT_BACKGROUND);
  XtSetArg( args[n], XmNbackground, color); ++n;

  work_area = XmCreateBulletinBoard (	/* Create bulletin-board	*/
	main_window,			/* Parent			*/
	"Work",				/* name				*/
	args, n);			/* args, # of args.		*/

  XtManageChild(work_area);
              
  n = 0;				/* Init arg count		*/
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); ++n;	/* Lay it flat  */
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);	++n;	/* How to pack	*/
  XtSetArg( args[n], XmNspacing, 5);	++n;        /* button interspacing */
  XtSetArg( args[n], XmNx, 170);	++n;        /* button X placement */    
  XtSetArg( args[n], XmNy, 105); 	++n;        /* button Y placement */   

  color = String_To_Pixel (DEFAULT_BACKGROUND);
  XtSetArg( args[n], XmNbackground, color); ++n;
 
  rc = XmCreateRowColumn (		/* Create row-col widget inst	*/
	work_area,	   		/* Parent			*/
	"RowCol",			/* Name				*/
	args, n);	  		/* Arglist, Arg count		*/

  XtManageChild(rc);
                 
                  
  /*=======================================================================
  ! Now create a EJECT (i.e. EJECT the CD) pushbutton.
  !   Inputs:	parent, name/label, PUSH_BUTTON/ARROW_LEFT/ARROW_RIGHT,
  !		X, Y, height, width, foreground, background, display-id
  !=======================================================================*/
                                                                       
  eject_button = Create_Button (rc, " EJECT ", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			DEF_PUSHBUTTON_FOREGROUND,
			DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(eject_button);                     
  XtAddCallback(eject_button, XmNactivateCallback, handle_eject_button, NULL);
  

  /*=======================================================================
  ! Now create a STOP (i.e. STOP the CD) pushbutton.
  !   Inputs:	parent, name/label, PUSH_BUTTON/ARROW_LEFT/ARROW_RIGHT,
  !		X, Y, height, width, foreground, background, display-id
  !=======================================================================*/
                                                                       
  stop_button = Create_Button (rc, " STOP ", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			DEF_PUSHBUTTON_FOREGROUND,
	     		DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(stop_button);                                 
  XtAddCallback(stop_button, XmNactivateCallback, handle_stop_button, NULL);
                                                                

  /*=======================================================================
  ! Now create a PLAY  CD pushbutton.
  !=======================================================================*/
                        
  play_button = Create_Button (rc, "  PLAY  ", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			DEF_PUSHBUTTON_FOREGROUND,
			DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(play_button);
  XtAddCallback(play_button, XmNactivateCallback, handle_play_button, NULL);
                                                   

  /*=======================================================================
  ! Now create a PAUSE (i.e. pause/resume selection) pushbutton.
  !=======================================================================*/

  pause_button = Create_Button (rc, " PAUSE ", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			DEF_PUSHBUTTON_FOREGROUND,
			DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(pause_button);
  XtAddCallback(pause_button, XmNactivateCallback, handle_pause_button, NULL);


  /*=======================================================================
  ! Now create a REPLAY (i.e. play last song) pushbutton.
  !=======================================================================*/

  replay_button = Create_Button (rc, "REPLAY", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			DEF_PUSHBUTTON_FOREGROUND,
			DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(replay_button);
  XtAddCallback(replay_button, XmNactivateCallback, handle_replay_button, NULL);

                            
  /*=======================================================================
  ! Add in SHUFFLE pushbutton.
  !=======================================================================*/
                                  
  shuffle_button = Create_Button (rc, "SHUFFLE", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			DEF_PUSHBUTTON_FOREGROUND,
			DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(shuffle_button);              
  XtAddCallback(shuffle_button,XmNactivateCallback,handle_shuffle_button, NULL);
                            

                            
  /*=======================================================================
  ! Add in EXIT pushbutton.
  !=======================================================================*/
                                  
  exit_button = Create_Button (rc, "  EXIT  ", PUSH_BUTTON,
		       	NOT_USED, NOT_USED, BUTTON_HEIGHT, BUTTON_WIDTH,  
			ALT_PUSHBUTTON_FOREGROUND, 
		    	DEF_PUSHBUTTON_BACKGROUND, display);
  XtManageChild(exit_button);
  XtAddCallback(exit_button, XmNactivateCallback, handle_exit_button, NULL);
                            
           
  /*=======================================================================
  ! Add decrement-track Arrow button.
  !=======================================================================*/
                                                                          
  decrArrow = Create_Button (work_area, "DECR", ARROW_LEFT,
     		   	170, 15, 40, 60,           
			ARROW_FOREGROUND,
			ARROW_BACKGROUND, display);
  XtManageChild(decrArrow);          
                              
  XtAddCallback(decrArrow, XmNactivateCallback, handle_track_decr, NULL);
                            
           
  /*=======================================================================
  ! Add increment-track Arrow button.
  !=======================================================================*/
  
  incrArrow = Create_Button (work_area, "INCR", ARROW_RIGHT,
		 	240, 15, 40, 60,
			ARROW_FOREGROUND,
		 	ARROW_BACKGROUND, display);

  XtManageChild(incrArrow);          
                              
  XtAddCallback(incrArrow, XmNactivateCallback, handle_track_incr, NULL);
                                                
                                 
  /*=======================================================================
  ! Add a scale widget to show current volume-level set on the CD.
  !   Inputs:	parent, name/label, X, Y, scale-X-size,scale_y_size 
  !		scale-min, scale-max, initial scale value,
  !		foreground color, background color,
  !		bottom shadow color, top shadow color,
  !	    	display-id
  !                            
  ! Note:  Start scale at 0, no matter what the actual volume-level min
  !	   value is.
  !=======================================================================*/
                
  volume_scale = Create_Scale (work_area, "Volume", "  Volume Level",
  		0, 1, 130, 20,
  		VOLUME_MINIMUM - VOLUME_MINIMUM,
       		VOLUME_MAXIMUM - VOLUME_MINIMUM,
	   	VOLUME_DEFAULT - VOLUME_MINIMUM,
	   	DEFAULT_FOREGROUND, DEFAULT_BACKGROUND,
       		BOTTOM_SHADOW_COLOR,  TOP_SHADOW_COLOR,
		HORIZONTAL_SCALE, display);
                          
  XtManageChild(volume_scale);
                              
  XtAddCallback(volume_scale,XmNvalueChangedCallback,handle_volume_scale, NULL);
                
  balance_scale = Create_Scale (work_area, "Balance", "(L)   Balance   (R)",
  		0, 75, 130, 20,        
 		-BALANCE_SCALE_MAX, BALANCE_SCALE_MAX, 0,          
	   	DEFAULT_FOREGROUND, DEFAULT_BACKGROUND,
       		BOTTOM_SHADOW_COLOR,  TOP_SHADOW_COLOR,	
		HORIZONTAL_SCALE, display);
                          
  XtManageChild(balance_scale);
                              
  XtAddCallback(balance_scale , XmNvalueChangedCallback,
		handle_balance_scale, NULL);

                                          
  /*=======================================================================
  ! Add a scale widget to show current TRACK-SELECTED on the CD.
  !   Inputs:	parent, name/label, X, Y, scale-size, 
  !		scale-min, scale-max, initial scale value,
  !	      	foreground color, background color,
  !		bottom shadow color, top shadow color,
  !	    	display-id       
  !
  !   Note:	Scale can also be used to change track #.
  !=======================================================================*/
                
  track_scale = Create_Scale (work_area, "This_Track",
		"  Track Now Playing",
  		320, 1, 180, 20,                           
    		CD_TRACK_MIN, CD_TRACK_MAX, CD_TRACK_MIN,
      	   	DEFAULT_FOREGROUND, DEFAULT_BACKGROUND,
       		BOTTOM_SHADOW_COLOR,  TOP_SHADOW_COLOR,	
		HORIZONTAL_SCALE, display);             
                                     
  XtManageChild(track_scale);                               
                              
  XtAddCallback(track_scale,XmNvalueChangedCallback,handle_track_scale, NULL);

                                          
  /*=======================================================================
  ! Add a scale widget to show track-time remaining in seconds
  !=======================================================================*/

  time_scale = Create_Scale (work_area, "Time_Scale", " Seconds",
  		650, 1, 80, 20,                           
  		0, 999, 0,
      	   	DEFAULT_FOREGROUND, DEFAULT_BACKGROUND,
       		BOTTOM_SHADOW_COLOR,  TOP_SHADOW_COLOR,	
		HORIZONTAL_SCALE, display);             
                                     
  XtManageChild(time_scale);                               
                              
  XtAddCallback(time_scale, XmNvalueChangedCallback,
		handle_time_scale, NULL); 
                
  timeLabel = Create_Label (work_area, "TL", "Remaining",
   			650, 76, LABEL_HEIGHT, 85,       
			DEFAULT_FOREGROUND,
			DEFAULT_BACKGROUND, display);            
                               
  XtManageChild(timeLabel);     

                                                    
  /*=======================================================================
  ! Add label widgets to display highest track.
  !   Inputs:	Parent, Name, Actual Label, x,Y, height, width,    
  !	    	foreground color, background color, display id.
  !=======================================================================*/
  n = 0;
  trackMax = Create_Label (work_area, "TM", "99",
			500, 25, LABEL_HEIGHT, 30,
			DEFAULT_FOREGROUND,
			DEFAULT_BACKGROUND, display);            
         
  XtManageChild(trackMax);     
                                                    
  /*=======================================================================
  ! Add Frame widget to frame the  TRACK-TIME.   
  !   Inputs:	Parent, Name, Actual Label, x,Y, height, width,    
  !	    	foreground color, background color, display id.
  !=======================================================================*/
                
  n = 0;					/* Init arg count	*/
  XtSetArg( args[n], XmNx, 550);	++n;	/* X Location		*/
  XtSetArg( args[n], XmNy, 20);	 	++n;	/* Y Location		*/
         
  XtSetArg( args[n], XmNshadowType,		/* Type of frame effect	*/
		XmSHADOW_ETCHED_IN);	++n;
              
  color = String_To_Pixel (TOP_SHADOW_COLOR);	/* Frame shadow colors	*/
  XtSetArg( args[n], XmNtopShadowColor, color);	++n;

  color = String_To_Pixel (BOTTOM_SHADOW_COLOR);
  XtSetArg( args[n], XmNbottomShadowColor, color); ++n;
                                             
  XtSetArg( args[n], XmNshadowThickness, SHADOW_THICKNESS+2); ++n;
 
  trackFrame = XmCreateFrame (		/* Create Frame widget inst	*/
	work_area,	   		/* Parent			*/
	"tFrame",			/* Name				*/
	args, n);	  		/* Arglist, Arg count		*/

  XtManageChild(trackFrame);

                                                    
  /*=======================================================================
  ! Add label widgets to display TRACK-TIME.   
  !   Inputs:	Parent, Name, Actual Label, x,Y, height, width,    
  !	    	foreground color, background color, display id.
  !=======================================================================*/
  n = 0;
  trackTime = Create_Label (trackFrame, "L1", "00:00",
			0, 0, LABEL_HEIGHT, 60,
			TRACK_TIME_FOREGROUND,
			TRACK_TIME_BACKGROUND, display);
         
  XtManageChild(trackTime);     
  

  /*=======================================================================
  ! Add label widget to add label to track-time display.
  !=======================================================================*/
                                        
  trackLabel = Create_Label (work_area, "L2", "Track Time",
   			535, 55, LABEL_HEIGHT, 90,       
			DEFAULT_FOREGROUND,
			DEFAULT_BACKGROUND, display);            
                               
  XtManageChild(trackLabel);     
  

  /*=======================================================================
  ! Add label widget to add label to Incr/Decr Arrows.
  !=======================================================================*/
                                        
  arrowsLabel = Create_Label (work_area, "L3", "Select Track",
			165, 55, LABEL_HEIGHT, 140,       
     			DEFAULT_FOREGROUND,
			DEFAULT_BACKGROUND, display);            
                             
  XtManageChild(arrowsLabel);     

                     
  /*=======================================================================
  ! Now lets realize everything, allocate the CD, setup our timer
  ! (typically to run once per second), and go into our event loop
  !=======================================================================*/
  
  XtRealizeWidget (toplevel);	/* Lets bring all the widgets up	*/
  
  init_icon();			/* Setup the Cd-player icon		*/
  
  alloc_cd_channel();		/* Allocate a channel for the CD	*/
  
  setup_default_block_size();
  
  the_timer ();			/* And start the clock timer		*/

  update_time_scale_max();	/* Make sure time_scale is correct	*/
  
  enable_eject ();		/* Allow eject until unit is started	*/
                   
  init_volume_and_balance ();	/* Get current volume-levels from CD	*/
				/* And set the balance/volume sliders	*/
  
  XtMainLoop ();                                                

}

/*                              END MAIN ROUTINE                            */
/*==========================================================================*/
/*==========================================================================*/
/*==========================================================================*/
/*==========================================================================*/

            
/*============================================================================
!  init_icon Routine.
!  Initialize/select the CD-reader icon.  The icons that have been included
!  in this program are of equal width and height, and the code assumes this
!  (i.e. if icon changes are made, then coding changes may also have to
!  be made).
!============================================================================*/
                            
void	init_icon()                                                      

{
  int		i,
		icon_size;
  Arg	      	ArgL [2];                               
  Window	icon_window_id = XtWindow(main_window);


  icon_size = get_icon_size();			/* 0=none set		*/

  if (icon_size == 0 ||
      icon_size < MEDIUM_ICON_WIDTH)	    	/* Use smallest icon	*/
    {
    cd_icon = XCreateBitmapFromData(display,
    			icon_window_id,
    			small_icon_bits,                  
    			SMALL_ICON_WIDTH,
    			SMALL_ICON_HEIGHT);
    }
  else
    {
    if (icon_size < LARGE_ICON_WIDTH)		/* Use medium icon	*/
      {
      cd_icon = XCreateBitmapFromData(display,
    			icon_window_id,
    			medium_icon_bits,                  
      			MEDIUM_ICON_WIDTH,
    			MEDIUM_ICON_HEIGHT);
      }
    else					/* Use largest icon	*/
      {
      cd_icon = XCreateBitmapFromData(display,
    			icon_window_id,
    		    	large_icon_bits,                  
      			LARGE_ICON_WIDTH,
    			LARGE_ICON_HEIGHT);
      }
    }

  i = 0;
  XtSetArg (ArgL[i], XmNiconPixmap, cd_icon);	i++;	/* Set PIXMAP	*/
  XtSetArg (ArgL[i], XmNiconName, ICON_LABEL);	i++;	/* And label	*/
  XtSetValues (toplevel, ArgL, i);
     
}                                    

     
/*============================================================================
!  get_icon_size Routine.
!  Get the size of the icon in-use.  Return the largest dimension.
!============================================================================*/

int get_icon_size ()

{
  XIconSize	*size_list;
  int		num_sizes,
		status;
  Window	root_window = XDefaultRootWindow (display);

  status = XGetIconSizes (display, root_window, &size_list, &num_sizes);

  if (status == 0)			/* If no icon-size set then	*/
    {					/* return 0.  Otherwise return	*/
    return 0;				/* the greater dimension...	*/
    }					/* icon-width or icon-height	*/
  else
    {
    if (size_list->max_width > size_list->max_height)
      {
      return size_list->max_width;
      }                           
    else
      {
      return size_list->max_height;
      }
    }
 
}

                         
/*============================================================================
! Create_Button Routine.
! Routine to create the Push or Arrow Buttons on the CD.  The widget-name
! is returned.  Note that X & Y locations are only used if > 0.
!============================================================================*/

Widget Create_Button (
	Widget 	theParent,		/* Name of parent		*/
	char	theName[],		/* Name of the button		*/
	int	button_type,		/* Push or Arrow left/right	*/
       	int	xLoc,			/* X location in window		*/
	int	yLoc,			/* Y location in window		*/
	int    	height,			/* Height (in pixels) of button	*/
       	int	width, 			/* Width (in pixels) of button	*/
	char	f_color[],  		/* Foreground color 		*/
	char	b_color[], 		/* Background color 		*/
	Display *dpy)			/* Display id			*/   
                                     
{	
  Widget	wgt;			/* Widget instance produced	*/
  Pixel		color;			/* Pixel color fore/back ground	*/
  Arg	   	ArgL[20];		/* Argument list	 	*/
  int		i;

  i = 0;				/* Init arg count		*/
                                                      
  if (xLoc > NOT_USED) {
    XtSetArg( ArgL[i], XmNx, xLoc); ++i;	/* X position for button*/
  }

  if (yLoc > NOT_USED) {
    XtSetArg( ArgL[i], XmNy, yLoc); ++i;	/* Y position for button*/
  }
  
  XtSetArg( ArgL[i], XmNheight, height); ++i;	/* Height (in pixels)	*/
  XtSetArg( ArgL[i], XmNwidth, width); ++i;	/* Width  (in pixels)	*/
  
  color = String_To_Pixel (f_color);		/* Get (pixel) color	*/
  XtSetArg( ArgL[i], XmNforeground,color); ++i;	/* Set foreground color	*/
                                         
  color = String_To_Pixel (b_color);		/* Get (pixel) color 	*/
  XtSetArg( ArgL[i], XmNbackground,color); ++i;	/* Set background color	*/
                       
  color = String_To_Pixel (BOTTOM_SHADOW_COLOR);
  XtSetArg( ArgL[i], XmNbottomShadowColor, color); ++i;
                       
  color = String_To_Pixel (TOP_SHADOW_COLOR);
  XtSetArg( ArgL[i], XmNtopShadowColor, color); ++i;

  XtSetArg( ArgL[i], XmNshadowThickness, SHADOW_THICKNESS); ++i;
  
  XtSetArg( ArgL[i], XmNrecomputeSize, True); ++i;
                     
  if (button_type == PUSH_BUTTON)	/* Standard PushButton?		*/
    {					/*   (Yes)			*/
      wgt = XmCreatePushButton(			/* Create Pushbutton	*/
      	theParent,				/* Parent		*/
      	theName,				/* Name			*/
	ArgL, i);		   	     	/* Arglist, Arg count	*/
    }
  else					/*   (No)			*/
    {					/*   Must be an Arrow Button.	*/
      if (button_type == ARROW_RIGHT) 		/* set RIGHT direction	*/
	{
	  XtSetArg( ArgL[i], XmNarrowDirection,
		XmARROW_RIGHT); ++i;          
	}   
      else					/* (or)			*/
	{      					/* set LEFT  direction	*/
	  XtSetArg( ArgL[i], XmNarrowDirection,
		XmARROW_LEFT); ++i;    
	} 
						/* (Now)		*/
      wgt = XmCreateArrowButton(		/* Create Arrow Button	*/
	theParent,				/* Parent		*/
      	theName,				/* Name			*/
	ArgL, i);	   	     		/* Arglist, Arg count	*/
    }

  return(wgt);				/* Return pushbutton/widget.	*/
} 

                                 
/*============================================================================
! Create_Scale Routine.
! Add a scale widget to show current TRACK-SELECTED on the CD.
!   Inputs:	parent, name, title/label, X, Y, Xsize, Ysize,
!		scale-min, scale-max, initial scale value,
!		bg color, fg color,
!		bottom shadow color, top shadow color,
! 	     	display-id
!============================================================================*/
                
Widget Create_Scale (   
	Widget	theParent,		/* Name of parent		*/
	char	theLabel[],		/* Name of the scale		*/
	char	theTitle[],		/* Title/label of the scale	*/
  	int	xLoc,			/* X location in window		*/
	int	yLoc,			/* Y location in window		*/
       	int	sXSize,			/* Width of the scale (pixels)	*/
       	int	sYSize,			/* Heightof the scale (pixels)	*/
	int	sMin,			/* Scale minimum value		*/
	int	sMax,			/* Scale maximum value		*/
	int	sInit,			/* Scale initial value		*/
	char	sf_color[],  		/* Foreground color 		*/
	char	sb_color[], 		/* Background color 		*/
	char	bs_color[], 		/* Bottom shadow color		*/
	char	ts_color[], 		/* Top    shadow color		*/
	int	s_orientation,		/* vertical or horizontal	*/
	Display *dpy)			/* Display id			*/
  
{	
  Widget	wgt;			/* Widget instance produced	*/
  Pixel		color;			/* Pixel color			*/
  XmString	Ctitle;			/* Compound-string for title	*/
  Arg	   	ArgL[20];		/* Argument list	 	*/
  int		i;

  i = 0;				/* Init arg count		*/
                                           
  XtSetArg( ArgL[i], XmNsensitive, True); ++i;	/* Allow input		*/
  
  XtSetArg( ArgL[i], XmNx, xLoc); ++i;	/* X position for scale.	*/
  XtSetArg( ArgL[i], XmNy, yLoc); ++i;	/* Y position for scale.	*/
                                       
  XtSetArg( ArgL[i], XmNscaleWidth, sXSize); ++i;/* scale Width		*/
  XtSetArg( ArgL[i], XmNscaleHeight, sYSize); ++i;/* scale Height	*/
  XtSetArg( ArgL[i], XmNshowValue, True); ++i;	/* Do show value	*/
                               
  XtSetArg( ArgL[i], XmNminimum, sMin);	++i;	/* Min scale		*/
  XtSetArg( ArgL[i], XmNmaximum, sMax); ++i;	/* Max scale		*/
  XtSetArg( ArgL[i], XmNvalue,  sInit);	++i;	/* Initital scale value	*/

  color = String_To_Pixel (sf_color);		/* Get (pixel) color	*/
  XtSetArg( ArgL[i], XmNforeground,color); ++i;	/* Set foreground color	*/

  color = String_To_Pixel (sb_color);		/* Get (pixel) color 	*/
  XtSetArg( ArgL[i], XmNbackground,color); ++i;	/* Set background color	*/

  color = String_To_Pixel (bs_color);		/* Get (pixel) color 	*/
  XtSetArg( ArgL[i], XmNbottomShadowColor,color); ++i;
 
  color = String_To_Pixel (ts_color);		/* Get (pixel) color 	*/
  XtSetArg( ArgL[i], XmNtopShadowColor,color);	++i;
  XtSetArg( ArgL[i], XmNshadowThickness, SHADOW_THICKNESS);  ++i;
                         
  Ctitle = XmStringCreateSimple (theTitle);	/* Compound-str title	*/
  XtSetArg( ArgL[i], XmNtitleString, Ctitle); ++i;  /* Set Scale title	*/

  if (s_orientation == VERTICAL_SCALE)                    
    {
    XtSetArg( ArgL[i], XmNorientation, XmVERTICAL);	++i;
    }
  else
    {
    XtSetArg( ArgL[i], XmNorientation, XmHORIZONTAL);	++i;
    }
                                                  
  wgt = XmCreateScale(			/* Create Scale			*/
      	theParent,			/* Parent			*/      
      	theLabel,			/* Name				*/      
	ArgL, i);		   	/* Arglist, Arg count		*/      

  XmStringFree (Ctitle);		/* Free up memory for title 	*/
       
  return(wgt);				/* Return scale/widget		*/
}                                        
       
                        
/*============================================================================
! Create_Label Routine.
! Add a label widget.  Inputs to this routine are:
!   Parent, Name of widget, Label, X, Y, height, width, colors & display-id.
!============================================================================*/

Widget Create_Label (   
  	Widget	theParent,		/* Name of parent		*/
	char  	theName[],		/* Name of the label		*/
	char	theLabel[],		/* Text for the label		*/
	int	xLoc,	       		/* X location in window		*/
     	int	yLoc,			/* Y location in window		*/
	int	height,			/* height of label.		*/
  	int	width,			/* width of the label		*/
	char	f_color[],  		/* Foreground color 		*/
	char	b_color[], 		/* Background color 		*/
	Display *dpy)			/* Display id			*/
{	
  Widget	wgt;			/* Widget instance produced	*/
  Pixel		color;			/* Pixel color			*/
  XmString	Clabel;			/* Compound-string label	*/
  Arg	   	ArgL[10];     		/* Argument list	 	*/
  int		i;

  i = 0;

  color = String_To_Pixel (f_color);
  XtSetArg( ArgL[i], XmNforeground, color); ++i;
                                     
  color = String_To_Pixel (b_color);
  XtSetArg( ArgL[i], XmNbackground, color); ++i;
       
  XtSetArg( ArgL[i], XmNx, xLoc);  ++i;		/* X position 		*/
  XtSetArg( ArgL[i], XmNy, yLoc);  ++i;		/* Y position 		*/
  XtSetArg( ArgL[i], XmNheight, height);++i;	/* Height (in pixels)	*/
  XtSetArg( ArgL[i], XmNwidth, width);	++i;	/* Width  (in pixels)	*/
                       
  XtSetArg( ArgL[i], XmNlabelType, XmSTRING); ++i;

  Clabel = XmStringCreateSimple (theLabel);	/* Compound-str label	*/
  XtSetArg( ArgL[i], XmNlabelString, Clabel); ++i;	/* Set string	*/
       
  wgt = XmCreateLabel(			/* Create Label Widget instance	*/
	theParent,	       		/* Parent			*/
       	theName,		       	/* Name				*/
	ArgL, i);			/* Arglist, Arg count		*/

  XmStringFree (Clabel);		/* Free up memory from string	*/

  return(wgt);				/* Return Label widget		*/
}

    
/*============================================================================
! String_To_Pixel Routine.
! This routine returns a pixel value when passed a named color.
!   Note:  Variable "display" has been previously declared.
!============================================================================*/

String_To_Pixel(char *spec)

{
  int i;
  Colormap cmap;               
   
  XColor color_struct;

  cmap = XDefaultColormap(display, DefaultScreen(display));

  i    = XParseColor(display, cmap, spec, &color_struct);
  i    = XAllocColor(display, cmap, &color_struct);
                          
  return(color_struct.pixel);
}

                                       
/*============================================================================
! handle_eject_button Routine.  Eject the current cd playing.
!============================================================================*/
                                          
void handle_eject_button(        
  	Widget wgt,
    	caddr_t client_data,
  	XmAnyCallbackStruct *call_data)
{
  play_flag 	= FALSE;	/* Set flag to show not started		*/
  cd_paused	= FALSE;	/* CD is not paused			*/
  shuffle_flag	= FALSE;	/* Not in shuffle mode			*/
  enable_eject();		/* Allow the CD to program-eject	*/
  stop_unit();			/* Spin down the CD (if playing)	*/
  eject_cd();			/* Eject the CD				*/
  reset_pause_button();		/* Set label to PAUSE & flag = FALSE	*/
  init_track_scale();		/* Init "track now playing" display	*/
}

                                          
/*============================================================================
! handle_stop_button Routine.  Stop the current cd playing.
!============================================================================*/
                                          
void handle_stop_button(
  	Widget wgt,
    	caddr_t client_data,
  	XmAnyCallbackStruct *call_data)                                   
{
  play_flag 	= FALSE;	/* Set flag to show not started		*/
  cd_paused	= FALSE;	/* CD is not paused			*/
  shuffle_flag	= FALSE;	/* Not in shuffle mode			*/
  stop_unit();			/* Spin down the CD (if playing)	*/
  reset_pause_button();		/* Set label to PAUSE & flag = FALSE	*/
  enable_eject();		/* Allow  removal of CD			*/
}          

          
/*============================================================================
! handle_play_button Routine.  Start CD on track 1.
!============================================================================*/
                                          
void handle_play_button(
	Widget wgt,
    	caddr_t client_data,
	XmAnyCallbackStruct *call_data)
{                                                        
  play_flag 	= TRUE;		/* Set flag to show now-playing		*/
  cd_paused	= FALSE;	/* CD is not paused			*/
  shuffle_flag	= FALSE;	/* Not in shuffle mode			*/
  reset_pause_button();		/* Make sure CD is not paused		*/
  get_status();			/* Get information about current CD	*/
  XmScaleGetValue (track_scale, &current_track);  /* Get track #	*/
  play_track(current_track);	/* Start the CD playing			*/
  set_volume_and_balance();	/* Set volume and balance levels on CD	*/
  disable_eject();		/* Disable CD eject.			*/
  update_time_scale_max();	/* Set time-scale to correct setting	*/
}

          
/*============================================================================
! handle_pause_button Routine.
! Pause the current CD, and change the button label from "PAUSE" to "RESUME"
! (or) if on "RESUME" then start CD and change button label to "PAUSE".
!============================================================================*/
                                          
void handle_pause_button(
	Widget wgt,
    	caddr_t client_data,                             
	XmAnyCallbackStruct *call_data)
{ 

  if (play_flag  == TRUE)
    {                       
    if (cd_paused == FALSE)	/* Were we paused already?		*/
      {				/* (No)					*/
      cd_paused = TRUE;		/* Set Flag				*/
      toggle_pause_label();	/* Change the button label		*/
      pause_cd();		/* Send PAUSE command to CD		*/
      } 
    else                          
      {				/* (Yes -- we were paused already)	*/
      cd_paused = FALSE;	/* Reset the flag			*/
      toggle_pause_label();	/* Change the button label		*/
      resume_cd();		/* Send RESUME command to CD		*/
      }
    }
} 

          
/*============================================================================
! handle_replay_button Routine.  Replay the current track on the CD.
!============================================================================*/
                                          
void handle_replay_button(
	Widget wgt,
    	caddr_t client_data,
	XmAnyCallbackStruct *call_data)

{                    

  shuffle_flag = FALSE;		/* Not in shuffle mode			*/

  if (play_flag  = TRUE && cd_paused == FALSE)
    {
    play_track(current_track);	/* Play the current track again		*/
    set_volume_and_balance();	/* Set volume and balance levels on CD	*/
    disable_eject();		/* Disable removal of CD		*/
    }
}
    
          
/*============================================================================
! handle_shuffle_button Routine.   
! Play tracks in a random order.  Keep playing in a random order until some
! other button is pushed, or until total_tracks * n are played.
!===========================================================================*/
                                          
void handle_shuffle_button(                                        
	Widget wgt,
    	caddr_t client_data,                       
	XmAnyCallbackStruct *call_data)

{   

  if (play_flag  == TRUE && cd_paused == FALSE)  
    {
      shuffle_flag = TRUE;
      do_shuffle();			/* Play a random track		*/
    }
}
                
          
/*===========================================================================
! handle_exit_button Routine.  Used to exit the program.
!===========================================================================*/
                                          
void handle_exit_button(
	Widget wgt,
    	caddr_t client_data,            
	XmAnyCallbackStruct *call_data)
                               
{
    XtCloseDisplay(XtDisplay(wgt));	/* Get rid of CD panel display	*/
    enable_eject();			/* Allow the CD to eject	*/
    stop_unit();			/* Stop Playing                 */
    dealloc_cd_channel();		/* Deassign the channel		*/
    exit(0);				/* Bye-Bye			*/
}


/*============================================================================
! handle_track_decr Routine.
! Callback for the << arrow button.  Decrement the track count by one, and
! update the display.  If track count is less than track-min, then set to
! track-max.
!============================================================================*/

void handle_track_decr 	(
	Widget wgt,
	caddr_t client_data,
  	XmAnyCallbackStruct *call_data)
                           
{                                
  int	this_track,               
	next_track;
                       
  shuffle_flag = FALSE;			/* Not in shuffle-mode		*/
                       
  if (play_flag  == TRUE && cd_paused == FALSE)  
    {
      XmScaleGetValue (track_scale, &this_track);
                                 
      next_track = --this_track;  

      if (next_track < 1)
        {          
          next_track = total_tracks;	
        }     
                                            
      play_track (next_track);		/* Start track playing		*/
      set_volume_and_balance();		/* Set vol & bal levels on CD	*/
    }                  
}


/*============================================================================
! handle_track_incr Routine.                      
! Callback for the >> arrow button.  Increment the track count by one, and
! update the select-track display. If track count is greater than track-max,
! then set to track-min.
!============================================================================*/
                       
void handle_track_incr	(  
	Widget	wgt,                                  
	caddr_t	client_data,
	XmAnyCallbackStruct	*call_data)
                   
{                                
  int 	this_track,              
	next_track;

  shuffle_flag = FALSE;			/* Hot in shuffle-mode		*/
              
  if (play_flag  == TRUE && cd_paused == FALSE)  
    {
      XmScaleGetValue (track_scale, &this_track);
  
      next_track = this_track + 1;

      if (next_track > total_tracks)
        {
          next_track = 1;
        }     

      play_track (next_track); 		/* Start track playing	      	*/
      set_volume_and_balance();		/* Set vol & bal levels on CD	*/
    }
}
                       

/*============================================================================
! handle_time_scale Routine.                      
! Callback for track-time slider input.
!============================================================================*/

void handle_time_scale (  
	Widget	wgt,                                  
	caddr_t	client_data,
	XmAnyCallbackStruct	*call_data)
                     
{                                

}                       


/*============================================================================
! handle_track_scale Routine.                      
! Callback for track-slider input.  Get new slider value, and update the
! CD playing.  This allows the slider to be used for input, as well as output.
!============================================================================*/
      
void handle_track_scale (  
	Widget	wgt,                                  
	caddr_t	client_data,
	XmAnyCallbackStruct	*call_data)
                     
{                                
  int	this_track;

  shuffle_flag = FALSE;			/* Hot in shuffle-mode		*/

  if (play_flag  == TRUE && cd_paused == FALSE)  
    {             
      XmScaleGetValue (track_scale, &this_track);
      play_track (this_track);		/* Start track playing		*/
      set_volume_and_balance();		/* Set vol & bal levels on CD	*/
    }
}
  
  
/*============================================================================
! handle_volume_scale Routine.                      
! Callback for volume-slider input.  Get the slider-value from the display
! panel, and update the actual volume-level in the cd-player.  Note that
! the volume-slider starts at 0 (i.e. VOLUME_DEFAULT - VOLUME_MINIMUM) while
! the actual volume-level put into the CD starts at VOLUME_MINIMUM.
!============================================================================*/
                   
void handle_volume_scale (  
	Widget	wgt,                                  
	caddr_t	client_data,
  	XmAnyCallbackStruct	*call_data)

{                                
  set_volume_and_balance();		/* Set vol & bal levels on CD	*/
} 


/*============================================================================
! handle_balance_scale Routine.                      
! Callback for balance-slider input.  Get the slider-value from the display
! panel, and set the balance between the left and right channels
! (i.e. channels 0 & 1).
!============================================================================*/
                   
void handle_balance_scale (  
	Widget	wgt,                                  
	caddr_t	client_data,
  	XmAnyCallbackStruct	*call_data)
          
{                                                      
  set_volume_and_balance();		/* Set vol & bal levels on CD	*/
}
  

/*============================================================================
! the_timer Routine.
! This routine is a timer which runs, gets the track-time status, updates
! the track-time, and then registers itself as a timeout callback to be
! called again.  This routine is executed once every TIMER_INTERVAL, which
! is typically every one second.
!
! The routine also handles automatic track SHUFFLE after the initial
! button-press.
! 
! Note:	The CD-player will automatically go to the next track after the
!	current track is finished playing.
!============================================================================*/
      
void the_timer ()
                           
{                     
  if (shuffle_flag == TRUE)			/* Are we shuffling CD	*/
    {						/* (yes)... did the CD	*/
      if (current_track != random_track)	/* player shift to next	*/
        {					/* track? (yes)... get	*/
        do_shuffle();				/* another random one	*/
        random_total = random_total + 1;	/* BUT... let's not	*/
        if (random_total >= total_tracks*4)	/* play on forever	*/
          {           
          shuffle_flag = FALSE; 
          random_total = 0;
          stop_unit();
          }           
        }
    }    

  get_status();					/* Get CD status	*/
  update_track_info();				/* Inc track/time scale	*/
  update_track_time();				/* and track-time clock*/
  get_volume();					/* Get current volume	*/
  XtAddTimeOut (TIMER_INTERVAL, the_timer);	/* & reset timeout	*/
} 
     

/*============================================================================
! update_track_time Routine.
! This is the common routine for updating the track-time display.
! It is called by the one second timer routine as well as by the button
! callback routines. If the player state has changed, then update the
! appropriate button/slider on the display.
!============================================================================*/
            
void update_track_time ()                                                 
                 
{ 
  Arg		ArgL[5];                                              
  char		track_time_char[6];
  XmString	the_track_time;
  int		track_time_min,
		time_remaining,
		max_time,
  		i = 0;  
     

  /*=====================================================================
  ! 1st lets update the track-time display                               
  !====================================================================*/

	track_time_min = track_time_sec / 60;	/* Figure out time	*/
	track_time_char [0] = track_time_min / 10 + '0';
	track_time_char [1] = track_time_min % 10 + '0';
	track_time_char [2] = ':';
	track_time_char [3] = (track_time_sec % 60) / 10 + '0';
	track_time_char [4] = (track_time_sec % 60) % 10 + '0';
	track_time_char [5] = '\0';
                                    
	the_track_time = XmStringCreateSimple (track_time_char);           
						/* cvt to compound str	*/

	XtSetArg( ArgL[i], XmNlabelString, the_track_time); ++i;
						/* Set arg for new time	*/
	XtSetValues(trackTime, ArgL, i);	/* Update time-display	*/
                         
	XmStringFree (the_track_time);		/* Recover memory	*/

   /*=====================================================================
   ! And now the track-time scale
   !====================================================================*/

	time_remaining = Track_Seconds [current_track] - track_time_sec ;
	XmScaleGetValue (time_scale, &max_time);  /* Get scale-max	*/

	if (time_remaining > max_time)		/* Make sure we don't	*/
	  {					/* exceed scale		*/
	  i = 0;
	  XtSetArg( ArgL[i], XmNmaximum, time_remaining); i++;
	  XtSetValues(time_scale, ArgL, i);	/* Do display update	*/
	  }

  	XmScaleSetValue (time_scale, time_remaining);

}                               
                         
                                                
/*============================================================================
! update_time_scale_max Routine.                 
! Update/reset the maximum value on the time_scale.
!============================================================================*/
                                     
void update_time_scale_max()

{                                              
  int		i = 0;
  Arg	 	ArgL[5];

  XtSetArg( ArgL[i], XmNminimum, 0);		++i;
  XtSetArg( ArgL[i], XmNvalue,   0);		++i;

  if (Track_Seconds[current_track] > 0 && current_track > 0)
    {
    XtSetArg( ArgL[i], XmNmaximum, Track_Seconds[current_track]); ++i;
    }
  else
    {
    XtSetArg( ArgL[i], XmNmaximum, 999);	++i;
    }

  XtSetValues(time_scale, ArgL, i);	/* Do display update	*/
}
  
/*============================================================================
! update_track_max Routine.                 
! Update the label that shows the max track # on the current CD.
!============================================================================*/
     
void update_track_max()

{                                              
  int		i;
  Arg		ArgL[5];
  char		track_max_char[3];
  XmString	the_track_max;

  track_max_char[0] = (total_tracks/10) + '0';	/* Stuff into string	*/
  track_max_char[1] = total_tracks % 10 + '0';
  track_max_char[2] = '\0';

  the_track_max = XmStringCreateSimple (track_max_char);           
						/* cvt to compound str	*/

  i = 0;
  XtSetArg( ArgL[i], XmNlabelString, the_track_max); ++i;
						/* Set arg for new time	*/
  XtSetValues(trackMax, ArgL, i);		/* Update track-max dpy	*/
              
  XmStringFree (the_track_max);			/* Recover memory	*/
}
            
  
/*============================================================================
! update_track_info Routine.                 
! This is the common routine that does the following:
!   1)  Updates display if necessary.
!   2)  Check to see if old CD has been removed (and updates display).
!   3)  If there is a new CD... we save the track time for each track.
!============================================================================*/
     
void update_track_info()

{                                              
  Arg	ArgL[5];			/* Argument list	 	*/
  int	i = 0,				/* Argument counter		*/
	this_track;			/* Track # from track_scale	*/


  /* Make sure track_scale reflects the current track playing
  !==========================================================*/
  XmScaleGetValue (track_scale, &this_track);

  if (this_track != current_track)
    {
    XmScaleSetValue (track_scale, current_track);
    }                       

  /* If the old CD is gone then make sure track-scale max is still ok
  !==================================================================*/
           
  if (saved_total_tracks != total_tracks)	/* We've removed old CD	*/
    {
      if (total_tracks == 0)			/* No CD		*/
  	{
	  saved_total_tracks = 0;
	  init_track_scale();                
	  update_track_max();
	}     
      else					/* New CD		*/
	{         
	  saved_total_tracks = total_tracks;
	  i = 0;
      	  XtSetArg( ArgL[i], XmNmaximum, total_tracks); ++i;
	  XtSetValues(track_scale, ArgL, i);	/* Do display update	*/

	  save_track_time ();		/* Save track-time for each	*/
					/* track on CD (Track_Time[])	*/
	  update_track_max();		/* Display total-tracks on CD	*/
	}
    }
}                                
  
                                                       
/*============================================================================
! init_track_scale Routine.
! Initialize the scale (i.e. set track-slider to initial values).
!============================================================================*/

void init_track_scale ()    

{
  Arg		ArgL[5];  		/* Argument list	 	*/
  int		i, 			/* Argument counter		*/
		track_max;		/* Top-end for the scale.	*/

  i = 0;
  
  if (total_tracks > CD_TRACK_MIN)	/* If there's a CD mounted...	*/
    {					/* then use its track-count	*/
      track_max = total_tracks;		/* to define the scale		*/
    }
  else
    {
      track_max = CD_TRACK_MAX;
    }                
                     
  /* Update scale	*/
  /*====================*/
    XtSetArg( ArgL[i], XmNminimum, CD_TRACK_MIN);	++i;
    XtSetArg( ArgL[i], XmNvalue,   CD_TRACK_MIN);	++i;  
    XtSetArg( ArgL[i], XmNmaximum, track_max);		++i;                  
    XtSetValues (track_scale, ArgL, i);
    current_track = CD_TRACK_MIN;
}   
                  
                             
/*============================================================================
! init_volume_and_balance routine
! Initialize the balance and volume sliders.  Try and use the current settings
! off of the cd-player itself.  Note:  get_volume() modifies the variables
! current_volume, left_volume, and right_volume.
!============================================================================*/

void init_volume_and_balance ()

{
  int	L_volume,
	R_volume,
	C_volume,
	balance_setting;

  get_volume ();  			/* Get values off of cd-player	*/
  L_volume = left_volume;
  R_volume = right_volume;
  C_volume = current_volume;

  if (L_volume < VOLUME_MINIMUM)	/* We need default values for	*/
	L_volume = VOLUME_DEFAULT;	/* slider calculations		*/

  if (R_volume < VOLUME_MINIMUM)
      	R_volume = VOLUME_DEFAULT;

  if (C_volume < VOLUME_MINIMUM)
      	C_volume = VOLUME_DEFAULT;


  /*---------------------------------------------------
  ! Calculate the correct balance-slider setting from
  ! the actual volume levels gotten from the cd-player.
  !----------------------------------------------------*/
  if (L_volume > R_volume)
    {
    balance_setting = -BALANCE_SCALE_MAX +
			(BALANCE_SCALE_MAX * (R_volume - VOLUME_MINIMUM))/   
    		       	(L_volume  - VOLUME_MINIMUM);
    }
  else
    {
    if (L_volume < R_volume)
      {               
      balance_setting = BALANCE_SCALE_MAX - 
			(BALANCE_SCALE_MAX * (L_volume - VOLUME_MINIMUM))/
		     	(R_volume - VOLUME_MINIMUM); 
      }    
    else                                    
      {
      balance_setting = 0;
      }
    }

  /*---------------------------------------------------
  ! Now set the balance and volume sliders.
  !----------------------------------------------------*/
  XmScaleSetValue (balance_scale, balance_setting);

  XmScaleSetValue (volume_scale, C_volume - VOLUME_MINIMUM );
}


/*============================================================================
! set_volume_and_balance Routine.                      
! Get the balance-level, and the volume-level, and set the CD accordingly.
! Note that the volume is set to (VOLUME_MINIMUM + volume_scale), unless
! scale is zero, and then volume is set to zero.
! (i.e. channels 0 & 1).
!============================================================================*/

void set_volume_and_balance()          

{
int	new_balance,
	new_volume,
	L_volume,
	R_volume;

  XmScaleGetValue (volume_scale,  &new_volume);		/* Get volume	*/

  if (new_volume == 0)			/* If scale is 0... then turn	*/
    {					/* volume all the way off	*/
    current_volume = VOLUME_OFF;
    left_volume    = VOLUME_OFF;
    right_volume   = VOLUME_OFF;
    }
  else					/* Else set to minimum + scale	*/
    { 
    current_volume = new_volume + VOLUME_MINIMUM;

    XmScaleGetValue (balance_scale, &new_balance);	/* Get balance	*/

    if (new_balance <= 0)		/* Convert to volume levels for	*/
      {					/* left and right channels	*/
      L_volume = current_volume;                      
      R_volume = current_volume +
  	 	(((current_volume - VOLUME_MINIMUM)
		* new_balance)/ BALANCE_SCALE_MAX );
      }       
    else                                           
      {
      L_volume = current_volume -
  		(((current_volume - VOLUME_MINIMUM)
		* new_balance)/ BALANCE_SCALE_MAX );
      R_volume = current_volume;                      
      }
    left_volume = L_volume;		/* Save left-channel  volume	*/
    right_volume= R_volume;		/* Save right-channel volume	*/
    }

  set_volume(left_volume, right_volume);/* Go out and actually change	*/
					/* the volume on the CD-player	*/

}
     
     
/*============================================================================
! reset_pause_button Routine.
! Reset the PAUSE button (i.e. make sure it says PAUSE, and make sure that
! "cd_paused" is set to FALSE.
!============================================================================*/
  
void reset_pause_button ()

{
  Pixel		color;			/* Needed for changing bg color	*/
  XmString	Cpause;			/* Compound-str for pause label	*/
  Arg	   	ArgL[5];		/* Argument list	 	*/
  int		i;     			/* Arg count			*/

  i = 0;	       			/* Init arg count		*/
                  
  cd_paused = FALSE;			/* Reset flag			*/
  color = String_To_Pixel (DEF_PUSHBUTTON_BACKGROUND); /* Get (pixel) color*/   
  XtSetArg( ArgL[i], XmNbackground,color); ++i;	/* Reset color		*/
  Cpause = XmStringCreateSimple (" PAUSE ");	/* Cvt to compound-str	*/
  XtSetArg( ArgL[i], XmNlabelString, Cpause); ++i;  /* Reset to "PAUSE"	*/
  XtSetValues (pause_button, ArgL, i);		/* Actually do changes	*/
  XmStringFree (Cpause);			/* Free memory for str	*/
} 

     
/*============================================================================
! toggle_pause_label Routine.
! Change the pause button from PAUSE to RESUME, or RESUME to PAUSE.
!============================================================================*/
  
void toggle_pause_label ()
                            
{
  Pixel		color;			/* Needed for changing bg color	*/
  XmString	Clabel;			/* Compound string for label	*/
  Arg	   	ArgL[5];	      	/* Argument list	 	*/
  int		i;		       	/* Arg count			*/


  i = 0;	       			/* Init arg count		*/

  if (cd_paused == FALSE)
    {                
      color = String_To_Pixel (DEF_PUSHBUTTON_BACKGROUND);/* Get color 	*/
      XtSetArg( ArgL[i], XmNbackground,color); ++i;	/* Set color	*/
      Clabel = XmStringCreateSimple (" PAUSE ");	/* Create str	*/
      XtSetArg( ArgL[i], XmNlabelString, Clabel); ++i;	/* Set label	*/
    }
  else
    {                
      color = String_To_Pixel (ALT_PUSHBUTTON_BACKGROUND);/* Get  color	*/
      XtSetArg( ArgL[i], XmNbackground,color); ++i;	/* Set color	*/
      Clabel = XmStringCreateSimple ("RESUME");		/* Create str	*/
      XtSetArg( ArgL[i], XmNlabelString, Clabel); ++i;	/* Set label	*/
    }

  XtSetValues (pause_button, ArgL, i);		/* Actually do changes	*/
  XmStringFree (Clabel);			/* Free memory for str	*/
}
                                   

/*============================================================================
! do_shuffle Routine.
! Get a random track between 1 and total_tracks.  Set the current track #
! to that track, and then play the current_track.  Only do a shuffle if a
! CD in the player.
!============================================================================*/

void	do_shuffle ()                  

{

  if (total_tracks > 0)				/* Is CD present?	*/
    {
    random_track = get_random_track();		/* Get any track #	*/
    while (random_track == old_random_track)	/* Make sure it's not	*/
      {						/* the one we just	*/
      random_track = get_random_track();	/* played		*/
      } 
    play_track (random_track);		/* Play the track		*/
    set_volume_and_balance();		/* Set vol & bal levels on CD	*/
    old_random_track = random_track;	/* Save track #			*/
    current_track = random_track;	/* Set current-track #		*/
  }
}

                                
/*============================================================================
! get_random_track Routine.
! Routine which returns a random track number between 1 and
! total_tracks.  Note:  We need a better random # generator (but not now).
!============================================================================*/

int get_random_track()                                                   
                  
{                                               
  random_number = (random_number/(current_track+1))                       
			* 1103515245 + 12345;
  return (((unsigned int) (random_number/65536) % total_tracks)+1); 
}


/*============================================================================
! setup_default_block_size Routine.
! This routine changes the default block size of the CD player
! to 512 bytes and is required for correct callibration of 
! the track time. The mode select command is tried three times
! because if the device has just been reset, the first two will
! fail.                                              
!============================================================================*/

int setup_default_block_size()
  
{                        
  char	mode_select_command [6]	=  {MODE_SELECT_OPCODE, 0x10, 0, 0, 12, 0},
	mode_select_data [12]	= {0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2, 0};
  int i, status;
                                                             
  for (i=0; i<3; i++)
    {                             
  	status = execute_command (mode_select_command,
    			6, mode_select_data, 12, WRITE);
    	if (status) return status;
    	bpt ();
    }

    return status;   
}   


/*============================================================================
! read_toc Routine.
! This routine reads the table of contents on the CD to determine the
! total number of tracks.
!============================================================================*/

int read_toc ()

{
  int	TTL_tracks = 0,
  	status;

  char	read_toc_command [10] =
		{READ_TOC_OPCODE, 0, 0, 0, 0, 0, 0, 0, 4, 0};
  char	toc_data[4];

  if (!execute_command (read_toc_command, 10, toc_data, 4, READ))
    {
    TTL_tracks = 0;    				/* No CD is present	*/
    }
  else  
    TTL_tracks = toc_data[3] - toc_data[2] + 1;

  return TTL_tracks;
}            

                        
/*============================================================================
! read_toc_track Routine.
! This routine reads the table of contents on the CD for the specified track
! and returns the block/address for the start of that track.
!============================================================================*/
                     
unsigned int read_toc_track (int track_num) 

#define READ_THE_TOC  	0X43                             

{
  unsigned char	read_toc_track_command [12] =
      {READ_THE_TOC, 0, 0, 0, 0, 0, 0, 0, 12, 0};
                                           
  unsigned char	toc_track_data[12];

  int track, status;

  unsigned int addr1, addr2, addr3, addr4, the_addr;
                                                                     
  read_toc_track_command[6] = track_num;
  
  if (!execute_command (read_toc_track_command,
			12, toc_track_data, 12, READ)) return FALSE;

  addr4 = toc_track_data[8];              
  addr3 = toc_track_data[9];                                                 
  addr2 = toc_track_data[10];
  addr1 = toc_track_data[11];

  addr4 = addr4 << 24;		/* Do all necessary byte-shifts		*/
  addr3 = addr3 << 16;
  addr2 = addr2 << 8;
  
  the_addr = addr4 + addr3 + addr2 + addr1;	/* Final address	*/

  return the_addr;		/* And return the block #		*/
}            
  

/*============================================================================
! save_track_time Routine.
! This routine saves the track-time for each track of the CD in the array
! Track_Seconds [].
!============================================================================*/
void save_track_time ()                    

{                        
  unsigned int	track_address[CD_TRACK_MAX],
		end_of_CD,  
		track_units;

  int	last_track,
	track,
	ttl_CD_secs,
	CD_secs,
	CD_mins,
	x;

  last_track = read_toc();		/* Get total # of tracks on CD	*/


  /* Save track address for each track on current CD	*/
  /*====================================================*/
    for (track = 1; track <= last_track; track++)
      {
      track_address [track] = read_toc_track (track);
      } 


  /* Now compute track-time (seconds) for each track)	*/
  /*====================================================*/
    for (track = 1; track <= (last_track - 1); track++)	/* Except last	*/
      {
      track_units = track_address[track+1] - track_address[track];
      Track_Seconds[track] = track_units / (75 * 4);
      }  
  
  end_of_CD = read_CD_capacity ();			/* Now last	*/

  if (SHOW_CD_TIME == 1)
    {
    ttl_CD_secs	= end_of_CD / (75 * 4);
    CD_mins	= ttl_CD_secs / 60;
    CD_secs	= ttl_CD_secs - (CD_mins * 60);
    printf ("=========================== \n");
    printf ("Total CD Play Time = %d:%d \n", CD_mins, CD_secs);
    }

  Track_Seconds[last_track] =
		(end_of_CD - track_address[last_track]) / (75 * 4);      
} 


/*============================================================================
! read_CD_capacity Routine.
!============================================================================*/
unsigned int read_CD_capacity ()

#define CD_CAPACITY		0X25

{
  unsigned char	CD_capacity  [10] = {CD_CAPACITY,  0, 0, 0, 0, 0, 0, 0, 0, 0};

  unsigned char	CD_capacity_data[12];                       

  unsigned int addr1, addr2, addr3, addr4, the_addr;

  int x;
 
  if (!execute_command (CD_capacity, 10, CD_capacity_data, 12, READ))
    {
    return FALSE;
    }

  addr4 = CD_capacity_data[0];		/* Extract the necessary data	*/
  addr3 = CD_capacity_data[1];
  addr2 = CD_capacity_data[2];
  addr1 = CD_capacity_data[3];

  addr4 = addr4 << 24;				/* Do byte-shifts	*/
  addr3 = addr3 << 16;
  addr2 = addr2 << 8;

  the_addr = addr4 + addr3 + addr2 + addr1;	/* Compute address	*/
  
  return the_addr;				/* return the block #	*/
}
        

/*============================================================================
! play_track Routine.            
! This routine plays the specified track on the CD.                        
!============================================================================*/

int play_track (int track_num)        
                                         
{
  int	status;

  char	play_track_command [10] =
		{PLAY_TRACK_OPCODE, 0, 0, 0, 0, 1, 0, 0,  1, 0};
                                                      
  play_track_command [4] = track_num;
  play_track_command [7] = total_tracks;
  status = execute_command(play_track_command, 10, 0, 0, READ);
  disable_eject();
  return status;	
}


/*============================================================================
! pause_cd Routine.
! This routine sends a PAUSE command to the CD-player.
!============================================================================*/

int pause_cd ()
{                                                                  
  char	pause_command [10] =   {PAUSE_OPCODE , 0, 0, 0, 0, 0, 0, 0, 0, 0};

  return execute_command (pause_command, 10, 0, 0, READ);
}


/*============================================================================
! resume_cd Routine.
! This routine sends a RESUME command to the CD-player.
!============================================================================*/

int resume_cd ()
{
  char	resume_command [10] = {RESUME_OPCODE, 0, 0, 0, 0, 0, 0, 0, 1, 0};

  return execute_command (resume_command, 10, 0, 0, READ);
}

    
/*============================================================================
! stop_unit Routine.
! This routine sends a stop command the the CD player.
!============================================================================*/

int stop_unit()

{                        
  char	stop_command [6] = {STOP_OPCODE  , 0, 0, 0, 0, 0};

  return execute_command (stop_command, 6, 0, 0, READ);
}


/*============================================================================
! eject_cd Routine.    
! This routine sends an eject-cd command the the CD player.
!============================================================================*/

int eject_cd()

{                        
  char	eject_command [6]   = {STOP_OPCODE  , 0, 0, 0, 2, 0};

  return execute_command (eject_command, 6, 0, 0, READ);
}                          

                 
/*============================================================================
! enable_eject Routine.     
! This routine enables the manual removal of a CD.
!============================================================================*/

int enable_eject()

{                                            
  char	media_enable [6]   = {MEDIA_OPCODE , 0, 0, 0, 0, 0};

  return execute_command (media_enable, 6, 0, 0, READ);
}                               


/*============================================================================
! disable_eject Routine.    
! This routine disables the manual removal of a CD.
!============================================================================*/

int disable_eject()

{                        
  char	media_disable [6]  = {MEDIA_OPCODE , 0, 0, 0, 1, 0};

  return execute_command (media_disable, 6, 0, 0, READ);
}

                                                
/*============================================================================
! get_volume Routine.     
! This routine gets the volume-level from the cd.
!============================================================================*/

void get_volume()
                           
{
                     
  int  	audio_stat	= 0,
	volume_level	= 0,
	volume_ch0	= 0,  
	volume_ch1	= 0,
	status		= 0;                            

  char 	cd_data [14];

  char	playback_status  [10] =
			{PLAYBACK_STATUS_OPCODE,  0, 0, 0, 0, 0, 0, 0, 14, 0};

        
  status = execute_command (playback_status, 10, cd_data, 14, READ);       

  audio_stat = cd_data[4]  & 0377;	/* Play/Eject=0, Stop/Pause=1	*/
  volume_ch0 = cd_data[11] & 0377;	/* Volume ch0 (low-order 8 bits)*/
  volume_ch1 = cd_data[13] & 0377;	/* Volume ch1 (low-order 8 bits)*/

  if (volume_ch0 > volume_ch1)		/* Make sure the volume-level	*/
    {					/* returned is the highest of	*/
    volume_level = volume_ch0;		/* the two channels		*/
    }
  else
    {
    volume_level = volume_ch1;
    }
                          
  left_volume    = volume_ch0;		/* Save left-channel  volume	*/
  right_volume   = volume_ch1;		/* Save right-channel volume	*/
  current_volume = volume_level;	/* And the higher of the two	*/

  if (volume_level > 0 && audio_stat == 0)
    {					/* Assume we are playing a CD	*/
    play_flag  = TRUE;			/* if status is 0, and volume	*/
    }					/* is not 0 (if volume was 0,	*/
					/* then we could be ejected)	*/
/*
  printf ("L= %d    R= %d     Max = %d    Stat= %d\n",
	  left_volume, right_volume, volume_level, audio_stat);
*/

}                               
  

/*============================================================================
! set_volume Routine.     
! This routine changes the volume of the cd-player in both channels 0 & 1.
!============================================================================*/
                                          
void set_volume(int volume_ch0, int volume_ch1)

{                                            
  int	status;

  char	playback_control [10] =
		{PLAYBACK_CONTROL_OPCODE, 0, 0, 0, 0, 0, 0, 0, 14, 0};

  char	cd_data [14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                 
  cd_data [10] = 1;
  cd_data [11] = volume_ch0;
  cd_data [12] = 2;
  cd_data [13] = volume_ch1;

  status = execute_command (playback_control, 10, cd_data, 14, WRITE);
               
}                               

           
/*============================================================================
! get_status Routine.
! This routine reads the current status of the CD player to determine the
! total number of tracks, whether the player is playing, and, if so, the
! current track number being played. If any of this information has changed
! since the last time the screen was updated, then the screen is updated
! again.
!============================================================================*/
      
void get_status()        
                
{
  char	read_subchan_command [10] =
		{READ_SUBCHAN_OPCODE, 0, 0x40, 1, 0, 0, 0, 0, 48, 0};

  char	subchan_data [48];

  int	i,
	track_time_units;
                                       
  total_tracks = read_toc();		/* Get total tracks on this CD	*/

  if (total_tracks == 0 || execute_command (read_subchan_command,        
  	10, subchan_data, 48,  READ) == FALSE)
    {
    current_track = 0;
    track_time_sec = 0;
    }                                       
  else
    { 
    current_track = subchan_data[6];
/*
    if (subchan_data[1] == 0x12) cd_paused = TRUE;
    printf ("Subchannel data: ");
    for (i=0; i<20; i++) printf (" %02x", subchan_data[i] & 0xff);
    printf ("\n");
*/
    track_time_units = (subchan_data[13] & 0xff) * 0x10000 + 
    	    		   (subchan_data[14] & 0xff) * 0x100 + 
    			   (subchan_data[15] & 0xff);    

    /*==================================================================
    ! The following operation converts the units (blocks from
    ! beginning of track) in the subchan_data [13], subchan_data [14],
    ! and subchan_data [15] fields into units of seconds. 
    !==================================================================*/
                        
    track_time_sec = track_time_units / (75 * 4);
            
    if ((track_time_sec != saved_track_time_sec) &&
      	    (track_time_sec != (saved_track_time_sec-1)))
      {     
      saved_track_time_sec = track_time_sec;
      }                                                  

    if (subchan_data[1] == 0x13)
      {
      stop_unit ();
      }
    }

  if (current_track != saved_current_track)
    {
    update_time_scale_max();
    saved_current_track = current_track;
    }
}

                            
/*============================================================================
! bpt Routine.
! This is a null routine (spin-wheels).
!============================================================================*/
void bpt ()
         
{
}


/*============================================================================
! exit_handler Routine.
! Callback routine for exit button. Stop the CD from playing, deassign
! the channel to the player, and exit.
!============================================================================*/

void exit_handler ()                                             
{       
  stop_unit ();           	/* spin down the unit		*/
  dealloc_cd_channel();		/* blow away the channel	*/
  exit(1);			/* go away			*/
}
                     

/*============================================================================
!=============================================================================
!=============================================================================
! Below are the only VMS-SPECIFIC routines in the program:
!============================================================================*/

/*============================================================================
! execute_command Routine.
! This routine sends the specified command to the CD player. It does
! so by filling in the generic class driver descriptor and issuing
! an IO$_DIAGNOSE QIO to GKDRIVER.
!============================================================================*/
                    
int execute_command (
		int *command_addr,
		int command_len,
		int data_addr,
		int data_len,
		int rw_flag)
        

/*=====================================
!  SCSI Generic Class Driver Constants 
!=====================================*/
#define FLAGS_READ		1
#define FLAGS_DISCONNECT	2
#define FLAGS_DISCONNECT	2      
#define GOOD_SCSI_STATUS	0
#define OPCODE			0
#define FLAGS			1                       
#define COMMAND_ADDRESS		2       
#define COMMAND_LENGTH		3        
#define DATA_ADDRESS		4          
#define DATA_LENGTH		5           
#define PAD_LENGTH		6            
#define PHASE_TIMEOUT		7         
#define DISCONNECT_TIMEOUT	8    

{

  char	request_sense_command [6] = {3, 0, 0, 0, 18, 0},
  	request_sense_data [18],
	scsi_status;

  int	gk_iosb[2],
    	gk_desc[15],
	i,
	status;

  gk_desc[OPCODE]		= 1;               
  gk_desc[FLAGS]		= rw_flag + FLAGS_DISCONNECT;
  gk_desc[COMMAND_ADDRESS]	= command_addr;
  gk_desc[COMMAND_LENGTH]	= command_len;
  gk_desc[DATA_ADDRESS]		= data_addr;
  gk_desc[DATA_LENGTH]		= data_len;
  gk_desc[PAD_LENGTH]		= 0;      
  gk_desc[PHASE_TIMEOUT]	= 0;   
  gk_desc[DISCONNECT_TIMEOUT]	= 60;
  
  for (i=9; i<15; i++) gk_desc[i] = 0;	/* Clear reserved fields	*/

/*======================================================================
! Issue the QIO to send the inquiry command and receive the inquiry data
!=======================================================================*/
            
  status = sys$qiow (1, gk_chan, IO$_DIAGNOSE, gk_iosb, 0, 0, 
  		   &gk_desc[0], 15*4, 0, 0, 0, 0);


/*==========================================
!  Check the various returned status values
!===========================================*/
                                                             
  if (!(status & 1)) sys$exit (status);

  if (!(gk_iosb[0] & 1)) sys$exit (gk_iosb[0] & 0xffff);

  scsi_status = (gk_iosb[1] >> 24) & SCSI_STATUS_MASK;

  if (scsi_status == GOOD_SCSI_STATUS) return TRUE;

  if (scsi_status == 2)
    {
    bpt ();
    gk_desc[OPCODE]	= 1;
    gk_desc[FLAGS]	= FLAGS_READ + FLAGS_DISCONNECT;
    gk_desc[COMMAND_ADDRESS]	= request_sense_command;
    gk_desc[COMMAND_LENGTH]	= 6;
    gk_desc[DATA_ADDRESS]	= request_sense_data;
    gk_desc[DATA_LENGTH]	= 18;  
    gk_desc[PAD_LENGTH]		= 0;    
    gk_desc[PHASE_TIMEOUT]	= 0; 
    gk_desc[DISCONNECT_TIMEOUT]	= 60;

    for (i=9; i<15; i++) gk_desc[i] = 0;	/* Clear reserved fields */

    status = sys$qiow (1, gk_chan, IO$_DIAGNOSE, gk_iosb, 0, 0, 
    	   &gk_desc[0], 15*4, 0, 0, 0, 0);
    }

  return FALSE;                                                         
}
  	

/*============================================================================
! alloc_cd_channel Routine.                    
! This routine assigns a channel to the CD player.
!============================================================================*/

void alloc_cd_channel ()
	
{
  int	status,
	gk_device_desc[2];

  gk_device_desc[0] = strlen (gk_device);
  gk_device_desc[1] = &gk_device[0];
  status = sys$assign (&gk_device_desc[0], &gk_chan, 0, 0);
  if (!(status & 1))
    {
    printf ("Unable to assign channel to %s", &gk_device[0]);
    exit (status);
    }       
}


/*============================================================================
! dealloc_cd_channel Routine.
! This routine deassigns a channel to the CD player 
!============================================================================*/

void dealloc_cd_channel ()
	
{
  int	status;

  status = sys$dassgn (gk_chan);
  if (!(status & 1))
    {
    printf ("Unable to deassign channel from %s", &gk_device[0]);
    exit (status);
    }
}

/*===========================================================================*/
/*===============================(END PROGRAM)===============================*/
/*===========================================================================*/
/*===========================================================================*/
/*			     Convenience Routines			     */

#if CONVENIENCE_ROUTINES

/*============================================================================
! play_MSF Routine.            
! This routine plays a segment starting at "sm" minutes and "ss" seconds,
! and goes to "em" minutes and "es" seconds.
!
! Note:	The program doesn't use this routine --  It is presented as an FYI
!	in case the user wants to utilize the MSF calls for the CD.
!============================================================================*/
                          
int play_MSF (	int sm, int ss,
		int em, int es)

#define PLAY_MSF_OPCODE		0X47

{
  unsigned char	play_MSF_command [10] =
			{PLAY_MSF_OPCODE, 0, 0, 0, 0, 0, 0, 0,  0, 0};
  int status;
                                                      
  if (sm == 0 && ss < 3) ss = 3;		/* Play starts at 3 sec	*/

  play_MSF_command [3] = sm;			/* Start minutes	*/
  play_MSF_command [4] = ss;			/* start seconds	*/
  play_MSF_command [5] = 0;			/* start frame		*/
                                                      
  play_MSF_command [6] = em;			/* end minutes		*/
  play_MSF_command [7] = es;			/* end seconds		*/      
  play_MSF_command [8] = 0;			/* end frame		*/

  status = execute_command(play_MSF_command, 10, 0, 0, READ);

  return status;	

}
        

/*============================================================================
! play_partial_track Routine.            
! This routine starts at "x" minutes into the current track, and plays for
! "y" minutes.  Note that if you try and play for more minutes than are
! remaining on the CD (from your start)... that nothing will happen!
! (we call that... being feature-rich)
!
! Note:	The program doesn't use this routine --  It is presented as an FYI
!	to illustrate the use of the MSF format for playing a CD.
!============================================================================*/
                                                                          
int play_partial_track (
		int track_num,
		int start_at_minutes,      
		int start_at_seconds,
		int duration_minutes,
		int duration_seconds)

{
  int	track_minutes	= 0,
	track_seconds	= 0,
	begin_minutes	= 0,
	begin_seconds	= 0,
	end_minutes	= 0,
	end_seconds	= 0;
  unsigned int
	total_seconds	= 0,
	start_here	= 0,
	stop_here	= 0,
	actual_CD_stop	= 0;

  total_seconds = read_toc_track (track_num) / (75*4);
				/* Calcuate track start (in seconds)	*/

  track_minutes = total_seconds / 60;
  track_seconds = total_seconds - (track_minutes * 60);

  begin_minutes = track_minutes + start_at_minutes; 
  begin_seconds = track_seconds + start_at_seconds;
				/* Now add in start offset - mins/sec	*/

  end_minutes = begin_minutes + duration_minutes;
  end_seconds = begin_seconds + duration_seconds;
				/* And compute the end    - mins/sec	*/

  start_here = (begin_minutes*60) + begin_seconds;
  stop_here  = (end_minutes*60)   + end_seconds;

  actual_CD_stop = read_CD_capacity() / (75*4);

  if (start_here > actual_CD_stop)		/* If start is greater	*/
    {		 				/* than CD capacity	*/
    return FALSE;				/* exit on error	*/
    }

  if (stop_here > actual_CD_stop)		/* If end is greater...	*/
    {						/* then set to actual	*/
    end_minutes = actual_CD_stop/60;		/* end			*/
    end_seconds = actual_CD_stop - (end_minutes*60);
    }
                                                                          
  printf ("play_MSF   min=%d    sec=%d    min=%d    sec=%d \n",
		begin_minutes, begin_seconds,
		end_minutes,   end_seconds);

  play_MSF (begin_minutes, begin_seconds,
	    end_minutes,   end_seconds);	/* Now play the segment	*/
}

#endif

/*===========================================================================*/
/*=================================(END ALL)=================================*/
/*===========================================================================*/
