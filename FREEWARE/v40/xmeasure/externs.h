/*
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
                %% \___________________________________%% \
                %% |                                   %%  \
                %% |              XMeasure             %%   \
                %% |          externs.h  c1994         %%    \
                %% |            Lyle W. West           %%    |
                %% |                                   %%    |
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
                \                                        \   |
                 \                                        \  |
                  \                                        \ |
                   \________________________________________\|



     Copyright (C) 1994 Lyle W. West, All Rights Reserved.
     Permission is granted to copy and use this program so long as [1] this
     copyright notice is preserved, and [2] no financial gain is involved
     in copying the program.  This program may not be sold as "shareware"
     or "public domain" software without the express, written permission
     of the author.


*/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Global Variables
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern Arg args[10];
extern int n;
extern int first;
extern int frame;
extern int indx;
extern int delta;
extern int CursorBound;
extern int InvSelPopAck;
extern int LastX, LastY;
extern int OffsetMode;
extern int OffsetVal;
extern int RelAbsMode;

extern char runmodes[3][20];
extern char WinNam[132];
extern char win_id[16];
extern char textstr[80];
extern char verstr[];
extern char ContStr[65];    /* Continuous string */
extern char SetStr[65];     /* Set string */
extern char MoveStr[65];    /* Movestring */

extern Widget MainForm, Crowcol, Rrowcol, sep1, sep2, cursmode, absrelmode;
extern Widget button, Infostr, Sizestr, X_Ytext, Winstr, DeltaLbl1, DeltaLbl2;
extern Widget toplevel, main_window, InvSelWarn, CurModeQues;
extern Widget ConfToglBtnLbl, FreeToglBtnLbl, ConfToglBtn, FreeToglBtn;
extern Widget RelToglBtnLbl, AbsToglBtnLbl, RelToglBtn, AbsToglBtn;
extern Widget M35ToglBtnLbl, M40ToglBtnLbl, M35ToglBtn, M40ToglBtn;
extern Widget M0ToglBtnLbl, M0ToglBtn, cursbtn, absrelbtn, DeltaXY1, DeltaXY2;
extern Widget OptRowCol, pulldown, option_menu, options[3];

extern Display *display;
extern Window Measure_Win;
extern Window Root_win;
extern XmString CsStr;
extern XmStringCharSet charset;

