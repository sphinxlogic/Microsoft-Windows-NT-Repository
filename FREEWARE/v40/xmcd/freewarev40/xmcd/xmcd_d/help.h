/*
 *   xmcd - Motif(tm) CD Audio Player
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
#ifndef __HELP_H__
#define __HELP_H__

#ifndef LINT
static char *_help_h_ident_ = "@(#)help.h	6.14 98/10/09";
#endif

/* Max number of widgets supported by the help system */
#define MAX_HELP_WIDGETS	115


/* Documentation topics list structure */
typedef struct doc_topic {
	char		*name;
	char		*path;
	Widget		actbtn;
	struct doc_topic *next;
} doc_topic_t;


/* Public functions */
extern void	help_init(void);
extern void	help_start(void);
extern void	help_popup(Widget);
extern void	help_popdown(void);
extern bool_t	help_isactive(void);


/* Callback functions */
extern void	help_topic_sel(Widget, XtPointer, XtPointer);

#endif	/* __HELP_H__ */
