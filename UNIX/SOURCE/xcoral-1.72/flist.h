/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

/*
 * C'est juste pour lint
 */

extern void	parse_file ( /* name */ );
extern char	*RestoreKillBuf ( /* i, len, dn */ );
extern void	LoadKillBuffer ( /* buf */ );
extern void	LoadFileNames ( /* buf */ );
extern char	*GetStringFromWindow ( /* w, gc, font, s, x, y, reply, flag */ );
extern char	*GetStringFromSelect ( /* window, type */ );
extern char	*GetStringFromDialogBox ( /* w, s */ );
extern void	ExecMenuFunc ();
extern void	InitEvent ();
extern void	InitKillBuf ();
extern void	InitDialogWindow ();
extern void	InitFileSelector ();
extern void	StoreInKillBuf ( /* s, len, n */ );
extern void	Help ( /* text */ );
extern void	Version ( /* text */ );
extern void 	NewWindow ( /* text */ );
extern int		DeleteWindow ( /*text */ );
extern void	DisplayOpenFiles ( /* text */ );
extern void	WaitButtonRelease ( /* button */ );
extern void	WaitForEvent ();
extern void	DisplayKillBuffer( /* text */ );
extern void	MenuForwardSearch ( /* text */ );
extern void	MenuBackwardSearch ( /* text */ );
extern void	MenuQueryReplace ( /* text */ );
extern void	MenuGlobalReplace ( /* text */ );
extern void	MenuGotoLine ( /* text */ );
extern void	DisplayKillBuffer ( /* text */ );
extern void	MakeMethod ( /* text */ );
extern void	MakeClass ( /* text */ );
extern void	MakeFunction ( /* text */ );
extern void	KbdReadFile ( /* text */ );
extern void	KbdInsertFile ( /* text */ );
extern void	KbdSaveFile ( /* text */ );
extern void	KbdWriteFile ( /* text */ );
extern void	MenuNewFile ( /* text */ );
extern void	MenuReadFile ( /* text */ );
extern void	MenuInsertFile ( /* text */ );
extern void	MenuSaveFile ( /* text */ );
extern void	MenuWriteFile ( /* text */ );
extern void 	MenuGotoLine ( /* text */ );
extern void	Display3D ( /* display, window */ );
extern void	CreateRGC ( /* display, window */ );
#ifndef S_ISREG
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif
