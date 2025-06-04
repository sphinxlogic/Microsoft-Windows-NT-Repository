/*
 *   xmcd - Motif CD Player
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __GEOM_H__
#define __GEOM_H__

#ifndef LINT
static char *_geom_h_ident_ = "@(#)geom.h	6.30 98/10/09";
#endif

/* Location of widgets relative to form origin, express in percentage of
 * form size.
 */

/* Main window widgets: normal mode */
#define LEFT_MODE		0
#define RIGHT_MODE		14
#define TOP_MODE		0
#define BOTTOM_MODE		16

#define LEFT_CHECKBOX		0
#define RIGHT_CHECKBOX		14
#define TOP_CHECKBOX		16
#define BOTTOM_CHECKBOX		68

#define LEFT_EJECT		0
#define RIGHT_EJECT		14
#define TOP_EJECT		68
#define BOTTOM_EJECT		84

#define LEFT_QUIT		0
#define RIGHT_QUIT		14
#define TOP_QUIT		84
#define BOTTOM_QUIT		100

#define LEFT_DISCIND		17
#define RIGHT_DISCIND		25
#define TOP_DISCIND		4
#define BOTTOM_DISCIND		28

#define LEFT_TRACKIND		25
#define RIGHT_TRACKIND		40
#define TOP_TRACKIND		4
#define BOTTOM_TRACKIND		28

#define LEFT_INDEXIND		40
#define RIGHT_INDEXIND		45
#define TOP_INDEXIND		4
#define BOTTOM_INDEXIND		28

#define LEFT_TIMEIND		45
#define RIGHT_TIMEIND		71
#define TOP_TIMEIND		4
#define BOTTOM_TIMEIND		28

#define LEFT_RPTCNTIND		17
#define RIGHT_RPTCNTIND		25
#define TOP_RPTCNTIND		28
#define BOTTOM_RPTCNTIND	42

#define LEFT_DBMODEIND		25
#define RIGHT_DBMODEIND		38
#define TOP_DBMODEIND		28
#define BOTTOM_DBMODEIND	42

#define LEFT_PROGMODEIND	38
#define RIGHT_PROGMODEIND	48
#define TOP_PROGMODEIND		28
#define BOTTOM_PROGMODEIND	42

#define LEFT_TIMEMODEIND	48
#define RIGHT_TIMEMODEIND	59
#define TOP_TIMEMODEIND		28
#define BOTTOM_TIMEMODEIND	42

#define LEFT_PLAYMODEIND	59
#define RIGHT_PLAYMODEIND	71
#define TOP_PLAYMODEIND		28
#define BOTTOM_PLAYMODEIND	42

#define LEFT_DTITLEIND		17
#define RIGHT_DTITLEIND		71
#define TOP_DTITLEIND		44
#define BOTTOM_DTITLEIND	53

#define LEFT_TTITLEIND		17
#define RIGHT_TTITLEIND		71
#define TOP_TTITLEIND		53
#define BOTTOM_TTITLEIND	63

#define LEFT_DBPROG		14
#define RIGHT_DBPROG		24
#define TOP_DBPROG		65
#define BOTTOM_DBPROG		100

#define LEFT_OPTIONS		24
#define RIGHT_OPTIONS		34
#define TOP_OPTIONS		65
#define BOTTOM_OPTIONS		80

#define LEFT_TIME		34
#define RIGHT_TIME		44
#define TOP_TIME		65
#define BOTTOM_TIME		80

#define LEFT_AB			44
#define RIGHT_AB		54
#define TOP_AB			65
#define BOTTOM_AB		80

#define LEFT_SAMPLE		54
#define RIGHT_SAMPLE		64
#define TOP_SAMPLE		65
#define BOTTOM_SAMPLE		80

#define LEFT_KEYPAD		64
#define RIGHT_KEYPAD		74
#define TOP_KEYPAD		65
#define BOTTOM_KEYPAD		80

#define LEFT_HELP		24
#define RIGHT_HELP		34
#define TOP_HELP		80
#define BOTTOM_HELP		100

#define LEFT_LEVEL		34
#define RIGHT_LEVEL		74
#define TOP_LEVEL		80
#define BOTTOM_LEVEL		100

#define LEFT_PLAYPAUSE		74
#define RIGHT_PLAYPAUSE		100
#define TOP_PLAYPAUSE		0
#define BOTTOM_PLAYPAUSE	18

#define LEFT_STOP		74
#define RIGHT_STOP		100
#define TOP_STOP		18
#define BOTTOM_STOP		36

#define LEFT_PREVDISC		74
#define RIGHT_PREVDISC		87
#define TOP_PREVDISC		36
#define BOTTOM_PREVDISC		52

#define LEFT_NEXTDISC		87
#define RIGHT_NEXTDISC		100
#define TOP_NEXTDISC		36
#define BOTTOM_NEXTDISC		52

#define LEFT_PREVTRK		74
#define RIGHT_PREVTRK		87
#define TOP_PREVTRK		52
#define BOTTOM_PREVTRK		68

#define LEFT_NEXTTRK		87
#define RIGHT_NEXTTRK		100
#define TOP_NEXTTRK		52
#define BOTTOM_NEXTTRK		68

#define LEFT_PREVIDX		74
#define RIGHT_PREVIDX		87
#define TOP_PREVIDX		68
#define BOTTOM_PREVIDX		84

#define LEFT_NEXTIDX		87
#define RIGHT_NEXTIDX		100
#define TOP_NEXTIDX		68
#define BOTTOM_NEXTIDX		84

#define LEFT_REW		74
#define RIGHT_REW		87
#define TOP_REW			84
#define BOTTOM_REW		100

#define LEFT_FF			87
#define RIGHT_FF		100
#define TOP_FF			84
#define BOTTOM_FF		100

/* Main window widgets: basic mode */
#define LEFT_BMODE		0
#define RIGHT_BMODE		17
#define TOP_BMODE		0
#define BOTTOM_BMODE		34

#define LEFT_BEJECT		0
#define RIGHT_BEJECT		17
#define TOP_BEJECT		34
#define BOTTOM_BEJECT		66

#define LEFT_BQUIT		0
#define RIGHT_BQUIT		17
#define TOP_BQUIT		66
#define BOTTOM_BQUIT		100

#define LEFT_BTRACKIND		20
#define RIGHT_BTRACKIND		34
#define TOP_BTRACKIND		8
#define BOTTOM_BTRACKIND	45

#define LEFT_BTIMEIND		34
#define RIGHT_BTIMEIND		68
#define TOP_BTIMEIND		8
#define BOTTOM_BTIMEIND		45

#define LEFT_BLEVEL		17
#define RIGHT_BLEVEL		72
#define TOP_BLEVEL		56
#define BOTTOM_BLEVEL		100

#define LEFT_BPLAYPAUSE		72
#define RIGHT_BPLAYPAUSE	100
#define TOP_BPLAYPAUSE		0
#define BOTTOM_BPLAYPAUSE	34

#define LEFT_BSTOP		72
#define RIGHT_BSTOP		100
#define TOP_BSTOP		34
#define BOTTOM_BSTOP		66

#define LEFT_BPREVTRK		72
#define RIGHT_BPREVTRK		86
#define TOP_BPREVTRK		66
#define BOTTOM_BPREVTRK		100

#define LEFT_BNEXTTRK		86
#define RIGHT_BNEXTTRK		100
#define TOP_BNEXTTRK		66
#define BOTTOM_BNEXTTRK		100

/* Keypad window widgets */
#define LEFT_KEYPADLBL		5
#define RIGHT_KEYPADLBL		95
#define TOP_KEYPADLBL		2
#define BOTTOM_KEYPADLBL	7

#define LEFT_KEYPADIND		17
#define RIGHT_KEYPADIND		83
#define TOP_KEYPADIND		8
#define BOTTOM_KEYPADIND	16

#define LEFT_KPMODEBOX		30
#define RIGHT_KPMODEBOX		70
#define TOP_KPMODEBOX		19
#define BOTTOM_KPMODEBOX	36

#define LEFT_KEY1		5
#define RIGHT_KEY1		35
#define TOP_KEY1		38
#define BOTTOM_KEY1		46

#define LEFT_KEY2		35
#define RIGHT_KEY2		65
#define TOP_KEY2		38
#define BOTTOM_KEY2		46

#define LEFT_KEY3		65
#define RIGHT_KEY3		95
#define TOP_KEY3		38
#define BOTTOM_KEY3		46

#define LEFT_KEY4		5
#define RIGHT_KEY4		35
#define TOP_KEY4		46
#define BOTTOM_KEY4		54

#define LEFT_KEY5		35
#define RIGHT_KEY5		65
#define TOP_KEY5		46
#define BOTTOM_KEY5		54

#define LEFT_KEY6		65
#define RIGHT_KEY6		95
#define TOP_KEY6		46
#define BOTTOM_KEY6		54

#define LEFT_KEY7		5
#define RIGHT_KEY7		35
#define TOP_KEY7		54
#define BOTTOM_KEY7		62

#define LEFT_KEY8		35
#define RIGHT_KEY8		65
#define TOP_KEY8		54
#define BOTTOM_KEY8		62

#define LEFT_KEY9		65
#define RIGHT_KEY9		95
#define TOP_KEY9		54
#define BOTTOM_KEY9		62

#define LEFT_KEY0		35
#define RIGHT_KEY0		65
#define TOP_KEY0		62
#define BOTTOM_KEY0		70

#define LEFT_CLEAR		5
#define RIGHT_CLEAR		35
#define TOP_CLEAR		62
#define BOTTOM_CLEAR		70

#define LEFT_ENTER		65
#define RIGHT_ENTER		95
#define TOP_ENTER		62
#define BOTTOM_ENTER		70

#define LEFT_WARPLBL		5
#define RIGHT_WARPLBL		95
#define TOP_WARPLBL		71
#define BOTTOM_WARPLBL		76

#define LEFT_WARPSCALE		5
#define RIGHT_WARPSCALE		95
#define TOP_WARPSCALE		76
#define BOTTOM_WARPSCALE	83

#define LEFT_KEYPADSEP		0
#define RIGHT_KEYPADSEP		100
#define TOP_KEYPADSEP		85

#define LEFT_KPCANCEL		30
#define RIGHT_KPCANCEL		70
#define TOP_KPCANCEL		88

/* Options window widgets */
#define LEFT_LOAD_LBL		6
#define RIGHT_LOAD_LBL		47
#define TOP_LOAD_LBL		2
#define BOTTOM_LOAD_LBL		6

#define LEFT_LOAD_CHKFRM	6
#define RIGHT_LOAD_CHKFRM	47
#define TOP_LOAD_CHKFRM		6

#define LEFT_LOAD_RADFRM	6
#define RIGHT_LOAD_RADFRM	47
#define TOP_LOAD_RADFRM		12
#define BOTTOM_LOAD_RADFRM	27

#define LEFT_EXIT_LBL		6
#define RIGHT_EXIT_LBL		47
#define TOP_EXIT_LBL		28
#define BOTTOM_EXIT_LBL		32

#define LEFT_EXIT_RADFRM	6
#define RIGHT_EXIT_RADFRM	47
#define TOP_EXIT_RADFRM		32
#define BOTTOM_EXIT_RADFRM	47

#define LEFT_DONE_LBL		6
#define RIGHT_DONE_LBL		47
#define TOP_DONE_LBL		48
#define BOTTOM_DONE_LBL		52

#define LEFT_DONE_CHKFRM	6
#define RIGHT_DONE_CHKFRM	47
#define TOP_DONE_CHKFRM		52
#define BOTTOM_DONE_CHKFRM	58

#define LEFT_EJECT_LBL		6
#define RIGHT_EJECT_LBL		47
#define TOP_EJECT_LBL		59
#define BOTTOM_EJECT_LBL	63

#define LEFT_EJECT_CHKFRM	6
#define RIGHT_EJECT_CHKFRM	47
#define TOP_EJECT_CHKFRM	63
#define BOTTOM_EJECT_CHKFRM	69

#define LEFT_CHG_LBL		53
#define RIGHT_CHG_LBL		94
#define TOP_CHG_LBL		2
#define BOTTOM_CHG_LBL		6

#define LEFT_CHG_CHKFRM		53
#define RIGHT_CHG_CHKFRM	94
#define TOP_CHG_CHKFRM		6
#define BOTTOM_CHG_CHKFRM	21

#define LEFT_CHROUTE_LBL	53
#define RIGHT_CHROUTE_LBL	94
#define TOP_CHROUTE_LBL		22
#define BOTTOM_CHROUTE_LBL	26

#define LEFT_CHROUTE_RADFRM	53
#define RIGHT_CHROUTE_RADFRM	94
#define TOP_CHROUTE_RADFRM	26
#define BOTTOM_CHROUTE_RADFRM	49

#define LEFT_VOLTP_LBL		53
#define RIGHT_VOLTP_LBL		94
#define TOP_VOLTP_LBL		50
#define BOTTOM_VOLTP_LBL	54

#define LEFT_VOLTP_RADFRM	53
#define RIGHT_VOLTP_RADFRM	94
#define TOP_VOLTP_RADFRM	54
#define BOTTOM_VOLTP_RADFRM	69

#define LEFT_BAL_LBL		20
#define RIGHT_BAL_LBL		80
#define TOP_BAL_LBL		71
#define BOTTOM_BAL_LBL		75

#define LEFT_BALL_LBL		5
#define RIGHT_BALL_LBL		20
#define TOP_BALL_LBL		75
#define BOTTOM_BALL_LBL		79

#define LEFT_BALR_LBL		80
#define RIGHT_BALR_LBL		95
#define TOP_BALR_LBL		75
#define BOTTOM_BALR_LBL		79

#define LEFT_BAL_SCALE		20
#define RIGHT_BAL_SCALE		80
#define TOP_BAL_SCALE		75
#define BOTTOM_BAL_SCALE	79

#define LEFT_BALCTR_BTN		35
#define RIGHT_BALCTR_BTN	65
#define TOP_BALCTR_BTN		80
#define BOTTOM_BALCTR_BTN	86

#define LEFT_OPTSEP		0
#define RIGHT_OPTSEP		100
#define TOP_OPTSEP		88

#define LEFT_RESET_BTN		4
#define RIGHT_RESET_BTN		32
#define TOP_RESET_BTN		91

#define LEFT_SAVE_BTN		36
#define RIGHT_SAVE_BTN		64
#define TOP_SAVE_BTN		91

#define LEFT_OPTOK_BTN		68
#define RIGHT_OPTOK_BTN		96
#define TOP_OPTOK_BTN		91

/* Database/Program window widgets */
#define LEFT_TOTTIMELBL		2
#define RIGHT_TOTTIMELBL	15
#define TOP_TOTTIMELBL		1
#define BOTTOM_TOTTIMELBL	5

#define LEFT_TOTTIMEIND		14
#define RIGHT_TOTTIMEIND	24
#define TOP_TOTTIMEIND		1
#define BOTTOM_TOTTIMEIND	5

#define LEFT_LOGO		33
#define RIGHT_LOGO		49
#define TOP_LOGO		1
#define BOTTOM_LOGO		11

#define LEFT_WEB		81
#define RIGHT_WEB		98
#define TOP_WEB			2
#define BOTTOM_WEB		7

#define LEFT_RMTDSBL		2
#define RIGHT_RMTDSBL		33
#define TOP_RMTDSBL		5
#define BOTTOM_RMTDSBL		9

#define LEFT_DTITLELBL		2
#define RIGHT_DTITLELBL		79
#define TOP_DTITLELBL		11
#define BOTTOM_DTITLELBL	14

#define LEFT_DTITLE		2
#define RIGHT_DTITLE		79
#define TOP_DTITLE		14
#define BOTTOM_DTITLE		22

#define LEFT_EXTDLBL		81
#define RIGHT_EXTDLBL		98
#define TOP_EXTDLBL		14
#define BOTTOM_EXTDLBL		17

#define LEFT_EXTD		81
#define RIGHT_EXTD		98
#define TOP_EXTD		17
#define BOTTOM_EXTD		22

#define LEFT_DBPROGSEP1		0
#define RIGHT_DBPROGSEP1	100
#define TOP_DBPROGSEP1		24

#define LEFT_TRKLISTLBL		2
#define RIGHT_TRKLISTLBL	79
#define TOP_TRKLISTLBL		26
#define BOTTOM_TRKLISTLBL	29

#define LEFT_TRKLIST		2
#define RIGHT_TRKLIST		79
#define TOP_TRKLIST		29
#define BOTTOM_TRKLIST		61

#define LEFT_RADIOLBL		81
#define RIGHT_RADIOLBL		98
#define TOP_RADIOLBL		40
#define BOTTOM_RADIOLBL		43

#define LEFT_RADIOBOX		82
#define RIGHT_RADIOBOX		97
#define TOP_RADIOBOX		43
#define BOTTOM_RADIOBOX		53

#define LEFT_DISCIDLBL		81
#define RIGHT_DISCIDLBL		98
#define TOP_DISCIDLBL		68
#define BOTTOM_DISCIDLBL	71

#define LEFT_DISCIDFRM		82
#define RIGHT_DISCIDFRM		97
#define TOP_DISCIDFRM		71
#define BOTTOM_DISCIDFRM	79

#define LEFT_TTITLELBL		2
#define RIGHT_TTITLELBL		79
#define TOP_TTITLELBL		62
#define BOTTOM_TTITLELBL	65

#define LEFT_TTITLE		2
#define RIGHT_TTITLE		79
#define TOP_TTITLE		65
#define BOTTOM_TTITLE		73

#define LEFT_EXTTLBL		81
#define RIGHT_EXTTLBL		98
#define TOP_EXTTLBL		54
#define BOTTOM_EXTTLBL		57

#define LEFT_EXTT		81
#define RIGHT_EXTT		98
#define TOP_EXTT		57
#define BOTTOM_EXTT		62

#define LEFT_PGMLBL		81
#define RIGHT_PGMLBL		98
#define TOP_PGMLBL		26
#define BOTTOM_PGMLBL		29

#define LEFT_ADDPGM		81
#define RIGHT_ADDPGM		98
#define TOP_ADDPGM		29
#define BOTTOM_ADDPGM		34

#define LEFT_CLRPGM		81
#define RIGHT_CLRPGM		98
#define TOP_CLRPGM		34
#define BOTTOM_CLRPGM		39

#define LEFT_PGMSEQLBL		2
#define RIGHT_PGMSEQLBL		79
#define TOP_PGMSEQLBL		74
#define BOTTOM_PGMSEQLBL	77

#define LEFT_PGMSEQ		2
#define RIGHT_PGMSEQ		79
#define TOP_PGMSEQ		77
#define BOTTOM_PGMSEQ		85

#define LEFT_SEND		81
#define RIGHT_SEND		98
#define TOP_SEND		80
#define BOTTOM_SEND		85

#define LEFT_DBPROGSEP2		0
#define RIGHT_DBPROGSEP2	100
#define TOP_DBPROGSEP2		87

#define LEFT_SAVEDB		6
#define RIGHT_SAVEDB		25
#define TOP_SAVEDB		91

#define LEFT_LINK		29
#define RIGHT_LINK		48
#define TOP_LINK		91

#define LEFT_LOADDB		52
#define RIGHT_LOADDB		71
#define TOP_LOADDB		91

#define LEFT_DPCANCEL		75
#define RIGHT_DPCANCEL		94
#define TOP_DPCANCEL		91

/* wwwWarp widgets */
#define LEFT_WLPIXMAP		30
#define RIGHT_WLPIXMAP		70
#define TOP_WLPIXMAP		3

#define LEFT_WLSELLBL		25
#define RIGHT_WLSELLBL		75
#define TOP_WLSELLBL		18

#define LEFT_WLRADBOX		8
#define RIGHT_WLRADBOX		92
#define TOP_WLRADBOX		26

#define LEFT_WLSRCHSITES	8

#define LEFT_WLSEP		0
#define RIGHT_WLSEP		100
#define TOP_WLSEP		83

#define LEFT_WLGO		15
#define RIGHT_WLGO		40
#define TOP_WLGO		87

#define LEFT_WLCANCEL		60
#define RIGHT_WLCANCEL		85
#define TOP_WLCANCEL		87

/* Disc Extended Info editor widgets */
#define LEFT_DISCNUMLBL		40
#define RIGHT_DISCNUMLBL	60
#define TOP_DISCNUMLBL		2
#define BOTTOM_DISCNUMLBL	7

#define LEFT_DISCLBL		2
#define RIGHT_DISCLBL		98
#define TOP_DISCLBL		8
#define BOTTOM_DISCLBL		12

#define LEFT_EXTDTXT		2
#define RIGHT_EXTDTXT		98
#define TOP_EXTDTXT		13
#define BOTTOM_EXTDTXT		83

#define LEFT_DBEXTDSEP		0
#define RIGHT_DBEXTDSEP		100
#define TOP_DBEXTDSEP		85

#define LEFT_DDOK		10
#define RIGHT_DDOK		30
#define TOP_DDOK		89

#define LEFT_DDCLEAR		40
#define RIGHT_DDCLEAR		60
#define TOP_DDCLEAR		89

#define LEFT_DDCANCEL		70
#define RIGHT_DDCANCEL		90
#define TOP_DDCANCEL		89

/* Track Extended Info editor widgets */
#define LEFT_EXTTPREV		38
#define RIGHT_EXTTPREV		43
#define TOP_EXTTPREV		2
#define BOTTOM_EXTTPREV		7

#define LEFT_EXTTNEXT		57
#define RIGHT_EXTTNEXT		62
#define TOP_EXTTNEXT		2
#define BOTTOM_EXTTNEXT		7

#define LEFT_EXTTTRKNUMLBL	40
#define RIGHT_EXTTTRKNUMLBL	60
#define TOP_EXTTTRKNUMLBL	2
#define BOTTOM_EXTTTRKNUMLBL	7

#define LEFT_EXTTAUTOTRKBTN	2
#define TOP_EXTTAUTOTRKBTN	2
#define BOTTOM_EXTTAUTOTRKBTN	7

#define LEFT_TRKLBL		2
#define RIGHT_TRKLBL		98
#define TOP_TRKLBL		7
#define BOTTOM_TRKLBL		12

#define LEFT_EXTTTXT		2
#define RIGHT_EXTTTXT		98
#define TOP_EXTTTXT		13
#define BOTTOM_EXTTTXT		83

#define LEFT_DBEXTTSEP		0
#define RIGHT_DBEXTTSEP		100
#define TOP_DBEXTTSEP		85

#define LEFT_DTOK		10
#define RIGHT_DTOK		30
#define TOP_DTOK		89

#define LEFT_DTCLEAR		40
#define RIGHT_DTCLEAR		60
#define TOP_DTCLEAR		89

#define LEFT_DTCANCEL		70
#define RIGHT_DTCANCEL		90
#define TOP_DTCANCEL		89

/* Help popup widgets */
#define LEFT_TOPIC		2
#define TOP_TOPIC		2

#define LEFT_HELPTXT		2
#define RIGHT_HELPTXT		98
#define TOP_HELPTXT		12
#define BOTTOM_HELPTXT		85

#define LEFT_HELPSEP		0
#define RIGHT_HELPSEP		100
#define TOP_HELPSEP		87

#define LEFT_ABOUT		15
#define RIGHT_ABOUT		40
#define TOP_ABOUT		90

#define LEFT_HELPCANCEL		60
#define RIGHT_HELPCANCEL	85
#define TOP_HELPCANCEL		90

/* Directory selector popup widgets */
#define LEFT_DIRLBL		4
#define RIGHT_DIRLBL		96
#define TOP_DIRLBL		3
#define BOTTOM_DIRLBL		9

#define LEFT_DIRLIST		4
#define RIGHT_DIRLIST		96
#define TOP_DIRLIST		12
#define BOTTOM_DIRLIST		73

#define LEFT_DIRSELSEP		0
#define RIGHT_DIRSELSEP		100
#define TOP_DIRSELSEP		78

#define LEFT_DSOK		5
#define RIGHT_DSOK		40
#define TOP_DSOK		84

#define LEFT_DSCANCEL		60
#define RIGHT_DSCANCEL		95
#define TOP_DSCANCEL		84

/* Link selector popup widgets */
#define LEFT_LINKLBL		3
#define RIGHT_LINKLBL		97
#define TOP_LINKLBL		1
#define BOTTOM_LINKLBL		9

#define LEFT_LINKLIST		3
#define RIGHT_LINKLIST		97
#define TOP_LINKLIST		9
#define BOTTOM_LINKLIST		86

#define LEFT_LINKSELSEP		0
#define RIGHT_LINKSELSEP	100
#define TOP_LINKSELSEP		88

#define LEFT_LSOK		10
#define RIGHT_LSOK		37
#define TOP_LSOK		91

#define LEFT_LSCANCEL		63
#define RIGHT_LSCANCEL		90
#define TOP_LSCANCEL		91

/* Authorization popup widgets */
#define LEFT_AUTHLBL		5
#define RIGHT_AUTHLBL		95
#define TOP_AUTHLBL		5
#define BOTTOM_AUTHLBL		12

#define LEFT_AUTHNAMELBL	5
#define RIGHT_AUTHNAMELBL	95
#define TOP_AUTHNAMELBL		17
#define BOTTOM_AUTHNAMELBL	24

#define LEFT_AUTHNAMETXT	5
#define RIGHT_AUTHNAMETXT	95
#define TOP_AUTHNAMETXT		24
#define BOTTOM_AUTHNAMETXT	43

#define LEFT_AUTHPASSLBL	5
#define RIGHT_AUTHPASSLBL	95
#define TOP_AUTHPASSLBL		45
#define BOTTOM_AUTHPASSLBL	52

#define LEFT_AUTHPASSTXT	5
#define RIGHT_AUTHPASSTXT	95
#define TOP_AUTHPASSTXT		52
#define BOTTOM_AUTHPASSTXT	71

#define LEFT_AUTHSEP		0
#define RIGHT_AUTHSEP		100
#define TOP_AUTHSEP		75

#define LEFT_AUTHOK		20
#define RIGHT_AUTHOK		40
#define TOP_AUTHOK		81

#define LEFT_AUTHCANCEL		60
#define RIGHT_AUTHCANCEL	80
#define TOP_AUTHCANCEL		81


/* Public function prototypes */
extern void	geom_force(widgets_t *);
extern void	geom_main_chgmode(widgets_t *);
extern int	geom_main_getmode(void);

#endif	/* __GEOM_H__ */

