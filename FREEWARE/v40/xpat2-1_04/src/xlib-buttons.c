/*****************************************************************************/
/*									     */
/*									     */
/*	X patience version 2 -- module Xlib-buttons.c			     */
/*									     */
/*	Management of the button area for the Xlib interface		     */
/*	written by Heiko Eissfeldt and Michael Bischoff			     */
/*	see COPYRIGHT.xpat2 for Copyright details			     */
/*									     */
/*									     */
/*****************************************************************************/
#ifdef useXlib
#include "X-pat.h"
#ifndef NO_ROUND_CARDS
#include	<X11/Xmu/Drawing.h>
#endif


struct confirm confirm = { 0 };
Window finished_win;

void show_message(const char *str, ...) {
    static char last_message[512] = " ";

    if (str) {
	va_list args;
	va_start(args, str);
	vsprintf(last_message, str, args);
    }
    XFillRectangle(dpy, table, whitegc, 0, graphic.buttons_height,
	       graphic.width, graphic.message_height);
    XDrawRectangle(dpy, table, blackgc, 0, graphic.buttons_height,
		   graphic.width-1, graphic.message_height-1);
    XDrawImageString(dpy, table, graphic.textgc, graphic.message_x, graphic.message_y,
	last_message, strlen(last_message));
}

static void (*execfunc)(void) = NULL;

void request_confirm(void (*dofunc)(void), const char *prompt) {
    if (execfunc)
	return;		/* request pending => deny another one */
    execfunc = dofunc;
    confirm.text = prompt;
    XMapWindow(dpy, confirm.win);
}

void cmd_Cancel(void) {
    if (execfunc) {
	execfunc = NULL;
	XUnmapWindow(dpy, confirm.win);
    }
}

void cmd_Confirm(void) {
    if (execfunc) {
	(*execfunc)();
	cmd_Cancel();
    }
}

void redraw_buttons(int x, int y, int w, int h, Window win, int num, struct singlebutton *p) {
    while (num--) {
	if (x >= p->x + p->w || x + w < p->x ||
	    y >= p->y + p->h || y + h < p->y) {	/* does not intersect */
	    ++p;
	    continue;
        }
#ifndef NO_ROUND_CARDS
	if (B_ROUND_W)	{
	    /* fill the background */
	    XmuFillRoundedRectangle(dpy, win, whitegc, p->x, p->y, 
		p->w-1, p->h-1, B_ROUND_W, B_ROUND_H);
	    /* draw the text */
            XDrawImageString(dpy, win, button.gc, p->x + button.bx, p->y +
  	       button.by + button.font->ascent, p->text, strlen(p->text));
	    /* draw border on button */
	    XmuDrawRoundedRectangle(dpy, win, blackgc, p->x, p->y, 
		p->w-1, p->h-1, B_ROUND_W, B_ROUND_H);
	} else	
#endif
	{
	    /* fill the background */
	    XFillRectangle(dpy, win, whitegc, p->x, p->y, p->w-1, p->h-1);
	    /* draw the text */
            XDrawImageString(dpy, win, button.gc, p->x + button.bx, p->y +
  	       button.by + button.font->ascent, p->text, strlen(p->text));
	    /* draw border on button */
	    XDrawRectangle(dpy, win, blackgc, p->x, p->y, p->w-1, p->h-1);
	}
	++p;

    }
}

#ifdef DEBUG
void cmd_DebugOut(void);

void cmd_DebugOut(void) {
    int i, j;
    static const char *vis[2] = { "hidden ", "" };

    printf("Debugging output:\ngame.ind[] = \n{");

    for (i = 0; i <= game.numpiles; ++i)
	printf(" %d", game.ind[i]);
    printf(" }\n");
    for (i = 0; i < rules.numstacks+rules.numslots/*game.numpiles*/; ++i) {
	struct pile *p;
	p = &graphic.pile[i];
	printf((i < rules.numstacks ? "Stack %d: " : "Slot %d: "),
	       (i < rules.numstacks ? i : i - rules.numstacks));
	printf("pos (%d,%d) c=%d, max=%d\n  ", p->x, p->y, p->xtotal, p->xmaxheight);
	for (j = game.ind[i]; j < game.ind[i+1]; ++j)
	    if (IS_JOKER(game.cards[j]))
		printf("Joker!");
	    else
		printf("%s%s %s  ", vis[game.visible[j]],
		US_rank_name[RANK(game.cards[j])],
		US_suit_name[SUIT(game.cards[j])]);
	printf("\n");
    }
}

#endif

struct buttons button = {
    0,
    4, 4,
    3, 3,
    0, 0, 0,
    {
	{ 0, 0, 0, 0, "quit",	  	rq_LeavePat,     rq_LeavePat,   rq_LeavePat },
	{ 0, 0, 0, 0, "cards",	  	cmd_DealCards,   cmd_DealCards, cmd_DealCards },
	{ 0, 0, 0, 0, "undo",	  	cmd_UndoMove,    cmd_UndoMove,  cmd_UndoMove },
	{ 0, 0, 0, 0, "redo",	  	cmd_RedoMove,    cmd_RedoMove,  cmd_RedoMove },
	{ 0, 0, 0, 0, "restart",  	cmd_RestartGame, NULL, NULL },
	{ 0, 0, 0, 0, "new game", 	rq_AnotherGame,  NULL, NULL },
	{ 0, 0, 0, 0, "score",	        cmd_ShowScore,   cmd_ShowScore, cmd_ShowScore },
	{ 0, 0, 0, 0, "hint",      	cmd_NextHint,    cmd_DoHint,	cmd_PreviousHint },
	{ 0, 0, 0, 0, "move to stack", 	cmd_ToStack,     cmd_ToStack,   cmd_ToStack },
	{ 0, 0, 0, 0, "replay",		cmd_ReplayGame,	 NULL, NULL },
	{ 0, 0, 0, 0, "save",		cmd_SaveGame,	 NULL, NULL },
#ifdef DEBUG
	{ 0, 0, 0, 0, "debug",		cmd_DebugOut,	 NULL, NULL },
#endif
	{ 0, 0, 0, 0, NULL, NULL, NULL, NULL }
    }
};

struct singlebutton confirmbuttons[3] = {
	{ 0, 0, 0, 0, "confirm",  cmd_Confirm, NULL, NULL },
	{ 0, 0, 0, 0, "cancel",	  cmd_Cancel, NULL, NULL },
	{ 0, 0, 0, 0, NULL, NULL, NULL, NULL }
};
#endif
