/***********************************************************
*  Rocks'n'Diamonds -- McDuffin Strikes Back!              *
*----------------------------------------------------------*
*  �1995 Artsoft Development                               *
*        Holger Schemel                                    *
*        33659 Bielefeld-Senne                             *
*        Telefon: (0521) 493245                            *
*        eMail: aeglos@valinor.owl.de                      *
*               aeglos@uni-paderborn.de                    *
*               q99492@pbhrzx.uni-paderborn.de             *
*----------------------------------------------------------*
*  buttons.h                                               *
***********************************************************/

#ifndef BUTTONS_H
#define BUTTONS_H

#include "main.h"

/* some positions in the video tape control window */
#define VIDEO_DISPLAY1_XPOS	5
#define VIDEO_DISPLAY1_YPOS	5
#define VIDEO_DISPLAY2_XPOS	5
#define VIDEO_DISPLAY2_YPOS	41
#define VIDEO_DISPLAY_XSIZE	90
#define VIDEO_DISPLAY_YSIZE	31
#define VIDEO_BUTTON_XSIZE	18
#define VIDEO_BUTTON_YSIZE	18
#define VIDEO_CONTROL_XPOS	5
#define VIDEO_CONTROL_YPOS	77
#define VIDEO_CONTROL_XSIZE	(VIDEO_DISPLAY_XSIZE)
#define VIDEO_CONTROL_YSIZE	(VIDEO_BUTTON_YSIZE)
#define VIDEO_BUTTON_EJECT_XPOS	(VIDEO_CONTROL_XPOS + 0 * VIDEO_BUTTON_XSIZE)
#define VIDEO_BUTTON_STOP_XPOS	(VIDEO_CONTROL_XPOS + 1 * VIDEO_BUTTON_XSIZE)
#define VIDEO_BUTTON_PAUSE_XPOS	(VIDEO_CONTROL_XPOS + 2 * VIDEO_BUTTON_XSIZE)
#define VIDEO_BUTTON_REC_XPOS	(VIDEO_CONTROL_XPOS + 3 * VIDEO_BUTTON_XSIZE)
#define VIDEO_BUTTON_PLAY_XPOS	(VIDEO_CONTROL_XPOS + 4 * VIDEO_BUTTON_XSIZE)
#define VIDEO_BUTTON_ANY_YPOS	(VIDEO_CONTROL_YPOS)
#define VIDEO_DATE_LABEL_XPOS	(VIDEO_DISPLAY1_XPOS)
#define VIDEO_DATE_LABEL_YPOS	(VIDEO_DISPLAY1_YPOS)
#define VIDEO_DATE_LABEL_XSIZE	(VIDEO_DISPLAY_XSIZE)
#define VIDEO_DATE_LABEL_YSIZE	(VIDEO_DISPLAY_YSIZE)
#define VIDEO_DATE_XPOS		(VIDEO_DISPLAY1_XPOS+1)
#define VIDEO_DATE_YPOS		(VIDEO_DISPLAY1_YPOS+14)
#define VIDEO_DATE_XSIZE	(VIDEO_DISPLAY_XSIZE)
#define VIDEO_DATE_YSIZE	16
#define VIDEO_REC_LABEL_XPOS	(VIDEO_DISPLAY2_XPOS)
#define VIDEO_REC_LABEL_YPOS	(VIDEO_DISPLAY2_YPOS)
#define VIDEO_REC_LABEL_XSIZE	20
#define VIDEO_REC_LABEL_YSIZE	12
#define VIDEO_REC_SYMBOL_XPOS	(VIDEO_DISPLAY2_XPOS+20)
#define VIDEO_REC_SYMBOL_YPOS	(VIDEO_DISPLAY2_YPOS)
#define VIDEO_REC_SYMBOL_XSIZE	16
#define VIDEO_REC_SYMBOL_YSIZE	16
#define VIDEO_PLAY_LABEL_XPOS	(VIDEO_DISPLAY2_XPOS+65)
#define VIDEO_PLAY_LABEL_YPOS	(VIDEO_DISPLAY2_YPOS)
#define VIDEO_PLAY_LABEL_XSIZE	22
#define VIDEO_PLAY_LABEL_YSIZE	12
#define VIDEO_PLAY_SYMBOL_XPOS	(VIDEO_DISPLAY2_XPOS+50)
#define VIDEO_PLAY_SYMBOL_YPOS	(VIDEO_DISPLAY2_YPOS)
#define VIDEO_PLAY_SYMBOL_XSIZE	13
#define VIDEO_PLAY_SYMBOL_YSIZE	13
#define VIDEO_PAUSE_LABEL_XPOS	(VIDEO_DISPLAY2_XPOS)
#define VIDEO_PAUSE_LABEL_YPOS	(VIDEO_DISPLAY2_YPOS+20)
#define VIDEO_PAUSE_LABEL_XSIZE	35
#define VIDEO_PAUSE_LABEL_YSIZE	8
#define VIDEO_PAUSE_SYMBOL_XPOS	(VIDEO_DISPLAY2_XPOS+35)
#define VIDEO_PAUSE_SYMBOL_YPOS	(VIDEO_DISPLAY2_YPOS)
#define VIDEO_PAUSE_SYMBOL_XSIZE 13
#define VIDEO_PAUSE_SYMBOL_YSIZE 13
#define VIDEO_TIME_XPOS		(VIDEO_DISPLAY2_XPOS+38)
#define VIDEO_TIME_YPOS		(VIDEO_DISPLAY2_YPOS+14)
#define VIDEO_TIME_XSIZE	50
#define VIDEO_TIME_YSIZE	16

#define ON_VIDEO_BUTTON(x,y)	((x)>=(VX+VIDEO_CONTROL_XPOS) &&	\
				 (x)< (VX+VIDEO_CONTROL_XPOS +		\
				       VIDEO_CONTROL_XSIZE) &&		\
				 (y)>=(VY+VIDEO_CONTROL_YPOS) &&	\
				 (y)< (VY+VIDEO_CONTROL_YPOS +		\
				       VIDEO_CONTROL_YSIZE))
#define VIDEO_BUTTON(x)		(((x)-(VX+VIDEO_CONTROL_XPOS))/VIDEO_BUTTON_XSIZE)

/* values for video tape control */
#define VIDEO_STATE_PLAY_OFF	(1L<<0)
#define VIDEO_STATE_PLAY_ON	(1L<<1)
#define VIDEO_STATE_PLAY	(VIDEO_STATE_PLAY_OFF	| VIDEO_STATE_PLAY_ON)
#define VIDEO_STATE_REC_OFF	(1L<<2)
#define VIDEO_STATE_REC_ON	(1L<<3)
#define VIDEO_STATE_REC		(VIDEO_STATE_REC_OFF	| VIDEO_STATE_REC_ON)
#define VIDEO_STATE_PAUSE_OFF	(1L<<4)
#define VIDEO_STATE_PAUSE_ON	(1L<<5)
#define VIDEO_STATE_PAUSE	(VIDEO_STATE_PAUSE_OFF	| VIDEO_STATE_PAUSE_ON)
#define VIDEO_STATE_DATE_OFF	(1L<<6)
#define VIDEO_STATE_DATE_ON	(1L<<7)
#define VIDEO_STATE_DATE	(VIDEO_STATE_DATE_OFF	| VIDEO_STATE_DATE_ON)
#define VIDEO_STATE_TIME_OFF	(1L<<8)
#define VIDEO_STATE_TIME_ON	(1L<<9)
#define VIDEO_STATE_TIME	(VIDEO_STATE_TIME_OFF	| VIDEO_STATE_TIME_ON)
#define VIDEO_PRESS_PLAY_ON	(1L<<10)
#define VIDEO_PRESS_PLAY_OFF	(1L<<11)
#define VIDEO_PRESS_PLAY	(VIDEO_PRESS_PLAY_OFF	| VIDEO_PRESS_PLAY_ON)
#define VIDEO_PRESS_REC_ON	(1L<<12)
#define VIDEO_PRESS_REC_OFF	(1L<<13)
#define VIDEO_PRESS_REC		(VIDEO_PRESS_REC_OFF	| VIDEO_PRESS_REC_ON)
#define VIDEO_PRESS_PAUSE_ON	(1L<<14)
#define VIDEO_PRESS_PAUSE_OFF	(1L<<15)
#define VIDEO_PRESS_PAUSE	(VIDEO_PRESS_PAUSE_OFF	| VIDEO_PRESS_PAUSE_ON)
#define VIDEO_PRESS_STOP_ON	(1L<<16)
#define VIDEO_PRESS_STOP_OFF	(1L<<17)
#define VIDEO_PRESS_STOP	(VIDEO_PRESS_STOP_OFF	| VIDEO_PRESS_STOP_ON)
#define VIDEO_PRESS_EJECT_ON	(1L<<18)
#define VIDEO_PRESS_EJECT_OFF	(1L<<19)
#define VIDEO_PRESS_EJECT	(VIDEO_PRESS_EJECT_OFF	| VIDEO_PRESS_EJECT_ON)

#define BUTTON_VIDEO_EJECT	1
#define BUTTON_VIDEO_STOP	2
#define BUTTON_VIDEO_PAUSE	3
#define BUTTON_VIDEO_REC	4
#define BUTTON_VIDEO_PLAY	5

#define VIDEO_STATE_OFF		(VIDEO_STATE_PLAY_OFF	|	\
				 VIDEO_STATE_REC_OFF	|	\
				 VIDEO_STATE_PAUSE_OFF	|	\
				 VIDEO_STATE_DATE_OFF	|	\
				 VIDEO_STATE_TIME_OFF)
#define VIDEO_PRESS_OFF		(VIDEO_PRESS_PLAY_OFF	|	\
				 VIDEO_PRESS_REC_OFF	|	\
				 VIDEO_PRESS_PAUSE_OFF	|	\
				 VIDEO_PRESS_STOP_OFF	|	\
				 VIDEO_PRESS_EJECT_OFF)
#define VIDEO_ALL_OFF		(VIDEO_STATE_OFF | VIDEO_PRESS_OFF)

#define VIDEO_STATE_ON		(VIDEO_STATE_PLAY_ON	|	\
				 VIDEO_STATE_REC_ON	|	\
				 VIDEO_STATE_PAUSE_ON	|	\
				 VIDEO_STATE_DATE_ON	|	\
				 VIDEO_STATE_TIME_ON)
#define VIDEO_PRESS_ON		(VIDEO_PRESS_PLAY_ON	|	\
				 VIDEO_PRESS_REC_ON	|	\
				 VIDEO_PRESS_PAUSE_ON	|	\
				 VIDEO_PRESS_STOP_ON	|	\
				 VIDEO_PRESS_EJECT_ON)
#define VIDEO_ALL_ON		(VIDEO_STATE_ON | VIDEO_PRESS_ON)

#define VIDEO_STATE		(VIDEO_STATE_ON | VIDEO_STATE_OFF)
#define VIDEO_PRESS		(VIDEO_PRESS_ON | VIDEO_PRESS_OFF)
#define VIDEO_ALL		(VIDEO_ALL_ON | VIDEO_ALL_OFF)


/* some positions in the sound control window */
#define SOUND_BUTTON_XSIZE	30
#define SOUND_BUTTON_YSIZE	30
#define SOUND_CONTROL_XPOS	5
#define SOUND_CONTROL_YPOS	245
#define SOUND_CONTROL_XSIZE	90
#define SOUND_CONTROL_YSIZE	(SOUND_BUTTON_YSIZE)
#define SOUND_BUTTON_MUSIC_XPOS	(SOUND_CONTROL_XPOS + 0 * SOUND_BUTTON_XSIZE)
#define SOUND_BUTTON_LOOPS_XPOS	(SOUND_CONTROL_XPOS + 1 * SOUND_BUTTON_XSIZE)
#define SOUND_BUTTON_SOUND_XPOS	(SOUND_CONTROL_XPOS + 2 * SOUND_BUTTON_XSIZE)
#define SOUND_BUTTON_ANY_YPOS	(SOUND_CONTROL_YPOS)

#define ON_SOUND_BUTTON(x,y)	((x)>=(DX+SOUND_CONTROL_XPOS) &&	\
				 (x)< (DX+SOUND_CONTROL_XPOS +		\
				       SOUND_CONTROL_XSIZE) &&		\
				 (y)>=(DY+SOUND_CONTROL_YPOS) &&	\
				 (y)< (DY+SOUND_CONTROL_YPOS +		\
				       SOUND_CONTROL_YSIZE))
#define SOUND_BUTTON(x)		(((x)-(DX+SOUND_CONTROL_XPOS))/SOUND_BUTTON_XSIZE)

/* values for sound control */
#define BUTTON_SOUND_MUSIC	(1L<<0)
#define BUTTON_SOUND_LOOPS	(1L<<1)
#define BUTTON_SOUND_SOUND	(1L<<2)
#define BUTTON_RELEASED		0
#define BUTTON_PRESSED		(1L<<3)
#define BUTTON_OFF		0
#define BUTTON_ON		(1L<<4)
#define BUTTON_SOUND_MUSIC_OFF	(BUTTON_SOUND_MUSIC | BUTTON_OFF)
#define BUTTON_SOUND_LOOPS_OFF	(BUTTON_SOUND_LOOPS | BUTTON_OFF)
#define BUTTON_SOUND_SOUND_OFF	(BUTTON_SOUND_SOUND | BUTTON_OFF)
#define BUTTON_SOUND_MUSIC_ON	(BUTTON_SOUND_MUSIC | BUTTON_ON)
#define BUTTON_SOUND_LOOPS_ON	(BUTTON_SOUND_LOOPS | BUTTON_ON)
#define BUTTON_SOUND_SOUND_ON	(BUTTON_SOUND_SOUND | BUTTON_ON)


/* some positions in the game control window */
#define GAME_BUTTON_XSIZE	30
#define GAME_BUTTON_YSIZE	30
#define GAME_CONTROL_XPOS	5
#define GAME_CONTROL_YPOS	215
#define GAME_CONTROL_XSIZE	90
#define GAME_CONTROL_YSIZE	(GAME_BUTTON_YSIZE)
#define GAME_BUTTON_STOP_XPOS	(GAME_CONTROL_XPOS + 0 * GAME_BUTTON_XSIZE)
#define GAME_BUTTON_PAUSE_XPOS	(GAME_CONTROL_XPOS + 1 * GAME_BUTTON_XSIZE)
#define GAME_BUTTON_PLAY_XPOS	(GAME_CONTROL_XPOS + 2 * GAME_BUTTON_XSIZE)
#define GAME_BUTTON_ANY_YPOS	(GAME_CONTROL_YPOS)

#define ON_GAME_BUTTON(x,y)	((x)>=(DX+GAME_CONTROL_XPOS) &&	\
				 (x)< (DX+GAME_CONTROL_XPOS +		\
				       GAME_CONTROL_XSIZE) &&		\
				 (y)>=(DY+GAME_CONTROL_YPOS) &&	\
				 (y)< (DY+GAME_CONTROL_YPOS +		\
				       GAME_CONTROL_YSIZE))
#define GAME_BUTTON(x)		(((x)-(DX+GAME_CONTROL_XPOS))/GAME_BUTTON_XSIZE)

/* values for game control */
#define BUTTON_GAME_STOP	(1L<<0)
#define BUTTON_GAME_PAUSE	(1L<<1)
#define BUTTON_GAME_PLAY	(1L<<2)


/* some positions in the asking window */
#define OK_BUTTON_XPOS		2
#define OK_BUTTON_YPOS		250
#define OK_BUTTON_GFX_YPOS	0
#define OK_BUTTON_XSIZE		46
#define OK_BUTTON_YSIZE		28
#define NO_BUTTON_XPOS		52
#define NO_BUTTON_YPOS		OK_BUTTON_YPOS
#define NO_BUTTON_XSIZE		OK_BUTTON_XSIZE
#define NO_BUTTON_YSIZE		OK_BUTTON_YSIZE
#define CONFIRM_BUTTON_XPOS	2
#define CONFIRM_BUTTON_GFX_YPOS	30
#define CONFIRM_BUTTON_YPOS	OK_BUTTON_YPOS
#define CONFIRM_BUTTON_XSIZE	96
#define CONFIRM_BUTTON_YSIZE	OK_BUTTON_YSIZE

#define ON_CHOOSE_BUTTON(x,y)	(((x)>=(DX+OK_BUTTON_XPOS) &&		\
				  (x)< (DX+OK_BUTTON_XPOS +		\
					OK_BUTTON_XSIZE) &&		\
				  (y)>=(DY+OK_BUTTON_YPOS) &&		\
				  (y)< (DY+OK_BUTTON_YPOS +		\
					OK_BUTTON_YSIZE)) ||		\
				 ((x)>=(DX+NO_BUTTON_XPOS) &&		\
				  (x)< (DX+NO_BUTTON_XPOS +		\
					NO_BUTTON_XSIZE) &&		\
				  (y)>=(DY+NO_BUTTON_YPOS) &&		\
				  (y)< (DY+NO_BUTTON_YPOS +		\
					NO_BUTTON_YSIZE)))
#define ON_CONFIRM_BUTTON(x,y)	(((x)>=(DX+CONFIRM_BUTTON_XPOS) &&	\
				  (x)< (DX+CONFIRM_BUTTON_XPOS +	\
					CONFIRM_BUTTON_XSIZE) &&	\
				  (y)>=(DY+CONFIRM_BUTTON_YPOS) &&	\
				  (y)< (DY+CONFIRM_BUTTON_YPOS +	\
					CONFIRM_BUTTON_YSIZE)))
#define CHOOSE_BUTTON(x)	(((x)-(DX+OK_BUTTON_XPOS))/OK_BUTTON_XSIZE)

/* values for asking control */
#define BUTTON_OK		(1L<<0)
#define BUTTON_NO		(1L<<1)
#define BUTTON_CONFIRM		(1L<<2)


/* some positions in the editor control window */
#define ED_BUTTON_EUP_XPOS	35
#define ED_BUTTON_EUP_YPOS	5
#define ED_BUTTON_EUP_XSIZE	30
#define ED_BUTTON_EUP_YSIZE	25
#define ED_BUTTON_EDOWN_XPOS	35
#define ED_BUTTON_EDOWN_YPOS	250
#define ED_BUTTON_EDOWN_XSIZE	30
#define ED_BUTTON_EDOWN_YSIZE	25
#define ED_BUTTON_ELEM_XPOS	6
#define ED_BUTTON_ELEM_YPOS	30
#define ED_BUTTON_ELEM_XSIZE	22
#define ED_BUTTON_ELEM_YSIZE	22

#define ED_BUTTON_EUP_Y2POS	140
#define ED_BUTTON_EDOWN_Y2POS	165
#define ED_BUTTON_ELEM_Y2POS	190

#define ED_BUTTON_CTRL_XPOS	5
#define ED_BUTTON_CTRL_YPOS	5
#define ED_BUTTON_CTRL_XSIZE	90
#define ED_BUTTON_CTRL_YSIZE	30
#define ED_BUTTON_FILL_XPOS	5
#define ED_BUTTON_FILL_YPOS	35
#define ED_BUTTON_FILL_XSIZE	90
#define ED_BUTTON_FILL_YSIZE	20
#define ED_BUTTON_LEFT_XPOS	5
#define ED_BUTTON_LEFT_YPOS	65
#define ED_BUTTON_LEFT_XSIZE	30
#define ED_BUTTON_LEFT_YSIZE	20
#define ED_BUTTON_UP_XPOS	35
#define ED_BUTTON_UP_YPOS	55
#define ED_BUTTON_UP_XSIZE	30
#define ED_BUTTON_UP_YSIZE	20
#define ED_BUTTON_DOWN_XPOS	35
#define ED_BUTTON_DOWN_YPOS	75
#define ED_BUTTON_DOWN_XSIZE	30
#define ED_BUTTON_DOWN_YSIZE	20
#define ED_BUTTON_RIGHT_XPOS	65
#define ED_BUTTON_RIGHT_YPOS	65
#define ED_BUTTON_RIGHT_XSIZE	30
#define ED_BUTTON_RIGHT_YSIZE	20

#define ED_BUTTON_EDIT_XPOS	5
#define ED_BUTTON_EDIT_YPOS	5
#define ED_BUTTON_EDIT_XSIZE	90
#define ED_BUTTON_EDIT_YSIZE	30
#define ED_BUTTON_CLEAR_XPOS	5
#define ED_BUTTON_CLEAR_YPOS	35
#define ED_BUTTON_CLEAR_XSIZE	90
#define ED_BUTTON_CLEAR_YSIZE	20
#define ED_BUTTON_UNDO_XPOS	5
#define ED_BUTTON_UNDO_YPOS	55
#define ED_BUTTON_UNDO_XSIZE	90
#define ED_BUTTON_UNDO_YSIZE	20
#define ED_BUTTON_EXIT_XPOS	5
#define ED_BUTTON_EXIT_YPOS	75
#define ED_BUTTON_EXIT_XSIZE	90
#define ED_BUTTON_EXIT_YSIZE	20

#define ED_BUTTON_MINUS_XPOS	2
#define ED_BUTTON_MINUS_YPOS	60
#define ED_BUTTON_MINUS_XSIZE	20
#define ED_BUTTON_MINUS_YSIZE	20
#define ED_WIN_COUNT_XPOS	(ED_BUTTON_MINUS_XPOS+ED_BUTTON_MINUS_XSIZE+2)
#define ED_WIN_COUNT_YPOS	ED_BUTTON_MINUS_YPOS
#define ED_WIN_COUNT_XSIZE	52
#define ED_WIN_COUNT_YSIZE	ED_BUTTON_MINUS_YSIZE
#define ED_BUTTON_PLUS_XPOS	(ED_WIN_COUNT_XPOS+ED_WIN_COUNT_XSIZE+2)
#define ED_BUTTON_PLUS_YPOS	ED_BUTTON_MINUS_YPOS
#define ED_BUTTON_PLUS_XSIZE	ED_BUTTON_MINUS_XSIZE
#define ED_BUTTON_PLUS_YSIZE	ED_BUTTON_MINUS_YSIZE

#define ED_COUNT_GADGET_XPOS	16
#define ED_COUNT_GADGET_YPOS	(16+3*MINI_TILEY+64)
#define ED_COUNT_GADGET_YSIZE	(ED_BUTTON_MINUS_YSIZE+4)
#define ED_COUNT_TEXT_XPOS	(ED_COUNT_GADGET_XPOS+DXSIZE+10)
#define ED_COUNT_TEXT_YPOS	(ED_COUNT_GADGET_YPOS+3)
#define ED_COUNT_TEXT_YSIZE	ED_COUNT_GADGET_YSIZE
#define ED_COUNT_VALUE_XPOS	(ED_COUNT_GADGET_XPOS+ED_BUTTON_MINUS_XSIZE+7)
#define ED_COUNT_VALUE_YPOS	ED_COUNT_TEXT_YPOS
#define ED_SIZE_GADGET_XPOS	(SX+21*MINI_TILEX)
#define ED_SIZE_GADGET_YPOS	(SY+4*MINI_TILEY)
#define ED_SIZE_GADGET_YSIZE	(ED_BUTTON_MINUS_YSIZE+4)
#define ED_SIZE_TEXT_XPOS	(ED_SIZE_GADGET_XPOS+DXSIZE+10)
#define ED_SIZE_TEXT_YPOS	(ED_SIZE_GADGET_YPOS+3)
#define ED_SIZE_TEXT_YSIZE	ED_COUNT_GADGET_YSIZE
#define ED_SIZE_VALUE_XPOS	(ED_SIZE_GADGET_XPOS+ED_BUTTON_MINUS_XSIZE+7)
#define ED_SIZE_VALUE_YPOS	ED_SIZE_TEXT_YPOS

#define ON_EDIT_BUTTON(x,y)	(((x)>=(VX+ED_BUTTON_CTRL_XPOS) &&	\
				  (x)< (VX+ED_BUTTON_CTRL_XPOS +	\
					ED_BUTTON_CTRL_XSIZE) &&	\
				  (y)>=(VY+ED_BUTTON_CTRL_YPOS) &&	\
				  (y)< (VY+ED_BUTTON_CTRL_YPOS +	\
					ED_BUTTON_CTRL_YSIZE +		\
					ED_BUTTON_FILL_YSIZE)) ||	\
				 ((x)>=(VX+ED_BUTTON_LEFT_XPOS) &&	\
				  (x)< (VX+ED_BUTTON_LEFT_XPOS +	\
					ED_BUTTON_LEFT_XSIZE +		\
					ED_BUTTON_UP_XSIZE +		\
					ED_BUTTON_RIGHT_XSIZE) &&	\
				  (y)>=(VY+ED_BUTTON_LEFT_YPOS) &&	\
				  (y)< (VY+ED_BUTTON_LEFT_YPOS +	\
					ED_BUTTON_LEFT_YSIZE)) ||	\
				 ((x)>=(VX+ED_BUTTON_UP_XPOS) &&	\
				  (x)< (VX+ED_BUTTON_UP_XPOS +		\
					ED_BUTTON_UP_XSIZE) &&		\
				  (y)>=(VY+ED_BUTTON_UP_YPOS) &&	\
				  (y)< (VY+ED_BUTTON_UP_YPOS +		\
					ED_BUTTON_UP_YSIZE +		\
					ED_BUTTON_DOWN_YSIZE)))

#define ON_CTRL_BUTTON(x,y)	((x)>=(VX+ED_BUTTON_EDIT_XPOS) &&	\
				 (x)< (VX+ED_BUTTON_EDIT_XPOS +		\
				       ED_BUTTON_EDIT_XSIZE) &&		\
				 (y)>=(VY+ED_BUTTON_EDIT_YPOS) &&	\
				 (y)< (VY+ED_BUTTON_EDIT_YPOS +		\
				       ED_BUTTON_EDIT_YSIZE +		\
				       ED_BUTTON_CLEAR_YSIZE +		\
				       ED_BUTTON_UNDO_YSIZE +		\
				       ED_BUTTON_EXIT_YSIZE))

#define ON_ELEM_BUTTON(x,y)	(((x)>=(DX+ED_BUTTON_EUP_XPOS) &&	\
				  (x)< (DX+ED_BUTTON_EUP_XPOS +		\
					ED_BUTTON_EUP_XSIZE) &&		\
				  (y)>=(DY+ED_BUTTON_EUP_YPOS) &&	\
				  (y)< (DY+ED_BUTTON_EUP_YPOS +		\
					ED_BUTTON_EUP_YSIZE)) ||	\
				 ((x)>=(DX+ED_BUTTON_EDOWN_XPOS) &&	\
				  (x)< (DX+ED_BUTTON_EDOWN_XPOS +	\
					ED_BUTTON_EDOWN_XSIZE) &&	\
				  (y)>=(DY+ED_BUTTON_EDOWN_YPOS) &&	\
				  (y)< (DY+ED_BUTTON_EDOWN_YPOS +	\
					ED_BUTTON_EDOWN_YSIZE)) ||	\
				 ((x)>=(DX+ED_BUTTON_ELEM_XPOS) &&	\
				  (x)< (DX+ED_BUTTON_ELEM_XPOS +	\
					MAX_ELEM_X*ED_BUTTON_ELEM_XSIZE) && \
				  (y)>=(DY+ED_BUTTON_ELEM_YPOS) &&	\
				  (y)< (DY+ED_BUTTON_ELEM_YPOS +	\
					MAX_ELEM_Y*ED_BUTTON_ELEM_YSIZE)))

#define ON_COUNT_BUTTON(x,y)	(((((x)>=ED_COUNT_GADGET_XPOS &&	\
				    (x)<(ED_COUNT_GADGET_XPOS +		\
					 ED_BUTTON_MINUS_XSIZE)) ||	\
				   ((x)>=(ED_COUNT_GADGET_XPOS +	\
					  (ED_BUTTON_PLUS_XPOS -	\
					   ED_BUTTON_MINUS_XPOS)) &&	\
				    (x)<(ED_COUNT_GADGET_XPOS +		\
					 (ED_BUTTON_PLUS_XPOS -		\
					  ED_BUTTON_MINUS_XPOS) +	\
					 ED_BUTTON_PLUS_XSIZE))) &&	\
				  ((y)>=ED_COUNT_GADGET_YPOS &&		\
				   (y)<(ED_COUNT_GADGET_YPOS +		\
					16*ED_COUNT_GADGET_YSIZE)) &&	\
				  (((y)-ED_COUNT_GADGET_YPOS) %		\
				   ED_COUNT_GADGET_YSIZE) <		\
				  ED_BUTTON_MINUS_YSIZE) ||		\
				 ((((x)>=ED_SIZE_GADGET_XPOS &&		\
				    (x)<(ED_SIZE_GADGET_XPOS +		\
					 ED_BUTTON_MINUS_XSIZE)) ||	\
				   ((x)>=(ED_SIZE_GADGET_XPOS +		\
					  (ED_BUTTON_PLUS_XPOS -	\
					   ED_BUTTON_MINUS_XPOS)) &&	\
				    (x)<(ED_SIZE_GADGET_XPOS +		\
					 (ED_BUTTON_PLUS_XPOS -		\
					  ED_BUTTON_MINUS_XPOS) +	\
					 ED_BUTTON_PLUS_XSIZE))) &&	\
				  ((y)>=ED_SIZE_GADGET_YPOS &&		\
				   (y)<(ED_SIZE_GADGET_YPOS +		\
					2*ED_SIZE_GADGET_YSIZE)) &&	\
				  (((y)-ED_SIZE_GADGET_YPOS) %		\
				   ED_SIZE_GADGET_YSIZE) <		\
				  ED_BUTTON_MINUS_YSIZE))

#define EDIT_BUTTON(x,y)	(((y) < (VY + ED_BUTTON_CTRL_YPOS +	\
					 ED_BUTTON_CTRL_YSIZE)) ? 0 :	\
				 ((y) < (VY + ED_BUTTON_CTRL_YPOS +	\
					 ED_BUTTON_CTRL_YSIZE +		\
					 ED_BUTTON_FILL_YSIZE)) ? 1 :	\
				 ((x) < (VX + ED_BUTTON_LEFT_XPOS +	\
					 ED_BUTTON_LEFT_XSIZE) ? 2 :	\
				  (x) > (VX + ED_BUTTON_LEFT_XPOS +	\
					 ED_BUTTON_LEFT_XSIZE +		\
					 ED_BUTTON_UP_XSIZE) ? 5 :	\
				  3+(((y)-(VY + ED_BUTTON_CTRL_YPOS +	\
					   ED_BUTTON_CTRL_YSIZE +	\
					   ED_BUTTON_FILL_YSIZE)) /	\
				     ED_BUTTON_UP_YSIZE)))

#define CTRL_BUTTON(x,y)	(((y) < (VY + ED_BUTTON_EDIT_YPOS +	\
					 ED_BUTTON_EDIT_YSIZE)) ? 0 :	\
				 1+(((y)-(VY + ED_BUTTON_EDIT_YPOS +	\
					 ED_BUTTON_EDIT_YSIZE)) /	\
				    ED_BUTTON_CLEAR_YSIZE))

#define ELEM_BUTTON(x,y)	(((y) < (DY + ED_BUTTON_EUP_YPOS +	\
					 ED_BUTTON_EUP_YSIZE)) ? 0 :	\
				 ((y) > (DY + ED_BUTTON_EDOWN_YPOS)) ? 1 : \
				 2+(((y) - (DY + ED_BUTTON_ELEM_YPOS)) /   \
				 ED_BUTTON_ELEM_YSIZE)*MAX_ELEM_X +	\
				 ((x) - (DX + ED_BUTTON_ELEM_XPOS)) /	\
				 ED_BUTTON_ELEM_XSIZE)

#define COUNT_BUTTON(x,y)	((x) < ED_SIZE_GADGET_XPOS ?		\
				 ((((y) - ED_COUNT_GADGET_YPOS) /	\
				   ED_COUNT_GADGET_YSIZE)*2 +		\
				  ((x) < (ED_COUNT_GADGET_XPOS +	\
					  ED_BUTTON_MINUS_XSIZE) ? 0 : 1)) : \
				 32+((((y) - ED_SIZE_GADGET_YPOS) /	\
				      ED_SIZE_GADGET_YSIZE)*2 +		\
				     ((x) < (ED_SIZE_GADGET_XPOS +	\
					     ED_BUTTON_MINUS_XSIZE) ? 0 : 1)))

/* values for asking control */
#define ED_BUTTON_CTRL		(1L<<0)
#define ED_BUTTON_FILL		(1L<<1)
#define ED_BUTTON_LEFT		(1L<<2)
#define ED_BUTTON_UP		(1L<<3)
#define ED_BUTTON_DOWN		(1L<<4)
#define ED_BUTTON_RIGHT		(1L<<5)
#define ED_BUTTON_EDIT		(1L<<6)
#define ED_BUTTON_CLEAR		(1L<<7)
#define ED_BUTTON_UNDO		(1L<<8)
#define ED_BUTTON_EXIT		(1L<<9)

#define ED_BUTTON_PRESSED	(1L<<10)
#define ED_BUTTON_RELEASED	(1L<<11)

#define ED_BUTTON_EUP		0
#define ED_BUTTON_EDOWN		1
#define ED_BUTTON_ELEM		2


void DrawVideoDisplay(unsigned long, unsigned long);
void DrawCompleteVideoDisplay(void);
void DrawSoundDisplay(unsigned long);
void DrawGameButton(unsigned long);
void DrawChooseButton(unsigned long);
void DrawConfirmButton(unsigned long);
void DrawEditButton(unsigned long state);
void DrawCtrlButton(unsigned long state);
void DrawElemButton(int, int);
void DrawCountButton(int, int);
int CheckVideoButtons(int, int, int);
int CheckSoundButtons(int, int, int);
int CheckGameButtons(int, int, int);
int CheckChooseButtons(int, int, int);
int CheckConfirmButton(int, int, int);
int CheckEditButtons(int, int, int);
int CheckCtrlButtons(int, int, int);
int CheckElemButtons(int, int, int);
int CheckCountButtons(int, int, int);

#endif
