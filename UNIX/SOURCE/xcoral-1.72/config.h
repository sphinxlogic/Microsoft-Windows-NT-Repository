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

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*
 *	Le nom
 */
#define CLIENTNAME	xcoral

/*
 *	Les Fontes
 */
#define TEXT_FONT	"-adobe-times-bold-r-normal--14-140-75-75-p-77-iso8859-1"
#define MENU_FONT	"-adobe-times-medium-r-normal--18-180-75-75-p-94-iso8859-1"

/*
 *	Les Couleurs
 */
#ifdef SVR4
#define COLOR_TEXT_BG		"black"
#define COLOR_TEXT_FG		"MediumSeaGreen"
#define COLOR_MENU_BG		"LightGray"
#define COLOR_MENU_FG		"LightYellow"
#define COLOR_DIALOG_BG		"LightGray"
#define COLOR_DIALOG_FG		"LightYellow"
#else
#define COLOR_TEXT_BG		"black"
#define COLOR_TEXT_FG		"darkseagreen1"
#define COLOR_MENU_BG		"lightslategray"
#define COLOR_MENU_FG		"yellowchiffon"
#define COLOR_DIALOG_BG		"lightslategray"
#define COLOR_DIALOG_FG		"yellowchiffon"
#endif

/*
 *	Mono
 */
#define BW_TEXT_BG		"black"
#define BW_TEXT_FG		"white"
#define BW_MENU_BG		"white"
#define BW_MENU_FG		"black"
#define BW_DIALOG_BG		"white"
#define BW_DIALOG_FG		"black"

#define APP_DEFAULT		"/usr/lib/X11/app-defaults"

#endif  /* _CONFIG_H_ */
