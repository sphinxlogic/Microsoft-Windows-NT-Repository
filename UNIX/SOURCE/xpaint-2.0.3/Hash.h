/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

#ifdef __STDC__
void	*HashCreate(int (*cmp)(), void (*free)(), int nelem);
void	HashDestroy(void *tbl);
void	*HashFind(void *tbl, int value, void *val);
int	HashAdd(void *tbl, int value, void *val);
int     HashRemove(void *tbl, int value, void *elem);
int     HashAll(void *tbl, int (*func)(void *));
#else
void	*HashCreate();
void	HashDestroy();
void	*HashFind();
int	HashAdd();
int     HashRemove();
int     HashAll();
#endif
