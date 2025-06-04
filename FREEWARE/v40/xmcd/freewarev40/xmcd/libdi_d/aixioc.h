/*
 *   libdi - CD Audio Player Device Interface Library
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free
 *   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __AIXIOC_H__
#define __AIXIOC_H__

#if defined(DI_AIXIOC) && !defined(DEMO_ONLY)

#ifndef LINT
static char *_aixioc_h_ident_ = "@(#)aixioc.h	6.6 98/01/02";
#endif

#include <sys/devinfo.h>
#include <sys/ide.h>
#include <sys/idecdrom.h>
#include <sys/scdisk.h>

/* Ioctl name lookup table structure */
typedef struct {
	int	cmd;
	char	*name;
} iocname_t;

/*
 * Public functions
 */
extern void	aixioc_init(curstat_t *, di_tbl_t *);
extern bool_t	aixioc_check_disc(curstat_t *);
extern void	aixioc_status_upd(curstat_t *);
extern void	aixioc_lock(curstat_t *, bool_t);
extern void	aixioc_repeat(curstat_t *, bool_t);
extern void	aixioc_shuffle(curstat_t *, bool_t);
extern void	aixioc_load_eject(curstat_t *);
extern void	aixioc_ab(curstat_t *);
extern void	aixioc_sample(curstat_t *);
extern void	aixioc_level(curstat_t *, byte_t, bool_t);
extern void	aixioc_play_pause(curstat_t *);
extern void	aixioc_stop(curstat_t *, bool_t);
extern void	aixioc_chgdisc(curstat_t *);
extern void	aixioc_prevtrk(curstat_t *);
extern void	aixioc_nexttrk(curstat_t *);
extern void	aixioc_previdx(curstat_t *);
extern void	aixioc_nextidx(curstat_t *);
extern void	aixioc_rew(curstat_t *, bool_t);
extern void	aixioc_ff(curstat_t *, bool_t);
extern void	aixioc_warp(curstat_t *);
extern void	aixioc_route(curstat_t *);
extern void	aixioc_mute_on(curstat_t *);
extern void	aixioc_mute_off(curstat_t *);
extern void	aixioc_start(curstat_t *);
extern void	aixioc_icon(curstat_t *, bool_t);
extern void	aixioc_halt(curstat_t *);
extern char	*aixioc_mode(void);
extern char	*aixioc_vers(void);

#else	/* DI_AIXIOC DEMO_ONLY */

#define aixioc_init	NULL

#endif	/* DI_AIXIOC DEMO_ONLY */

#endif	/* __AIXIOC_H__ */

