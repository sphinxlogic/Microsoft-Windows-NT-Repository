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

#ifndef _MENUS_H_
#define _MENUS_H_

#include <X11/Xlib.h>

#define 	SHADOW 
#define 	DEPTH_UP	0
#define 	DEPTH_DOWN	1
#define 	DEPTH_WIDTH	1

#define	MAX_MENUS	15
#define 	MAX_ITEMS	30
#define	MENU		1
#define 	ITEM		0
#define	SHAD		5
#define 	BAR_SPACE	12

/*
 *	Structure de sauvegarde du contexte courant.
 */
typedef struct {
	Window	w;		/* La fenetre mappee */
	int	type;		/* Menu ou item */
	int	no_m, no_i;	/* Le numeros correspondants */
} SWindow;

/*
 * 	Les ressources.
 */
typedef struct {
	GC	ngc;	/* GC normal */
	GC	igc;	/* GC pour l'inverse */
	GC	bgc;	/* GC pour le bar */
	GC	ugc;	/* Historique */
	int 	height_bar;
	unsigned long top_shadow;
	unsigned long bot_shadow;
	XFontStruct *font;
	unsigned long fg, bg;
} ResourcesMenu;

/*
 *	Structure des menus verticaux.
 */
typedef struct {
	Window	v_frame;  /* La fenetre contenant les items */
	Window	shadow;	/* La fenetre pour l'ombre */
	Window	trans;	/* La fenetre mere */
	Window	w_item [MAX_ITEMS];	/* Les items */
	void (*func [MAX_ITEMS])();		/* Les callbacks */
	int nb_items;			/* Le nombre d'items */
	char **iname;	/* Les chaines de caracteres associes */
	int x;		/* Position horizontale des menus */
	int width, height;	/* Largeur et hauteur */
} VMenu;

/*
 *	Structure d'un menu XY
 */
typedef struct {
	/*
	 *	La barre des titres.
	 */
	Window w_bar;	/* La fenetre des titres */
	Window parent;	/* Fille de root */
	Window w_title [MAX_MENUS]; 	/* Le tableau des fenetres des titres */
	int title_width [MAX_MENUS];	/* Le tableau des largeurs des titres */
	int title_height;	/* La hauteur des titres en pixels */
	int y_title;	/* La position des menus */
	int bar_height, bar_width;	/* La largeur et la hauteur du bar */
	/*
	 *	La fenetre masquee par les menus deroulants
	 */
	Window w_under;	/* La fenetre masquee par les menus */
	GC Ugc;		/* Le contexte graphique */
	Pixmap save;	/* Si ya pas le save under */
	int pix_width;
	int save_ok;	/* Pour sauver une fois et une seule */
	/*
	 *	Les Infos generales
	 */	
	Window mapped;	/* Le menu mappe */
	int n_last_mapped;	/* Le numero du menu mappe */
	int n_last_unmapped; /* Le numero du dernier menu unmappe */
	Window w_last_item; /* Le dernier item mappe */
	int n_last_item;	/* Le numero du dernier item mappe */
	int nb_menus;	/* Le nombre de menus */
	VMenu *vmenu [MAX_MENUS];	/* Les menus verticaux */
	char **titlename;	/* Les noms des menus */
	char ***itemname;	/* Les noms des items */
	int (***f_item) ();	/* Les callbacks associees */
	unsigned int y_menu;	/* Position par rapport au parent */
	int h_item;		/* Hauteur d'un item en pixels	 */
	int hmax_menu;		/* Hauteur du plus grand menu en pixels	*/
	XFontStruct *font;		/* La fonte */
	GC Igc;			/* Le contexte graphique pour l'inverse*/
	GC Ngc;			/* Le contexte graphique normal */
	GC Bgc;			/* Le contexte graphique du bar */

	int width_relief;		/* Hauteur du relief	*/
	
	unsigned long bg_bar;	/* La couleur du fond du bar */
	unsigned long fg_bar;	/* La couleur du devant du bar	 */
	unsigned long bg_menu;	/* La couleur du font des menus */ 
	unsigned long fg_menu;	/* La couleur du devant des menus */
	int no_menu;		/* Le numero de menu courant	 */
	unsigned long   top_sh;
	unsigned long   bot_sh;
} XYMenu;

/*
 *	Public
 */
extern void	InitMenusRes ( /*display, font, fg, bg, ts, bs */ );
extern XYMenu 	*MakeMenu ( /* display, w, font, title, item, fnt, fg, bg */ );
extern int 	MouseInMenuBar ( /* display, w */ );
extern void 	SetMenuPixmap ( /* display, menu, width */ );
extern void 	SetHiddenWindow ( /* menu, w, gc */ ); 
extern void 	RefreshMenuBar ( /* display, menu */ );
extern int 	ButtonPressInMenu ( /* w, menu */ );
extern void 	DeleteMenu ( /* display, menu */ );
extern Window	HandleMenu ( /* display, ev, frame, menu, n, vm, item */ );

#define SetMenuBarWidth(m, x)	(m -> bar_width = x)
#define GetMenuWindowBar(m) ( m -> w_bar )
#define HeightMenuBar(font) ( (font -> ascent + font -> descent) + BAR_SPACE )

#endif /* _MENUS_H_ */
