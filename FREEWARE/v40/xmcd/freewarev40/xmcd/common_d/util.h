/*
 *   util.h - Common utility routines for xmcd, cda and libdi.
 *
 *   xmcd  - Motif(tm) CD Audio Player
 *   cda   - Command-line CD Audio Player
 *   libdi - CD Audio Player Device Interface Library
 *
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
#ifndef __UTIL_H__
#define __UTIL_H__

#ifndef LINT
static char *_util_h_ident_ = "@(#)util.h	6.19 98/10/27";
#endif


/* Utility macros */
#define SQR(x)		((x) * (x))	/* Compute the square of a number */
#define DBGPRN		if (app_data.debug) (void) fprintf
#define DBGDUMP		if (app_data.debug) util_dbgdump

#define EXE_ERR		0xdeadbeef	/* Magic retcode for util_runcmd() */


/* Public function prototypes */
extern void		util_init(void);
extern void		util_start(void);
extern uid_t		util_get_ouid(void);
extern gid_t		util_get_ogid(void);
extern struct utsname	*util_get_uname(void);
extern sword32_t	util_ltobcd(sword32_t);
extern sword32_t	util_bcdtol(sword32_t);
extern bool_t		util_stob(char *);
extern char		*util_basename(char *);
extern char		*util_dirname(char *);
extern char		*util_loginname(void);
extern char		*util_homedir(uid_t);
extern char		*util_uhomedir(char *);
extern bool_t		util_mkdir(char *, mode_t);
extern bool_t		util_isexecutable(char *);
extern bool_t		util_checkcmd(char *);
extern int		util_runcmd(char *, void (*)(int), int);
extern int		util_isqrt(int);
extern void		util_blktomsf(word32_t, byte_t *, byte_t *, byte_t *,
				      word32_t);
extern void		util_msftoblk(byte_t, byte_t, byte_t, word32_t *,
				      word32_t);
extern void		util_delayms(unsigned long);
extern int		util_strcasecmp(char *, char *);
extern int		util_strncasecmp(char *, char *, int);
extern void		util_b64encode(byte_t *, int, byte_t *, bool_t);
extern char		*util_text_reduce(char *);
extern char		*util_cgi_xlate(char *);
extern word16_t		util_bswap16(word16_t);
extern word32_t		util_bswap24(word32_t);
extern word32_t		util_bswap32(word32_t);
extern word16_t		util_lswap16(word16_t);
extern word32_t		util_lswap24(word32_t);
extern word32_t		util_lswap32(word32_t);
extern void		util_dbgdump(char *, byte_t *, int);

#ifdef __VMS
extern DIR		*util_opendir(char *);
extern void		util_closedir(DIR *);
extern struct dirent	*util_readdir(DIR *);
extern pid_t		util_waitpid(pid_t, int *, int);
extern int		util_link(char *, char *);
extern int		util_unlink(char *);
#endif

#ifdef MEM_DEBUG
void			*util_dbg_malloc(char *name, size_t size);
void			*util_dbg_realloc(char *name, void *ptr, size_t size);
void			*util_dbg_calloc(char *name, size_t nelem,
					 size_t elsize);
void			util_dbg_free(void *ptr);
#endif

#endif	/* __UTIL_H__ */


