/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

typedef struct {
	char	*title;
	int	nprompt;
	struct textPromptInfo {
		char	*prompt;	/* displayed prompt */
		int	len;		/* maximum string length */
		char	*str;		/* displayed initial string */
		char	*rstr;		/* returned string value */
		Widget	w;		/* internal -- don't use */
	} *prompts;
} TextPromptInfo;

/*
TextPrompt(parent, name, prompts, ok(), cancel(), closure)
*/
extern void TextPrompt(Widget, char *, TextPromptInfo *, XtCallbackProc , XtCallbackProc , void *);
